import numpy as np
import viren2d
from vito import imvis, imutils


def demo_ninja():
    img = imutils.imread('ninja.jpg', mode='RGB')
    
    painter = viren2d.Painter()
    painter.set_canvas_image(img)
    
    line_style = viren2d.LineStyle(line_width=3, color='navy-blue!80')
    
    text_style = viren2d.TextStyle()
    text_style.alignment = viren2d.HorizontalAlignment.Center
    text_style.font_family = 'xkcd'
    text_style.line_spacing = 1.0
    text_style.bold = True
    text_style.color = "crimson"
    
    
    # Tabi #TODO draw a bounding box instead
    bbox_style = viren2d.BoundingBox2DStyle()
    bbox_style.line_style = line_style
    bbox_style.line_style.color = "teal-green"
    bbox_style.text_style = text_style
    bbox_style.text_style.color = "carrot"
    bbox_style.text_style.font_size = 10
    bbox_style.box_fill_color = viren2d.Color.Same.with_alpha(0.3)
    bbox_style.text_fill_color = 'none'
    bbox_style.label_position = viren2d.label_position("left") #FIXME label positioning is wrong (not shown) for T2B alignments!
    bbox_style.clip_label = False
    bbox_style.label_padding = (0, 0)
    
    rect_tabi = viren2d.Rect(center=(252, 148), size=(60, 30), rotation=45, radius=0.25)
#    line_style.color = "teal-green"
#    painter.draw_rect(rect_tabi, line_style, fill_color=line_style.color.with_alpha(0.3))
    painter.draw_bbox2d(rect_tabi, ['Tabi'], bbox_style)
    
#        ((80, 68, 150, 90, -10), (0, 255, 255), 'Lens')],

    # Katana
#    line_style.color = "teal-green"
#    painter.draw_rect(viren2d.Rect.from_ltwh(177, 76, 147, 53, 0, 0.25), line_style, fill_color=line_style.color.with_alpha(0.3))


    # Trajectory
    traj_sword = [(323, 96), (321, 83), (317, 68), (310, 54), (305, 44), (294, 35),
        (283, 29), (273, 27), (261, 26), (246, 28), (231, 33), (217, 40), (207, 49),
        (201, 62), (196, 74), (192, 87), (183, 100), (175, 112), (159, 120), (144, 123),
        (128, 123), (115, 119)]
    line_style.line_width = 4
    line_style.line_cap = viren2d.LineCap.Round
    line_style.color = 'navy-blue'
    painter.draw_trajectory(traj_sword, line_style, "white!100")


    # Arrow
    arrow_style = viren2d.ArrowStyle(line_width=3, color='crimson', tip_length=0.3)
    painter.draw_arrow((210, 30), (240, 50), arrow_style)

    
    
    line_style.line_width = 2
    painter.draw_textbox(['Mildly', 'infuriated'], position=(210, 30), anchor=viren2d.TextAnchor.Right, text_style=text_style, padding=(5, 5), fill_color="white!60",
                         line_style=line_style)

    
    img_np = np.array(painter.get_canvas(copy=False), copy=False)
    imvis.imshow(img_np)


if __name__ == '__main__':
    demo_ninja()

