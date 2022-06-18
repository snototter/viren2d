import numpy as np
import viren2d

def demo_markers():
    # Set up empty canvas:
    painter = viren2d.Painter()
    painter.set_canvas_rgb(width=800, height=100, color='white!0')

    # Style specifications:
    text_style = viren2d.TextStyle(
        family='xkcd', size=16, color=(0.3, 0.3, 0.3))
    marker_style = viren2d.MarkerStyle(
        size=19, thickness=1, color='navy-blue')

    # Draw text showing what we will see in each row:
    painter.draw_text(
        ['Marker code:'], (5, 5), viren2d.TextAnchor.TopLeft, text_style)
    painter.draw_text(
        ['Not filled:'], (5, 50), viren2d.TextAnchor.Left, text_style)
    painter.draw_text(
        ['Filled:'], (5, 85), viren2d.TextAnchor.Left, text_style)

    # Adjust text style for the marker code outputs:
    text_style.size = 16
    text_style.family = 'monospace'

    # Iterate and visualize the available markers:
    x = 110
    for code in viren2d.marker_codes():
        # Put the marker's char code on top:
        painter.draw_text(
            [f"'{code}'"], (x, 5), viren2d.TextAnchor.Top, text_style)

        # Then, draw the marker's outline (if it's shape allows):
        marker_style.marker = code
        marker_style.filled = False
        if not marker_style.is_filled():
            painter.draw_marker((x, 45), marker_style)

        # Finally, draw the filled marker (if it's shape allows):
        marker_style.filled = True
        if marker_style.is_filled():
            painter.draw_marker((x, 80), marker_style)
        x += 32

    # Return the visualization as a NumPy buffer (let NumPy take care of
    # the memory copy):
    shared_canvas = painter.get_canvas(copy=False)
    return np.array(shared_canvas, copy=True)

