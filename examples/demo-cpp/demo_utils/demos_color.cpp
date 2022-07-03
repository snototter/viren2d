#include <demo_utils/demos.h>

namespace viren2d {
namespace demos {

  ////Vec3d rgb2hsv(const Color &rgb) {
  ////  auto rgbvec = 255 * Vec3d(rgb.red, rgb.green, rgb.blue);
  ////  double max = rgbvec.MaxValue();
  ////  double diff = max - rgbvec.MinValue();

  ////  double saturation = (max > 0.0) ? (100.0 * diff / max) : 0.0;
  ////  const double r = rgbvec[0];
  ////  const double g = rgbvec[1];
  ////  const double b = rgbvec[2];
  ////  double hue;
  ////  if (saturation > 0.0) {
  ////    if (r == max) {
  ////      hue = 60.0 * (g-b) / diff;
  ////    } else if (g == max) {
  ////      hue = 120.0 + 60.0 * (b-r) / diff;
  ////    } else {
  ////      hue = 240.0 + 60.0 * (r-g) / diff;
  ////    }
  ////  } else {
  ////    hue = 0.0;
  ////  }

  ////  if (hue < 0.0) {
  ////    hue += 360.0;
  ////  }

  ////  return Vec3d(std::round(hue),
  ////                        std::round(saturation),
  ////                        std::round(max * 100.0/255.0));
  ////}


  ////double hsv_dist(Vec3d hsv) {
  ////  // ref hsv
  ////  double h1 = 0.0;
  ////  double s1 = 1.0;
  ////  double v1 = 0.5;
  ////  // https://math.stackexchange.com/a/4018304
  ////  double h2 = hsv[0];
  ////  double s2 = hsv[1] / 100.0;
  ////  double v2 = hsv[2] / 100.0;
  ////  double dist = std::sqrt((v2 - v1) * (v2 - v1)
  ////            + (s1*s1 * v1*v1) + (s2*s2 * v2*v2)
  ////            - 2 * s1 * s2 * v1 * v2 * std::cos(werkzeugkiste::geometry::deg2rad(h2 - h1)));

  ////  std::cout << "FOOOOOOOOO " << hsv << ", " << dist << " T1: " << ((v2 - v1) * (v2 - v1)
  ////                                                                   + (s1*s1 * v1*v1) + (s2*s2 * v2*v2)) << std::endl;
  ////  return dist;
  ////}


  //// TODO check resources linked at https://stackoverflow.com/a/35114586
  //void DemoColors() {
  //  auto painter = CreatePainter();
  //  const int canvas_width = 750;
  //  painter->SetCanvas(448, canvas_width,
  //                     Color::White.WithAlpha(0.0));

  //  auto text_style = TextStyle(18, "xkcd",
  //                                       Color::Black,
  //                                       false, false, 1.0,
  //                                       HorizontalAlignment::Center);
  //  auto line_style = LineStyle(1, Color::Black);
  ////FIXME check horz alignment: single line vs multi-line text!!

  //  const Vec2d box_size{130.0, 36.0};
  //  const double box_spacing = 20.0;
  //  const double left = box_size.width() / 2.0 + box_spacing / 2.0;
  //  double x = left;
  //  double y = box_spacing / 2.0;

  //  for (const auto &color_name : ListNamedColors()) {
  //    auto color = Color(color_name);

  //    text_style.color = color.Grayscale().Inverse();
  //    painter->DrawTextBox({color_name}, {x, y},
  //                         Anchor::Top,
  //                         text_style, {6, 6}, 0,
  //                         line_style, //LineStyle::Invalid,
  //                         Color(color_name),
  //                         0.3, box_size);

  //    x += box_size.width() + box_spacing;
  //    if ((x + box_size.width() / 2.0 + box_spacing / 2.0) > canvas_width) {
  //      x = left;
  //      y += box_size.height() + box_spacing;
  //    }
  //  }

  //  ShowCanvas(painter->GetCanvas(false), "demo-output-colors.png");
  //  painter.reset();
  //}
} // namespace demos
} // namespace viren2d
