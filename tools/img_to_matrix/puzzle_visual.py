import matplotlib.pyplot as plt
import numpy as np

matrix = np.array([
    [1, 1, 2, 2, 2, 3, 3, 3],
    [1, 4, 2, 4, 2, 5, 3, 3],
    [1, 4, 2, 4, 2, 3, 3, 3],
    [1, 4, 4, 4, 2, 6, 7, 3],
    [1, 4, 4, 4, 2, 6, 7, 7],
    [1, 4, 8, 4, 2, 6, 7, 7],
    [8, 4, 8, 4, 2, 6, 6, 7],
    [8, 8, 8, 8, 7, 7, 7, 7]
])

unique_values = np.unique(matrix)
color_map = {val: i for i, val in enumerate(unique_values)}
color_indices = np.vectorize(color_map.get)(matrix)

fig, ax = plt.subplots()
cax = ax.imshow(color_indices, cmap='tab20', origin='upper')
cax.set_clim(-0.5, len(unique_values)-0.5)

# Draw black borders
for (i, j), val in np.ndenumerate(matrix):
    ax.add_patch(plt.Rectangle((j-0.5, i-0.5), 1, 1, fill=False, edgecolor='black', lw=1))

plt.colorbar(cax, ticks=range(len(unique_values)), label='Value')

plt.savefig("matrix_visual_with_borders.png", dpi=300)
print("Saved visualization as matrix_visual_with_borders.png")
