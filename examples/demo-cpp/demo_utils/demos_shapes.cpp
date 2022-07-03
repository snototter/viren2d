#include <viren2d/viren2d.h>
#include <demo_utils/demos.h>

namespace viren2d {
namespace demos {
void DemoRects() {
  PrintDemoHeader("Rectangles");

  auto painter = CreatePainter();
  painter->SetCanvas(600, 600, Color::White);
  painter->DrawGrid({}, {}, 50, 50,
                    LineStyle(1.0, "gray!60"));

  auto style = LineStyle(3, "navy-blue!90");

  auto rect = Rect(100, 150, 100, 200);
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
  painter->DrawRect(rect, style, Color::Same.WithAlpha(0.4));// style.color.WithAlpha(0.4));

  rect.cx += 150;
  rect.rotation += 10;
  rect.radius = 30;
  painter->DrawRect(rect, style, style.color.WithAlpha(0.4));

  ProcessDemoOutput(painter->GetCanvas(false), "demo-output-rects.png");
  painter.reset();
}


} // namespace demos
} // namespace viren2d



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


//void DemoLines() {
//  auto painter = CreatePainter();
//  std::ostringstream lbl;
//  painter->SetCanvas(400, 400, Color::White);

//  painter->DrawGrid({}, {}, 50, 50, LineStyle(1.0, "gray!50"));

//  Vec2d pt1 {50, 50};
//  Vec2d pt2 {150, 350};

//  const auto rotation = werkzeugkiste::geometry::AngleDegFromDirectionVec(pt1.DirectionVector(pt2));
////  painter->SetDefaultTextStyle(TextStyle(18, "Arial"));

//  LineStyle line_style(22, "azure!60", {}, 0.0,
//                                LineCap::Butt);
//  painter->DrawLine({50.0, 50.0}, {150.0, 350.0}, line_style);
//  lbl << "LineCap::" << LineCapToString(line_style.cap);
//  painter->DrawText({lbl.str()}, {100, 200}, Anchor::Center,
//                    TextStyle(), {0, 0}, rotation);
//  lbl.str(std::string());  // Reset stringstream


//  line_style.cap = LineCap::Round;
//  painter->DrawLine({150.0, 50.0}, {250.0, 350}, line_style);
//  lbl << "LineCap::" << LineCapToString(line_style.cap);
//  painter->DrawText({lbl.str()}, {200, 200}, Anchor::Center,
//                    TextStyle(), {0, 0}, rotation);
//  lbl.str(std::string());

//  line_style.cap = LineCap::Square;
//  painter->DrawLine({250.0, 50.0}, {350.0, 350.0}, line_style);
//  lbl << "LineCap::" << LineCapToString(line_style.cap);
//  painter->DrawText({lbl.str()}, {300, 200}, Anchor::Center,
//                    TextStyle(), {0, 0}, rotation);
//  lbl.str(std::string());

//  ShowCanvas(painter->GetCanvas(false), "demo-output-lines.png");
//  painter.reset();
//}





//void DemoCircles() {
//  auto painter = CreatePainter();
//  painter->SetCanvas(500, 500, Color::White);

//  painter->DrawGrid({}, {}, 50, 50,
//                    LineStyle(1.0, "gray!60"));

//  auto style = LineStyle(3, "navy-blue!90");

////  SetLogLevel("trace");
//  painter->DrawCircle({100, 100}, 50, style, "red");

//  style.dash_pattern = {20, 15};
//  painter->DrawCircle({250, 100}, 50, style);

//  style.dash_pattern = {};
//  style.width = 0;
//  auto fill = Color("blue!40");
//  painter->DrawCircle({400, 100}, 50, style, fill);

//  style.width = 4;
//  painter->DrawArc({100, 250}, 50,   0,  90, style, false);
//  painter->DrawArc({100, 250}, 50, 110, 160, style, false);
//  painter->DrawArc({100, 250}, 50, 180, 210, style, false);
//  painter->DrawArc({100, 250}, 50, 230, 250, style, false);

//  style.width = 2;
//  style.dash_pattern = {10, 4};
//  painter->DrawArc({250, 250}, 50,   0,  90, style);
//  painter->DrawArc({250, 250}, 50, 110, 160, style);
//  painter->DrawArc({250, 250}, 50, 180, 210, style);
//  painter->DrawArc({250, 250}, 50, 230, 250, style);

//  style.dash_pattern = {};
//  style.width = 0;
//  painter->DrawArc({400, 250}, 50, 0, 90, style, true, fill);
//  painter->DrawArc({400, 250}, 50, 110, 160, style, true, fill);
//  painter->DrawArc({400, 250}, 50, 180, 210, style, true, fill);
//  painter->DrawArc({400, 250}, 50, 230, 250, style, true, fill);

//  style.width = 3;
//  painter->DrawEllipse({100, 400, 100, 50, 0, 45, -45}, style);
////  painter->DrawEllipse(Ellipse({100, 400}, {100, 50}, 0, 45, -45), style);

//  style.dash_pattern = {10, 10};
//  painter->DrawEllipse(Ellipse({250, 400}, {100, 50}, 45, 45, -45, false),
//                       style, fill);

//  style.dash_pattern = {};
//  style.width = 0;
//  painter->DrawEllipse(Ellipse({400, 400}, {100, 50}, 180, 45, -45, true),
//                       style, fill);

//  ShowCanvas(painter->GetCanvas(false), "demo-output-circles.png");
//  painter.reset();
//}


//void DemoPolygon() {
//  auto painter = CreatePainter();
//  painter->SetCanvas(800, 800, Color::White);

//  painter->DrawGrid({}, {}, 50, 50,
//                    LineStyle(1.0, "gray!80"));

//  auto line_style = LineStyle(3.0, "crimson");
//  painter->DrawPolygon({{10, 10}, {50, 50}, {70, 50}, {60, 60}, {20, 10}},
//                       line_style);

//  line_style.color = "azure";
//  painter->DrawPolygon({{100, 100}, {250, 350}, {180, 180}, {400, 200}},
//                       line_style, Color::Same.WithAlpha(0.4));

//  line_style.color = "midnight-blue";
//  painter->DrawPolygon({{500, 100}, {600, 150}, {550, 300}, {500, 150}},
//                       line_style, "azure!20");

//  ShowCanvas(painter->GetCanvas(false), "demo-output-polygon.png");
//  painter.reset();
//}





//void DemoText() {
//  auto painter = CreatePainter();
//  painter->SetCanvas(550, 600, Color::White);


//  const std::vector<std::string> anchors = {
//    "center", "north", "north-east",
//    "east", "south-east", "south",
//    "south-west", "west", "north-west"
//  };

//  const std::vector<std::string> families = {"monospace", "sans-serif", "xkcd"};


//  for (size_t idx_family = 0; idx_family < families.size(); ++idx_family) {
//    auto text_style = TextStyle();
//    text_style.size = 16;
//    text_style.family = families[idx_family];
//    text_style.line_spacing = 1.0;
//    text_style.alignment = HorizontalAlignment::Center;

//    auto padding = Vec2d::All(idx_family * 3);

//    std::ostringstream s;
//    s << "Padding: " << static_cast<int>(padding.x());
//    Vec2d pos = {100.0 + idx_family * 200.0, 10.0};
//    painter->DrawTextBox({families[idx_family], s.str()},
//                         pos, Anchor::Top,
//                         text_style, {6, 6}, 0, LineStyle(1, "black"), "azure!40");

//    text_style.size = 14;
//    text_style.color = "navy-blue";

//    for (size_t idx_anchor = 0; idx_anchor < anchors.size(); ++idx_anchor) {
//      std::ostringstream txt;
////      txt << "!nofill, noclip!";
//      txt << AnchorToString(AnchorFromString(anchors[idx_anchor]));
////      txt << anchors[idx_anchor];


//      pos.SetY(100.0 + idx_anchor * 50.0);
//      //painter->DrawCircle(pos, 5, LineStyle::Invalid, "black!40");
//      painter->DrawMarker(pos, MarkerStyle('9', 30, 1, "crimson!90", false));
//      if (idx_family == 0) {
//        // Just draw the text
//        painter->DrawText({txt.str()}, pos, AnchorFromString(anchors[idx_anchor]),
//                          text_style, padding);
//      } else { //if (idx_family == 1) {
//        // Draw a text box; size calculated from the text
//        painter->DrawTextBox({txt.str()}, pos, AnchorFromString(anchors[idx_anchor]),
//                             text_style, padding, 0.0,
//                             LineStyle::Invalid,
//                             "azure!40",
//                             0.25);
//      }
//    }
//  }

//  ShowCanvas(painter->GetCanvas(false), "demo-output-text.png");
//  painter.reset();
//}


//void DemoTrajectories() {
//  auto painter = CreatePainter();
//  painter->SetCanvas(800, 800, Color::White);

//  painter->DrawGrid({0, 0}, {0, 0}, 50, 50, {1});

//  std::vector<Vec2d> trajectory{{500, 100}, {600, 150}, {550, 300}, {500, 150}};
//  auto style = LineStyle(5, "midnight-blue", {}, 0.0, LineCap::Round);
//  bool oldest_first = false;
//  for (const auto &offset : {Vec2d(-400, 50), Vec2d(-100, 200), Vec2d(50, 300)}) {
//    std::vector<Vec2d> traj;
//    for (const auto &pt : trajectory) {
//      traj.push_back(pt + offset);
//    }
//    painter->DrawTrajectory(traj, style, Color("gray!20"), oldest_first);
//    oldest_first = !oldest_first;
//  }

//  trajectory = {{50, 400}, {100, 450}, {150, 400}, {200, 450},
//                {150, 500}, {200, 550}, {250, 500}, {300, 600},
//                {350, 500}, {400, 600}, {450, 550}, {500, 700}};
//  style.color = "red";
//  painter->DrawTrajectory(trajectory, style, "black!100", true, 5, ColorFadeOutLinear);

//  std::vector<Vec2d> pts;
//  for (const auto &pt : trajectory)
//    pts.push_back(pt + Vec2d{0.0, 100.0});
//  painter->DrawTrajectory(pts, style, "black!100");

//  pts.clear();
//  for (const auto &pt : trajectory)
//    pts.push_back(pt + Vec2d{0.0, 200.0});
//  painter->DrawTrajectory(pts, style, "black!100");

//  ShowCanvas(painter->GetCanvas(false), "demo-output-polygon.png");
//  painter.reset();
//}



//void DemoMarkers() {
//  auto painter = CreatePainter();
//  painter->SetCanvas(100, 800, Color::White); //.WithAlpha(0.0));

//  auto text_style = TextStyle();
//  text_style.size = 16;
//  text_style.family = "xkcd";//"monospace";
//  text_style.color = RGBa(60, 60, 60);

//  Vec2d pos {110.0, 5.0};
//  MarkerStyle marker_style;
//  marker_style.color = "navy-blue";
//  marker_style.size = 19;
//  marker_style.thickness = 1;

//  painter->DrawText({"Marker code:"}, {5.0, pos.y()},
//                    Anchor::TopLeft, text_style);

//  painter->DrawText({"Not filled:"}, {5.0, 50.0},
//                    Anchor::Left, text_style);

//  painter->DrawText({"Filled:"}, {5.0, 85.0},
//                    Anchor::Left, text_style);

//  text_style.size = 16;
//  text_style.family = "monospace";

//  for (char m : ListMarkers()) {
//    std::ostringstream s;
////    s << '\'' << m << '\'';
//    s << m;
//    painter->DrawText({s.str()}, pos, Anchor::Top, text_style);

//    Vec2d mpos{pos.x(), 45.0};
//    marker_style.filled = false;
//    marker_style.marker = MarkerFromChar(m);
//    if (!marker_style.IsFilled()) {
//      painter->DrawMarker(mpos, marker_style);
//    }

//    marker_style.filled = true;
//    if (marker_style.IsFilled()) {
//      mpos = {pos.x(), 80.0};
//      painter->DrawMarker(mpos, marker_style);
//    }

//    pos.x() += 32;
//  }

//  ShowCanvas(painter->GetCanvas(false), "demo-output-markers.png");
//  painter.reset();
//}




//void DemoConversionOpenCV() {
//#ifdef viren2d_WITH_OPENCV
//#ifdef EXAMPLE_IMAGE_FILE
//  std::string image_filename(EXAMPLE_IMAGE_FILE);
//#else  // EXAMPLE_IMAGE_FILE
//  std::string image_filename("../examples/data/flamingo.jpg");
//  std::cerr << "Filename of example image not given, using default (relative!) path: "
//            << image_filename << std::endl;
//#endif  // EXAMPLE_IMAGE_FILE
//  cv::Mat img_cv = cv::imread(image_filename);

//  // To make this example a bit more interesting,
//  // let's get a non-continuous OpenCV matrix:
//  cv::Mat roi = img_cv.colRange(50, img_cv.cols - 50);

//  ///// FIXME make separate ROI demo
//  ImageBuffer buf;
//  buf.CreateSharedBuffer(
//        roi.data, roi.rows, roi.cols,
//        roi.channels(), roi.step1(0), roi.step1(1),
//        ImageBufferType::UInt8);

//  ImageBuffer gray = buf.ToGrayscale(3);
//  ImageBuffer blend = buf.Blend(gray, 0.7);

//  {
//    cv::Mat cvtmp(blend.Height(), blend.Width(),
//                    CV_MAKETYPE(CV_8U, blend.Channels()),
//                    blend.MutableData(), blend.RowStride());

//      cv::imshow("Blend", cvtmp);
//  }


//  for (int ch = 0; ch < buf.Channels(); ++ch) {
//    double min, max;
//    Vec2i minloc, maxloc;
//    buf.MinMaxLocation(&min, &max, &minloc, &maxloc, ch);
//    std::cout << "Channel " << ch << ", min at " << minloc << " ("
//              << min << "), max at " << maxloc << " (" << max << ")" << std::endl;
//  }
////  SetLogLevel(LogLevel::Trace);
//  LoadImageUInt8("/home/snototter/workspace/utilities/vito/examples/depth.png", 0);
//  buf.Pixelate(15, 23, 0, 50, 250, 200);
////  ImageBuffer buf_roi = buf.ROI(0, 50, 250, 200);
////  for (int r = 0; r < buf_roi.Height(); ++r) {
////    for (int c = 0; c < buf_roi.Width(); ++c) {
////      buf_roi.AtChecked<uint8_t>(r, c, 0) = 255;
////      buf_roi.AtChecked<uint8_t>(r, c, 1) = 0;
////      buf_roi.AtChecked<uint8_t>(r, c, 2) = 255;
////    }
////  }


//  // Create a shared buffer (on purpose) and change
//  // color format to demonstrate the potential side-effect:
//  ImageBuffer img_buf;
//  img_buf.CreateSharedBuffer(
//        roi.data, roi.rows, roi.cols,
//        roi.channels(), roi.step1(0), roi.step1(1),

//        ImageBufferType::UInt8);
//  img_buf.SwapChannels(0, 2);

//  // Now, use the ImageBuffer to set up a canvas and
//  // draw something
//  auto painter = CreatePainter();
//  painter->SetCanvas(img_buf);

//  painter->DrawArrow(
//        {0.0, 0.0}, {img_buf.Width() / 2.0, img_buf.Height() / 2.0},
//        ArrowStyle(10, "navy-blue!80", 0.2, 20.0));

//  // Retrieve the visualization, and show the image.
//  // Since we'll use cv::imshow, we need to convert
//  // the buffer to BGR(A) format:
//  ImageBuffer copy = painter->GetCanvas(true);
//  copy.SwapChannels(0, 2);
//  cv::Mat cv_buffer(copy.Height(), copy.Width(),
//                    CV_MAKETYPE(CV_8U, copy.Channels()),
//                    copy.MutableData(), copy.RowStride());

//  cv::imshow("Painter's Canvas", cv_buffer);
//  cv::imshow("Shared Buffer Side Effects", img_cv);

//  cv::waitKey();
//  painter.reset();
//#else  // viren2d_WITH_OPENCV
//  std::cerr << "OpenCV is not available - cannot display the canvas." << std::endl;
//#endif  // viren2d_WITH_OPENCV
//}

