import numpy as np
import viren2d


def demo_markers():
    # Set up empty canvas:
    painter = viren2d.Painter()
    painter.set_canvas_rgb(width=600, height=310, color='white!0')

    # Style specifications:
    text_style = viren2d.TextStyle(color=(0.3, 0.3, 0.3))
    marker_style = viren2d.MarkerStyle(
        size=25, thickness=1, color='navy-blue')

    def prepare_display_row(y1, y2, y3):
        text_style.family = 'xkcd'
        text_style.size = 18

        painter.draw_text(
            ['Marker code:'], (5, y1), 'left', text_style)
        painter.draw_text(
            ['Not filled:'], (5, y2), 'left', text_style)
        painter.draw_text(
            ['Filled:'], (5, y3), 'left', text_style)

        # Adjust text style for the marker code outputs:    
        text_style.family = 'monospace'
        text_style.size = 18

        # Draw a delimiter below this row
        painter.draw_line(
            (15, y3 + 30), (585, y3 + 30),
            viren2d.LineStyle(width=1, color=(0.3, 0.3, 0.3, 0.6)))

    # Iterate and visualize the available markers:
    y1, y2, y3 = 25, 75, 125
    left = 145
    prepare_display_row(y1, y2, y3)
    x = left
    for code in viren2d.marker_codes():
        # Put the marker's char code on top:
        painter.draw_text([f"'{code}'"], (x, y1), 'center', text_style)

        # Draw the marker's outline (if it's shape allows):
        marker_style.marker = code
        marker_style.filled = False
        if not marker_style.is_filled():
            painter.draw_marker((x, y2), marker_style)

        # Draw the filled marker (if it's shape allows):
        marker_style.filled = True
        if marker_style.is_filled():
            painter.draw_marker((x, y3), marker_style)

        x += 42
        if x >= 580:  # Start the 2nd row
            x = left
            y1 += 160
            y2 += 160
            y3 += 160
            prepare_display_row(y1, y2, y3)

    # Return the visualization as a NumPy buffer (let NumPy take care of
    # the memory copy):
    shared_canvas = painter.get_canvas(copy=False)
    return np.array(shared_canvas, copy=True)