#include <vector>

#include <werkzeugkiste/geometry/primitives.h>
namespace wkg = werkzeugkiste::geometry;

#include <viren2d/viren2d.h>

#include <demo_utils/demos.h>


namespace viren2d {
namespace demos {
/// Draws the two circles and their tangents.
void TangentsDemoHelper(
    std::unique_ptr<Painter> &painter,
    const wkg::Circle &circ1,
    const wkg::Circle &circ2) {
  LineStyle tangent_style(3, Color::CoordinateAxisColor('x'));
  LineStyle circle_style(3, "azure");

  // Draw the transverse common tangents:
  wkg::Line2d t1, t2;
  const int num_transverse = circ1.TransverseCommonTangents(circ2, &t1, &t2);

  std::vector<wkg::Line2d> transverse_tangents;
  if (num_transverse > 0) {
    transverse_tangents.push_back(t1);
  }
  if (num_transverse > 1) {
    transverse_tangents.push_back(t2);
  }
  for (const auto &tangent : transverse_tangents) {
    painter->DrawLine(tangent.From(), tangent.To(), tangent_style);
  }

  // Draw the direct common tangents:
  wkg::Line2d d1, d2;
  const int num_direct = circ1.DirectCommonTangents(circ2, &d1, &d2);

  std::vector<wkg::Line2d> direct_tangents;
  if (num_direct > 0) {
    direct_tangents.push_back(d1);
  }
  if (num_direct > 1) {
    direct_tangents.push_back(d2);
  }

  tangent_style.color = Color::CoordinateAxisColor('y');
  for (const auto &tan : direct_tangents) {
    painter->DrawLine(tan.From(), tan.To(), tangent_style);
  }

  // Draw the circles
  painter->DrawCircle(
        circ1.Center(), circ1.Radius(),
        circle_style, Color::Same.WithAlpha(0.3));
  painter->DrawCircle(
        circ2.Center(), circ2.Radius(),
        circle_style, Color::Same.WithAlpha(0.3));
}


void DemoCircleTangents() {
  PrintDemoHeader("Circle Tangents");

  auto painter = CreatePainter();
  painter->SetCanvas(512, 512, Color::White.WithAlpha(0.0));

  // Two separate circles, with all four tangents
  wkg::Circle circle1{{100.0, 100.0}, 90.0};
  wkg::Circle circle2{{300.0,  60.0}, 50.0};
  TangentsDemoHelper(painter, circle1, circle2);

  // Same radii
  circle1 = {{440.0,  50.0}, 40.0};
  circle2 = {{455.0, 160.0}, 40.0};
  TangentsDemoHelper(painter, circle1, circle2);

  // Intersecting (outside), different radii
  circle1 = {{285.0, 450.0}, 50.0};
  circle2 = {{375.0, 450.0}, 40.0};
  TangentsDemoHelper(painter, circle1, circle2);

  // Overlapping
  circle1 = {{260.0, 245.0}, 80.0};
  circle2 = {{350.0, 230.0}, 60.0};
  TangentsDemoHelper(painter, circle1, circle2);

  // Intersecting (inner)
  circle1 = {{100.0, 400.0}, 80.0};
  circle2 = {{140.0, 400.0}, 40.0};
  TangentsDemoHelper(painter, circle1, circle2);

  // Inside (no tangent)
  circle1 = {{425.0, 340.0}, 35.0};
  circle2 = {{420.0, 340.0}, 50.0};
  TangentsDemoHelper(painter, circle1, circle2);

  ProcessDemoOutput(painter->GetCanvas(false), "circle-tangents.png");
  painter.reset();
}


void DemoCircles() {
  PrintDemoHeader("Circles & Ellipses");

  auto painter = CreatePainter();
  painter->SetCanvas(500, 500, Color::White);

  painter->DrawGrid({}, {}, 50, 50, LineStyle(1.0, "gray!60"));

  auto style = LineStyle(3, "navy-blue!90");

  painter->DrawCircle({100, 100}, 50, style, "red");

  style.dash_pattern = {20, 15};
  painter->DrawCircle({250, 100}, 50, style);

  style.dash_pattern = {};
  style.width = 0;
  auto fill = Color("blue!40");
  painter->DrawCircle({400, 100}, 50, style, fill);

  style.width = 4;
  painter->DrawArc({100, 250}, 50,   0,  90, style, false);
  painter->DrawArc({100, 250}, 50, 110, 160, style, false);
  painter->DrawArc({100, 250}, 50, 180, 210, style, false);
  painter->DrawArc({100, 250}, 50, 230, 250, style, false);

  style.width = 2;
  style.dash_pattern = {10, 4};
  painter->DrawArc({250, 250}, 50,   0,  90, style);
  painter->DrawArc({250, 250}, 50, 110, 160, style);
  painter->DrawArc({250, 250}, 50, 180, 210, style);
  painter->DrawArc({250, 250}, 50, 230, 250, style);

  style.dash_pattern = {};
  style.width = 0;
  painter->DrawArc({400, 250}, 50, 0, 90, style, true, fill);
  painter->DrawArc({400, 250}, 50, 110, 160, style, true, fill);
  painter->DrawArc({400, 250}, 50, 180, 210, style, true, fill);
  painter->DrawArc({400, 250}, 50, 230, 250, style, true, fill);

  style.width = 3;
  painter->DrawEllipse({100, 400, 100, 50, 0, 45, -45}, style);

  style.dash_pattern = {10, 10};
  painter->DrawEllipse(
        Ellipse({250, 400}, {100, 50}, 45, 45, -45, false),
        style, fill);

  style.dash_pattern = {};
  style.width = 0;
  painter->DrawEllipse(
        Ellipse({400, 400}, {100, 50}, 180, 45, -45, true),
        style, fill);

  ProcessDemoOutput(painter->GetCanvas(false), "demo-output-circles.png");
  painter.reset();
}


void DemoPolygons() {
  PrintDemoHeader("Polygons");

  auto painter = CreatePainter();
  painter->SetCanvas(800, 800, Color::White);

  painter->DrawGrid({}, {}, 50, 50,
                    LineStyle(1.0, "gray!80"));

  auto line_style = LineStyle(3.0, "crimson");
  painter->DrawPolygon(
        {{10, 10}, {50, 50}, {70, 50}, {60, 60}, {20, 10}},
        line_style);

  line_style.color = "azure";
  painter->DrawPolygon(
        {{100, 100}, {250, 350}, {180, 180}, {400, 200}},
        line_style, Color::Same.WithAlpha(0.4));

  line_style.color = "midnight-blue";
  painter->DrawPolygon(
        {{500, 100}, {600, 150}, {550, 300}, {500, 150}},
        line_style, "azure!20");

  ProcessDemoOutput(painter->GetCanvas(false), "demo-output-polygon.png");
  painter.reset();
}


void DemoRects() {
  PrintDemoHeader("Rectangles");

  auto painter = CreatePainter();
  painter->SetCanvas(600, 600, Color::White);
  painter->DrawGrid({}, {}, 50, 50, LineStyle(1.0, "gray!60"));

  auto style = LineStyle(3, "navy-blue!90");

  auto rect = Rect(100, 150, 100, 200);
  painter->DrawRect(rect, style, "light-blue!30");

  rect.cx += 150;
  rect.radius = 0.5;
  painter->DrawRect(rect, style);

  rect.cx += 200;
  rect.rotation = 45;
  painter->DrawRect(rect, style);

  rect.cx = 100;
  rect.cy += 300;
  rect.rotation = 10;
  rect.radius = 0.1;
  painter->DrawRect(rect, style, Color::Same.WithAlpha(0.4));

  rect.cx += 150;
  rect.rotation += 10;
  rect.radius = 30;
  painter->DrawRect(rect, style, style.color.WithAlpha(0.4));

  ProcessDemoOutput(painter->GetCanvas(false), "demo-output-rects.png");
  painter.reset();
}


void DemoMarkers() {
  PrintDemoHeader("Markers");

  auto painter = CreatePainter();
  painter->SetCanvas(100, 800, Color::White); //.WithAlpha(0.0));

  auto text_style = TextStyle();
  text_style.size = 16;
  text_style.family = "xkcd";//"monospace";
  text_style.color = RGBa(60, 60, 60);

  Vec2d pos {110.0, 5.0};
  MarkerStyle marker_style;
  marker_style.color = "navy-blue";
  marker_style.size = 19;
  marker_style.thickness = 1;

  painter->DrawText({"Marker code:"}, {5.0, pos.y()},
                    Anchor::TopLeft, text_style);

  painter->DrawText({"Not filled:"}, {5.0, 50.0},
                    Anchor::Left, text_style);

  painter->DrawText({"Filled:"}, {5.0, 85.0},
                    Anchor::Left, text_style);

  text_style.size = 16;
  text_style.family = "monospace";

  for (Marker m : ListMarkers()) {
    std::ostringstream s;
    s << m;
    painter->DrawText({s.str()}, pos, Anchor::Top, text_style);

    Vec2d mpos{pos.x(), 45.0};
    marker_style.filled = false;
    marker_style.marker = m;
    if (!marker_style.IsFilled()) {
      painter->DrawMarker(mpos, marker_style);
    }

    marker_style.filled = true;
    if (marker_style.IsFilled()) {
      mpos = {pos.x(), 80.0};
      painter->DrawMarker(mpos, marker_style);
    }

    pos.x() += 32;
  }

  ProcessDemoOutput(painter->GetCanvas(false), "demo-output-markers.png");
  painter.reset();
}

} // namespace demos
} // namespace viren2d
