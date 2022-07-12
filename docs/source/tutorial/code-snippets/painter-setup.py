import viren2d

painter = viren2d.Painter()

# Either create an empty canvas:
painter.set_canvas_rgb(height=600, width=800, color='navy-blue')

# Initialize with a given NumPy ndarray `np_image`:
painter.set_canvas_image(image_numpy)

# Or initialize from an image file:
painter.set_canvas_filename('path/to/image.jpg')
