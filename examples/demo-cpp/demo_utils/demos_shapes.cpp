#include <viren2d/viren2d.h>
#include <demo_utils/demos.h>

namespace viren2d {
namespace demos {

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

  for (char m : ListMarkers()) {
    std::ostringstream s;
//    s << '\'' << m << '\'';
    s << m;
    painter->DrawText({s.str()}, pos, Anchor::Top, text_style);

    Vec2d mpos{pos.x(), 45.0};
    marker_style.filled = false;
    marker_style.marker = MarkerFromChar(m);
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
