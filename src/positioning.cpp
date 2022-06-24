#include <algorithm>
#include <utility>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <cmath>

#include <werkzeugkiste/strings/strings.h>

#include <viren2d/positioning.h>

#include <helpers/enum.h>

//TODO logging


namespace viren2d {

HorizontalAlignment HorizontalAlignmentFromString(const std::string &align) {
  std::string slug = werkzeugkiste::strings::Lower(align);
  slug.erase(std::remove_if(slug.begin(), slug.end(), [](char ch) -> bool {
      return ::isspace(ch) || (ch == '-') || (ch == '_');
    }), slug.end());

  if ((slug.compare("left") == 0)
      || (slug.compare("west") == 0)) {
    return HorizontalAlignment::Left;
  } else if ((slug.compare("center") == 0)
             || (slug.compare("middle") == 0)) {
    return HorizontalAlignment::Center;
  } else if ((slug.compare("right") == 0)
             || (slug.compare("east") == 0)) {
    return HorizontalAlignment::Right;
  }

  std::string s("Could not deduce HorizontalAlignment from string \"");
  s += align;
  s += "\".";
  throw std::invalid_argument(s);
}


std::string HorizontalAlignmentToString(HorizontalAlignment align) {
  switch (align) {
    case HorizontalAlignment::Left:
      return "left";

    case HorizontalAlignment::Center:
      return "hcenter";

    case HorizontalAlignment::Right:
      return "right";

    default: {
        std::ostringstream s;
        s << "HorizontalAlignment value ("
          << static_cast<int>(align)
          << ") has not been mapped to string representation!";
        throw std::invalid_argument(s.str());
      }
  }
}


std::ostream &operator<<(std::ostream &os, HorizontalAlignment align) {
  os << HorizontalAlignmentToString(align);
  return os;
}


VerticalAlignment VerticalAlignmentFromString(const std::string &align) {
  std::string slug = werkzeugkiste::strings::Lower(align);
  slug.erase(std::remove_if(slug.begin(), slug.end(), [](char ch) -> bool {
      return ::isspace(ch) || (ch == '-') || (ch == '_');
    }), slug.end());

  if ((slug.compare("top") == 0)
      || (slug.compare("north") == 0)) {
    return VerticalAlignment::Top;
  } else if ((slug.compare("center") == 0)
             || (slug.compare("middle") == 0)) {
    return VerticalAlignment::Center;
  } else if ((slug.compare("bottom") == 0)
             || (slug.compare("south") == 0)) {
    return VerticalAlignment::Bottom;
  }

  std::string s("Could not deduce VerticalAlignment from string \"");
  s += align;
  s += "\".";
  throw std::invalid_argument(s);
}


std::string VerticalAlignmentToString(VerticalAlignment align) {
  switch (align) {
    case VerticalAlignment::Top:
      return "top";

    case VerticalAlignment::Center:
      return "vcenter";

    case VerticalAlignment::Bottom:
      return "bottom";

    default: {
        std::ostringstream s;
        s << "VerticalAlignment value ("
          << static_cast<int>(align)
          << ") has not been mapped to string representation!";
        throw std::invalid_argument(s.str());
      }
  }
}


std::ostream &operator<<(std::ostream &os, VerticalAlignment align) {
  os << VerticalAlignmentToString(align);
  return os;
}


Anchor operator|(HorizontalAlignment lhs, VerticalAlignment rhs) {
  static_assert(std::is_same<typename std::underlying_type<Anchor>::type,
                             typename std::underlying_type<HorizontalAlignment>::type>::value,
                "Underlying type of Anchor and HorizontalAlignment must be the same");
  static_assert(std::is_same<typename std::underlying_type<Anchor>::type,
                             typename std::underlying_type<VerticalAlignment>::type>::value,
                "Underlying type of Anchor and VerticalAlignment must be the same");
  using underlying = typename std::underlying_type<Anchor>::type;
  return static_cast<Anchor> (
    static_cast<underlying>(lhs) | static_cast<underlying>(rhs)
  );
}


Anchor operator|(VerticalAlignment lhs, HorizontalAlignment rhs) {
  return rhs | lhs;
}


Anchor AnchorFromString(const std::string &anchor) {
  std::string slug = werkzeugkiste::strings::Lower(anchor);
  slug.erase(std::remove_if(slug.begin(), slug.end(), [](char ch) -> bool {
      return ::isspace(ch) || (ch == '-') || (ch == '_');
    }), slug.end());

  // We support:
  // * Standard corner terminology (bottom right, top, center, ...)
  // * Eight principal compass directions (south, north-west, east, ...)
  //TODO support abbreviations? n,e,s,w; t,r,b,l;
  if (slug.compare("center") == 0) {
    return Anchor::Center;
  } else if ((slug.compare("right") == 0)
             || (slug.compare("east") == 0)) {
    return Anchor::Right;
  } else if ((slug.compare("bottomright") == 0)
             || (slug.compare("southeast") == 0)) {
    return Anchor::BottomRight;
  } else if ((slug.compare("bottom") == 0)
             || (slug.compare("south") == 0)) {
    return Anchor::Bottom;
  } else if ((slug.compare("bottomleft") == 0)
             || (slug.compare("southwest") == 0)) {
    return Anchor::BottomLeft;
  } else if ((slug.compare("left") == 0)
             || (slug.compare("west") == 0)) {
    return Anchor::Left;
  } else if ((slug.compare("topleft") == 0)
             || (slug.compare("northwest") == 0)) {
    return Anchor::TopLeft;
  } else if ((slug.compare("top") == 0)
             || (slug.compare("north") == 0)) {
    return Anchor::Top;
  } else if ((slug.compare("topright") == 0)
             || (slug.compare("northeast") == 0)) {
    return Anchor::TopRight;
  }

  std::string s("Could not deduce Anchor from string \"");
  s += anchor;
  s += "\".";
  throw std::invalid_argument(s);
}


std::string AnchorToString(Anchor anchor) {
  switch (anchor) {
    case Anchor::Center:
      return "center";

    case Anchor::Top:
      return "top";

    case Anchor::TopRight:
      return "top-right";

    case Anchor::Right:
      return "right";

    case Anchor::BottomRight:
      return "bottom-right";

    case Anchor::Bottom:
      return "bottom";

    case Anchor::BottomLeft:
      return "bottom-left";

    case Anchor::Left:
      return "left";

    case Anchor::TopLeft:
      return "top-left";

    default:
      throw std::invalid_argument(
            "Invalid/incomplete anchor: you must "
            "specify both the horizontal & vertical alignment!");
  }
}


std::ostream &operator<<(std::ostream &os, Anchor anchor) {
  os << AnchorToString(anchor);
  return os;
}


std::vector<Anchor> ListAnchors() {
  return {
    Anchor::TopLeft, Anchor::Top, Anchor::TopRight,
    Anchor::Left, Anchor::Center, Anchor::Right,
    Anchor::BottomLeft, Anchor::Bottom, Anchor::BottomRight
  };
}


LabelPosition LabelPositionFromString(const std::string &pos) {
  std::string slug = werkzeugkiste::strings::Lower(pos);
  slug.erase(std::remove_if(slug.begin(), slug.end(), [](char ch) -> bool {
      return ::isspace(ch) || (ch == '-') || (ch == '_');
    }), slug.end());

  if (slug.compare("top") == 0) {
    return LabelPosition::Top;
  } else if (slug.compare("bottom") == 0) {
    return LabelPosition::Bottom;
  } else if ((slug.compare("left") == 0)
             || (slug.compare("leftb2t") == 0)) {
    return LabelPosition::LeftB2T;
  } else if (slug.compare("leftt2b") == 0) {
    return LabelPosition::LeftT2B;
  } else if ((slug.compare("right") == 0)
             || (slug.compare("rightt2b") == 0)) {
    return LabelPosition::RightT2B;
  } else if (slug.compare("rightb2t") == 0) {
    return LabelPosition::RightB2T;
  }

  std::string s("Could not deduce LabelPosition from string \"");
  s += pos;
  s += "\".";
  throw std::invalid_argument(s);
}


std::string LabelPositionToString(LabelPosition pos) {
  switch (pos) {
    case LabelPosition::Top:
      return "top";

    case LabelPosition::Bottom:
      return "bottom";

    case LabelPosition::LeftB2T:
      return "left-b2t";

    case LabelPosition::LeftT2B:
      return "left-t2b";

    case LabelPosition::RightB2T:
      return "right-b2t";

    case LabelPosition::RightT2B:
      return "right-t2b";

    default: {
        std::ostringstream s;
        s << "LabelPosition value ("
          << static_cast<int>(pos)
          << ") has not been mapped to string representation!";
        throw std::invalid_argument(s.str());
      }
  }
}


std::ostream &operator<<(std::ostream &os, LabelPosition pos) {
  os << LabelPositionToString(pos);
  return os;
}

} // namespace viren2d
