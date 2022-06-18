import numpy as np
import viren2d

def demo_color_names():
    # Set up empty canvas:
    painter = viren2d.Painter()
    canvas_width = 750
    painter.set_canvas_rgb(
        width=canvas_width, height=448, color='white!0')

    # Style specifications:
    text_style = viren2d.TextStyle(family='xkcd', size=18)
    text_style.alignment = 'center'

    line_style = viren2d.LineStyle(1, 'black')

    # Prepare grid/box layout:
    box_size = viren2d.Vec2d(130, 36)
    box_spacing = 20
    left = box_size.width / 2.0 + box_spacing / 2.0
    x = left
    y = box_spacing / 2.0
    
    # Iterate and visualize the available color names:
    for color_name in viren2d.color_names():
        color = viren2d.Color(color_name)
        text_style.color = color.grayscale().inverse()

        painter.draw_textbox(
            [color_name], (x, y), anchor=viren2d.TextAnchor.Top,
            text_style=text_style, padding=(6, 6),
            rotation=0, line_style=line_style,
            fill_color=color, radius=0.3,
            fixed_size=box_size)

        x += box_size.width + box_spacing
        if ((x + box_size.width / 2.0 + box_spacing / 2.0) > canvas_width):
            x = left
            y += box_size.height + box_spacing

    # Return the visualization as a NumPy buffer (let NumPy take care of
    # the memory copy):
    shared_canvas = painter.get_canvas(copy=False)
    return np.array(shared_canvas, copy=True)

