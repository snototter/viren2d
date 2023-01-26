import numpy as np
import viren2d
from rtd_demo_images.constants import VIREN2D_DATA_PATH


def demo_tracking_by_detection():
    painter = viren2d.Painter()
    painter.set_canvas_filename(VIREN2D_DATA_PATH / 'ninja.jpg')

    # Create a customized bounding box style
    line_style = viren2d.LineStyle(width=4, color='navy-blue!80')
    
    text_style = viren2d.TextStyle(
        family='xkcd', size=18, color='navy-blue', halign='center')
    
    bbox_style = viren2d.BoundingBox2DStyle(
        line_style=line_style, text_style=text_style,
        box_fill_color = 'same!15', text_fill_color = 'white!60')
    
    # Draw the bounding box containing the Tabi
    rect_tabi = viren2d.Rect(
        center=(508, 285), size=(120, 70), rotation=18, radius=20)
    painter.draw_bounding_box_2d(
        rect_tabi, bbox_style, label_top=['Tabi socks'])

    # Draw the bounding box around the face
    bbox_style.line_style.color = 'crimson'
    bbox_style.text_style.color = 'crimson'
    rect_face = viren2d.Rect(
        center=(525, 120), size=(80, 100), rotation=10, radius=20)
    painter.draw_bounding_box_2d(
        rect_face, bbox_style, label_top=['Angry'], label_bottom=['Warrior'])


    # Draw the bounding box containing the lens
    bbox_style.line_style.color = 'teal-green'
    bbox_style.text_style.color = 'black'
    rect_lens = viren2d.Rect(
        center=(150, 145), size=(310, 190), rotation=-5, radius=20)
    painter.draw_bounding_box_2d(
        rect_lens, bbox_style, label_top=['IP camera'],
        label_right=['Varifocal lens'], right_t2b=True)

    # Draw the trajectory of the katana:
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

    # Mark the tip of the sword:
    marker_style = viren2d.MarkerStyle(
        marker='5', size=30, filled=True, color=line_style.color,
        bg_border=3, bg_color='ivory!70')
    painter.draw_marker(traj_sword[0], marker_style)

    return np.array(painter.canvas, copy=True)
