import numpy as np
import viren2d
from vito import imvis, imutils


def demo_ninja():
    img = imutils.imread('ninja.jpg', mode='RGB')
    
    painter = viren2d.Painter()
    painter.set_canvas_image(img)
    
    line_style = viren2d.LineStyle(line_width=3, color='navy-blue!80')

    # Arrow
    arrow_style = viren2d.ArrowStyle(line_width=3, color='crimson', tip_length=0.3)
    painter.draw_arrow((210, 20), (240, 50), arrow_style)
    text_style = viren2d.TextStyle()
    text_style.alignment = viren2d.HorizontalAlignment.Right
    test_style.font_family = 'xkcd'
    painter.draw_text(['Mildly', 'infuriated'], position=(210, 20), anchor=viren2d.TextAnchor.Right, text_style=text_style, padding=(5, 5))
    
    # Tabi #TODO draw a bounding box instead
    rect_tabi = viren2d.Rect(center=(252, 148), size=(60, 30), rotation=15, radius=0.25)
    painter.draw_rect(rect_tabi, line_style, fill_color=line_style.color.with_alpha(0.3))
    
#        ((80, 68, 150, 90, -10), (0, 255, 255), 'Lens')],

    # Katana
    line_style.color = "teal-green"
    painter.draw_rect(viren2d.Rect.from_ltwh(177, 76, 147, 53, 0, 0.25), line_style, fill_color=line_style.color.with_alpha(0.3))
    
    img_np = np.array(painter.get_canvas(copy=False), copy=False)
    imvis.imshow(img_np)


if __name__ == '__main__':
    demo_ninja()

