import numpy as np
import viren2d


def demo_text_anchors():
    # Plot available anchors with corresponding reference point
    painter = viren2d.Painter()
    canvas_width = 440
    canvas_height = 240
    painter.set_canvas_rgb(
        width=canvas_width, height=canvas_height, color='white!0')

    # Style specifications:
    text_style = viren2d.TextStyle(
        family='xkcd', size=18, halign='center', color='white')

    line_style = viren2d.LineStyle(
        width=1, dash_pattern=[10, 10], color=(0.4, 0.4, 0.4))

    marker_style = viren2d.MarkerStyle(
        viren2d.Marker.RotatedReticle, size=14, thickness=3, filled=True,
        color='rose-red!90', bg_border=2, bg_color='ivory!80')

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

    x = offset_x
    y = offset_y
    padding = (6, 6)
    for anchor in viren2d.Anchor.list_all():
        pos = (x, y)
        if anchor != viren2d.Anchor.Center:
            painter.draw_marker(pos, marker_style)

        painter.draw_text_box(
            [str(anchor)], pos, anchor=anchor, text_style=text_style,
            padding=padding, rotation=0, line_style=viren2d.LineStyle.Invalid,
            fill_color='azure!90', radius=0.15)

        x += delta_x
        if x >= canvas_width:
            x = offset_x
            y += delta_y

    return np.array(painter.canvas, copy=True)


def demo_multiline_text():
    # Horizontal & vertical alignment for fixed-size text boxes
    painter = viren2d.Painter()
    canvas_width = 750
    canvas_height = 185
    painter.set_canvas_rgb(
        width=canvas_width, height=canvas_height, color='white!0')

    poem = [
        'This nature frames my world!',
        '',
        "It's a Leitmotiv,",
        'a compass, an anchor,',
        'a constant companion,',
        'a steady hand...'
    ]
    padding = (14, 6)
    fixed_box_size = (220, 170)
    color = 'tangerine!90'

    line_style = viren2d.LineStyle.Invalid

    text_style = viren2d.TextStyle(family='xkcd', size=14)
    text_style.halign = 'left'
    text_style.valign = 'top'
    painter.draw_text_box(
        poem, (5, 5), anchor='top-left', text_style=text_style,
        padding=padding, rotation=0, line_style=line_style,
        fill_color=color, radius=0.1, fixed_size=fixed_box_size)

    text_style.halign = 'center'
    text_style.valign = 'center'
    painter.draw_text_box(
        poem, (canvas_width / 2, 5), anchor='top', text_style=text_style,
        padding=padding, rotation=0, line_style=line_style,
        fill_color=color, radius=0.1, fixed_size=fixed_box_size)

    text_style.halign = 'right'
    text_style.valign = 'bottom'
    painter.draw_text_box(
        poem, (canvas_width - 5, 5), anchor='top-right', text_style=text_style,
        padding=padding, rotation=0, line_style=line_style,
        fill_color=color, radius=0.1, fixed_size=fixed_box_size)

    return np.array(painter.canvas, copy=True)
