import numpy as np
import viren2d
import os


def demo_relief_shading():
    # Prepare the lookup table for the color bar
    row = np.array([[i] * 2 for i in range(256)]).astype(np.uint8).flatten()
    colorbar_data = np.repeat(row.reshape((1,-1)), 25, axis=0)
    
    cwidth = 600
    cheight = 300
    painter = viren2d.Painter(width=cwidth, height=cheight, color='white!0')

    text_style = viren2d.TextStyle(family='xkcd', size=21)

    # Load the relief data & crop to some craters on the upper-left quadrant:
    moon_full = viren2d.load_image_uint8(
        os.path.join(os.path.abspath(os.path.dirname(__file__)), '..', '..', 'data', 'lunar-farside.jpg'))
    moon = moon_full.roi(left=200, top=200, width=200, height=200)

    cmap_names = ['Earth', 'Relief', 'Relief Low Contrast']

    column_width = (cwidth - (len(cmap_names) * 10)) / len(cmap_names)
    scale = column_width / moon.width

    x = column_width / 2 + 5
    for cmap_name in cmap_names:
        # Show the color bar on top of the shaded relief:
        colorbar = viren2d.colorize_scaled(
            data=colorbar_data, colormap=cmap_name, low=0, high=255)

        painter.draw_image(
            colorbar, position=(x, 15), anchor='center',
            clip_factor=0.5, scale_x=column_width/colorbar.width)

        painter.draw_text(
            [cmap_name], position=(x, 15), anchor='center',
            text_style=text_style)

        # Colorize & shade the relief of the farside of the moon:
        vis = viren2d.colorize_scaled(
            data=moon, colormap=cmap_name)

        relief = viren2d.relief_shading(relief=moon, colorized=vis)

        painter.draw_image(
            relief, position=(x, 35), anchor='top', clip_factor=1,
            scale_x=scale, scale_y=scale)

        # Also show the inputs to relief shading:
        painter.draw_image(
            moon, position=(x - 20, cheight - 5), anchor='bottom-right',
            clip_factor=1, scale_x=scale / 3, scale_y=scale / 3)
        
        painter.draw_image(
            vis, position=(x + 20, cheight - 5), anchor='bottom-left',
            clip_factor=1, scale_x=scale / 3, scale_y=scale / 3)

        # Draw a marker that looks like element-wise matrix multiplication:
        pos_multiplier = (x, cheight - 5 - moon.width * scale / 6)
        painter.draw_marker(
            pt=pos_multiplier,
            marker_style=viren2d.MarkerStyle('.', size=23, color='ivory'))
        painter.draw_marker(
            pt=pos_multiplier,
            marker_style=viren2d.MarkerStyle('o', size=18, color='black', thickness=2))
        painter.draw_marker(
            pt=pos_multiplier,
            marker_style=viren2d.MarkerStyle('x', size=12, color='black', thickness=2))

        x += (column_width + 10)

    return np.array(painter.canvas, copy=True)


def demo_colormaps():
    peaks = viren2d.peaks()
    canvas_width = 500
    cmap_bins = [256, 16, 8]
    column_width = (canvas_width - (len(cmap_bins) * 10)) / len(cmap_bins)
    scale = column_width / peaks.width

    canvas_height = int(scale * peaks.height + 10.5)
    painter = viren2d.Painter(
        width=canvas_width, height=canvas_height, color='white!0')
    
    text_style = viren2d.TextStyle(family='xkcd', size=18, color='black')

    x = column_width / 2 + 5
    for bins in cmap_bins:
        vis = viren2d.colorize_scaled(
            data=peaks, colormap='gouldian', low=-6.5, high=8, bins=bins)
        
        painter.draw_image(
            image=vis, position=(x, canvas_height / 2), anchor='center',
            scale_x=scale, scale_y=scale, rotation=0, clip_factor=0.2)

        painter.draw_text(
            text=[f'{bins} Bins'], anchor='bottom-left', rotation=-90, 
            position=(x + column_width / 2 - 5, canvas_height * 0.85),
            text_style=text_style)
        
        x += column_width + 10

    return np.array(painter.canvas, copy=True)


def demo_colorize_labels():
    try:
        from PIL import Image
        data_path = os.path.join(
            os.path.abspath(os.path.dirname(__file__)), '..', '..', 'data')

        class_labels = viren2d.load_image_uint8(
            os.path.join(data_path, 'semseg-classes.png'))
        # Instance labels are stored as 16-bit png, requires Pillow or any
        # other external library to load properly:
        instance_labels = np.array(
            Image.open(os.path.join(data_path, 'semseg-instances.png')))
        
        text_style = viren2d.TextStyle(
            family='xkcd', size=21, color='white')

        painter = viren2d.Painter(height=215, width=600, color='white!0')
        
        vis_cls = viren2d.colorize_labels(
            labels=class_labels, colormap='glasbey-light')
        
        scale = (painter.canvas.width / 2 - 10) / vis_cls.width

        painter.draw_image(
            vis_cls, position=(5, 5), anchor='top-left', scale_x=scale,
            scale_y=scale, clip_factor=0.15)

        painter.draw_text(
            text=['Class Labels'],
            position=(0.25 * painter.canvas.width, painter.canvas.height - 15),
            anchor='bottom', text_style=text_style)
        
        vis_ids = viren2d.colorize_labels(
            labels=instance_labels, colormap='glasbey-light')
        
        painter.draw_image(
            image=vis_ids, position=(painter.canvas.width - 5, 5),
            anchor='top-right', scale_x=scale, scale_y=scale, clip_factor=0.15)

        painter.draw_text(
            text=['Instance IDs'],
            position=(0.75 * painter.canvas.width, painter.canvas.height - 15),
            anchor='bottom', text_style=text_style)
        
        return np.array(painter.canvas, copy=True)

    except ModuleNotFoundError:
        print('Optional depency PIL is not installed - skipping label colorization demo.')
        return None
