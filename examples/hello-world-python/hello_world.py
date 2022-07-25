import numpy as np
import viren2d
from PIL import Image

# Set up a white canvas:
width, height = 400, 50
painter = viren2d.Painter(width=width, height=height, color='#1a1c1d')

# Draw "Hello World":
text_style = viren2d.TextStyle(
    family='sans-serif', size=35, color='#c0bab1', bold=True)

bbox = painter.draw_text(
    text=['Hello World!'], position=(width/2, height/2),
    anchor='center', text_style=text_style)

# Draw markers to the left & right:
marker_style = viren2d.MarkerStyle(
    marker='5', size=30, filled=True, color='midnight-blue',
    bg_border=2, bg_color='ivory')

painter.draw_marker(
    position=(bbox.cx - bbox.width/2 - 35, height/2),
    marker_style=marker_style)
painter.draw_marker(
    position=(bbox.cx + bbox.width/2 + 35, height/2),
    marker_style=marker_style)

# Display the image:
shared_canvas_np = np.array(painter.canvas, copy=False)
image = Image.fromarray(shared_canvas_np)
image.show()
