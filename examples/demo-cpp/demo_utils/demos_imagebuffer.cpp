#include <iostream>

#ifdef viren2d_WITH_OPENCV
#  include <opencv2/opencv.hpp>
#  include <opencv2/highgui.hpp>
#endif // viren2d_WITH_OPENCV

#include <werkzeugkiste/files/filesys.h>

#include <viren2d/viren2d.h>
#include <demo_utils/demos.h>

namespace viren2d {
namespace demos {
//TODO split into different demos
void DemoImageBufferConversionOpenCV() {
  PrintDemoHeader("ImageBuffer - OpenCV Conversion");

#ifdef viren2d_WITH_OPENCV
#  ifdef viren2d_EXAMPLE_DATA_PATH
  const std::string image_filename = werkzeugkiste::files::FullFile(
        viren2d_EXAMPLE_DATA_PATH, "flamingo.jpg");
#  else  // viren2d_EXAMPLE_IMAGE_FILE
  const std::string image_filename("../examples/data/flamingo.jpg");
  std::cerr << "Path to example data not given, using default (relative!) path: "
            << image_filename << std::endl;
#  endif  // viren2d_EXAMPLE_IMAGE_FILE

  cv::Mat img_cv = cv::imread(image_filename);

  // To make this example a bit more interesting,
  // let's get a non-continuous OpenCV matrix:
  cv::Mat roi = img_cv.colRange(50, img_cv.cols - 50);

  ///// FIXME make separate ROI demo
  ImageBuffer buf;
  buf.CreateSharedBuffer(
        roi.data, roi.rows, roi.cols,
        roi.channels(), roi.step1(0), roi.step1(1),
        ImageBufferType::UInt8);

  ImageBuffer gray = viren2d::ConvertRGB2Gray(buf, 3);
  ImageBuffer blend = buf.Blend(gray, 0.7);

  ImageBuffer hsv = viren2d::ConvertRGB2HSV(buf, true);
  viren2d::SaveImageUInt8("todo-hsv.png", hsv);

  ImageBuffer rgb = viren2d::ConvertHSV2RGB(hsv, 3);
  viren2d::SaveImageUInt8("todo-rgb.png", rgb);

  {//FIXME refactor this into a proper demo!
    cv::Mat cvtmp(blend.Height(), blend.Width(),
                    CV_MAKETYPE(CV_8U, blend.Channels()),
                    blend.MutableData(), blend.RowStride());

      cv::imshow("Blend", cvtmp);
  }


  for (int ch = 0; ch < buf.Channels(); ++ch) {
    double min, max;
    Vec2i minloc, maxloc;
    buf.MinMaxLocation(&min, &max, &minloc, &maxloc, ch);
    std::cout << "Channel " << ch << ", min at " << minloc << " ("
              << min << "), max at " << maxloc << " (" << max << ")" << std::endl;
  }
//  SetLogLevel(LogLevel::Trace);
  LoadImageUInt8("/home/snototter/workspace/utilities/vito/examples/depth.png", 0);
  buf.Pixelate(15, 23, 0, 50, 250, 200);
//  ImageBuffer buf_roi = buf.ROI(0, 50, 250, 200);
//  for (int r = 0; r < buf_roi.Height(); ++r) {
//    for (int c = 0; c < buf_roi.Width(); ++c) {
//      buf_roi.AtChecked<uint8_t>(r, c, 0) = 255;
//      buf_roi.AtChecked<uint8_t>(r, c, 1) = 0;
//      buf_roi.AtChecked<uint8_t>(r, c, 2) = 255;
//    }
//  }


  // Create a shared buffer (on purpose) and change
  // color format to demonstrate the potential side-effect:
  ImageBuffer img_buf;
  img_buf.CreateSharedBuffer(
        roi.data, roi.rows, roi.cols,
        roi.channels(), roi.step1(0), roi.step1(1),

        ImageBufferType::UInt8);
  img_buf.SwapChannels(0, 2);

  // Now, use the ImageBuffer to set up a canvas and
  // draw something
  auto painter = CreatePainter();
  painter->SetCanvas(img_buf);

  painter->DrawArrow(
        {0.0, 0.0}, {img_buf.Width() / 2.0, img_buf.Height() / 2.0},
        ArrowStyle(10, "navy-blue!80", 0.2, 20.0));

  // Retrieve the visualization, and show the image.
  // Since we'll use cv::imshow, we need to convert
  // the buffer to BGR(A) format:
  ImageBuffer copy = painter->GetCanvas(true);
  copy.SwapChannels(0, 2);
  cv::Mat cv_buffer(copy.Height(), copy.Width(),
                    CV_MAKETYPE(CV_8U, copy.Channels()),
                    copy.MutableData(), copy.RowStride());

  cv::imshow("Painter's Canvas", cv_buffer);
  cv::imshow("Shared Buffer Side Effects", img_cv);

  cv::waitKey();
  painter.reset();
#else  // viren2d_WITH_OPENCV
  std::cerr << "OpenCV is not available - cannot show the conversion demo." << std::endl;
#endif  // viren2d_WITH_OPENCV
}


} // namespace demos
} // namespace viren2d
