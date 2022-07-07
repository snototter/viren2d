#include <viren2d/viren2d.h>

#include <demo_utils/demos.h>

namespace viren2d {
namespace demos {
void DemoText() {
  PrintDemoHeader("Text & Text Boxes");
  auto painter = CreatePainter();
  painter->SetCanvas(550, 600, Color::White);


  const std::vector<std::string> anchors = {
    "center", "north", "north-east",
    "east", "south-east", "south",
    "south-west", "west", "north-west"
  };

  const std::vector<std::string> families = {"monospace", "sans-serif", "xkcd"};


  for (size_t idx_family = 0; idx_family < families.size(); ++idx_family) {
    auto text_style = TextStyle();
    text_style.size = 16;
    text_style.family = families[idx_family];
    text_style.line_spacing = 1.0;
    text_style.alignment = HorizontalAlignment::Center;

    auto padding = Vec2d::All(idx_family * 3);

    std::ostringstream s;
    s << "Padding: " << static_cast<int>(padding.x());
    Vec2d pos = {100.0 + idx_family * 200.0, 10.0};
    painter->DrawTextBox({families[idx_family], s.str()},
                         pos, Anchor::Top,
                         text_style, {6, 6}, 0, LineStyle(1, "black"), "azure!40");

    text_style.size = 14;
    text_style.color = "navy-blue";

    for (size_t idx_anchor = 0; idx_anchor < anchors.size(); ++idx_anchor) {
      std::ostringstream txt;
//      txt << "!nofill, noclip!";
      txt << AnchorToString(AnchorFromString(anchors[idx_anchor]));
//      txt << anchors[idx_anchor];


      pos.SetY(100.0 + idx_anchor * 50.0);
      //painter->DrawCircle(pos, 5, LineStyle::Invalid, "black!40");
      painter->DrawMarker(pos, MarkerStyle(Marker::Pentagram, 30, 1, "crimson!90", false));
      if (idx_family == 0) {
        // Just draw the text
        painter->DrawText({txt.str()}, pos, AnchorFromString(anchors[idx_anchor]),
                          text_style, padding);
      } else { //if (idx_family == 1) {
        // Draw a text box; size calculated from the text
        painter->DrawTextBox({txt.str()}, pos, AnchorFromString(anchors[idx_anchor]),
                             text_style, padding, 0.0,
                             LineStyle::Invalid,
                             "azure!40",
                             0.25);
      }
    }
  }

  ProcessDemoOutput(painter->GetCanvas(false), "demo-output-text.png");
  painter.reset();
}


} // namespace demos
} // namespace viren2d
