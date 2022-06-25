import numpy as np
import viren2d
from vito import imvis

# Set up a white canvas:
width, height = 400, 50
painter = viren2d.Painter(width=width, height=height, color='#ffffff')

# Draw "Hello World":
text_style = viren2d.TextStyle(
    family='sans-serif', size=35, color='crimson', bold=True)

bbox = painter.draw_text(
    text=['Hello World!'], anchor_position=(width/2, height/2),
    anchor='center', text_style=text_style)

# Draw markers to the left & right:
marker_style = viren2d.MarkerStyle(
    marker='5', size=40, filled=True, color='midnight-blue', join='round')

painter.draw_marker(
    pt=(bbox.cx - bbox.width/2 - 30, height/2), marker_style=marker_style)
painter.draw_marker(
    pt=(bbox.cx + bbox.width/2 + 30, height/2), marker_style=marker_style)

# Display the image:
shared_canvas_np = np.array(painter.canvas, copy=False)
imvis.imshow(shared_canvas_np)
