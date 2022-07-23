#include <viren2d/viren2d.h>
#include <demo_utils/demos.h>
#include <iostream>

#include <werkzeugkiste/files/filesys.h>

#ifdef viren2d_WITH_OPENCV
#  include <opencv2/core.hpp>
#  include <opencv2/core/eigen.hpp>
#endif // viren2d_WITH_OPENCV

namespace viren2d {
namespace demos {

void DemoPinhole() {
  PrintDemoHeader("Pinhole Camera Visualizations");

#ifdef viren2d_EXAMPLE_DATA_PATH
  const std::string image_filename = werkzeugkiste::files::FullFile(
        viren2d_EXAMPLE_DATA_PATH, "ninja.jpg");
#else
  const std::string image_filename("../examples/data/ninja.jpg");
  std::cerr << "Path to example data not given, using default (relative!) path: "
            << image_filename << std::endl;
#endif

  ImageBuffer img = LoadImageUInt8(image_filename);

//  DrawXYZAxesImpl
  std::unique_ptr<Painter> painter = CreatePainter();
  painter->SetCanvas(img);

#ifdef viren2d_WITH_OPENCV
  cv::Mat Kcv = (cv::Mat_<double>(3,3) << 523.17808219, 0.0, 341.0,
                 0.0, 523.17808219, 256.0,
                 0.0, 0.0, 1.0);
  cv::Mat Rcv = (cv::Mat_<double>(3,3) << 0.99013141,  0.14006482, -0.00465153,
                 0.05439048, -0.41465762, -0.90835056,
                -0.12915675,  0.89913342, -0.41818372);
  Matrix3x3d K, R;
  cv::cv2eigen(Kcv, K);
  cv::cv2eigen(Rcv, R);
#else
  Matrix3x3d K, R;
  K << 523.17808219, 0.0, 341.0,
       0.0, 523.17808219, 256.0,
       0.0, 0.0, 1.0;
  R <<  0.99013141,  0.14006482, -0.00465153,
        0.05439048, -0.41465762, -0.90835056,
       -0.12915675,  0.89913342, -0.41818372;
#endif
  Vec3d t(-51.84341161, 17.32680283, 82.51435241);
//  viren2d::Vec3d t(-5, 24, 90);


// TODO move color map customization to colormaps demo
//   SetCustomColorMap(ColorMap::Custom1, {"maroon", "forest-green", "azure"});
//       Color::FromObjectID(0, ColorMap::Custom1),
//       Color::FromObjectID(1, ColorMap::Custom1),
//       Color::FromObjectID(2, ColorMap::Custom1)


  painter->DrawXYZAxes(
        K, R, t, {12, 12, 0}, Vec3d::All(48),
        ArrowStyle(7, Color::Black, 35, 25, false));

  ProcessDemoOutput(painter->GetCanvas(false), "demo-output-pinhole.png");
}

} // namespace demos
} // namespace viren2d
