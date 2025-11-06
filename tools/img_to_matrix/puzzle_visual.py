import matplotlib.pyplot as plt
import numpy as np
import matplotlib.colors as mcolors

matrix2 = np.array([
    [-1, 1, -1, 2, 2, -1, -1, 3],
    [1, -1, 2, -1, -1, 5, 3, -1],
    [1, 4, -1, -1, 2, -1, 3, 3],
    [1, -1, 4, 4, 2, 6, 7, 3],
    [-1, -1, 4, -1, 2, 6, 7, 7],
    [-1, 4, 8, -1, 2, -1, 7, 7],
    [8, 4, -1, -1, 2, 6, 6, -1],
    [8, 8, 8, 8, 7, -1, 7, 7]
])

# Second matrix
matrix3 = np.array([
    [1, 1, 2, 2, 2, 3, 3, 3],
    [1, 4, 2, 4, 2, 5, 3, 3],
    [1, 4, 2, 4, 2, 3, 3, 3],
    [1, 4, 4, 4, 2, 6, 7, 3],
    [1, 4, 4, 4, 2, 6, 7, 7],
    [1, 4, 8, 4, 2, 6, 7, 7],
    [8, 4, 8, 4, 2, 6, 6, 7],
    [8, 8, 8, 8, 7, 7, 7, 7]
])

# Third matrix
# matrix = np.array([
#     [1, 1, 2, 2, 2, -1, -1, 3],
#     [1, 4, 2, 4, 2, 5, 3, 3],
#     [1, 4, 2, 4, 2, 3, 3, 3],
#     [1, 4, 4, 4, 2, 6, 7, 3],
#     [1, 4, 4, 4, 2, 6, 7, 7],
#     [1, 4, 8, 4, 2, 6, 7, 7],
#     [8, 4, 8, 4, 2, 6, 6, 7],
#     [8, 8, 8, 8, 7, 7, 7, 7]
# ])

matrix = [
    [1, 1, 1, 2, 2, 1, 3, 3, 4],
    [1, 5, 5, 5, 2, 1, 3, 4, 4],
    [1, 5, 2, 2, 2, 1, 3, 3, 3],
    [1, 5, 5, 1, 1, 1, 6, 6, 6],
    [1, 1, 1, 1, 1, 1, 6, 1, 6],
    [1, 1, 1, 1, 7, 1, 1, 1, 6],
    [8, 8, 8, 1, 7, 1, 7, 1, 1],
    [8, 9, 9, 1, 7, 7, 7, 1, 1],
    [8, 8, 9, 1, 1, 1, 1, 1, 1]
]

# Define specific colors for numbers 1-13
predefined_colors = [
    [0.3, 0.3, 0.3, 1.0],  # Dark gray for -1
    [0.12, 0.47, 0.71, 1.0],  # Blue
    [1.0, 0.5, 0.05, 1.0],   # Orange
    [0.17, 0.63, 0.17, 1.0],  # Green
    [0.84, 0.15, 0.16, 1.0],  # Red
    [0.58, 0.40, 0.74, 1.0],  # Purple
    [0.55, 0.34, 0.29, 1.0],  # Brown
    [0.89, 0.47, 0.76, 1.0],  # Pink
    [0.50, 0.50, 0.50, 1.0],  # Medium gray
    [0.74, 0.74, 0.13, 1.0],  # Olive
    [0.09, 0.75, 0.81, 1.0],  # Cyan
    [0.87, 0.80, 0.14, 1.0],  # Gold
    [0.68, 0.92, 0.34, 1.0],  # Lime
    [0.98, 0.70, 0.68, 1.0],  # Light salmon
]

custom_cmap = mcolors.ListedColormap(predefined_colors)

# Get unique values excluding -1
unique_values = np.unique(matrix)
unique_values = unique_values[unique_values != -1]  # Remove -1 from unique values

# Create a mapping where -1 maps to 0, and other values map to 1, 2, 3, etc.
color_map = {-1: 0}
for i, val in enumerate(unique_values):
    color_map[val] = i + 1
    
color_indices = np.vectorize(color_map.get)(matrix)

fig, ax = plt.subplots()
cax = ax.imshow(color_indices, cmap=custom_cmap, origin='upper')
cax.set_clim(-0.5, len(color_map)-0.5)

# Draw black borders and add question marks for -1 values
for (i, j), val in np.ndenumerate(matrix):
    ax.add_patch(plt.Rectangle((j-0.5, i-0.5), 1, 1, fill=False, edgecolor='black', lw=1))
    if val == -1:
        ax.text(j, i, '?', ha='center', va='center', color='white', fontsize=12, fontweight='bold')

# Create custom colorbar with -1 labeled separately
cbar = plt.colorbar(cax, ticks=range(len(color_map)), label='Value')
cbar.ax.set_yticklabels(['?'] + list(unique_values))

plt.savefig("matrix_visual_with_borders.png", dpi=300)
print("Saved visualization as matrix_visual_with_borders.png")
