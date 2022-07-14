from mailbox import linesep
import numpy as np
import viren2d


def _dash2str(pattern):
    return '[' + ', '.join([f'{int(d)}' for d in pattern]) + ']'


def cheat_sheet_linestyle():
     # Set up empty canvas:
    painter = viren2d.Painter()
    canvas_width = 400
    canvas_height = 440
    painter.set_canvas_rgb(
        width=canvas_width, height=canvas_height, color='white!0')

   # Style specifications:
    text_style = viren2d.TextStyle(
        family='xkcd', size=22, color=(0.3, 0.3, 0.3))

    # Draw titles:
    painter.draw_text(
        ['Line Cap:'], (canvas_width / 4, 5),
        'north', text_style)
    
    painter.draw_text(
        ['Line Join:'], (3 * canvas_width / 4, 5),
        'north', text_style)
    
    painter.draw_text(
        ['Dash Pattern:'], (canvas_width / 4, 190),
        'north', text_style)
    text_style.size = 18

    ### Different LineCap settings
    line_style = viren2d.LineStyle(
        width=29, color='navy-blue!80')

    y = 60
    line_style.cap = 'butt'
    painter.draw_line(
        (40, y), (canvas_width / 2 - 40, y), line_style)
    
    text_style.color = 'white'
    painter.draw_text(
        [str(line_style.cap)], (canvas_width / 4, y),
        'center', text_style)

    y = 100
    line_style.cap = 'round'
    painter.draw_line(
        (40, y), (canvas_width / 2 - 40, y), line_style)

    painter.draw_text(
        [str(line_style.cap)], (canvas_width / 4, y),
        'center', text_style)

    y = 140
    line_style.cap = 'square'
    painter.draw_line(
        (40, y), (canvas_width / 2 - 40, y), line_style)

    painter.draw_text(
        [str(line_style.cap)], (canvas_width / 4, y),
        'center', text_style)

    ### Different LineJoin settings
    line_style.cap = 'round'

    horizontal_length = canvas_width / 2 - 40
    y = 60
    poly = [
        (canvas_width / 2 + 20, y),
        (canvas_width / 2 + horizontal_length, y),
        (canvas_width / 2 + horizontal_length, y + 30)]
    line_style.join = 'miter'
    painter.draw_polygon(poly, line_style)

    painter.draw_text(
        [str(line_style.join)], (0.58 * canvas_width, y),
        'left', text_style)

    horizontal_length -= 40
    y = 100
    poly = [
        (canvas_width / 2 + 20, y),
        (canvas_width / 2 + horizontal_length, y),
        (canvas_width / 2 + horizontal_length, y + 30)]
    line_style.join = 'round'
    painter.draw_polygon(poly, line_style)

    painter.draw_text(
        [str(line_style.join)], (0.58 * canvas_width, y),
        'left', text_style)

    y = 140
    horizontal_length -= 40
    poly = [
        (canvas_width / 2 + 20, y),
        (canvas_width / 2 + horizontal_length, y),
        (canvas_width / 2 + horizontal_length, y + 30)]
    line_style.join = 'bevel'
    painter.draw_polygon(poly, line_style)

    painter.draw_text(
        [str(line_style.join)], (0.58 * canvas_width, y),
        'left', text_style)

    ### Dash patterns
    text_style.color = (0.3, 0.3, 0.3)
    line_style.width = 5
    
    y = 230
    line_style.dash_pattern = [20]
    painter.draw_line((30, y), (canvas_width - 30, y), line_style)
    
    painter.draw_text(
        [_dash2str(line_style.dash_pattern)], (canvas_width / 5, y),
        anchor='top-left', text_style=text_style, padding=(0, 5))

    y = 275
    line_style.dash_pattern = [40, 20]
    painter.draw_line((30, y), (canvas_width - 30, y), line_style)
    painter.draw_text(
        [_dash2str(line_style.dash_pattern)], (canvas_width / 5, y),
        anchor='top-left', text_style=text_style, padding=(0, 5))
    
    y = 320
    line_style.dash_pattern = [10, 10, 40]
    painter.draw_line((30, y), (canvas_width - 30, y), line_style)
    painter.draw_text(
        [_dash2str(line_style.dash_pattern)], (canvas_width / 5, y),
        anchor='top-left', text_style=text_style, padding=(0, 5))
    
    y = 365
    line_style.dash_pattern = [10, 10, 40, 10]
    painter.draw_line((30, y), (canvas_width - 30, y), line_style)
    painter.draw_text(
        [_dash2str(line_style.dash_pattern)], (canvas_width / 5, y),
        anchor='top-left', text_style=text_style, padding=(0, 5))

    y = 405
    line_style.dash_offset = 45
    painter.draw_line((30, y), (canvas_width - 30, y), line_style)
    painter.draw_text(
        [_dash2str(line_style.dash_pattern)
         + f', with {int(line_style.dash_offset)}px offset'],
        (canvas_width / 5, y),
        anchor='top-left', text_style=text_style, padding=(0, 5))

    # Return the visualization as a NumPy buffer (let NumPy take care of
    # the memory copy):
    shared_canvas = painter.get_canvas(copy=False)
    return np.array(shared_canvas, copy=True)


def cheat_sheet_arrowstyle():
     # Set up empty canvas:
    painter = viren2d.Painter()
    canvas_width = 600
    canvas_height = 220
    painter.set_canvas_rgb(
        width=canvas_width, height=canvas_height, color='white!0')

   # Style specifications:
    text_style = viren2d.TextStyle(
        family='xkcd', size=18, color=(0.3, 0.3, 0.3),
        alignment=viren2d.HorizontalAlignment.Center,
        line_spacing=1.1)
    y_txt = 110  # Same vertical anchor for all text

    arrow_style = viren2d.ArrowStyle(
        width=5, color='navy-blue')

    # 1st pair of arrows (leftmost)
    offset_x = 60  # Distance between the 2 arrows of each "pair"
    x1 = 50
    arrow_style.join = 'miter'
    arrow_style.tip_length = 0.3
    txt = f'{arrow_style.tip_length:0.1f}'
    painter.draw_arrow((x1, y_txt - 10), (x1, 5), arrow_style)
    
    arrow_style.tip_length = 0.5
    txt += f' & {arrow_style.tip_length:0.1f},'
    painter.draw_arrow((x1 + offset_x, 100), (x1 + offset_x, 5), arrow_style)
    
    painter.draw_text(
        [f'Angle: {int(arrow_style.tip_angle):d}°,',
        'Lengths: ' + txt, 'Round Cap,', 'Open'],
        (x1 + offset_x / 2, y_txt), 'north', text_style)
    
    # 2nd pair of arrows
    x1 = 195
    arrow_style.color = 'tealgreen'
    arrow_style.tip_length = 0.4
    arrow_style.tip_closed = True
    arrow_style.tip_angle = 10
    txt = f'{int(arrow_style.tip_angle)}°'
    painter.draw_arrow((x1, y_txt - 10), (x1, 5), arrow_style)
    
    arrow_style.tip_angle = 40
    txt += f' & {int(arrow_style.tip_angle)}°,'
    painter.draw_arrow((x1 + offset_x, 100), (x1 + offset_x, 5), arrow_style)

    painter.draw_text(
        ['Angle:', txt, f'Length: {arrow_style.tip_length:.1f},',
         'Closed'],
        (x1 + offset_x / 2, y_txt), 'north', text_style)

    # 3rd pair of arrows
    x1 = 345
    arrow_style.color = 'maroon'
    arrow_style.tip_closed = False
    arrow_style.join = 'round'
    arrow_style.tip_length = 0.8
    arrow_style.tip_angle = 12
    painter.draw_arrow((x1, y_txt - 10), (x1, 5), arrow_style)
    
    arrow_style.tip_closed = True
    painter.draw_arrow((x1 + offset_x, 100), (x1 + offset_x, 5), arrow_style)
    
    painter.draw_text(
        [f'Length: {arrow_style.tip_length:0.1f},', 
         f'Angle: {int(arrow_style.tip_angle):d}°,',
          'Round Join,', 'Open & Closed'],
        (x1 + offset_x / 2, y_txt), 'north', text_style)

    # 4th pair of arrows
    x1 = 495
    arrow_style.color = 'indigo'
    arrow_style.dash_pattern = [20, 15]
    txt_dash = _dash2str(arrow_style.dash_pattern)
    arrow_style.tip_closed = False
    arrow_style.tip_angle = 20
    arrow_style.tip_length = 0.4
    painter.draw_arrow((x1, y_txt - 10), (x1, 5), arrow_style)
    
    arrow_style.tip_closed = True
    arrow_style.dash_pattern = [15]
    txt_dash += ' & ' + _dash2str(arrow_style.dash_pattern)
    painter.draw_arrow((x1 + offset_x, 100), (x1 + offset_x, 5), arrow_style)

    painter.draw_text(
        [f'Length: {arrow_style.tip_length:0.2f},', 
         f'Angle: {int(arrow_style.tip_angle):d}°,',
         'Dash Patterns:', txt_dash],
        (x1 + offset_x / 2, y_txt), 'north', text_style)
        
    # Return the visualization as a NumPy buffer (let NumPy take care of
    # the memory copy):
    shared_canvas = painter.get_canvas(copy=False)
    return np.array(shared_canvas, copy=True)



def cheat_sheet_colormaps():
    cmap_categories = {
        'sequential': [
            'cividis', 'cvd-sequential', 'cvd-sequential-vivid', 'gouldian',
            'viridis', 'inferno', 'hell', 'thermal', 'black-body', 'hot',
            'cold', 'water', 'copper', 'spring', 'summer', 'autumn', 'winter',
            'gray', 'yarg'
        ],
        'cyclic': [
            'cvd-orientation', 'orientation-4', 'orientation-6', 'hsv',
            'optical-flow', 'twilight', 'twilight-shifted'
        ],
        'categorical': [
            'categories-10', 'categories-12', 'categories-20', 'glasbey-dark',
            'glasbey-light'
        ],
        'color-blind': [
            'cividis', 'cvd-sequential', 'cvd-sequential-vivid',
            'cvd-diverging', 'cvd-orientation'
        ],
        'diverging': [
            'cvd-diverging', 'seismic', 'temperature',
            'temperature-dark', 'spectral-diverging',
            
        ],
        'rainbow': [
            'jet', 'rainbow', 'turbo'
        ],
        'specialized': [
            'ocean', 'water',  'earth', 'terrain', 'relief',
            'relief-low-contrast', 'thermal', 'spectral', 'disparity',
            'optical-flow'
        ]
    }

    rtd_background_color = viren2d.RGBa(26, 28, 29)
    rtd_text_color = viren2d.RGBa(192, 186, 177)

    text_style = viren2d.TextStyle(
        family='xkcd', size=18, color=rtd_text_color, alignment='right')

    painter = viren2d.Painter()
    canvas_width = 480
    row_height = 40

    # Gradient image from 0..255
    row = np.array([i for i in range(256)]).astype(np.uint8).flatten()
    data = np.repeat(row.reshape((1,-1)), row_height - 5, axis=0)

    sheets = list()
    for cat in cmap_categories:
        canvas_height = (row_height + 5) * len(cmap_categories[cat]) + 5
        painter.set_canvas_rgb(
            width=canvas_width, height=canvas_height, color="white!0")

        y = 5
        for cmap in cmap_categories[cat]:
            vis = viren2d.colorize(data, colormap=cmap, low=0, high=255)

            painter.draw_image(
                vis, position=(canvas_width-5, y + row_height / 2),
                anchor='right', clip_factor=0.3)

            x = canvas_width - vis.width - 15
            painter.draw_text_box(
                [cmap], position=(x, y), anchor='topright', text_style=text_style,
                line_style=viren2d.LineStyle.Invalid, fill_color=rtd_background_color,
                fixed_size=(x - 5, row_height - 5), padding=(10, 5), radius=0.3)

            y += row_height + 5

        sheets.append((cat, np.array(painter.canvas, copy=True)))

    return sheets
