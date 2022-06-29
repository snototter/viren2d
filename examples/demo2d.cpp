#include <iostream>
#include <cmath>

//#include <Eigen/Dense>

#ifdef viren2d_WITH_OPENCV
#include <opencv2/opencv.hpp>
//#include <opencv2/core/eigen.hpp>
#include <opencv2/highgui.hpp>
#endif // viren2d_WITH_OPENCV

#include <viren2d/viren2d.h>
#include <werkzeugkiste/container/sort.h>
#include <werkzeugkiste/geometry/utils.h>


/// Helper to show and save the canvas.
void ShowCanvas(viren2d::ImageBuffer canvas, const std::string &filename) {
  // Save to disk
  if (!filename.empty()) {
    viren2d::SaveImage(filename, canvas);
    std::cout << "Canvas saved to '" << filename << "'." << std::endl;
  }

#ifdef viren2d_WITH_OPENCV
  viren2d::ImageBuffer copy(canvas);
  copy.SwapChannels(0, 2);
  cv::Mat cv_buffer(copy.Height(), copy.Width(),
                    CV_MAKETYPE(CV_8U, copy.Channels()),
                    copy.MutableData(), copy.RowStride());
  cv::imshow("Painter's Canvas", cv_buffer);
  cv::waitKey();
#else  // viren2d_WITH_OPENCV
  std::cerr << "OpenCV is not available - cannot display the canvas." << std::endl;
#endif  // viren2d_WITH_OPENCV
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
  painter->SetCanvas(448, canvas_width,
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

  for (const auto &color_name : viren2d::ListNamedColors()) {
    auto color = viren2d::Color(color_name);

    text_style.color = color.Grayscale().Inverse();
    painter->DrawTextBox({color_name}, {x, y},
                         viren2d::Anchor::Top,
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

  viren2d::LineStyle line_style(22, "azure!60", {}, 0.0,
                                viren2d::LineCap::Butt);
  painter->DrawLine({50.0, 50.0}, {150.0, 350.0}, line_style);
  lbl << "LineCap::" << viren2d::LineCapToString(line_style.cap);
  painter->DrawText({lbl.str()}, {100, 200}, viren2d::Anchor::Center,
                    viren2d::TextStyle(), {0, 0}, rotation);
  lbl.str(std::string());  // Reset stringstream


  line_style.cap = viren2d::LineCap::Round;
  painter->DrawLine({150.0, 50.0}, {250.0, 350}, line_style);
  lbl << "LineCap::" << viren2d::LineCapToString(line_style.cap);
  painter->DrawText({lbl.str()}, {200, 200}, viren2d::Anchor::Center,
                    viren2d::TextStyle(), {0, 0}, rotation);
  lbl.str(std::string());

  line_style.cap = viren2d::LineCap::Square;
  painter->DrawLine({250.0, 50.0}, {350.0, 350.0}, line_style);
  lbl << "LineCap::" << viren2d::LineCapToString(line_style.cap);
  painter->DrawText({lbl.str()}, {300, 200}, viren2d::Anchor::Center,
                    viren2d::TextStyle(), {0, 0}, rotation);
  lbl.str(std::string());

  ShowCanvas(painter->GetCanvas(false), "demo-output-lines.png");
  painter.reset();
}


void DemoArrows() {
  auto painter = viren2d::CreatePainter();
  painter->SetCanvas(800, 800, viren2d::Color::White);

  painter->DrawGrid({}, {}, 50, 50,
                    viren2d::LineStyle(1.0, "gray!80"));

  auto style = viren2d::ArrowStyle(6, "navy-blue", 0.15, 20.0,
                                   true, false, {}, 0.0,
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
  style.width = 0;
  auto fill = viren2d::Color("blue!40");
  painter->DrawCircle({400, 100}, 50, style, fill);

  style.width = 4;
  painter->DrawArc({100, 250}, 50,   0,  90, style, false);
  painter->DrawArc({100, 250}, 50, 110, 160, style, false);
  painter->DrawArc({100, 250}, 50, 180, 210, style, false);
  painter->DrawArc({100, 250}, 50, 230, 250, style, false);

  style.width = 2;
  style.dash_pattern = {10, 4};
  painter->DrawArc({250, 250}, 50,   0,  90, style);
  painter->DrawArc({250, 250}, 50, 110, 160, style);
  painter->DrawArc({250, 250}, 50, 180, 210, style);
  painter->DrawArc({250, 250}, 50, 230, 250, style);

  style.dash_pattern = {};
  style.width = 0;
  painter->DrawArc({400, 250}, 50, 0, 90, style, true, fill);
  painter->DrawArc({400, 250}, 50, 110, 160, style, true, fill);
  painter->DrawArc({400, 250}, 50, 180, 210, style, true, fill);
  painter->DrawArc({400, 250}, 50, 230, 250, style, true, fill);

  style.width = 3;
  painter->DrawEllipse({100, 400, 100, 50, 0, 45, -45}, style);
//  painter->DrawEllipse(viren2d::Ellipse({100, 400}, {100, 50}, 0, 45, -45), style);

  style.dash_pattern = {10, 10};
  painter->DrawEllipse(viren2d::Ellipse({250, 400}, {100, 50}, 45, 45, -45, false),
                       style, fill);

  style.dash_pattern = {};
  style.width = 0;
  painter->DrawEllipse(viren2d::Ellipse({400, 400}, {100, 50}, 180, 45, -45, true),
                       style, fill);

  ShowCanvas(painter->GetCanvas(false), "demo-output-circles.png");
  painter.reset();
}


void DemoPolygon() {
  auto painter = viren2d::CreatePainter();
  painter->SetCanvas(800, 800, viren2d::Color::White);

  painter->DrawGrid({}, {}, 50, 50,
                    viren2d::LineStyle(1.0, "gray!80"));

  auto line_style = viren2d::LineStyle(3.0, "crimson");
  painter->DrawPolygon({{10, 10}, {50, 50}, {70, 50}, {60, 60}, {20, 10}},
                       line_style);

  line_style.color = "azure";
  painter->DrawPolygon({{100, 100}, {250, 350}, {180, 180}, {400, 200}},
                       line_style, viren2d::Color::Same.WithAlpha(0.4));

  line_style.color = "midnight-blue";
  painter->DrawPolygon({{500, 100}, {600, 150}, {550, 300}, {500, 150}},
                       line_style, "azure!20");

  ShowCanvas(painter->GetCanvas(false), "demo-output-polygon.png");
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
  painter->DrawRect(rect, style, viren2d::Color::Same.WithAlpha(0.4));// style.color.WithAlpha(0.4));

  rect.cx += 150;
  rect.rotation += 10;
  rect.radius = 30;
  painter->DrawRect(rect, style, style.color.WithAlpha(0.4));

  ShowCanvas(painter->GetCanvas(false), "demo-output-rects.png");
  painter.reset();
}


void DemoText() {
  auto painter = viren2d::CreatePainter();
  painter->SetCanvas(550, 600, viren2d::Color::White);


  const std::vector<std::string> anchors = {
    "center", "north", "north-east",
    "east", "south-east", "south",
    "south-west", "west", "north-west"
  };

  const std::vector<std::string> families = {"monospace", "sans-serif", "xkcd"};


  for (size_t idx_family = 0; idx_family < families.size(); ++idx_family) {
    auto text_style = viren2d::TextStyle();
    text_style.size = 16;
    text_style.family = families[idx_family];
    text_style.line_spacing = 1.0;
    text_style.alignment = viren2d::HorizontalAlignment::Center;

    auto padding = viren2d::Vec2d::All(idx_family * 3);

    std::ostringstream s;
    s << "Padding: " << static_cast<int>(padding.x());
    viren2d::Vec2d pos = {100.0 + idx_family * 200.0, 10.0};
    painter->DrawTextBox({families[idx_family], s.str()},
                         pos, viren2d::Anchor::Top,
                         text_style, {6, 6}, 0, viren2d::LineStyle(1, "black"), "azure!40");

    text_style.size = 14;
    text_style.color = "navy-blue";

    for (size_t idx_anchor = 0; idx_anchor < anchors.size(); ++idx_anchor) {
      std::ostringstream txt;
//      txt << "!nofill, noclip!";
      txt << viren2d::AnchorToString(viren2d::AnchorFromString(anchors[idx_anchor]));
//      txt << anchors[idx_anchor];


      pos.SetY(100.0 + idx_anchor * 50.0);
      //painter->DrawCircle(pos, 5, viren2d::LineStyle::Invalid, "black!40");
      painter->DrawMarker(pos, viren2d::MarkerStyle('9', 30, 1, "crimson!90", false));
      if (idx_family == 0) {
        // Just draw the text
        painter->DrawText({txt.str()}, pos, viren2d::AnchorFromString(anchors[idx_anchor]),
                          text_style, padding);
      } else { //if (idx_family == 1) {
        // Draw a text box; size calculated from the text
        painter->DrawTextBox({txt.str()}, pos, viren2d::AnchorFromString(anchors[idx_anchor]),
                             text_style, padding, 0.0,
                             viren2d::LineStyle::Invalid,
                             "azure!40",
                             0.25);
      }
    }
  }

  ShowCanvas(painter->GetCanvas(false), "demo-output-text.png");
  painter.reset();
}


void DemoTrajectories() {
  auto painter = viren2d::CreatePainter();
  painter->SetCanvas(800, 800, viren2d::Color::White);

  std::vector<viren2d::Vec2d> trajectory{{500, 100}, {600, 150}, {550, 300}, {500, 150}};
  auto style = viren2d::LineStyle(5, "midnight-blue", {}, 0.0, viren2d::LineCap::Round);
  bool oldest_first = false;
  for (const auto &offset : {viren2d::Vec2d(-400, 50), viren2d::Vec2d(-100, 200), viren2d::Vec2d(50, 300)}) {
    std::vector<viren2d::Vec2d> traj;
    for (const auto &pt : trajectory) {
      traj.push_back(pt + offset);
    }
    painter->DrawTrajectory(traj, style, viren2d::Color("gray!20"), oldest_first);
    oldest_first = !oldest_first;
  }

  trajectory = {{50, 400}, {100, 450}, {150, 400}, {200, 450},
                {150, 500}, {200, 550}, {250, 500}, {300, 600},
                {350, 500}, {400, 600}, {450, 550}, {500, 700}};
  style.color = "red";
  painter->DrawTrajectory(trajectory, style, "black!100", true, 5, viren2d::ColorFadeOutLinear);

  std::vector<viren2d::Vec2d> pts;
  for (const auto &pt : trajectory)
    pts.push_back(pt + viren2d::Vec2d{0.0, 100.0});
  painter->DrawTrajectory(pts, style, "black!100");

  pts.clear();
  for (const auto &pt : trajectory)
    pts.push_back(pt + viren2d::Vec2d{0.0, 200.0});
  painter->DrawTrajectory(pts, style, "black!100");

  ShowCanvas(painter->GetCanvas(false), "demo-output-polygon.png");
  painter.reset();
}



void DemoMarkers() {
  auto painter = viren2d::CreatePainter();
  painter->SetCanvas(100, 800, viren2d::Color::White); //.WithAlpha(0.0));

  auto text_style = viren2d::TextStyle();
  text_style.size = 16;
  text_style.family = "xkcd";//"monospace";
  text_style.color = viren2d::RGBa(60, 60, 60);

  viren2d::Vec2d pos {110.0, 5.0};
  viren2d::MarkerStyle marker_style;
  marker_style.color = "navy-blue";
  marker_style.size = 19;
  marker_style.thickness = 1;

  painter->DrawText({"Marker code:"}, {5.0, pos.y()},
                    viren2d::Anchor::TopLeft, text_style);

  painter->DrawText({"Not filled:"}, {5.0, 50.0},
                    viren2d::Anchor::Left, text_style);

  painter->DrawText({"Filled:"}, {5.0, 85.0},
                    viren2d::Anchor::Left, text_style);

  text_style.size = 16;
  text_style.family = "monospace";

  for (char m : viren2d::ListMarkers()) {
    std::ostringstream s;
//    s << '\'' << m << '\'';
    s << m;
    painter->DrawText({s.str()}, pos, viren2d::Anchor::Top, text_style);

    viren2d::Vec2d mpos{pos.x(), 45.0};
    marker_style.filled = false;
    marker_style.marker = viren2d::MarkerFromChar(m);
    if (!marker_style.IsFilled()) {
      painter->DrawMarker(mpos, marker_style);
    }

    marker_style.filled = true;
    if (marker_style.IsFilled()) {
      mpos = {pos.x(), 80.0};
      painter->DrawMarker(mpos, marker_style);
    }

    pos.x() += 32;
  }

  ShowCanvas(painter->GetCanvas(false), "demo-output-markers.png");
  painter.reset();
}

void DemoBoundingBox2D() {
  auto painter = viren2d::CreatePainter();
  for (auto label_pos : {viren2d::LabelPosition::Top,
       viren2d::LabelPosition::Bottom, viren2d::LabelPosition::LeftB2T,
       viren2d::LabelPosition::RightB2T}) {
    painter->SetCanvas(600, 600, viren2d::Color::White);

    painter->DrawGrid({}, {}, 50, 50,
                      viren2d::LineStyle(1.0, "gray!60"));

    viren2d::BoundingBox2DStyle style;
    style.label_padding.SetX(10);
    style.label_padding.SetY(5);
    style.text_fill_color = viren2d::Color::Same.WithAlpha(0.3);
    style.clip_label = true;
    style.text_style = viren2d::TextStyle(10, "monospace");

    style.label_position = label_pos;
    style.text_style.alignment = viren2d::HorizontalAlignment::Left;

    painter->DrawBoundingBox2D(viren2d::Rect::FromLTWH(50, 50, 100, 80, 0.2),
                               {"fill B & T, clip ABCDEFG"}, style);


    style.box_fill_color = viren2d::Color::Same.WithAlpha(0.5);
    style.text_fill_color = viren2d::Color::Same.WithAlpha(0.2);
    style.clip_label = false;
    painter->DrawBoundingBox2D(viren2d::Rect::FromLTWH(50, 150, 100, 80, 0.2),
                               {"fill B 0.5 & T 0.2, noclip"}, style);


    style.box_fill_color = viren2d::Color::Invalid;
    style.text_fill_color = viren2d::Color::Same.WithAlpha(0.3);
    style.clip_label = true;
    painter->DrawBoundingBox2D(viren2d::Rect::FromLTWH(50, 250, 100, 80, 0.2),
                               {"fill T 0.3, clip label"}, style);


    style.box_fill_color = viren2d::Color("crimson!50");
    style.text_fill_color = viren2d::Color::Invalid;
    style.clip_label = false;
    painter->DrawBoundingBox2D(viren2d::Rect::FromLTWH(50, 350, 100, 80, 0.2),
                               {"fill B crimson!50, noclip", style.ToString()}, style);

    style.box_fill_color = viren2d::Color::Invalid;
    style.text_fill_color = viren2d::Color::Invalid;
    style.clip_label = true;
    painter->DrawBoundingBox2D(viren2d::Rect::FromLTWH(50, 450, 100, 80, 0),
                               {"no fill, but clip", style.ToString()}, style);


    style.clip_label = true;
    style.box_fill_color = viren2d::Color::Same.WithAlpha(0.3);
    style.text_fill_color = viren2d::Color("crimson!60");
    style.text_style = viren2d::TextStyle(14, "Arial", "black", true);

    style.text_style.alignment = viren2d::HorizontalAlignment::Center;
    painter->DrawBoundingBox2D(viren2d::Rect::FromLTWH(200, 50, 100, 250, 0.4),
                               {"Centered", "label"}, style);


    style.text_style.alignment = viren2d::HorizontalAlignment::Left;
    style.text_fill_color = viren2d::Color::Same.WithAlpha(0.3);
    painter->DrawBoundingBox2D(viren2d::Rect::FromLTWH(350, 50, 100, 250),
                               {"Left-aligned", "label"}, style);

    style.box_fill_color = viren2d::Color::Invalid;
    style.text_fill_color = viren2d::Color::Same.WithAlpha(0.5);
    style.text_style.alignment = viren2d::HorizontalAlignment::Right;
    painter->DrawBoundingBox2D(viren2d::Rect::FromLTWH(475, 50, 100, 250),
                               {"Right-aligned", "label"}, style);

    ShowCanvas(painter->GetCanvas(false), "demo-output-rects.png");
  }
  painter.reset();
}


void DemoConversionOpenCV() {
#ifdef viren2d_WITH_OPENCV
#ifdef EXAMPLE_IMAGE_FILE
  std::string image_filename(EXAMPLE_IMAGE_FILE);
#else  // EXAMPLE_IMAGE_FILE
  std::string image_filename("../examples/flamingo.jpg");
  std::cerr << "Filename of example image not given, using default (relative!) path: "
            << image_filename << std::endl;
#endif  // EXAMPLE_IMAGE_FILE
  cv::Mat img_cv = cv::imread(image_filename);

  // To make this example a bit more interesting,
  // let's get a non-continuous OpenCV matrix:
  cv::Mat roi = img_cv.colRange(50, img_cv.cols - 50);

  ///// FIXME make separate ROI demo
  viren2d::ImageBuffer buf;
  buf.CreateSharedBuffer(
        roi.data, roi.rows, roi.cols,
        roi.channels(), roi.step1(0),
        viren2d::ImageBufferType::UInt8, roi.step1(1));

  viren2d::ImageBuffer buf_roi = buf.ROI(10, 50, 100, 200);
  for (int r = 0; r < buf_roi.Height(); ++r) {
    for (int c = 0; c < buf_roi.Width(); ++c) {
      buf_roi.AtChecked<uint8_t>(r, c, 0) = 255;
      buf_roi.AtChecked<uint8_t>(r, c, 1) = 0;
      buf_roi.AtChecked<uint8_t>(r, c, 2) = 255;
    }
  }


  // Create a shared buffer (on purpose) and change
  // color format to demonstrate the potential side-effect:
  viren2d::ImageBuffer img_buf;
  img_buf.CreateSharedBuffer(
        roi.data, roi.rows, roi.cols,
        roi.channels(), roi.step1(0), viren2d::ImageBufferType::UInt8, roi.step1(1));
  img_buf.SwapChannels(0, 2);

  // Now, use the ImageBuffer to set up a canvas and
  // draw something
  auto painter = viren2d::CreatePainter();
  painter->SetCanvas(img_buf);

  painter->DrawArrow(
        {0.0, 0.0}, {img_buf.Width() / 2.0, img_buf.Height() / 2.0},
        viren2d::ArrowStyle(10, "navy-blue!80", 0.2, 20.0));

  // Retrieve the visualization, and show the image.
  // Since we'll use cv::imshow, we need to convert
  // the buffer to BGR(A) format:
  viren2d::ImageBuffer copy = painter->GetCanvas(true);
  copy.SwapChannels(0, 2);
  cv::Mat cv_buffer(copy.Height(), copy.Width(),
                    CV_MAKETYPE(CV_8U, copy.Channels()),
                    copy.MutableData(), copy.RowStride());

  cv::imshow("Painter's Canvas", cv_buffer);
  cv::imshow("Shared Buffer Side Effects", img_cv);

  cv::waitKey();
  painter.reset();
#else  // viren2d_WITH_OPENCV
  std::cerr << "OpenCV is not available - cannot display the canvas." << std::endl;
#endif  // viren2d_WITH_OPENCV
}


int main(int /*argc*/, char **/*argv*/) {
  DemoConversionOpenCV();
//  //viren2d::Color::FromCategory("Person");
//  std::cout << "Color by ID: " << viren2d::Color::FromID(17) << std::endl;
////  if (!viren2d::SetLogLevel("trace")) {
////    std::cout << "Could not adjust log level - did you compile viren2d"
////                 " with an appropriate viren2d_LOG_LEVEL_xxx definition?"
////              << std::endl;
////  }
////  DemoColors();
////  DemoLines();
//////  DemoArrows();
////  DemoCircles();
////  DemoRects();
//  DemoText();
//  DemoMarkers();
////  DemoBoundingBox2D();
//  DemoPolygon();
  DemoTrajectories();

  // TODO create examples out of this sandbox mess

// demo set canvas from image & draw some rectangles:
////  // compare image.png cv-image.png diff.png
//#ifdef EXAMPLE_IMAGE_FILE
//  std::string image_filename(EXAMPLE_IMAGE_FILE);
//#else  // EXAMPLE_IMAGE_FILE
//  std::string image_filename("../examples/flamingo.jpg");
//#endif  // EXAMPLE_IMAGE_FILE
//  viren2d::ImageBuffer image_buffer = viren2d::LoadImage(image_filename, 4);

//  auto painter = viren2d::CreatePainter();

//  painter->SetCanvas(image_buffer);

//  for (int i = 0; i < 4; ++i) {
//    viren2d::Rect rect(40 + i*100, 256, 80, 120, i*30, 30);

//    // Invocation with explicit types:
//    painter->DrawRect(rect,
//                      viren2d::LineStyle(6, viren2d::Color("indigo!90")),
//                      viren2d::Color("cyan!20"));

//    rect.cy -= (rect.height + 10);
//    // Invocation with implicit casts:
//    // * Rect can be created from an initializer_list of doubles
//    // * Color can be created from a C string
//    painter->DrawRect({40.0 + i*100.0, 100, 80, 120, i*30.0, 20},
//                      viren2d::LineStyle(6, "taupe!90"),
//                      "cyan!60");
//  }

//  //  painter->DrawLine(viren2d::Vec2d(10, 10), viren2d::Vec2d(image_buffer.width-10, image_buffer.height-10),
//  //                    viren2d::LineStyle(10, viren2d::colors::Maroon(0.8)));

//  painter->DrawLine({10.0, 10.0}, {image_buffer.width-10.0, image_buffer.height-10.0},
//                    viren2d::LineStyle(10, "maroon!80", {}, viren2d::LineCap::Round));

//  viren2d::SaveImage("test.jpg", painter->GetCanvas(false));
//  ShowCanvas(painter->GetCanvas(false), "demo-image-canvas.jpg");


////  //   ImagePainter painter(painter1); // copy construct
////  //  auto painter = std::move(painter1);
////  ////  ImagePainter painter = std::move(painter1); // move constructor
////  ////  ImagePainter painter; painter = std::move(painter1); // move assignment
////  ////  ImagePainter painter; painter = painter1; // copy assignment, copy construct & move assignment

  return 0;
}
