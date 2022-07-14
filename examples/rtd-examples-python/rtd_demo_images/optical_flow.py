import numpy as np
import viren2d
import os


def demo_optical_flow():
    filename = os.path.join(
        os.path.abspath(os.path.dirname(__file__)),
        '..', '..', 'data', 'sintel-alley2.flo')
    flow = viren2d.load_optical_flow(filename)
    (_, max_motion, _, _) = flow.magnitude().min_max(0)
    
    painter = viren2d.Painter(height=230, width=600, color='white!0')
    grid_style = viren2d.LineStyle(2, "black!60")

    scale = (painter.width / 2 - 15) / flow.width

    for cmap, x in [('optical-flow', 5), ('orientation-6', 310)]:
        flow_vis = viren2d.colorize_optical_flow(
            flow=flow, colormap=cmap, motion_normalizer=max_motion)

        painter.draw_image(
            flow_vis, position=(x, 5), anchor='topleft', clip_factor=0.2,
            scale_x=scale, scale_y=scale)

        legend = viren2d.optical_flow_legend(
            size=120, colormap=cmap, line_style=grid_style)

        painter.draw_image(
            legend,
            position=(
                x + (flow_vis.width - 10) * scale,
                5 + (flow_vis.height - 10) * scale),
            anchor='bottom-right', line_style=grid_style, clip_factor=1,
            scale_x=scale, scale_y=scale)

    return np.array(painter.canvas, copy=True)
