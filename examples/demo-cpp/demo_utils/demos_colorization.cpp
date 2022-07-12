#include <viren2d/viren2d.h>
#include <demo_utils/demos.h>
#include <iostream>

#include <werkzeugkiste/files/filesys.h>

namespace viren2d {
namespace demos {

void DemoOpticalFlow() {
  PrintDemoHeader("Optical Flow Visualization");

#ifdef viren2d_EXAMPLE_DATA_PATH
  const std::string flow_filename = werkzeugkiste::files::FullFile(
        viren2d_EXAMPLE_DATA_PATH, "sintel-alley2.flo");
#else
  const std::string flow_filename("../examples/data/sintel-alley2.flo");
  std::cerr << "Path to example data not given, using default (relative!) path: "
            << flow_filename << std::endl;
#endif

  // Colorize exemplary flow:
  const ColorMap colormap = ColorMap::OpticalFlow;
  const ImageBuffer flow = LoadOpticalFlow(flow_filename);

  double max_motion;
  const ImageBuffer magnitude = flow.Magnitude();
  magnitude.MinMaxLocation(nullptr, &max_motion);
  const ImageBuffer flow_vis = ColorizeOpticalFlow(flow, colormap, max_motion);

  const ImageBuffer mask = magnitude.MaskRange<float>(max_motion - 5, max_motion);
  ProcessDemoOutput(mask, "demo-mask.jpg");
  std::cout << "Maximum flow magnitude: " << max_motion
            << ", " << magnitude << std::endl;

  // Then, overlay the flow wheel legend:
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

} // namespace demos
} // namespace viren2d
