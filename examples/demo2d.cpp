#include <iostream>

//#include <Eigen/Dense>

#ifdef WITH_OPENCV
#include <opencv2/opencv.hpp>
//#include <opencv2/core/eigen.hpp>
#include <opencv2/highgui.hpp>
#endif // WITH_OPENCV

#include <viren2d/viren2d.h>


int main(int /*argc*/, char **/*argv*/) {
  std::cout << viren2d::Color(viren2d::NamedColor::Black).ToString() << std::endl
            << viren2d::Color("white", 0.1).ToString() << std::endl
            << viren2d::Color("red").ToHexString() << std::endl
            << viren2d::Color("green", 0.1).ToHexString() << std::endl;
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
  std::string image_filename("../examples/flamingo.jpg");
  viren2d::ImageBuffer image_buffer = viren2d::LoadImage(image_filename, 4);

  auto painter = viren2d::CreateImagePainter();

////  painter->SetCanvas(image_filename);
  painter->SetCanvas(image_buffer);
////  painter->SetCanvas(600, 400, viren2d::RGBA(255, 255, 255));

  for (int i = 0; i < 4; ++i) {
    painter->DrawRect(viren2d::Rect(40 + i*100, 256, 80, 120, i*30, 30),
                      viren2d::LineStyle(6, viren2d::Color("indigo!90")),
                      viren2d::Color("cyan!20"));
  }

////  painter->SetCanvas(img.cols, img.rows, viren2d::RGBA(0, 0, 200));
//  painter->DrawLine(viren2d::Vec2d(10, 10), viren2d::Vec2d(image_buffer.width-10, image_buffer.height-10),
//                    viren2d::LineStyle(10, viren2d::colors::Maroon(0.8)));

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

#ifdef WITH_OPENCV
  // The last bit of OpenCV dependency (only for displaying the image ;-)
  viren2d::ImageBuffer img_buffer = painter->GetCanvas(true);
  img_buffer.RGB2BGR(); // Warning: Currently, the buffer is shared!!
  cv::Mat cv_buffer(img_buffer.height, img_buffer.width,
                    CV_MAKETYPE(CV_8U, img_buffer.channels),
                    img_buffer.data, img_buffer.stride);
  cv::imshow("ImageBuffer --> cv::Mat", cv_buffer);
  cv::waitKey();
#endif // WITH_OPENCV

  return 0;
}
