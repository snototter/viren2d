#include <viren2d/viren2d.h>
#include <demo_utils/demos.h>
#include <iostream>

#include <werkzeugkiste/files/filesys.h>

namespace viren2d {
namespace demos {

void DemoOpticalFlowColorization(
    const ImageBuffer &flow_vis, ColorMap colormap) {
  // Overlay the flow wheel legend:
  std::unique_ptr<Painter> painter = CreatePainter();
  painter->SetCanvas(flow_vis);

  LineStyle grid_style(2, "black!60");
  const ImageBuffer legend = OpticalFlowLegend(
        120, colormap, grid_style);
  painter->DrawImage(
        legend, {flow_vis.Width() - 10.0, flow_vis.Height() - 10.0},
        Anchor::BottomRight, 1.0, 1.0, 1.0, 0.0, 1.0, grid_style);

  ProcessDemoOutput(painter->GetCanvas(false), "demo-output-optical-flow.png");
}


void DemoOpticalFlowOverlay(
    const ImageBuffer &flow_vis, ColorMap colormap,
    const std::string &image_filename) {
  const ImageBuffer input_image = LoadImageUInt8(image_filename);
  const ImageBuffer gray = ConvertRGB2Gray(input_image, 3);
  const ImageBuffer image = input_image.Blend(gray, 0.8);

  LinearColorGradient gradient(
        Vec2d(0, 0), Vec2d(image.Width(), image.Height()));
  gradient.AddIntensityStop(0.1, 1.0);
  gradient.AddIntensityStop(0.3, 0.1);
  gradient.AddIntensityStop(0.5, 0.0);
  gradient.AddIntensityStop(0.7, 0.1);
  gradient.AddIntensityStop(0.9, 1.0);
  const ImageBuffer weights = gradient.Mask(image.Width(), image.Height(), 1);

  ImageBuffer overlay = flow_vis.Blend(image, weights);

  std::unique_ptr<Painter> painter = CreatePainter();
  painter->SetCanvas(overlay);

  LineStyle grid_style(2, "black!60");
  const ImageBuffer legend = OpticalFlowLegend(
        120, colormap, grid_style);
  painter->DrawImage(
        legend, {flow_vis.Width() - 10.0, flow_vis.Height() - 10.0},
        Anchor::BottomRight, 1.0, 1.0, 1.0, 0.0, 1.0, grid_style);

  ProcessDemoOutput(painter->GetCanvas(false), "demo-output-optical-flow-overlay.jpg");
}


void DemoOpticalFlow() {
  PrintDemoHeader("Optical Flow Visualization");

#ifdef viren2d_EXAMPLE_DATA_PATH
  const std::string flow_filename = werkzeugkiste::files::FullFile(
        viren2d_EXAMPLE_DATA_PATH, "sintel-alley2.flo");
  const std::string image_filename = werkzeugkiste::files::FullFile(
        viren2d_EXAMPLE_DATA_PATH, "sintel-alley2.png");
#else
  const std::string flow_filename("../examples/data/sintel-alley2.flo");
  const std::string image_filename("../examples/data/sintel-alley2.png");
  std::cerr << "Path to example data not configured, using default "
               "(relative!) path: " << flow_filename << std::endl;
#endif

  // Colorize exemplary flow:
  const ColorMap colormap = ColorMap::OpticalFlow;
  const ImageBuffer flow = LoadOpticalFlow(flow_filename);

  double max_motion;
  const ImageBuffer magnitude = flow.Magnitude();
  magnitude.MinMaxLocation(nullptr, &max_motion);
  const ImageBuffer flow_vis = ColorizeOpticalFlow(flow, colormap, max_motion);

  //TODO remove or refactor into separate demo
//  const ImageBuffer mask = magnitude.MaskRange<float>(max_motion - 5, max_motion);
//  ProcessDemoOutput(mask, "demo-mask.jpg");
//  std::cout << "Maximum flow magnitude: " << max_motion
//            << ", " << magnitude << std::endl;
//  const ImageBuffer normed = magnitude.Normalize<ImageBufferType::UInt8, float>(0, 255/16, 0);
//  ProcessDemoOutput(normed, "demo-normalized.jpg");

  DemoOpticalFlowColorization(flow_vis, colormap);
  DemoOpticalFlowOverlay(flow_vis, colormap, image_filename);
}

} // namespace demos
} // namespace viren2d
