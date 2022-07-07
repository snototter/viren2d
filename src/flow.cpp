#include <stdexcept>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cstdint>
#include <cstring>

// public viren2d headers
#include <viren2d/flow.h>

// private viren2d headers
#include <helpers/logging.h>


namespace viren2d {

/// First 4 bytes of a .flo file should be this magic number.
/// On little endian systems, both tags will be the same.
constexpr float kFlowTagFloat = 202021.25f;
constexpr const char *kFlowTagAscii = "PIEH";

ImageBuffer LoadOpticalFlow(const std::string &filename) {
  //TODO doc
  //  0-3     tag: "PIEH" in ASCII, which in little endian is the float 202021.25
  //          --> sanity check that floats are represented correctly
  //  4-7     width as an integer
  //  8-11    height as an integer
  //  12-end  data in row-major order (width*height*2*4 bytes total)
  std::ifstream file(filename, std::ios::in | std::ios::binary);

  if (!file.is_open()) {
    std::string s("Could not open optical flow file: \"");
    s += filename;
    s += "\"!";
    throw std::logic_error(s);
  }

  union flo_spec {
    char buffer[4];
    std::int32_t ival;
    float fval;
  } tag, width, height;

  file.read(tag.buffer, 4);
  if (std::strncmp(tag.buffer, kFlowTagAscii, 4) != 0) {
    std::stringstream s;
    s << "Invalid magic number " << std::fixed << std::setprecision(2)
      << tag.fval << "/\"" << tag.buffer[0] << tag.buffer[1] << tag.buffer[2]
      << tag.buffer[3] << "\" instead of " << std::fixed << std::setprecision(2)
      << kFlowTagFloat << "/\"" << kFlowTagAscii[0] << kFlowTagAscii[1]
      << kFlowTagAscii[2] << kFlowTagAscii[3] << "\" in file \""
      << filename << "\"";
    throw std::logic_error(s.str());
  }

  file.read(width.buffer, 4);
  file.read(height.buffer, 4);
  ImageBuffer flow(height.ival, width.ival, 2, ImageBufferType::Float);
  //TODO read - if contiguous, read only once with reinterpret_cast; else read line after line

  if (!flow.IsContiguous()) {
    SPDLOG_CRITICAL("FIXME freshly allocated buffer is NOT contiguous!");
  }
  file.read(
        reinterpret_cast<char *>(flow.MutableData()),
        width.ival * height.ival * 2 * sizeof(float));

  file.close();
  return flow;
}


void SaveOpticalFlow(const std::string &filename, const ImageBuffer &flow) {
  throw std::logic_error("Not yet implemented!");
}


} // namespace viren2d
