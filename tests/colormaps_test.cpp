#include <exception>
#include <cstdlib>
#include <gtest/gtest.h>

#include <werkzeugkiste/container/sort.h>
#include <werkzeugkiste/strings/strings.h>

#include <viren2d/colormaps.h>
#include <helpers/enum.h>

namespace wks = werkzeugkiste::strings;
namespace wkc = werkzeugkiste::container;


typedef viren2d::ContinuousEnumIterator<viren2d::ColorMap,
  viren2d::ColorMap::Autumn, viren2d::ColorMap::Yarg> ColormapIterator;


TEST(ColorMapTest, StringRepresentation) {
  // Check that all color maps have been mapped to their string representation:
  for (viren2d::ColorMap cm: ColormapIterator()) {
    viren2d::ColorMap cp = viren2d::ColorMapFromString(
          wks::Upper(viren2d::ColorMapToString(cm)));
    EXPECT_EQ(cp, cm);
  }

  // Check that all color map enums are listed:
  const auto cmaps = viren2d::ListColorMaps();
  std::size_t cnt = 0;
  for (viren2d::ColorMap cm: ColormapIterator()) {
    EXPECT_TRUE(wkc::ContainsValue(cmaps, cm));
    ++cnt;
  }
  EXPECT_EQ(cnt, cmaps.size());
}
