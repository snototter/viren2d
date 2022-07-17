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
//  std::vector<LabelPosition> label_positions {LabelPosition::Top,
//        LabelPosition::Bottom, LabelPosition::LeftB2T,
//        LabelPosition::RightB2T};

  painter->SetCanvas(600, 600, Color::White);

  painter->DrawGrid({}, {}, 50, 50, LineStyle(1.0, "gray!60"));

  BoundingBox2DStyle style;
  style.label_padding.SetX(10);
  style.label_padding.SetY(5);
  style.text_fill_color = Color::Same.WithAlpha(0.3);
  style.clip_label = true;
  style.text_style = TextStyle(10, "monospace");

//  style.label_position = label_positions[randmod(label_positions.size())];
  style.text_style.halign = HorizontalAlignment::Left;

  painter->DrawBoundingBox2D(
        Rect::FromLTWH(50, 50, 100, 80, 0.2),
        {"fill B & T, clip ABCDEFG"}, style);


  style.box_fill_color = Color::Same.WithAlpha(0.5);
  style.text_fill_color = Color::Same.WithAlpha(0.2);
//  style.label_position = label_positions[randmod(label_positions.size())];
  style.clip_label = false;
  painter->DrawBoundingBox2D(
        Rect::FromLTWH(50, 150, 100, 80, 0.2),
        {"fill B 0.5 & T 0.2, noclip"}, style);


  style.box_fill_color = Color::Invalid;
  style.text_fill_color = Color::Same.WithAlpha(0.3);
//  style.label_position = label_positions[randmod(label_positions.size())];
  style.clip_label = true;
  painter->DrawBoundingBox2D(
        Rect::FromLTWH(50, 250, 100, 80, 0.2),
        {"fill T 0.3, clip label"}, style);


  style.box_fill_color = Color("crimson!50");
  style.text_fill_color = Color::Invalid;
//  style.label_position = label_positions[randmod(label_positions.size())];
  style.clip_label = false;
  painter->DrawBoundingBox2D(
        Rect::FromLTWH(50, 350, 100, 80, 0.2),
        {"fill B crimson!50, noclip", style.ToString()}, style);

  style.box_fill_color = Color::Invalid;
  style.text_fill_color = Color::Invalid;
//  style.label_position = label_positions[randmod(label_positions.size())];
  style.clip_label = true;
  painter->DrawBoundingBox2D(
        Rect::FromLTWH(50, 450, 100, 80, 0),
        {"no fill, but clip", style.ToString()}, style);


  style.clip_label = true;
  style.box_fill_color = Color::Same.WithAlpha(0.3);
  style.text_fill_color = Color("crimson!60");
  style.text_style = TextStyle(14, "Arial", "black", true);
//  style.label_position = label_positions[randmod(label_positions.size())];

  style.text_style.halign = HorizontalAlignment::Center;
  painter->DrawBoundingBox2D(
        Rect::FromLTWH(200, 50, 100, 250, 0.4),
        {"Centered", "label"}, style);


  style.text_style.halign = HorizontalAlignment::Left;
  style.text_fill_color = Color::Same.WithAlpha(0.3);
//  style.label_position = label_positions[randmod(label_positions.size())];
  painter->DrawBoundingBox2D(
        Rect::FromLTWH(350, 50, 100, 250),
        {"Left-aligned", "label"}, style);

  style.clip_label = true;
  style.box_fill_color = Color::Invalid;
  style.text_fill_color = Color::Same.WithAlpha(0.5);
  style.text_style.halign = HorizontalAlignment::Right;
//  style.label_position = label_positions[randmod(label_positions.size())];
  painter->DrawBoundingBox2D(
        Rect::FromLTWH(475, 50, 100, 250),
        {"Right-aligned", "label (clipped)"}, style);

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
