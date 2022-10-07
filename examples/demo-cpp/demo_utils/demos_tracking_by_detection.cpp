#include <cstdlib>
#include <vector>

#include <viren2d/viren2d.h>

#include <demo_utils/demos.h>

namespace viren2d {
namespace demos {
/// Returns `rand() % n`.
int randmod(int n) {
  // std::rand will return a value between 0 and RAND_MAX.
  // This can cause a modulo bias, refer to https://stackoverflow.com/a/10984975
  // for further details and the sampling code below:
  int x;

  do {
    x = std::rand();
  } while (x >= (RAND_MAX - RAND_MAX % n));

  return (x % n);
}


void DemoBoundingBoxes2D() {
  PrintDemoHeader("Bounding Boxes in 2D");

  auto painter = CreatePainter();
  std::vector<std::string> lbl_top = {"This is at", "the top"};
  std::vector<std::string> lbl_bottom = {"This is along", "the bottom"};
  std::vector<std::string> lbl_left = {"The left edge"};
  std::vector<std::string> lbl_right = {"The right edge"};

  painter->SetCanvas(800, 1200, Color::White);
  painter->DrawGrid({}, {}, 50, 50, LineStyle(1.0, "gray!40"));

  BoundingBox2DStyle style(
        LineStyle(3, "navy-blue"), TextStyle(14, "xkcd", "midnight-blue"),
        "same!20", "orange!20", {6, 6}, true);

  painter->DrawBoundingBox2D(
        Rect::FromLTWH(50, 50, 100, 220, 0.2), style, lbl_top, lbl_bottom);

  painter->DrawBoundingBox2D(
        Rect::FromLTWH(200, 50, 100, 220, 0.2), style, {}, {},
        lbl_left, false);

  painter->DrawBoundingBox2D(
        Rect::FromLTWH(350, 50, 100, 220, 0.2), style, {}, {},
        {}, true, lbl_right, true);

  painter->DrawBoundingBox2D(
        Rect::FromLTWH(500, 50, 100, 220, 0.2), style, {}, {},
        lbl_left, false, lbl_right, false);

  painter->DrawBoundingBox2D(
        Rect::FromLTWH(650, 50, 100, 220, 0.2), style, {}, {},
        lbl_left, true, lbl_right, false);

  painter->DrawBoundingBox2D(
        Rect::FromLTWH(800, 50, 100, 220, 0.2), style, lbl_top, lbl_bottom,
        lbl_left, true, lbl_right, false);

  // Don't fill the text box:
  style.text_fill_color = Color::Invalid;
  painter->DrawBoundingBox2D(
        Rect::FromLTWH(50, 300, 100, 220, 0.2), style, lbl_top, lbl_bottom);

  painter->DrawBoundingBox2D(
        Rect::FromLTWH(200, 300, 100, 220, 0.2), style, {}, {},
        lbl_left, false, lbl_right, true);

  painter->DrawBoundingBox2D(
        Rect::FromLTWH(350, 300, 100, 220, 0.2), style, {}, {},
        lbl_left, true, lbl_right, true);

  painter->DrawBoundingBox2D(
        Rect::FromLTWH(500, 300, 100, 220, 0.2), style, {}, {},
        lbl_left, false, lbl_right, false);

  painter->DrawBoundingBox2D(
        Rect::FromLTWH(650, 300, 100, 220, 0.2), style, {}, {},
        lbl_left, true, lbl_right, false);

  painter->DrawBoundingBox2D(
        Rect::FromLTWH(800, 300, 100, 220, 0.2), style, lbl_top, lbl_bottom,
        lbl_left, true, lbl_right, false);

  ProcessDemoOutput(painter->GetCanvas(false), "demo-output-bbox2d.png");
}


void DemoTrajectories() {
  PrintDemoHeader("Trajectories");
  auto painter = CreatePainter();
  painter->SetCanvas(800, 800, Color::White);

  painter->DrawGrid({0, 0}, {0, 0}, 50, 50, LineStyle(1, "gray"));

  std::vector<Vec2d> trajectory{{500, 100}, {600, 150}, {550, 300}, {500, 150}};
  auto style = LineStyle(5, "midnight-blue", {}, 0.0, LineCap::Round);
  bool oldest_first = false;
  for (const auto &offset : {Vec2d(-400, 50), Vec2d(-100, 200), Vec2d(50, 300)}) {
    std::vector<Vec2d> traj;
    for (const auto &pt : trajectory) {
      traj.push_back(pt + offset);
    }
    painter->DrawTrajectory(traj, style, Color("gray!20"), oldest_first);
    oldest_first = !oldest_first;
  }

  trajectory = {
    {50, 400}, {100, 450}, {150, 400}, {200, 450},
    {150, 500}, {200, 550}, {250, 500}, {300, 600},
    {350, 500}, {400, 600}, {450, 550}, {500, 700}};
  style.color = "red";
  painter->DrawTrajectory(
        trajectory, style, "black!100", true, 5, ColorFadeOutLinear);

  std::vector<Vec2d> pts;
  for (const auto &pt : trajectory)
    pts.push_back(pt + Vec2d{0.0, 100.0});
  painter->DrawTrajectory(pts, style, "black!100");

  pts.clear();
  for (const auto &pt : trajectory)
    pts.push_back(pt + Vec2d{0.0, 200.0});
  painter->DrawTrajectory(pts, style, "black!100");

  ProcessDemoOutput(painter->GetCanvas(false), "demo-output-trajectories.png");
  painter.reset();
}



} // namespace demos
} // namespace viren2d
