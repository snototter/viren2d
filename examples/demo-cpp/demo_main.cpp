#include <iostream>
#include <string>

//#include <Eigen/Dense>

#ifdef viren2d_WITH_OPENCV
#include <opencv2/opencv.hpp>
//#include <opencv2/core/eigen.hpp>
#include <opencv2/highgui.hpp>
#endif // viren2d_WITH_OPENCV

#include <viren2d/imagebuffer.h>
#include <demo_utils/demos.h>

namespace viren2d {
namespace demos {

void PrintDemoHeader(const std::string &title) {
  std::cout
      << "\n----------------------------------------------------------------------\n  "
      << title
      << "\n----------------------------------------------------------------------"
      << std::endl;
}


void ProcessDemoOutput(const ImageBuffer &canvas, const std::string &filename) {
  // Save to disk
  if (!filename.empty()) {
    viren2d::SaveImageUInt8(filename, canvas);
    std::cout << "Canvas saved to '" << filename << "'." << std::endl;
  }

#ifdef viren2d_WITH_OPENCV
  viren2d::ImageBuffer copy(canvas);
  if (copy.Channels() >= 3) {
    copy.SwapChannels(0, 2);
  }
  cv::Mat cv_buffer(copy.Height(), copy.Width(),
                    CV_MAKETYPE(CV_8U, copy.Channels()),
                    copy.MutableData(), copy.RowStride());
  cv::imshow("Painter's Canvas", cv_buffer);
  cv::waitKey();
#else  // viren2d_WITH_OPENCV
  std::cerr << "OpenCV is not available - cannot display the canvas." << std::endl;
#endif  // viren2d_WITH_OPENCV
}


void Showcase() {
//  DemoArrows();
//  DemoBoundingBoxes2D();
//  DemoCircles();
  DemoCircleTangents();
//  DemoColorMaps();
//  DemoImageBufferConversionOpenCV();
//  DemoLines();
//  DemoMarkers();
//  DemoOpticalFlow();
  DemoPinhole();
//  DemoPolygons();
//  DemoRects();
//  DemoText();
//  DemoTrajectories();
}

} // namespace demos
} // namespace viren2d


int main(int /*argc*/, char **/*argv*/) {
  viren2d::demos::Showcase();
  return 0;
}
//  DemoConversionOpenCV();
////  //viren2d::Color::FromCategory("Person");
////  std::cout << "Color by ID: " << viren2d::Color::FromID(17) << std::endl;
//////  if (!viren2d::SetLogLevel("trace")) {
//////    std::cout << "Could not adjust log level - did you compile viren2d"
//////                 " with an appropriate viren2d_LOG_LEVEL_xxx definition?"
//////              << std::endl;
//////  }
//////  DemoColors();
//////  DemoLines();
////////  DemoArrows();
//////  DemoCircles();
//////  DemoRects();
////  DemoText();
////  DemoMarkers();
//////  DemoBoundingBox2D();
////  DemoPolygon();
//  DemoTrajectories();

//  // TODO create examples out of this sandbox mess

//// demo set canvas from image & draw some rectangles:
//////  // compare image.png cv-image.png diff.png
////#ifdef EXAMPLE_IMAGE_FILE
////  std::string image_filename(EXAMPLE_IMAGE_FILE);
////#else  // EXAMPLE_IMAGE_FILE
////  std::string image_filename("../examples/data/flamingo.jpg");
////#endif  // EXAMPLE_IMAGE_FILE
////  viren2d::ImageBuffer image_buffer = viren2d::LoadImageUInt8(image_filename, 4);

////  auto painter = viren2d::CreatePainter();

////  painter->SetCanvas(image_buffer);

////  for (int i = 0; i < 4; ++i) {
////    viren2d::Rect rect(40 + i*100, 256, 80, 120, i*30, 30);

////    // Invocation with explicit types:
////    painter->DrawRect(rect,
////                      viren2d::LineStyle(6, viren2d::Color("indigo!90")),
////                      viren2d::Color("cyan!20"));

////    rect.cy -= (rect.height + 10);
////    // Invocation with implicit casts:
////    // * Rect can be created from an initializer_list of doubles
////    // * Color can be created from a C string
////    painter->DrawRect({40.0 + i*100.0, 100, 80, 120, i*30.0, 20},
////                      viren2d::LineStyle(6, "taupe!90"),
////                      "cyan!60");
////  }

////  //  painter->DrawLine(viren2d::Vec2d(10, 10), viren2d::Vec2d(image_buffer.width-10, image_buffer.height-10),
////  //                    viren2d::LineStyle(10, viren2d::colors::Maroon(0.8)));

////  painter->DrawLine({10.0, 10.0}, {image_buffer.width-10.0, image_buffer.height-10.0},
////                    viren2d::LineStyle(10, "maroon!80", {}, viren2d::LineCap::Round));

////  viren2d::SaveImage("test.jpg", painter->GetCanvas(false));
////  ShowCanvas(painter->GetCanvas(false), "demo-image-canvas.jpg");


//////  //   ImagePainter painter(painter1); // copy construct
//////  //  auto painter = std::move(painter1);
//////  ////  ImagePainter painter = std::move(painter1); // move constructor
//////  ////  ImagePainter painter; painter = std::move(painter1); // move assignment
//////  ////  ImagePainter painter; painter = painter1; // copy assignment, copy construct & move assignment
