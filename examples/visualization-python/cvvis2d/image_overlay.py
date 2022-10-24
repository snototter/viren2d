import viren2d


class ImageOverlay(object):
    """
    Draws an image at a predefined location within the canvas.

    Can be used to overlay depth maps, etc.

    TODO document parametrization
    """
    pass
    # def __init__(self):
    #     self.text_style = viren2d.TextStyle(
    #         family='monospace', size=14, color='#1a1c1d',
    #         bold=False, italic=False)
    #     self.line_style = viren2d.LineStyle.Invalid
    #     self.fill_color = viren2d.Color(1, 1, 1, 0.8)
    #     self.position = viren2d.Vec2d(0.5, 10)
    #     self.anchor = viren2d.Anchor.Top
    #     self.padding = viren2d.Vec2d(5, 5)
    #     self.corner_radius = 0.2
    
    # def apply(
    #         self, painter: viren2d.Painter,
    #         text: Union[str, List[str]]) -> bool:
    #     # If position is negative, we compute the position based on the
    #     # image/canvas dimension (i.e. going "back" from the opposite border).
    #     # If position is specified as a fraction of the image/canvas size,
    #     # we compute the absolute values.
    #     position = self.position
    #     if position[0] < 0:
    #         if position[0] >= -1.0:
    #             position[0] = painter.width + position[0] * painter.width
    #         else:
    #             position[0] = painter.width + position[0]
    #     elif position[0] <= 1.0:
    #         position[0] *= painter.width

    #     if position[1] < 0:
    #         if position[1] >= -1.0:
    #             position[1] = painter.height + position[1] * painter.height
    #         else:
    #             position[1] = painter.height + position[1]
    #     elif position[1] <= 1.0:
    #         position[1] *= painter.height

    #     return painter.draw_text_box(
    #         text=[text] if isinstance(text, str) else text,
    #         position=position, anchor=self.anchor,
    #         text_style=self.text_style, padding=self.padding, rotation=0,
    #         line_style=self.line_style, fill_color=self.fill_color,
    #         radius=self.corner_radius)