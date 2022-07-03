
#include <viren2d/viren2d.h>
#include <demo_utils/demos.h>

namespace viren2d {
namespace demos {
void DemoArrows() {
  PrintDemoHeader("Arrows");

  auto painter = viren2d::CreatePainter();
  painter->SetCanvas(800, 800, viren2d::Color::White);

  painter->DrawGrid({}, {}, 50, 50,
                    viren2d::LineStyle(1.0, "gray!80"));

  auto style = viren2d::ArrowStyle(6, "navy-blue", 0.15, 20.0,
                                   true, false, {}, 0.0,
                                   viren2d::LineCap::Butt,
                                   viren2d::LineJoin::Round);

  auto size = painter->GetCanvasSize();
  viren2d::Vec2d center(size.x() / 2.0, size.y() / 2.0);
  const double radius = std::min(size.x(), size.y()) / 2.0 - 50;

  for (int angle = 0; angle < 360; angle += 15) {
    const double angle_rad = angle * M_PI / 180.0;
    auto tip = center + radius * viren2d::Vec2d(std::cos(angle_rad),
                                                std::sin(angle_rad));
    painter->DrawArrow(center, tip, style);
  }

  // Closed solid arrow (top-left)
  painter->DrawArrow({50, 50}, {200, 50},
                     viren2d::ArrowStyle(4, "forest-green", 0.15, 30, true, true));

  // Open solid arrow (top-right)
  painter->DrawArrow({size.x() - 50.0, 50.0}, {size.x() - 200.0, 50.0},
                     viren2d::ArrowStyle(4, "crimson!80", 0.15, 30, false, true));

  // Closed dashed arrow
  painter->DrawArrow({50, size.y() - 50.0}, {200, size.y() - 50.0},
                     viren2d::ArrowStyle(4, "forest-green", 0.15, 30, true, true, {15, 10}));

  // Open dashed arrow (bottom-right)
  painter->DrawArrow({size.x() - 50.0, size.y() - 50.0}, {size.x() - 200.0, size.y() - 50.0},
                     viren2d::ArrowStyle(4, "crimson!60", 0.15, 30, false, true,
                                         {15, 10}));

  ProcessDemoOutput(painter->GetCanvas(false), "demo-output-arrows.png");
  painter.reset();
}

} // namespace demos
} // namespace viren2d
