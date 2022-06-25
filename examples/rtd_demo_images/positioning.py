import numpy as np
import viren2d


def demo_text_anchors():
    # Set up empty canvas:
    painter = viren2d.Painter()
    canvas_width = 440
    canvas_height = 240
    painter.set_canvas_rgb(
        width=canvas_width, height=canvas_height, color='white!0')

    # Style specifications:
    text_style = viren2d.TextStyle(family='xkcd', size=18)
    text_style.alignment = 'center'

    line_style = viren2d.LineStyle(
        width=1, dash_pattern=[10, 10], color=(0.4, 0.4, 0.4))

    marker_style = viren2d.MarkerStyle(
        viren2d.Marker.Pentagram, size=18, thickness=1,
        filled=True, color='navy-blue!80')

    # Draw a grid
    offset_x = 20
    offset_y = 20
    delta_x = 200
    delta_y = 100
    for i in range(3):
        x = offset_x + i * delta_x
        y = offset_y + i * delta_y
        painter.draw_line((5, y), (canvas_width - 5, y), line_style)
        painter.draw_line((x, 5), (x, canvas_height - 5), line_style)

    y = offset_y
    x = offset_x
    padding = (6, 6)
    for anchor in viren2d.Anchor.list_all():
        pos = (x, y)
        if anchor != viren2d.Anchor.Center:
            painter.draw_marker(pos, marker_style)

        painter.draw_textbox(
            [str(anchor)], pos, anchor=anchor, text_style=text_style,
            padding=padding, rotation=0, line_style=viren2d.LineStyle.Invalid,
            fill_color='azure!40', radius=0.15)

        x += delta_x
        if x >= canvas_width:
            x = offset_x
            y += delta_y

    # Return the visualization as a NumPy buffer (let NumPy take care of
    # the memory copy):
    shared_canvas = painter.get_canvas(copy=False)
    return np.array(shared_canvas, copy=True)


def demo_multiline_text():
    # Set up empty canvas:
    painter = viren2d.Painter()
    canvas_width = 750
    canvas_height = 155
    painter.set_canvas_rgb(
        width=canvas_width, height=canvas_height, color='white!0')

    poem = [
        'This nature frames my world!',
        '',
        'It´s a Leitmotiv,',
        'a compass, an anchor,',
        'a constant companion,',
        'a steady hand...'
    ]
    padding = (14, 6)
    color = 'tangerine!90'

    line_style = viren2d.LineStyle.Invalid

    text_style = viren2d.TextStyle(family='xkcd', size=14)
    text_style.alignment = 'left'
    painter.draw_textbox(
        poem, (5, 5), anchor='top-left', text_style=text_style,
        padding=padding, rotation=0, line_style=line_style,
        fill_color=color, radius=0.1)

    text_style.alignment = 'center'
    painter.draw_textbox(
        poem, (canvas_width / 2, 5), anchor='top', text_style=text_style,
        padding=padding, rotation=0, line_style=line_style,
        fill_color=color, radius=0.1)

    text_style.alignment = 'right'
    painter.draw_textbox(
        poem, (canvas_width - 5, 5), anchor='top-right', text_style=text_style,
        padding=padding, rotation=0, line_style=line_style,
        fill_color=color, radius=0.1)

    # Return the visualization as a NumPy buffer (let NumPy take care of
    # the memory copy):
    shared_canvas = painter.get_canvas(copy=False)
    return np.array(shared_canvas, copy=True)
