import cv2 as cv
import numpy as np
import math
import time

def extract_grid(file_name):
    start = time.perf_counter()

    # Read the input image
    original = cv.imread(file_name)
    # Save the original image for reference
    cv.imwrite("solution/original.png", original)

    # Convert the image to grayscale for easier contour detection
    gray = cv.cvtColor(original, cv.COLOR_BGR2GRAY)

    # Find contours in the grayscale image
    contours, _ = cv.findContours(gray, cv.RETR_TREE, cv.CHAIN_APPROX_NONE)
    # Sort contours by area in descending order
    contours = sorted(contours, key=cv.contourArea, reverse=True)

    # Extract the bounding box of the puzzle grid (usually the second largest contour)
    x, y, w, h = cv.boundingRect(contours[1])

    # Crop the grid area from the original image
    grid = original[y:y+h, x:x+w]
    # Save the cropped grid image
    cv.imwrite("solution/grid.png", grid)

    # Convert the cropped grid to grayscale
    gray = cv.cvtColor(grid, cv.COLOR_BGR2GRAY)
    # Save the grayscale grid image
    cv.imwrite("solution/gray-grid.png", gray)

    # Find contours again within the cropped grayscale grid
    # Apply threshold to ensure better contour detection
    _, thresh = cv.threshold(gray, 127, 255, cv.THRESH_BINARY)
    contours, _ = cv.findContours(thresh, cv.RETR_TREE, cv.CHAIN_APPROX_NONE)
    # Sort contours by area
    contours = sorted(contours, key=cv.contourArea)

    # Calculate the total number of cells and the grid size
    total_cells = len(contours) - 2
    if total_cells <= 0:
        print("No valid cells detected! Aborting")
        return None
    
    grid_size = int(math.sqrt(total_cells))
    if total_cells != grid_size**2:
        print("Unable to detect full grid! Aborting")
    # Initialize variables to store board configuration
    # Calculate the width and height of each cell
    cell_width = w // grid_size
    cell_height = h // grid_size

    # Initialize variables to store colors and board configuration
    colors = []
    board = []
    color_index = 1
    color_map = {}
    reverse_color_map = {}
    padding = 10

    # Iterate through each cell in the grid to extract colors
    for i in range(grid_size):
        row = []
        for j in range(grid_size):
            # Calculate cell coordinates with padding
            cell_x = j * cell_width
            cell_y = i * cell_height
            padding = 15
            cell = grid[cell_y+padding:cell_y+cell_height-padding, cell_x+padding:cell_x+cell_width-padding]
            
            # Get the average color of the cell
            avg_color = cell.mean(axis=0).mean(axis=0)
            avg_color = avg_color.astype(int)
            avg_color = tuple(avg_color)
            
            # Map each unique color to an index
            if avg_color not in color_map:
                color_map[avg_color] = str(color_index)
                reverse_color_map[str(color_index)] = avg_color
                color_index += 1
            row.append(color_map[avg_color])
            
        # Append the row to the board configuration
        board.append(row)
                
    # Check if the number of detected colors matches the grid size; abort if mismatched
    if len(color_map) != grid_size:
        print("Too many colors detected! Aborting")
        return None

    # Convert the string representation to integers for C++ compatibility
    cpp_board = []
    for row in board:
        cpp_row = [int(cell) for cell in row]
        cpp_board.append(cpp_row)
    
    print("C++ Board Format:")
    print("vector<vector<int>> board = {")
    for i, row in enumerate(cpp_board):
        print(f"    {{{', '.join(map(str, row))}}}", end="")
        if i < len(cpp_board) - 1:
            print(",")
        else:
            print("")
    print("};")

    
    end = time.perf_counter()
    elapsed = end - start
    print(f'Time taken: {elapsed:.6f} seconds')
    
    return board, reverse_color_map, grid_size, (w, h), (cell_width, cell_height)

# Example usage
if __name__ == "__main__":
    result = extract_grid("puzzle.png")
    if result:
        board, color_map, grid_size, dimensions, cell_dimensions = result
        print(f"Grid size: {grid_size}x{grid_size}")
        print("Board representation:")
        for row in board:
            print(row)
