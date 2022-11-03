#include <ostream>
#include <iomanip>
#include <exception>

#include <viren2d/colorgradients.h>
#include <viren2d/drawing.h>

#include <helpers/logging.h>


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


ImageBuffer DrawColorGradient(
    const ColorGradient &gradient,
    int width, int height, int channels,
    const Color &background_color) {
  auto painter = CreatePainter();
  painter->SetCanvas(height, width, background_color);

  //TODO draw!!!
  painter->DrawGradient(gradient);

  //TODO doc return
  if (channels == 1) {
    return painter->GetCanvas(false).Channel(0);
  } else {
    if ((channels == 3) || (channels == 4)) {
      return painter->GetCanvas(false).ToChannels(channels);
    } else {
      std::ostringstream s;
      s << "Invalid number of output channels requested ("
        << channels << "), only 1, 3 & 4 are supported!";
      throw std::invalid_argument(s.str());
    }
  }
}

} // namespace viren2d
