# import numpy as np
# import viren2d
# from PIL import Image
# from typing import List, Tuple, Union


# class BoundingBoxPainter(object):
#     def __init__(self):
#         self._painter = viren2d.Painter()
#         # Rounded corners for each bounding box
#         self._corner_radius = 0.2
#         # Default box style
#         line_style = viren2d.LineStyle(
#             width=3, 
#         )
#         text_style = viren2d.TextStyle()
#         self._box_style = viren2d.BoundingBox2DStyle(
#             line_style=line_style, text_style=text_style,
#             box_fill_color='same!20', text_fill_color='white!40',
#             label_position='top', label_padding=(6, 6),
#             clip_label=False)

#     def draw(
#             self, image: np.ndarray, boxes: List[Tuple[Union[int, str], float,
#             float, float, float, float]], copy: bool = False) -> np.ndarray:
#         self._painter.set_canvas_image(image)
#         for obj_class, left, top, width, height, score in boxes:
#             lbl = '{:s}: {:.2f}'.format(
#                 obj_class if isinstance(obj_class, str) else str(int(obj_class)), score)
#             color = (
#                 viren2d.color_from_object_category(obj_class) if isinstance(obj_class, str)
#                 else viren2d.color_from_object_id(int(obj_class)))
#             self._box_style.line_style.color = color
#             self._painter.draw_bounding_box_2d(
#                 viren2d.Rect.from_ltwh(
#                     left, top, width, height, self._corner_radius),
#                 label=[lbl], box_style=self._box_style)
#         return self._painter.get_canvas(copy=copy)


# if __name__ == '__main__':
#     pu2d = BoundingBoxPainter()

#     width, height = 400, 300
#     img = 255 * np.ones((height, width, 3), dtype=np.uint8)

#     for _ in range(10):
#         n = np.random.randint(1, 15)
#         coords = np.random.rand(n, 6)
#         coords[:, 0] = np.floor(coords[:, 0] * 10)
#         coords[:, 1] *= width
#         coords[:, 2] *= height
#         coords[:, 3] *= (width / 2)
#         coords[:, 4] *= (height / 2)
#         vis = pu2d.draw(img, coords, copy=False)
#         image = Image.fromarray(np.array(vis, copy=False))
#         image.show()