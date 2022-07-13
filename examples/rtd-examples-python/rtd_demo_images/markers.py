import numpy as np
import viren2d


def cheat_sheet_markers():
    # Set up empty canvas:
    painter = viren2d.Painter()
    painter.set_canvas_rgb(
        width=600, height=310, color=viren2d.RGBa(26, 28, 29))

    # Style specifications:
    text_style = viren2d.TextStyle(color=viren2d.RGBa(192, 186, 177))
    marker_style = viren2d.MarkerStyle(
        size=27, thickness=2, cap='round', color='azure',
        bg_color='ivory', bg_border=3)

    def _prepare_display_row(y1, y2, y3):
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
    
    def _thickness(marker):
        # Due to the fixed marker size, we should adjust the line thickness for
        # a neater visualization.
        if marker == viren2d.Marker.Enneagram:
            return 1
        elif marker in [viren2d.Marker.Plus, viren2d.Marker.Cross,
                viren2d.Marker.Star]:
            return 4
        else:
            return 2

    # Iterate and visualize the available markers:
    y1, y2, y3 = 25, 75, 125
    left = 145
    _prepare_display_row(y1, y2, y3)
    x = left
    for marker in viren2d.Marker.list_all():
        # Put the marker's char code on top:
        painter.draw_text([str(marker)], (x, y1), 'center', text_style)
        
        # Draw the marker's outline (if it's shape allows):
        marker_style.marker = marker
        marker_style.thickness = _thickness(marker)
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
            _prepare_display_row(y1, y2, y3)

    return np.array(painter.canvas, copy=True)
