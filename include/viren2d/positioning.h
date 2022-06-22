#ifndef __VIREN2D_POSITIONING_H__
#define __VIREN2D_POSITIONING_H__

#include <vector>
#include <string>
#include <ostream>
#include <initializer_list>

//TODO(snototter) add tasks and progress to all:
//TODO [ ] add documentation
//TODO [ ] add C++ test (tests/xxx_test.cpp)
//TODO [ ] add Python bindings
//TODO [ ] add Python test (tests/test_xxx.py)
//TODO [ ] add C++ demo
//TODO [ ] add Python demo


namespace viren2d {

//TODO bindings!!!!
//TODO to/from string!
enum class HorizontalAlignment : unsigned char {
  Left   = 1,
  Center = 1 << 1,
  Right  = 1 << 2
};

HorizontalAlignment HorizontalAlignmentFromString(const std::string &align);
std::string HorizontalAlignmentToString(HorizontalAlignment align);
std::ostream &operator<<(std::ostream &os, HorizontalAlignment align);

//TODO bindings!!!!
//TODO to/from string!
enum class VerticalAlignment : unsigned char {
  Top    = 1 << 3,
  Center = 1 << 4,
  Bottom = 1 << 5
};

VerticalAlignment VerticalAlignmentFromString(const std::string &align);
std::string VerticalAlignmentToString(VerticalAlignment align);
std::ostream &operator<<(std::ostream &os, VerticalAlignment align);

// Macro to reuse the vertical/horizontal anchors in
// the following position/anchor enum definitions
#define __ALIGNMENT(HORZ, VERT) \
  static_cast<unsigned char>(HORZ) \
    | static_cast<unsigned char>(VERT)

// only for draw_text
// TODO doc & test
// public use: prefer Center, Left, ... over combining Horz & Vert
enum class TextAnchor : unsigned char {
  Center = __ALIGNMENT(HorizontalAlignment::Center,
                       VerticalAlignment::Center),

  Left =   __ALIGNMENT(HorizontalAlignment::Left,
                       VerticalAlignment::Center),
  Right =  __ALIGNMENT(HorizontalAlignment::Right,
                       VerticalAlignment::Center),
  Top =    __ALIGNMENT(HorizontalAlignment::Center,
                       VerticalAlignment::Top),
  Bottom = __ALIGNMENT(HorizontalAlignment::Center,
                       VerticalAlignment::Bottom),

  TopLeft =     __ALIGNMENT(HorizontalAlignment::Left,
                            VerticalAlignment::Top),
  TopRight =    __ALIGNMENT(HorizontalAlignment::Right,
                            VerticalAlignment::Top),
  BottomLeft =  __ALIGNMENT(HorizontalAlignment::Left,
                            VerticalAlignment::Bottom),
  BottomRight = __ALIGNMENT(HorizontalAlignment::Right,
                            VerticalAlignment::Bottom)
};

TextAnchor operator|(HorizontalAlignment lhs, VerticalAlignment rhs);
TextAnchor operator|(VerticalAlignment lhs, HorizontalAlignment rhs);

//TODO doc, test, etc
//TODO bindings!!!!
TextAnchor TextAnchorFromString(const std::string &anchor);
std::string TextAnchorToString(TextAnchor anchor);
std::ostream &operator<<(std::ostream &os, TextAnchor anchor);
std::vector<TextAnchor> ListTextAnchors();


enum class LabelPosition : unsigned char {
  Top = 1,
  Bottom = 1 << 1,

  Right = 1 << 2,
  RightT2B = Right,
  RightB2T = 1 << 3,

  Left = 1 << 4,
  LeftB2T = Left,
  LeftT2B = 1 << 5
};

//TODO doc, test, etc
//TODO bindings!!!!
LabelPosition LabelPositionFromString(const std::string &pos);
std::string LabelPositionToString(LabelPosition pos);
std::ostream &operator<<(std::ostream &os, LabelPosition pos);

} // namespace viren2d

#endif // __VIREN2D_POSITIONING_H__

