R"example(import viren2d
import numpy as np
# Create a custom color map.
# Example 1: Define colors manually.
viren2d.set_custom_colormap(
    'custom1', ['#800000', (0.94, 0.13, 0.15), 'rose-red'])
# Example 2: Reverse an existing color map.
cmap = viren2d.get_colormap('water')
viren2d.set_custom_colormap('custom2', cmap[::-1])
# Apply a custom color map, e.g. for label colorization:
labels = np.array(
    [[0, 1, 2], [-3, -4, -6], [20000, 20001, 20003]], dtype=np.int32)
vis = viren2d.colorize_labels(labels=labels, colormap='custom1'))example"

