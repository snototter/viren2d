#include <iostream>
#include <cmath>

//#include <Eigen/Dense>

#ifdef WITH_OPENCV
#include <opencv2/opencv.hpp>
//#include <opencv2/core/eigen.hpp>
#include <opencv2/highgui.hpp>
#endif // WITH_OPENCV

#include <viren2d/viren2d.h>
#include <werkzeugkiste/container/sort.h>
#include <werkzeugkiste/geometry/utils.h>


/** Helper to show and save the canvas. */
void ShowCanvas(viren2d::ImageBuffer canvas, const std::string &filename) {
  // Save to disk
  if (!filename.empty()) {
    viren2d::SaveImage(filename, canvas);
    std::cout << "Canvas saved to '" << filename << "'." << std::endl;
  }

#ifdef WITH_OPENCV
  viren2d::ImageBuffer copy(canvas);
  copy.RGB2BGR(); // Warning: Currently, the buffer is shared!!
  cv::Mat cv_buffer(copy.height, copy.width,
                    CV_MAKETYPE(CV_8U, copy.channels),
                    copy.data, copy.stride);
  cv::imshow("Painter's Canvas", cv_buffer);
  cv::waitKey();
#else  // WITH_OPENCV
  std::cerr << "OpenCV is not available - cannot display the canvas." << std::endl;
#endif  // WITH_OPENCV
}



//viren2d::Vec3d rgb2hsv(const viren2d::Color &rgb) {
//  auto rgbvec = 255 * viren2d::Vec3d(rgb.red, rgb.green, rgb.blue);
//  double max = rgbvec.MaxValue();
//  double diff = max - rgbvec.MinValue();

//  double saturation = (max > 0.0) ? (100.0 * diff / max) : 0.0;
//  const double r = rgbvec[0];
//  const double g = rgbvec[1];
//  const double b = rgbvec[2];
//  double hue;
//  if (saturation > 0.0) {
//    if (r == max) {
//      hue = 60.0 * (g-b) / diff;
//    } else if (g == max) {
//      hue = 120.0 + 60.0 * (b-r) / diff;
//    } else {
//      hue = 240.0 + 60.0 * (r-g) / diff;
//    }
//  } else {
//    hue = 0.0;
//  }

//  if (hue < 0.0) {
//    hue += 360.0;
//  }

//  return viren2d::Vec3d(std::round(hue),
//                        std::round(saturation),
//                        std::round(max * 100.0/255.0));
//}


//double hsv_dist(viren2d::Vec3d hsv) {
//  // ref hsv
//  double h1 = 0.0;
//  double s1 = 1.0;
//  double v1 = 0.5;
//  // https://math.stackexchange.com/a/4018304
//  double h2 = hsv[0];
//  double s2 = hsv[1] / 100.0;
//  double v2 = hsv[2] / 100.0;
//  double dist = std::sqrt((v2 - v1) * (v2 - v1)
//            + (s1*s1 * v1*v1) + (s2*s2 * v2*v2)
//            - 2 * s1 * s2 * v1 * v2 * std::cos(werkzeugkiste::geometry::deg2rad(h2 - h1)));

//  std::cout << "FOOOOOOOOO " << hsv << ", " << dist << " T1: " << ((v2 - v1) * (v2 - v1)
//                                                                   + (s1*s1 * v1*v1) + (s2*s2 * v2*v2)) << std::endl;
//  return dist;
//}


// TODO check resources linked at https://stackoverflow.com/a/35114586
void DemoColors() {
  auto painter = viren2d::CreatePainter();
  const int canvas_width = 750;
  painter->SetCanvas(canvas_width, 448,
                     viren2d::Color::White.WithAlpha(0.0));

  auto text_style = viren2d::TextStyle(18, "xkcd",
                                       viren2d::Color::Black,
                                       false, false, 1.0,
                                       viren2d::HorizontalAlignment::Center);
  auto line_style = viren2d::LineStyle(1, viren2d::Color::Black);
//FIXME check horz alignment: single line vs multi-line text!!

  const viren2d::Vec2d box_size{130.0, 36.0};
  const double box_spacing = 20.0;
  const double left = box_size.width() / 2.0 + box_spacing / 2.0;
  double x = left;
  double y = box_spacing / 2.0;

  auto color_names = viren2d::ListNamedColors();
//  std::vector<double> dists;
//  for (const auto &color_name : color_names) {
//    auto color = viren2d::Color(color_name);
////    dists.push_back(color.blue + color.green * 2 + color.red * 3);
//    auto hsv = rgb2hsv(color);
////    dists.push_back(hsv[0]); // we'll sort by hue
//    dists.push_back(hsv_dist(hsv));
//  }
//  color_names = werkzeugkiste::container::SortByExternalKeys(color_names, dists);

  for (const auto &color_name : color_names) { //viren2d::ListNamedColors()) {
    auto color = viren2d::Color(color_name);

    text_style.font_color = color.Grayscale().Inverse();
    std::ostringstream s;
    s << color_name;
//    auto hsv = rgb2hsv(color);
//    s << std::setprecision(5) << hsv_dist(hsv);
    painter->DrawTextBox({s.str()}, {x, y},
                         viren2d::TextAnchor::Top,
                         text_style, {6, 6}, 0,
                         line_style, //viren2d::LineStyle::Invalid,
                         viren2d::Color(color_name),
                         0.3, box_size);

    x += box_size.width() + box_spacing;
    if ((x + box_size.width() / 2.0 + box_spacing / 2.0) > canvas_width) {
      x = left;
      y += box_size.height() + box_spacing;
    }
  }

  ShowCanvas(painter->GetCanvas(false), "demo-output-colors.png");
  painter.reset();
}


void DemoLines() {
  auto painter = viren2d::CreatePainter();
  std::ostringstream lbl;
  painter->SetCanvas(400, 400, viren2d::Color::White);

  painter->DrawGrid({}, {}, 50, 50, viren2d::LineStyle(1.0, "gray!50"));

  viren2d::Vec2d pt1 {50, 50};
  viren2d::Vec2d pt2 {150, 350};

  const auto rotation = werkzeugkiste::geometry::AngleDegFromDirectionVec(pt1.DirectionVector(pt2));
//  painter->SetDefaultTextStyle(viren2d::TextStyle(18, "Arial"));

  viren2d::LineStyle line_style(22, "azure!60", {},
                                viren2d::LineCap::Butt);
  painter->DrawLine({50.0, 50.0}, {150.0, 350.0}, line_style);
  lbl << "LineCap::" << viren2d::LineCapToString(line_style.line_cap);
  painter->DrawText({lbl.str()}, {100, 200}, viren2d::TextAnchor::Center,
                    viren2d::TextStyle(), {0, 0}, rotation);
  lbl.str(std::string());  // Reset stringstream


  line_style.line_cap = viren2d::LineCap::Round;
  painter->DrawLine({150.0, 50.0}, {250.0, 350}, line_style);
  lbl << "LineCap::" << viren2d::LineCapToString(line_style.line_cap);
  painter->DrawText({lbl.str()}, {200, 200}, viren2d::TextAnchor::Center,
                    viren2d::TextStyle(), {0, 0}, rotation);
  lbl.str(std::string());

  line_style.line_cap = viren2d::LineCap::Square;
  painter->DrawLine({250.0, 50.0}, {350.0, 350.0}, line_style);
  lbl << "LineCap::" << viren2d::LineCapToString(line_style.line_cap);
  painter->DrawText({lbl.str()}, {300, 200}, viren2d::TextAnchor::Center,
                    viren2d::TextStyle(), {0, 0}, rotation);
  lbl.str(std::string());


//  //FIXME fading line/trajectory
//  line_style.color = {0.0, 0.0, 0.8};
//  line_style.line_width = 1;
//  double step_length = 10;
//  int num_steps = static_cast<int>(painter->GetCanvasSize().width() / step_length);
//  for (int i = 0; i < num_steps; ++i) {
//    painter->DrawLine({0.0 + i * step_length, 10.0}, {50.0 + i * step_length, 10.0}, line_style);
//    line_style.line_width += 0.3;
//  }
//  std::cout << "final line style: " << line_style << std::endl; //TODO remove trajectory test

//  //FIXME trajectory test
//  ApplyColor(context, Color({0.0, 0.0, 0.8}));
//  cairo_move_to(context, 0, 30);
//  const double lw = 13;
//  cairo_set_line_width(context, 1);
//  cairo_line_to(context, 400, 30-lw/2.0);
//  cairo_line_to(context, 400, 30+lw/2.0);
//  cairo_close_path(context);
//  cairo_fill_preserve(context);
//  cairo_stroke(context);

  ShowCanvas(painter->GetCanvas(false), "demo-output-lines.png");
  painter.reset();
}


void DemoArrows() {
  auto painter = viren2d::CreatePainter();
  painter->SetCanvas(800, 800, viren2d::Color::White);

  painter->DrawGrid({}, {}, 50, 50,
                    viren2d::LineStyle(1.0, "gray!80"));

  auto style = viren2d::ArrowStyle(6, "navy-blue", 0.15, 20.0,
                                   true, false, {},
                                   viren2d::LineCap::Butt,
                                   viren2d::LineJoin::Round);

  auto size = painter->GetCanvasSize();
  viren2d::Vec2d center(size.x() / 2.0, size.y() / 2.0);
  const double radius = std::min(size.x(), size.y()) / 2.0 - 50;

  for (int angle = 0; angle < 360; angle += 15) {
    const double angle_rad = angle * M_PI / 180.0;
    auto tip = center + radius * viren2d::Vec2d(std::cos(angle_rad),
                                                std::sin(angle_rad));
    painter->DrawArrow(center, tip, style);
  }

  // Closed solid arrow (top-left)
  painter->DrawArrow({50, 50}, {200, 50},
                     viren2d::ArrowStyle(4, "forest-green", 0.15, 30, true, true));

  // Open solid arrow (top-right)
  painter->DrawArrow({size.x() - 50.0, 50.0}, {size.x() - 200.0, 50.0},
                     viren2d::ArrowStyle(4, "crimson!80", 0.15, 30, false, true));

  // Closed dashed arrow
  painter->DrawArrow({50, size.y() - 50.0}, {200, size.y() - 50.0},
                     viren2d::ArrowStyle(4, "forest-green", 0.15, 30, true, true, {15, 10}));

  // Open dashed arrow (bottom-right)
  painter->DrawArrow({size.x() - 50.0, size.y() - 50.0}, {size.x() - 200.0, size.y() - 50.0},
                     viren2d::ArrowStyle(4, "crimson!60", 0.15, 30, false, true,
                                         {15, 10}));

  ShowCanvas(painter->GetCanvas(false), "demo-output-arrows.png");
  painter.reset();
}


void DemoCircles() {
  auto painter = viren2d::CreatePainter();
  painter->SetCanvas(500, 500, viren2d::Color::White);

  painter->DrawGrid({}, {}, 50, 50,
                    viren2d::LineStyle(1.0, "gray!60"));

  auto style = viren2d::LineStyle(3, "navy-blue!90");

  viren2d::SetLogLevel("trace");
  painter->DrawCircle({100, 100}, 50, style, "red");

  style.dash_pattern = {20, 15};
  painter->DrawCircle({250, 100}, 50, style);

  style.dash_pattern = {};
  style.line_width = 0;
  auto fill = viren2d::Color("blue!40");
  painter->DrawCircle({400, 100}, 50, style, fill);

  style.line_width = 4;
  painter->DrawArc({100, 250}, 50,   0,  90, style, false);
  painter->DrawArc({100, 250}, 50, 110, 160, style, false);
  painter->DrawArc({100, 250}, 50, 180, 210, style, false);
  painter->DrawArc({100, 250}, 50, 230, 250, style, false);

  style.line_width = 2;
  style.dash_pattern = {10, 4};
  painter->DrawArc({250, 250}, 50,   0,  90, style);
  painter->DrawArc({250, 250}, 50, 110, 160, style);
  painter->DrawArc({250, 250}, 50, 180, 210, style);
  painter->DrawArc({250, 250}, 50, 230, 250, style);

  style.dash_pattern = {};
  style.line_width = 0;
  painter->DrawArc({400, 250}, 50, 0, 90, style, true, fill);
  painter->DrawArc({400, 250}, 50, 110, 160, style, true, fill);
  painter->DrawArc({400, 250}, 50, 180, 210, style, true, fill);
  painter->DrawArc({400, 250}, 50, 230, 250, style, true, fill);

  style.line_width = 3;
  painter->DrawEllipse({100, 400, 100, 50, 0, 45, -45}, style);
//  painter->DrawEllipse(viren2d::Ellipse({100, 400}, {100, 50}, 0, 45, -45), style);

  style.dash_pattern = {10, 10};
  painter->DrawEllipse(viren2d::Ellipse({250, 400}, {100, 50}, 45, 45, -45, false),
                       style, fill);

  style.dash_pattern = {};
  style.line_width = 0;
  painter->DrawEllipse(viren2d::Ellipse({400, 400}, {100, 50}, 180, 45, -45, true),
                       style, fill);

  ShowCanvas(painter->GetCanvas(false), "demo-output-circles.png");
  painter.reset();
}


void DemoRects() {
  auto painter = viren2d::CreatePainter();
  painter->SetCanvas(600, 600, viren2d::Color::White);

  painter->DrawGrid({}, {}, 50, 50,
                    viren2d::LineStyle(1.0, "gray!60"));

  auto style = viren2d::LineStyle(3, "navy-blue!90");

  auto rect = viren2d::Rect(100, 150, 100, 200);
  painter->DrawRect(rect, style, "light-blue!30");

  rect.cx += 150;
  rect.radius = 0.5;
  painter->DrawRect(rect, style);

  rect.cx += 200;
  rect.rotation = 45;
  painter->DrawRect(rect, style);

  rect.cx = 100;
  rect.cy += 300;
  rect.rotation = 10;
  rect.radius = 0.1;
  painter->DrawRect(rect, style, style.color.WithAlpha(0.4));

  rect.cx += 150;
  rect.rotation += 10;
  rect.radius = 30;
  painter->DrawRect(rect, style, style.color.WithAlpha(0.4));

  ShowCanvas(painter->GetCanvas(false), "demo-output-rects.png");
  painter.reset();
}


void DemoText() {
  auto painter = viren2d::CreatePainter();
  painter->SetCanvas(600, 600, viren2d::Color::White);

  painter->DrawGrid({}, {}, 50, 50,
                    viren2d::LineStyle(1.0, "gray!40"));

  const std::vector<std::string> anchors = {
    "center", "north", "north-east",
    "east", "south-east", "south",
    "south-west", "west", "north-west"
  };

  const std::vector<std::string> families = {"monospace", "serif", "xkcd"};


  for (size_t idx_family = 0; idx_family < families.size(); ++idx_family) {
    auto text_style = viren2d::TextStyle();
    text_style.font_size = 16;
//    text_style.use_font_height = false;
    text_style.line_spacing = 1.0;
    text_style.alignment = viren2d::HorizontalAlignment::Center;

    std::ostringstream s;

    s << "\"" << families[idx_family] << "\"";
    viren2d::Vec2d pos = {100.0 + idx_family * 200.0, 10.0};
    painter->DrawTextBox({"Font:", s.str()},
                         pos, viren2d::TextAnchor::Top,
                         text_style, {6, 6}, 0, viren2d::LineStyle(1, "black"), "black!10");

    text_style.font_size = 14;
    text_style.font_family = families[idx_family];
    text_style.font_color = "navy-blue";
//    painter->SetDefaultTextStyle(text_style);

//    viren2d::Vec2d padding(idx_family * 5,
//                           (idx_family > 0) ? (idx_family - 1) * 10 : 0);
    auto padding = viren2d::Vec2d::All(idx_family * 3);


    for (size_t idx_anchor = 0; idx_anchor < anchors.size(); ++idx_anchor) {
      std::ostringstream txt;
      txt << viren2d::TextAnchorToString(viren2d::TextAnchorFromString(anchors[idx_anchor]));
//      txt << anchors[idx_anchor];


      pos.SetY(100.0 + idx_anchor * 50.0);
      painter->DrawCircle(pos, 5, viren2d::LineStyle::Invalid, "black!40");
      if (idx_family == 0) {
        // Just draw the text
        painter->DrawText({txt.str()}, pos, viren2d::TextAnchorFromString(anchors[idx_anchor]),
                          text_style, padding);
      } else if (idx_family == 1) {
        // Draw a text box; size calculated from the text
        painter->DrawTextBox({txt.str()}, pos, viren2d::TextAnchorFromString(anchors[idx_anchor]),
                             text_style, padding, 0.0,
                             viren2d::LineStyle::Invalid,
                             "maroon!80",
                             0.25);
      } else if (idx_family == 2) {
        // Draw a text box with a fixed size
        painter->DrawTextBox({txt.str()}, pos, viren2d::TextAnchorFromString(anchors[idx_anchor]),
                             text_style, padding, 0.0,
                             viren2d::LineStyle::Invalid,
                             "orchid!40",
                             0.25, {10, -1});
      }
    }
  }

  ShowCanvas(painter->GetCanvas(false), "demo-output-text.png");
  painter.reset();
}



void DemoBoundingBox2D() {
  auto painter = viren2d::CreatePainter();
  painter->SetCanvas(600, 600, viren2d::Color::White);

  painter->DrawGrid({}, {}, 50, 50,
                    viren2d::LineStyle(1.0, "gray!60"));

  viren2d::BoundingBox2DStyle style;
  style.alpha_box_fill = 0.3;
  style.alpha_text_fill = 0.3;
  style.clip_label = true;
  style.text_style = viren2d::TextStyle(10, "monospace");

  painter->DrawBoundingBox2D(viren2d::Rect::FromLRWH(50, 50, 100, 80, 0, 0.2),
                             "fill B & T, clip ABCDEFGHIJKLMNOPQRSTUVWXYZ", style);


  style.alpha_box_fill = 0.1;
  style.alpha_text_fill = 0.3;
  style.clip_label = false;
  painter->DrawBoundingBox2D(viren2d::Rect::FromLRWH(50, 150, 100, 80, 0, 0.2),
                             "fill B & T, noclip ABCDEFGHIJKLMNOPQRSTUVWXYZ", style);


  style.alpha_box_fill = 0.0;
  style.alpha_text_fill = 0.3;
  style.clip_label = false;
  painter->DrawBoundingBox2D(viren2d::Rect::FromLRWH(50, 250, 100, 80, -5, 0.2),
                             "fill T, noclip ABCDEFGHIJKLMNOPQRSTUVWXYZ", style);


  style.alpha_box_fill = 0.3;
  style.alpha_text_fill = 0.0;
  style.clip_label = false;
  painter->DrawBoundingBox2D(viren2d::Rect::FromLRWH(50, 350, 100, 80, -5, 0.2),
                             "fill B, noclip ABCDEFGHIJKLMNOPQRSTUVWXYZ", style);

  style.alpha_box_fill = 0.0;
  style.alpha_text_fill = 0.0;
  style.clip_label = false;
  painter->DrawBoundingBox2D(viren2d::Rect::FromLRWH(50, 450, 100, 80, -5, 0.2),
                             "nofill, noclip ABCDEFGHIJKLMNOPQRSTUVWXYZ", style);

  style.clip_label = true;
  style.alpha_box_fill = 0.3;
  style.alpha_text_fill = 0.0;
  style.text_style = viren2d::TextStyle(14, "Arial", "black", true);

  style.text_alignment = viren2d::HorizontalAlignment::Center;
  painter->DrawBoundingBox2D(viren2d::Rect::FromLRWH(200, 50, 100, 250, 0, 0.4),
                             "This!", style);


  style.text_alignment = viren2d::HorizontalAlignment::Left;

  painter->DrawBoundingBox2D(viren2d::Rect::FromLRWH(350, 50, 100, 250),
                             "This!", style);

  style.alpha_box_fill = 0.0;
  style.alpha_text_fill = 0.5;
  painter->DrawBoundingBox2D(viren2d::Rect::FromLRWH(475, 50, 100, 250),
                             "This!", style);

  ShowCanvas(painter->GetCanvas(false), "demo-output-rects.png");
  painter.reset();
}



int main(int /*argc*/, char **/*argv*/) {
//  if (!viren2d::SetLogLevel("trace")) {
//    std::cout << "Could not adjust log level - did you compile viren2d"
//                 " with an appropriate viren2d_LOG_LEVEL_xxx definition?"
//              << std::endl;
//  }
//  DemoColors();
//  DemoLines();
////  DemoArrows();
//  DemoCircles();
//  DemoRects();
//  DemoText();
  DemoBoundingBox2D();

  if (true)
    return 0;

  std::cout << viren2d::Color(viren2d::NamedColor::Black).ToString() << std::endl
            << viren2d::Color("white").ToString() << std::endl
            << viren2d::Color("red").ToHexString() << std::endl
            << viren2d::Color("green").ToHexString() << std::endl;
  auto x = viren2d::Color("blue");
  x = viren2d::Color("#aabbcc");
  //------------------------------------------------------
  viren2d::Vec2d vd1(1, 2), vd2(3, 9);
  viren2d::Vec3i vi1(13, 42, -1), vi2(13,42, 0);
  std::cout << vd1 << " == " << vd2 << ": " << (vd1 == vd2) << std::endl
            << vd1 << " == " << vd1 << ": " << (vd1 == vd1) << std::endl
            << vi1 << " == " << vi2 << ": " << (vi1 == vi2) << std::endl
            << vi1 << " == " << vi1 << ": " << (vi1 == vi1) << std::endl;

////  viren2d::Vec3d cast = static_cast<viren2d::Vec3d>(vi3);
////  std::cout << "Casted?: " << cast << std::endl;
//  std::cout << "Arithmetic: " << vd1 << " - " << vd2 << " = " << (vd1 - vd2) << std::endl
//            << "  '+' = " << (vd1 + vd2) << std::endl
//            << "3 * " << vd1 << " = " << (3 * vd1) << " == lhs|rhs == " << (vd1 * 3) << std::endl
//            << vd1 << "/ 2 = " << (vd1 / 2) << std::endl
//            << vi1 << "/ 2 = " << (vi1 / 2) << std::endl << std::endl
//            << "Length " << vi1 << " = " << vi1.Length() << std::endl
//            << "L2 to  " << vi2 << " = " << vi1.Distance(vi2) << std::endl << std::endl;

//  //------------------------------------------------------

//  viren2d::Color color = viren2d::RGBA(255, 0, 255);
//  std::cout << color.ToString() << std::endl;
//  //------------------------------------------------------

//  // compare image.png cv-image.png diff.png
#ifdef EXAMPLE_IMAGE_FILE
  std::string image_filename(EXAMPLE_IMAGE_FILE);
#else  // EXAMPLE_IMAGE_FILE
  std::string image_filename("../examples/flamingo.jpg");
#endif  // EXAMPLE_IMAGE_FILE
  viren2d::ImageBuffer image_buffer = viren2d::LoadImage(image_filename, 4);

  auto painter = viren2d::CreatePainter();

////  painter->SetCanvas(image_filename);
  painter->SetCanvas(image_buffer);
////  painter->SetCanvas(600, 400, viren2d::RGBA(255, 255, 255));

  for (int i = 0; i < 4; ++i) {
    viren2d::Rect rect(40 + i*100, 256, 80, 120, i*30, 30);

    // Invocation with explicit types:
    painter->DrawRect(rect,
                      viren2d::LineStyle(6, viren2d::Color("indigo!90")),
                      viren2d::Color("cyan!20"));

    rect.cy -= (rect.height + 10);
    // Invocation with implicit casts:
    // * Rect can be created from an initializer_list of doubles
    // * Color can be created from a C string
    painter->DrawRect({40.0 + i*100.0, 100, 80, 120, i*30.0, 20},
                      viren2d::LineStyle(6, "taupe!90"),
                      "cyan!60");
  }

  //  painter->DrawLine(viren2d::Vec2d(10, 10), viren2d::Vec2d(image_buffer.width-10, image_buffer.height-10),
  //                    viren2d::LineStyle(10, viren2d::colors::Maroon(0.8)));

  painter->DrawLine({10.0, 10.0}, {image_buffer.width-10.0, image_buffer.height-10.0},
                    viren2d::LineStyle(10, "maroon!80", {}, viren2d::LineCap::Round));

  viren2d::SaveImage("test.jpg", painter->GetCanvas(false));

#ifdef WITH_OPENCV
  // The last bit of OpenCV dependency (only for displaying the image ;-)
  viren2d::ImageBuffer img_buffer = painter->GetCanvas(true);
  img_buffer.RGB2BGR(); // Warning: Currently, the buffer is shared!!
  cv::Mat cv_buffer(img_buffer.height, img_buffer.width,
                    CV_MAKETYPE(CV_8U, img_buffer.channels),
                    img_buffer.data, img_buffer.stride);
  cv::imshow("Painter's Canvas", cv_buffer);
  cv::waitKey();
#endif // WITH_OPENCV


////  painter->SetCanvas(img.cols, img.rows, viren2d::RGBA(0, 0, 200));



//  painter->DrawLine(viren2d::Vec2d(10, 10), viren2d::Vec2d(image_buffer.width-10, image_buffer.height-10),
//                    viren2d::LineStyle(6, viren2d::colors::LimeGreen(), {5, 10, 40, 10},
//                                    viren2d::LineStyle::Cap::Round));

//  painter->DrawCircle(viren2d::Vec2d(70, 90), 35,
//                      viren2d::LineStyle(6, viren2d::RGBA(0, 0, 200), {20, 20}, viren2d::LineStyle::Cap::Round),
//                      viren2d::rgba(0, 1, 1, .3));


//  //   ImagePainter painter(painter1); // copy construct
//  //  auto painter = std::move(painter1);
//  ////  ImagePainter painter = std::move(painter1); // move constructor
//  ////  ImagePainter painter; painter = std::move(painter1); // move assignment
//  ////  ImagePainter painter; painter = painter1; // copy assignment, copy construct & move assignment

//  viren2d::ImageBuffer canvas = painter->GetCanvas(false);
//  viren2d::SaveImage("dummy-canvas.png", canvas);

  return 0;
}
