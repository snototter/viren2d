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
    img_dimmed = img.dim(0.4)

    # Colorize optical flow    
    colormap = 'optical-flow'
    flow = viren2d.load_optical_flow(VIREN2D_DATA_PATH / 'sintel-alley2.flo')
    (_, max_motion, _, _) = flow.magnitude().min_max(0)
    flow_vis = viren2d.colorize_optical_flow(
            flow=flow, colormap=colormap, motion_normalizer=max_motion)
    
    # Blend input image & flow with a smooth gradient
    grad = viren2d.LinearColorGradient((0, 0), (img.width, img.height))
    grad.add_intensity_stop(0.1, 1.0)
    grad.add_intensity_stop(0.3, 0.3)
    grad.add_intensity_stop(0.6, 0.2)
    grad.add_intensity_stop(0.9, 1.0)
    alpha = grad.mask(width=img.width, height=img.height, channels=1)

    vis = flow_vis.blend_mask(img_dimmed, alpha)

    # Overlay color map legend
    legend = viren2d.optical_flow_legend(size=100, colormap=colormap)
    painter = viren2d.Painter(
        width=vis.width, height=vis.height + 10, color='white!0')
    painter.draw_image(vis, position=(0, 0), anchor='top-left', clip_factor=0.1)
    painter.draw_image(
        legend, position=(vis.width - 10, vis.height - 20),
        anchor='bottom-right', clip_factor=1)

    return np.array(painter.canvas, copy=True)
