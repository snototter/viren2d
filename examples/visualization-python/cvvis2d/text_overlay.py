from turtle import pos
from typing import Any, List, Union
import viren2d
import datetime


def frame_label(
      cam_label: str, frame_number: int = None,
      timestamp: datetime.datetime = None,
      num_digits: int = 5,
      include_date: bool = True) -> str:
    """
    Returns a default string representation to tag/label an image based on
    the current frame number (if not None) and timestamp (if not None).
    """
    lbl = cam_label
    if frame_number is not None:
        lbl += f' #{frame_number:0{num_digits}d}'

    if timestamp is not None:
        lbl += ', {:s}.{:03d}'.format(
            timestamp.strftime("%Y-%m-%d %H:%M:%S" if include_date else "%H:%M:%S"),
            int(timestamp.microsecond / 1e3))
    return lbl


class TextOverlay(object):
    """
    Draws text/a text box at a predefined location within the image.

    Can be used to overlay time, frame number, camera label, etc.

    TODO document parametrization
    """
    def __init__(self):
        self.text_style = viren2d.TextStyle(
            family='monospace', size=14, color='#1a1c1d',
            bold=False, italic=False)
        self.line_style = viren2d.LineStyle.Invalid
        self.fill_color = viren2d.Color(1, 1, 1, 0.8)
        self.position = viren2d.Vec2d(0.5, 10)
        self.anchor = viren2d.Anchor.Top
        self.padding = viren2d.Vec2d(5, 5)
        self.corner_radius = 0.2
    
    def apply(
            self, painter: viren2d.Painter,
            text: Union[str, List[str]]) -> bool:
        # If position is negative, we compute the position based on the
        # image/canvas dimension (i.e. going "back" from the opposite border).
        # If position is specified as a fraction of the image/canvas size,
        # we compute the absolute values.
        position = self.position
        if position[0] < 0:
            if position[0] >= -1.0:
                position[0] = painter.width + position[0] * painter.width
            else:
                position[0] = painter.width + position[0]
        elif position[0] <= 1.0:
            position[0] *= painter.width

        if position[1] < 0:
            if position[1] >= -1.0:
                position[1] = painter.height + position[1] * painter.height
            else:
                position[1] = painter.height + position[1]
        elif position[1] <= 1.0:
            position[1] *= painter.height

        return painter.draw_text_box(
            text=[text] if isinstance(text, str) else text,
            position=position, anchor=self.anchor,
            text_style=self.text_style, padding=self.padding, rotation=0,
            line_style=self.line_style, fill_color=self.fill_color,
            radius=self.corner_radius)


class StaticTextOverlay(TextOverlay):
    #TODO document (useful if your text never changes)
    def __init__(self):
        super().__init__()
        self.position = viren2d.Vec2d(-10, 10)
        self.anchor = viren2d.Anchor.TopRight
        self.text = 'Static Text'
    
    def apply(self, painter: viren2d.Painter) -> bool:
        return super().apply(painter, self.text)
