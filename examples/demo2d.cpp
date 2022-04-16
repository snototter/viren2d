/*
check rgb/bgr
https://stackoverflow.com/questions/66127550/python-cairo-save-figure-as-np-array-gives-weird-results
opencv memory layout:
https://docs.opencv.org/4.x/db/da5/tutorial_how_to_scan_images.html

cpp style guide
https://google.github.io/styleguide/cppguide.html#Pointer_and_Reference_Expressions
r vs lvalue: ;-) https://stackoverflow.com/a/33829750/400948

pybind11 return numpy:
https://stackoverflow.com/questions/44659924/returning-numpy-arrays-via-pybind11

Image loading lightweight:
https://stackoverflow.com/a/40812978/400948
--> recommends https://github.com/nothings/stb

Logging:
https://github.com/gabime/spdlog

Clean library with CMake
https://github.com/robotology/how-to-export-cpp-library/blob/master/CMakeLists.txt

CMake set up header only library:
https://stackoverflow.com/questions/28629084/how-to-get-a-header-only-library-portably-from-version-control-using-cmake

CMake include other target
https://stackoverflow.com/a/61097014/400948

TODO repo structure
* examples/vivi-demo.cpp
* include/vivi/
* src/vivi
* tests / would be nice...

Cairo tutorials
https://www.cairographics.org/tutorial/
https://www.cairographics.org/FAQ/#paint_from_a_surface
https://zetcode.com/gfx/cairo/cairobackends/

FindCairo.cmake
https://github.com/preshing/CairoSample/blob/master/modules/FindCairo.cmake

opencv <--> eigen3
https://stackoverflow.com/questions/14783329/opencv-cvmat-and-eigenmatrix
*/

#include <iostream>

//#include <cairo/cairo.h>
//#include <cairo/cairo-svg.h>


//#include <Eigen/Dense>

//#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
//#include <opencv2/core/eigen.hpp>
#include <opencv2/highgui.hpp>

#include <vivi/vivi.hpp>

int main(int argc, char **argv)
{
  //------------------------------------------------------
  vivi::Vec2d vd1(1, 2), vd2(3, 9);
  vivi::Vec3i vi1(13, 42, -1), vi2(13,42, 0);
  std::cout << vd1 << " == " << vd2 << ": " << (vd1 == vd2) << std::endl
            << vd1 << " == " << vd1 << ": " << (vd1 == vd1) << std::endl
            << vi1 << " == " << vi2 << ": " << (vi1 == vi2) << std::endl
            << vi1 << " == " << vi1 << ": " << (vi1 == vi1) << std::endl;

//  vivi::Vec3d cast = static_cast<vivi::Vec3d>(vi3);
//  std::cout << "Casted?: " << cast << std::endl;
  std::cout << "Arithmetic: " << vd1 << " - " << vd2 << " = " << (vd1 - vd2) << std::endl
            << "  '+' = " << (vd1 + vd2) << std::endl
            << "3 * " << vd1 << " = " << (3 * vd1) << " == lhs|rhs == " << (vd1 * 3) << std::endl
            << vd1 << "/ 2 = " << (vd1 / 2) << std::endl
            << vi1 << "/ 2 = " << (vi1 / 2) << std::endl << std::endl
            << "Length " << vi1 << " = " << vi1.Length() << std::endl
            << "L2 to  " << vi2 << " = " << vi1.Distance(vi2) << std::endl << std::endl;

  //------------------------------------------------------

  vivi::Color color = vivi::RGBA(255, 0, 255);
  std::cout << color.ToString() << std::endl;
  //------------------------------------------------------

  // compare image.png cv-image.png diff.png
  std::string image_filename("../examples/flamingo.jpg");
  cv::Mat orig = cv::imread(image_filename);
  cv::Mat img = orig;

  auto painter = vivi::CreateImagePainter();//vivi::ImagePainter painter1;
//  painter->SetCanvas(img);

  painter->SetCanvas(image_filename);
//  painter->SetCanvas(600, 400, vivi::RGBA(255, 255, 255));

  for (int i = 0; i < 4; ++i)
  {
//    painter->SetCanvas(img);
    painter->DrawRect(vivi::Rect(40 + i*100, img.rows/2, 80, 120, i*30, 30),
                      vivi::LineStyle(6, vivi::colors::Indigo(0.9), {10, 10}),
                      vivi::colors::Cyan(0.2));
    cv::Mat mat = painter->GetCanvas();
    cv::imshow("img", mat);
    cv::waitKey(200);
  }
  painter->SaveCanvas("canvas-stb-test.png");

//  painter->SetCanvas(img.cols, img.rows, vivi::RGBA(0, 0, 200));
  painter->DrawLine(vivi::Vec2d(10, 10), vivi::Vec2d(img.cols-10, img.rows-10),
                    vivi::LineStyle(10, vivi::RGBA(200, 0, 200)));

  painter->DrawLine(vivi::Vec2d(10, 10), vivi::Vec2d(img.cols-10, img.rows-10),
                    vivi::LineStyle(6, vivi::RGBA(0, 0, 200), {20, 10}));

  painter->DrawCircle(vivi::Vec2d(70, 90), 35,
                      vivi::LineStyle(6, vivi::RGBA(0, 0, 200), {20, 10}),
                      vivi::rgba(0, 1, 1, .3));


  //   ImagePainter painter(painter1); // copy construct
  //  auto painter = std::move(painter1);
  ////  ImagePainter painter = std::move(painter1); // move constructor
  ////  ImagePainter painter; painter = std::move(painter1); // move assignment
  ////  ImagePainter painter; painter = painter1; // copy assignment, copy construct & move assignment


  painter->DummyShow();
  return 0;
}
