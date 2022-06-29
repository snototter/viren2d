# Get the visualization as `viren2d.ImageBuffer`:
img_buffer = painter.get_canvas()

# Convert to NumPy array (avoid unnecessary copy):
import numpy as np
img_np = np.array(img_buffer, copy=False)

# For convenience, the canvas is also exposed as property of the painter:
img_np = np.array(painter.canvas, copy=False)
