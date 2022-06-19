import viren2d

painter = viren2d.Painter()

# Either create an empty canvas:
painter.set_canvas_rgb(width=800, height=600, color='navy-blue')

# Initialize with a given NumPy image:
painter.set_canvas_image(np_image)

# Or initialize from an image file:
painter.set_canvas_filename('path/to/image.jpg')
