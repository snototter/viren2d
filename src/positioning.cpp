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

  std::ostringstream s;
  s << "Could not deduce HorizontalAlignment from string \""
    << align << "\".";
  throw std::invalid_argument(s.str());
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

  std::ostringstream s;
  s << "Could not deduce VerticalAlignment from string \""
    << align << "\".";
  throw std::invalid_argument(s.str());
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


TextAnchor operator|(HorizontalAlignment lhs, VerticalAlignment rhs) {
  static_assert(std::is_same<typename std::underlying_type<TextAnchor>::type,
                             typename std::underlying_type<HorizontalAlignment>::type>::value,
                "Underlying type of TextAnchor and HorizontalAlignment must be the same");
  static_assert(std::is_same<typename std::underlying_type<TextAnchor>::type,
                             typename std::underlying_type<VerticalAlignment>::type>::value,
                "Underlying type of TextAnchor and VerticalAlignment must be the same");
  using underlying = typename std::underlying_type<TextAnchor>::type;
  return static_cast<TextAnchor> (
    static_cast<underlying>(lhs) | static_cast<underlying>(rhs)
  );
}


TextAnchor operator|(VerticalAlignment lhs, HorizontalAlignment rhs) {
  return rhs | lhs;
}


TextAnchor TextAnchorFromString(const std::string &anchor) {
  std::string slug = werkzeugkiste::strings::Lower(anchor);
  slug.erase(std::remove_if(slug.begin(), slug.end(), [](char ch) -> bool {
      return ::isspace(ch) || (ch == '-') || (ch == '_');
    }), slug.end());

  // We support:
  // * Standard corner terminology (bottom right, top, center, ...)
  // * Eight principal compass directions (south, north-west, east, ...)
  //TODO support abbreviations? n,e,s,w; t,r,b,l;
  if (slug.compare("center") == 0) {
    return TextAnchor::Center;
  } else if ((slug.compare("right") == 0)
             || (slug.compare("east") == 0)) {
    return TextAnchor::Right;
  } else if ((slug.compare("bottomright") == 0)
             || (slug.compare("southeast") == 0)) {
    return TextAnchor::BottomRight;
  } else if ((slug.compare("bottom") == 0)
             || (slug.compare("south") == 0)) {
    return TextAnchor::Bottom;
  } else if ((slug.compare("bottomleft") == 0)
             || (slug.compare("southwest") == 0)) {
    return TextAnchor::BottomLeft;
  } else if ((slug.compare("left") == 0)
             || (slug.compare("west") == 0)) {
    return TextAnchor::Left;
  } else if ((slug.compare("topleft") == 0)
             || (slug.compare("northwest") == 0)) {
    return TextAnchor::TopLeft;
  } else if ((slug.compare("top") == 0)
             || (slug.compare("north") == 0)) {
    return TextAnchor::Top;
  } else if ((slug.compare("topright") == 0)
             || (slug.compare("northeast") == 0)) {
    return TextAnchor::TopRight;
  }

  std::ostringstream s;
  s << "Could not deduce TextAnchor from string \""
    << anchor << "\".";
  throw std::invalid_argument(s.str());
}


std::string TextAnchorToString(TextAnchor anchor) {
  switch (anchor) {
    case TextAnchor::Center:
      return "center";

    case TextAnchor::Top:
      return "top";

    case TextAnchor::TopRight:
      return "top-right";

    case TextAnchor::Right:
      return "right";

    case TextAnchor::BottomRight:
      return "bottom-right";

    case TextAnchor::Bottom:
      return "bottom";

    case TextAnchor::BottomLeft:
      return "bottom-left";

    case TextAnchor::Left:
      return "left";

    case TextAnchor::TopLeft:
      return "top-left";

    default:
      throw std::invalid_argument("Incomplete text anchor: you must specify both horizontal & vertical alignment");
  }
}


std::ostream &operator<<(std::ostream &os, TextAnchor anchor) {
  os << TextAnchorToString(anchor);
  return os;
}



BoundingBoxLabelPosition BoundingBoxLabelPositionFromString(const std::string &pos) {
  std::string slug = werkzeugkiste::strings::Lower(pos);
  slug.erase(std::remove_if(slug.begin(), slug.end(), [](char ch) -> bool {
      return ::isspace(ch) || (ch == '-') || (ch == '_');
    }), slug.end());

  if (slug.compare("top") == 0) {
    return BoundingBoxLabelPosition::Top;
  } else if (slug.compare("bottom") == 0) {
    return BoundingBoxLabelPosition::Bottom;
  } else if ((slug.compare("left") == 0)
             || (slug.compare("leftb2t") == 0)) {
    return BoundingBoxLabelPosition::LeftB2T;
  } else if (slug.compare("leftt2b") == 0) {
    return BoundingBoxLabelPosition::LeftT2B;
  } else if ((slug.compare("right") == 0)
             || (slug.compare("rightt2b") == 0)) {
    return BoundingBoxLabelPosition::RightT2B;
  } else if (slug.compare("rightb2t") == 0) {
    return BoundingBoxLabelPosition::RightB2T;
  }

  std::ostringstream s;
  s << "Could not deduce BoundingBoxLabelPosition from string \""
    << pos << "\".";
  throw std::invalid_argument(s.str());
}


std::string BoundingBoxLabelPositionToString(BoundingBoxLabelPosition pos) {
  switch (pos) {
    case BoundingBoxLabelPosition::Top:
      return "top";

    case BoundingBoxLabelPosition::Bottom:
      return "bottom";

    case BoundingBoxLabelPosition::LeftB2T:
      return "left-b2t";

    case BoundingBoxLabelPosition::LeftT2B:
      return "left-t2b";

    case BoundingBoxLabelPosition::RightB2T:
      return "right-b2t";

    case BoundingBoxLabelPosition::RightT2B:
      return "right-t2b";

    default: {
        std::ostringstream s;
        s << "BoundingBoxLabelPosition value ("
          << static_cast<int>(pos)
          << ") has not been mapped to string representation!";
        throw std::invalid_argument(s.str());
      }
  }
}


std::ostream &operator<<(std::ostream &os, BoundingBoxLabelPosition pos) {
  os << BoundingBoxLabelPositionToString(pos);
  return os;
}

} // namespace viren2d
