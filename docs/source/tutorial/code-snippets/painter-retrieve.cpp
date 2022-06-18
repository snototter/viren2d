// Get the visualization as `viren2d::ImageBuffer`:
auto canvas = painter->GetCanvas(true);

// Optionally, if you've set up OpenCV, you can simply create a cv::Mat header
// from the ImageBuffer. Note that OpenCV expects images in BGR(A) format:
#include <opencv2/opencv.hpp>
canvas.RGB2BGR();
cv::Mat cv_buffer(
    canvas.height, canvas.width,
    CV_MAKETYPE(CV_8U, canvas.channels),
    canvas.data, canvas.stride);
