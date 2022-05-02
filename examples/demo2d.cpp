#include <iostream>
#include <cmath>

//#include <Eigen/Dense>

#ifdef WITH_OPENCV
#include <opencv2/opencv.hpp>
//#include <opencv2/core/eigen.hpp>
#include <opencv2/highgui.hpp>
#endif // WITH_OPENCV

#include <viren2d/viren2d.h>
#include <viren2d/math.h>


/** Helper to show and save the canvas. */
void ShowCanvas(viren2d::ImageBuffer canvas, const std::string &filename) {
  // Save to disk
  if (!filename.empty()) {
    try {
      viren2d::SaveImage(filename, canvas);
    }  catch (const std::runtime_error &e) {
      std::cerr << "Could not save canvas to \""
                << filename << "\": " << e.what()
                << std::endl;
    }
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

void DemoLines() {
  auto painter = viren2d::CreateImagePainter();
  painter->SetCanvas(400, 400, viren2d::Color::White);

  painter->DrawGrid({}, {}, 50, 50, viren2d::LineStyle(1.0, "gray!50"));

  viren2d::LineStyle line_style(20, "azure!60", {},
                                viren2d::LineCap::Butt);
  painter->DrawLine({50.0, 50.0}, {150.0, 350.0}, line_style);
  //TODO add text linecapToString

  line_style.line_cap = viren2d::LineCap::Round;
  painter->DrawLine({150.0, 50.0}, {250.0, 350}, line_style);
  //TODO add text linecapToString

  line_style.line_cap = viren2d::LineCap::Square;
  painter->DrawLine({250.0, 50.0}, {350.0, 350.0}, line_style);
  //TODO add text linecapToString

  ShowCanvas(painter->GetCanvas(false), "demo-output-lines.png");
}

void DemoArrow1() {
  auto painter = viren2d::CreateImagePainter();
  painter->SetCanvas(600, 400, viren2d::Color::White);

  painter->DrawGrid(viren2d::Vec2d(), viren2d::Vec2d(), 10, 10, viren2d::LineStyle(0.5, "gray!80"));

//  painter->DrawLine({10, 10}, {60, 60}, viren2d::LineStyle(1, "navy-blue!80"));

  painter->DrawArrow({20, 10}, {120, 140}, viren2d::ArrowStyle(4, "navy-blue", 0.2, 30, false, true,
                                                               {}, viren2d::LineCap::Round, viren2d::LineJoin::Round));

  painter->DrawArrow({40, 40}, {200, 40}, viren2d::ArrowStyle(6, "crimson!50", 50, 10, true, true,
                                                              {}, viren2d::LineCap::Round));

  painter->DrawArrow({80, 80}, {200, 80}, viren2d::ArrowStyle(6, "crimson!50", 50, 10, false, false, {}, viren2d::LineCap::Round));

  painter->DrawArrow({40, 200}, {200, 200}, viren2d::ArrowStyle(5, "crimson!50", 50, 45, false, false, {}, viren2d::LineCap::Round));

  painter->DrawArrow({40, 250}, {400, 250}, viren2d::ArrowStyle(20, "crimson!50", 0.3, 50, true, true, {}, viren2d::LineCap::Round, viren2d::LineJoin::Round));

  ShowCanvas(painter->GetCanvas(true), "demo-output-arrow1.png");
}

void DemoArrow2() {
  auto painter = viren2d::CreateImagePainter();
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
    const double angle_rad = viren2d::deg2rad(angle);
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

  ShowCanvas(painter->GetCanvas(false), "demo-output-arrow2.png");
}

int main(int /*argc*/, char **/*argv*/) {
  DemoLines();
//  DemoArrow1();
  DemoArrow2();

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

  auto painter = viren2d::CreateImagePainter();

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

  try {
    viren2d::SaveImage("test.jpg", painter->GetCanvas(false));
  }  catch (const std::runtime_error &e) {
    std::cerr << "Could not save canvas: " << e.what() << std::endl;
  }

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
