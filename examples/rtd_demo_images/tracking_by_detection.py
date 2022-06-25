import numpy as np
import viren2d
import pathlib


def demo_tracking_by_detection():
    painter = viren2d.Painter()
    painter.set_canvas_filename(
        str(pathlib.Path(__file__).parent.parent / 'ninja.jpg'))

    line_style = viren2d.LineStyle(width=4, color='navy-blue!80')
    
    text_style = viren2d.TextStyle(
        family='xkcd', alignment=viren2d.HorizontalAlignment.Center)
    
    # Customize the style
    bbox_style = viren2d.BoundingBox2DStyle()
    bbox_style.line_style = line_style
    bbox_style.line_style.color = "navy-blue"
    bbox_style.text_style = text_style
    bbox_style.text_style.color = bbox_style.line_style.color
    bbox_style.text_style.size = 18
    bbox_style.box_fill_color = 'same!15'
    bbox_style.text_fill_color = 'white!60'
    bbox_style.label_position = 'top'
    
    # Tabi bounding box
    rect_tabi = viren2d.Rect(
        center=(505, 290), size=(120, 70), rotation=18, radius=20)
    painter.draw_bounding_box_2d(rect_tabi, ['Tabi socks'], bbox_style)

    # Lens bounding box
    bbox_style.label_position = 'left'
    bbox_style.line_style.color = 'teal-green'
    bbox_style.text_style.color = 'black'
    rect_lens = viren2d.Rect(
        center=(150, 145), size=(310, 190), rotation=-5, radius=20)
    painter.draw_bounding_box_2d(rect_lens, ['Camera lens'], bbox_style)

    # Trajectory of the katana:
    traj_sword = [
        (646, 192), (642, 166), (634, 136), (620, 108), (610,  88), (588,  70),
        (566,  58), (546,  54), (522,  52), (492,  56), (462,  66), (434,  80),
        (414,  98), (402, 124), (392, 148), (384, 174), (366, 200), (350, 224),
        (318, 240), (288, 246), (256, 246), (230, 238)]

    line_style.cap = viren2d.LineCap.Round
    line_style.color = 'navy-blue'
    painter.draw_trajectory(
        traj_sword, line_style=line_style,
        fade_out_color="white!100", tail_first=False,
        smoothing_window=3)

    # Mark the tip of the Katana - Create a border effect
    # by drawing 2 differently sized markers:
    marker_style = viren2d.MarkerStyle()
    marker_style.marker = '5'
    marker_style.filled = True
    for size, color in [(35, 'white!80'), (25, line_style.color)]:
        marker_style.color = color
        marker_style.size = size
        painter.draw_marker(traj_sword[0], marker_style)

    # Arrow
    arrow_style = viren2d.ArrowStyle(width=4, color='crimson', tip_length=0.3)
    painter.draw_arrow((420, 60), (480, 100), arrow_style)

    # Text box
    text_style.line_spacing = 1.0
    text_style.color = 'crimson'
    line_style.width = 2
    painter.draw_textbox(
        ['Mood:', 'Mildly infuriated'], position=(420, 60),
        anchor=viren2d.Anchor.BottomRight,
        text_style=text_style, padding=(10, 5),
        fill_color="white!80", line_style=line_style)

    shared_canvas = painter.get_canvas(copy=False)
    return np.array(shared_canvas, copy=True)
