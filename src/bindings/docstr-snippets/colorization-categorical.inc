R"example(import viren2d
import numpy as np
labels = np.array(
    [[1, 2, 3], [4, 5, 6], [20000, 20001, 20003]],
    dtype=np.int32)
vis = viren2d.colorize_labels(
    labels, colormap='category-20', output_channels=3))example"

