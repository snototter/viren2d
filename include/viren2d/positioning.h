#ifndef __VIREN2D_POSITIONING_H__
#define __VIREN2D_POSITIONING_H__

#include <vector>
#include <string>
#include <ostream>
#include <stdexcept>


namespace viren2d {

/// Available options for horizontal alignment.
enum class HorizontalAlignment : unsigned char {
  Left   = 1,
  Center = 1 << 1,
  Right  = 1 << 2
};

//TODO doc
HorizontalAlignment HorizontalAlignmentFromString(const std::string &align);
std::string HorizontalAlignmentToString(HorizontalAlignment align);
std::ostream &operator<<(std::ostream &os, HorizontalAlignment align);


/// Available options for vertical alignment.
enum class VerticalAlignment : unsigned char {
  Top    = 1 << 3,
  Center = 1 << 4,
  Bottom = 1 << 5
};

//TODO doc
VerticalAlignment VerticalAlignmentFromString(const std::string &align);
std::string VerticalAlignmentToString(VerticalAlignment align);
std::ostream &operator<<(std::ostream &os, VerticalAlignment align);


/// Internal macro to reuse the vertical/horizontal anchors in
/// the following position/anchor enum definitions
#define __ALIGNMENT(HORZ, VERT) \
  static_cast<unsigned char>(HORZ) \
    | static_cast<unsigned char>(VERT)


// TODO doc & test
// public use: prefer Center, Left, ... over combining Horz & Vert
enum class Anchor : unsigned char {
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


/// Builds an Anchor enum from its horizontal and vertical components.
Anchor operator|(HorizontalAlignment lhs, VerticalAlignment rhs);


/// Builds an Anchor enum from its vertical and horizontal components.
Anchor operator|(VerticalAlignment lhs, HorizontalAlignment rhs);


/// Parses a string representation of an Anchor.
Anchor AnchorFromString(const std::string &anchor);

//TODO doc
std::string AnchorToString(Anchor anchor);
std::ostream &operator<<(std::ostream &os, Anchor anchor);


/// Returns a list of all defined anchors.
std::vector<Anchor> ListAnchors();


/// Returns the horizontal component of the given anchor.
inline HorizontalAlignment HorizontalAlignmentFromAnchor(Anchor anchor) {
  switch(anchor) {
    case Anchor::TopLeft:
    case Anchor::Left:
    case Anchor::BottomLeft:
      return HorizontalAlignment::Left;

    case Anchor::Top:
    case Anchor::Center:
    case Anchor::Bottom:
      return HorizontalAlignment::Center;

    case Anchor::TopRight:
    case Anchor::Right:
    case Anchor::BottomRight:
      return HorizontalAlignment::Right;
  }
  // Ending up here would be a library issue/development error:
  std::string msg("Anchor \"");
  msg += AnchorToString(anchor);
  msg += "\" not handled in HorizontalAlignmentFromAnchor switch.";
  throw std::logic_error(msg);
}


/// Returns the vertical component of the given anchor.
inline VerticalAlignment VerticalAlignmentFromAnchor(Anchor anchor) {
  switch(anchor) {
    case Anchor::TopLeft:
    case Anchor::Top:
    case Anchor::TopRight:
      return VerticalAlignment::Top;

    case Anchor::Left:
    case Anchor::Center:
    case Anchor::Right:
      return VerticalAlignment::Center;

    case Anchor::BottomLeft:
    case Anchor::Bottom:
    case Anchor::BottomRight:
      return VerticalAlignment::Bottom;
  }

  // Ending up here would be a library issue/development error:
  std::string msg("Anchor \"");
  msg += AnchorToString(anchor);
  msg += "\" not handled in HorizontalAlignmentFromAnchor switch.";
  throw std::logic_error(msg);
}


//TODO doc
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



LabelPosition LabelPositionFromString(const std::string &pos);
std::string LabelPositionToString(LabelPosition pos);
std::ostream &operator<<(std::ostream &os, LabelPosition pos);

} // namespace viren2d

#endif // __VIREN2D_POSITIONING_H__

