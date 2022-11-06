import numpy as np
import viren2d
from rtd_demo_images.constants import VIREN2D_DATA_PATH


def demo_optical_flow_colormaps():
    flow_full = viren2d.load_optical_flow(
        VIREN2D_DATA_PATH / 'sintel-alley2.flo')
    flow = flow_full.roi(495, 20, 380, 290)
    (_, max_motion, _, _) = flow.magnitude().min_max(0)
    
    painter = viren2d.Painter(height=230, width=600, color='white!0')
    grid_style = viren2d.LineStyle(2, "black!60")
    text_style = viren2d.TextStyle(family='xkcd', size=18)

    scale = (painter.width / 2 - 15) / flow.width

    for cmap, x in [('optical-flow', 5), ('orientation-6', 310)]:
        flow_vis = viren2d.colorize_optical_flow(
            flow=flow, colormap=cmap, motion_normalizer=max_motion)

        painter.draw_image(
            flow_vis, position=(x, 5), anchor='topleft', clip_factor=0.2,
            scale_x=scale, scale_y=scale)

        legend = viren2d.optical_flow_legend(
            size=100, colormap=cmap, line_style=grid_style)

        painter.draw_image(
            legend,
            position=(
                x + (flow_vis.width - 10) * scale,
                5 + (flow_vis.height - 10) * scale),
            anchor='bottom-right', line_style=grid_style, clip_factor=1,
            scale_x=scale, scale_y=scale)
        
        painter.draw_text(
            [f'Color map: {cmap}'], position=(
                x + flow_vis.width * scale / 2, 10),
            anchor='top', text_style=text_style)

    return np.array(painter.canvas, copy=True)


def demo_optical_flow_overlay():
    img = viren2d.load_image_uint8(VIREN2D_DATA_PATH / 'sintel-alley2.png')
    img_dimmed = (0.2 * np.array(img)).astype(np.uint8)
    
    flow = viren2d.load_optical_flow(VIREN2D_DATA_PATH / 'sintel-alley2.flo')
    (_, max_motion, _, _) = flow.magnitude().min_max(0)
    flow_vis = viren2d.colorize_optical_flow(
            flow=flow, colormap='orientation-6', motion_normalizer=max_motion)
    
    # #TODO declare gradient, use gradient overlay
    vis = flow_vis.blend(img_dimmed, 0.4)
    return np.array(vis, copy=True)
