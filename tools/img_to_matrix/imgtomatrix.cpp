#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <chrono>

using namespace cv;
using namespace std;

// Function to extract grid and return board configuration
tuple<vector<vector<int>>, map<string, Scalar>, int, tuple<int, int>, tuple<int, int>> extract_grid(const string& file_name) {
    auto start = chrono::high_resolution_clock::now();

    // Read the input image
    Mat original = imread(file_name);
    if (original.empty()) {
        cerr << "Error: Image not found!" << endl;
        return {};
    }
    
    // Save the original image for reference
    imwrite("solution/original.png", original);

    // Convert the image to grayscale for easier contour detection
    Mat gray;
    cvtColor(original, gray, COLOR_BGR2GRAY);

    // Find contours in the grayscale image
    vector<vector<Point>> contours;
    findContours(gray, contours, RETR_TREE, CHAIN_APPROX_NONE);

    // Sort contours by area in descending order
    sort(contours.begin(), contours.end(), [](const vector<Point>& a, const vector<Point>& b) {
        return contourArea(a) > contourArea(b);
    });

    // Extract the bounding box of the puzzle grid (usually the second largest contour)
    Rect bounding_box = boundingRect(contours[1]);

    // Crop the grid area from the original image
    Mat grid = original(bounding_box);

    // Save the cropped grid image
    imwrite("solution/grid.png", grid);

    // Convert the cropped grid to grayscale
    cvtColor(grid, gray, COLOR_BGR2GRAY);
    imwrite("solution/gray-grid.png", gray);

    // Find contours again within the cropped grayscale grid
    Mat thresh;
    threshold(gray, thresh, 127, 255, THRESH_BINARY);
    findContours(thresh, contours, RETR_TREE, CHAIN_APPROX_NONE);

    // Sort contours by area
    sort(contours.begin(), contours.end(), [](const vector<Point>& a, const vector<Point>& b) {
        return contourArea(a) < contourArea(b);
    });

    // Calculate the total number of cells and the grid size
    int total_cells = contours.size() - 2;
    if (total_cells <= 0) {
        cerr << "No valid cells detected! Aborting" << endl;
        return {};
    }

    int grid_size = static_cast<int>(sqrt(total_cells));
    if (total_cells != grid_size * grid_size) {
        cerr << "Unable to detect full grid! Aborting" << endl;
        return {};
    }

    // Calculate the width and height of each cell
    int cell_width = bounding_box.width / grid_size;
    int cell_height = bounding_box.height / grid_size;

    // Initialize variables to store colors and board configuration
    map<string, Scalar> color_map;
    map<int, Scalar> reverse_color_map;
    int color_index = 1;
    vector<vector<int>> board;

    // Iterate through each cell in the grid to extract colors
    for (int i = 0; i < grid_size; ++i) {
        vector<int> row;
        for (int j = 0; j < grid_size; ++j) {
            // Calculate cell coordinates with padding
            int padding = 15;
            int cell_x = j * cell_width;
            int cell_y = i * cell_height;
            Rect cell_rect(cell_x + padding, cell_y + padding, cell_width - 2 * padding, cell_height - 2 * padding);
            Mat cell = grid(cell_rect);

            // Get the average color of the cell
            Scalar avg_color = mean(cell);
            string color_key = to_string(int(avg_color[0])) + "," + to_string(int(avg_color[1])) + "," + to_string(int(avg_color[2]));

            // Map each unique color to an index
            if (color_map.find(color_key) == color_map.end()) {
                color_map[color_key] = avg_color;
                reverse_color_map[color_index] = avg_color;
                row.push_back(color_index++);
            } else {
                row.push_back(distance(color_map.begin(), color_map.find(color_key)) + 1);
            }
        }
        board.push_back(row);
    }

    // Check if the number of detected colors matches the grid size; abort if mismatched
    if (color_map.size() != grid_size) {
        cerr << "Too many colors detected! Aborting" << endl;
        return {};
    }

    // Print the C++ board format for debugging
    cout << "C++ Board Format:" << endl;
    cout << "vector<vector<int>> board = {" << endl;
    for (size_t i = 0; i < board.size(); ++i) {
        cout << "    {";
        for (size_t j = 0; j < board[i].size(); ++j) {
            cout << board[i][j];
            if (j < board[i].size() - 1)
                cout << ", ";
        }
        cout << "}";
        if (i < board.size() - 1)
            cout << ",";
        cout << endl;
    }
    cout << "};" << endl;

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Time taken: " << elapsed.count() << " seconds" << endl;

    return make_tuple(board, color_map, grid_size, make_tuple(bounding_box.width, bounding_box.height), make_tuple(cell_width, cell_height));
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <image_filename>" << endl;
        return 1;
    }

    string file_name = argv[1];
    auto result = extract_grid(file_name);

    if (!get<0>(result).empty()) {
        vector<vector<int>> board = get<0>(result);
        auto color_map = get<1>(result);
        int grid_size = get<2>(result);
        auto dimensions = get<3>(result);
        auto cell_dimensions = get<4>(result);

        cout << "Grid size: " << grid_size << "x" << grid_size << endl;
        cout << "Board representation:" << endl;
        for (const auto& row : board) {
            for (const auto& cell : row) {
                cout << cell << " ";
            }
            cout << endl;
        }
    }

    return 0;
}

