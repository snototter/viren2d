#include <ostream>
#include <iomanip>
#include <exception>

#include <viren2d/colorgradients.h>
#include <viren2d/drawing.h>

#include <helpers/logging.h>


//TODOs:
// * implement ImageBuffer.blend(other, mask)
// * bind blend(other, mask)
// * doc & clean up ColorGradient
// * implement ColorGradient bindings
// * implement ImageBuffer blend bindings
// * rtd doc ColorGradient
// * rtd doc Clipping
// * rtd doc ImageBuffer blending
// * explicitly log error before each exception
// * replace all stringstreams by ostringstreams
// * replace 's' by 'msg'/'err',...
// * add python wrappers (functions), e.g. blend(a, b) --> return ImageBuffer(a).blend(b)
// * move all python binding helpers in corresponding namespaces


namespace viren2d {
bool ColorGradient::AddColorStop(double offset, const Color &color) {
  if ((offset < 0.0) || (offset > 1.0)) {
    SPDLOG_WARN(
          "Ignoring invalid offset {:f}. Only values in [0, 1] are accepted.",
          offset);
    return false;
  }

  color_stops.push_back(std::make_pair(offset, color));
  return true;
}


bool ColorGradient::IsValid() const {
  return color_stops.size() > 1;
}


std::string ColorGradient::ToString() const {
  std::ostringstream s;
  s << "ColorGradient(" << color_stops.size() << " color "
    << ((color_stops.size() == 1) ? "stop" : "stops")
    << ')';
  return s.str();
}


ImageBuffer ColorGradient::Visualization(
    int width, int height, int channels,
    const Color &background_color) const {
  auto painter = CreatePainter();
  painter->SetCanvas(height, width, background_color);

  painter->DrawGradient(*this);

  if ((channels == 3) || (channels == 4)) {
    return painter->GetCanvas(false).ToChannels(channels);
  } else {
    std::ostringstream s;
    s << "Invalid number of output channels requested ("
      << channels << "), only 3 or 4 are supported!";
    throw std::invalid_argument(s.str());
  }
}


ImageBuffer ColorGradient::Mask(
    int width, int height, int channels,
    const Color &background_color) const {
  auto painter = CreatePainter();
  painter->SetCanvas(height, width, background_color);

  painter->DrawGradient(*this);

  if ((channels == 3) || (channels == 4)) {
    return painter->GetCanvas(false).ToChannels(channels).AsType(
        ImageBufferType::Double, 1.0/255.0);
  } else if (channels == 1) {
    return painter->GetCanvas(false).Channel(0).AsType(
        ImageBufferType::Double, 1.0/255.0);
  } else {
    std::ostringstream s;
    s << "Invalid number of output channels requested ("
      << channels << "), only 3 or 4 are supported!";
    throw std::invalid_argument(s.str());
  }
}


std::string LinearColorGradient::ToString() const {
  std::ostringstream s;
  s << "LinearColorGradient(" << color_stops.size() << " color "
    << ((color_stops.size() == 1) ? "stop" : "stops")
    << ", " << start_point << " to " << end_point << ')';
  return s.str();
}


std::string RadialColorGradient::ToString() const {
  std::ostringstream s;
  s << "RadialColorGradient(" << color_stops.size() << " color "
    << ((color_stops.size() == 1) ? "stop" : "stops")
    << ", " << start_center << ", r=" << std::fixed << std::setprecision(1)
    << start_radius << " to " << end_center << ", r=" << end_radius << ')';
  return s.str();
}

} // namespace viren2d
