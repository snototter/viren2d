// Get a deep copy of the painter's canvas (for the following channel swap):
viren2d::ImageBuffer canvas = painter->GetCanvas(true);

// Optionally, if you've set up OpenCV, you can simply create a cv::Mat header
// from the ImageBuffer. Note that OpenCV expects images in BGR(A) format:
#include <opencv2/opencv.hpp>

canvas.SwapChannels(0, 2);
cv::Mat cv_buffer(
    canvas.Height(), canvas.Width(),
    CV_MAKETYPE(CV_8U, canvas.Channels()),
    canvas.MutableData(), canvas.RowStride());
