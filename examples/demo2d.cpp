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
  vivi::Color color = vivi::RGBA(255, 0, 255);
  std::cout << color.ToString() << std::endl;

  // compare image.png cv-image.png diff.png
  cv::Mat orig = cv::imread("/home/snototter/workspace/sandbox/vivi/cpp-only/flamingo.jpg");
  cv::Mat img = orig;

  auto painter1 = vivi::CreateImagePainter();//vivi::ImagePainter painter1;
  painter1->SetCanvas(img);

//   ImagePainter painter(painter1); // copy construct
  auto painter = std::move(painter1);
//  ImagePainter painter = std::move(painter1); // move constructor
//  ImagePainter painter; painter = std::move(painter1); // move assignment
//  ImagePainter painter; painter = painter1; // copy assignment, copy construct & move assignment
  painter->SetCanvas(img.cols, img.rows, vivi::RGBA(0, 0, 200));

//  painter->DrawLine(cv::Vec2d(10, 10), cv::Vec2d(img.cols-10, img.rows-10), 3.9, cv::Vec3d(1, 0, 0));
//  painter->DrawLine(cv::Vec2d(10, 10), cv::Vec2d(img.cols-10, 10), 1, cv::Vec3d(1, 0, 1));
  img = painter->GetCanvas();
  std::cout << "img?" << img.empty() << std::endl;

  cv::imshow("image", img);
//  if (!painter1->Empty())
//    cv::imshow("prev-painter", painter1->GetCanvas());

  cv::waitKey();
  return 0;
}

//int main(int argc, char **argv)
//{
//  // compare image.png cv-image.png diff.png
//  cv::Mat img = cv::imread("/home/snototter/workspace/sandbox/vivi/cpp-only/flamingo.jpg");

////  cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, img.cols, img.rows);
//  cairo_surface_t *surface_img = Mat2Cairo(img);
//  cairo_surface_t *surface = cairo_svg_surface_create("svgtest.svg", img.cols, img.rows); //TODO naive assumption pt == px
////  cairo_svg_surface_restrict_to_version(surface, CAIRO_SVG_VERSION_1_1);
////  cairo_svg_surface_set_document_unit(surface, CAIRO_SVG_UNIT_PX);

//  cairo_t *cr = cairo_create(surface);

//    cairo_save(cr);
//    cairo_set_source_rgba(cr, 0, 0, 1, 1);
//    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
//        CAIRO_FONT_WEIGHT_BOLD);
//    cairo_set_font_size(cr, 20.0);

//    cairo_move_to(cr, 10.0, 50.0);
//    cairo_show_text(cr, "ABCabcqQ?!# 0.123 [a](b){c}");
//    cairo_restore(cr);

//  cairo_surface_flush(surface_img);
//  cairo_surface_finish(surface_img);
//  cairo_set_source_surface(cr, surface_img, 0, 0);
//  cairo_paint(cr);
////  cairo_surface_flush(surface);
////  cairo_surface_finish(surface);

////  cairo_save(cr);
////  cairo_set_source_rgba(cr, 0, 0, 1, 1);
////  cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
////      CAIRO_FONT_WEIGHT_BOLD);
////  cairo_set_font_size(cr, 20.0);

////  cairo_move_to(cr, 10.0, 50.0);
////  cairo_show_text(cr, "ABCabcqQ?!# 0.123 [a](b){c}");
////  cairo_restore(cr);

////  cairo_set_source_rgb(cr_svg, 0, 0, 0);
////    cairo_select_font_face(cr_svg, "Sans", CAIRO_FONT_SLANT_NORMAL,
////        CAIRO_FONT_WEIGHT_NORMAL);
////    cairo_set_font_size(cr_svg, 40.0);

////    cairo_move_to(cr_svg, 10.0, 50.0);
////    cairo_show_text(cr_svg, "Disziplin ist Macht.");

//  cairo_destroy(cr);
//  cairo_surface_destroy(surface);
//  cairo_surface_destroy(surface_img);

//  return 0;
//}
