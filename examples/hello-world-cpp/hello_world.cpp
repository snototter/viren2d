#include <iostream>

#ifdef WITH_OPENCV
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#endif // WITH_OPENCV

#include <viren2d/viren2d.h>


int main(int /*argc*/, char **/*argv*/) {
  auto painter = viren2d::CreatePainter();
  
  painter->SetCanvas(400, 50, viren2d::Color::White);

  // Draw "Hello World"
  viren2d::TextStyle text_style(35.0, "sans-serif", "crimson", true, false);

  viren2d::Rect bbox = painter->DrawText(
      {"Hello World!"}, {200.0, 25.0}, viren2d::Anchor::Center, text_style);

  // Draw markers to the left & right:
  viren2d::MarkerStyle marker_style(
        viren2d::Marker::Pentagram, 40.0, 1.0, "midnight-blue", true);

  painter->DrawMarker({bbox.left() - 30.0, 25.0}, marker_style);
  painter->DrawMarker({bbox.right() + 30.0, 25.0}, marker_style);

#ifdef WITH_OPENCV
  // Request a copy of the canvas, because an ImageBuffer swaps
  // its channels **in-place**.
  viren2d::ImageBuffer img_buffer = painter->GetCanvas(true);

  // Convert color format to BGR(A) for OpenCV
  img_buffer.SwapChannels(0, 2);

  // Create an OpenCV matrix header which reuses the memory (to
  // avoid additional memory allocation)
  cv::Mat cv_buffer(
      img_buffer.Height(), img_buffer.Width(),
      CV_MAKETYPE(CV_8U, img_buffer.Channels()),
      img_buffer.MutableData(), img_buffer.RowStride());

  cv::imshow("Canvas", cv_buffer);
  cv::waitKey(-1);
#endif // WITH_OPENCV
  return 0;
}
