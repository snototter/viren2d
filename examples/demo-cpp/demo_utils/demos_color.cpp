#include <viren2d/viren2d.h>
#include <demo_utils/demos.h>

namespace viren2d {
namespace demos {

void DemoColorMaps() {
  PrintDemoHeader("Color maps");

  auto painter = CreatePainter();
  painter->SetCanvas(520, 1040, Color::White);

  LineStyle line_style(3, Color::Black);

  ImageBuffer peaks = Peaks(500, 500);

  // For colorizing only a single/few image(s), you should use
  // `ColorizeScaled` instead of the `StreamColorizer`.
  // This class is useful, if you have to apply the same colorization
  // over and over again (e.g. when streaming from a ToF sensor).
  StreamColorizer pseudocolor(ColorMap::Inferno);

  ImageBuffer vis = pseudocolor(peaks);
  painter->DrawImage(vis, {10, 10}, Anchor::TopLeft, 1.0, 1.0, 1.0, 0.0, 0.2, line_style);

  pseudocolor.SetBins(10);
  vis = pseudocolor(peaks);
  painter->DrawImage(vis, {1030, 10}, Anchor::TopRight, 1.0, 1.0, 1.0, 0.0, 0.2, line_style);

  ProcessDemoOutput(painter->GetCanvas(false), "demo-output-color-maps.png");
  painter.reset();
}


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


void DemoColorGradients() {
  PrintDemoHeader("Color gradients");

  LinearColorGradient gradient_linear({0, 0}, {600, 200});
  gradient_linear.AddColorStop(0.1, "crimson");
  gradient_linear.AddColorStop(0.3, "navy-blue");
  gradient_linear.AddColorStop(0.5, "crimson");
  gradient_linear.AddColorStop(0.7, "navy-blue");
  gradient_linear.AddColorStop(0.9, "crimson");

  auto grad1 = DrawColorGradient(gradient_linear, 600, 200, 3);
  auto painter = CreatePainter();
  painter->SetCanvas(grad1.Height() * 2, grad1.Width(), "white");
  painter->DrawImage(grad1, {0, 0});

  const Rect clip_rect(
    grad1.Width() / 2, grad1.Height() * 1.5, grad1.Width(),
    grad1.Height() / 2, 10, 10);
  painter->SetClipRegion(clip_rect);

  LinearColorGradient gradient_linear2(
        {0, 0}, {clip_rect.width, 0.0});
  gradient_linear2.AddColorStop(0.0, "red");
  gradient_linear2.AddColorStop(0.25, "green");
  gradient_linear2.AddColorStop(0.5, "blue");
  gradient_linear2.AddColorStop(0.75, "cyan");
  gradient_linear2.AddColorStop(1.0, "purple");
  painter->DrawGradient(gradient_linear2);
  //painter->SetClipRegion({50.0, 50.0}, 100.0);

  ProcessDemoOutput(painter->GetCanvas(false),
        "demo-output-color-gradients.png");

  RadialColorGradient gradient_radial({50, 50}, 10, {50, 50}, 40);
  gradient_radial.AddColorStop(0.0, "black!100");
  gradient_radial.AddColorStop(0.8, "black!0");
  ProcessDemoOutput(
        DrawColorGradient(gradient_radial, 600, 200, 4),
        "demo-output-color-gradients.png");
}

} // namespace demos
} // namespace viren2d
