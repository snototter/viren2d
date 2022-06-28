#ifndef __VIREN2D_IMAGEBUFFER_H__
#define __VIREN2D_IMAGEBUFFER_H__

#include <string>
#include <ostream>
#include <initializer_list>
#include <cmath>

namespace viren2d {
enum class ImageBufferType : int {
  UInt8 = 0,
  Int32,
  Float,
  Double
};

std::string ImageBufferTypeToString(ImageBufferType t);
int ItemSizeFromImageBufferType(ImageBufferType t);

//---------------------------------------------------- Image buffer
// TODO
// 1) encapsulate, no more public members
// 2) dtype enum (ImageBufferType::UInt8, UChar, Float, Double, ...)
// 3) elemsize from imagebuffertype (uint8/uchar -> 1, float -> sizeof(float), ...)
// 4) pybind: check elemsize matches dtype
// 5) imagebuffer::createcopy/... uses dtype parameter instead of item size
// 6) at implementieren
/**
 * @brief An ImageBuffer holds 8-bit images.
 *
 * Usage: either copy existing image data via @see CreateCopy(), or
 *        use existing memory via @see CreateSharedBuffer(). The
 *        latter does NOT take ownership of the memory (cleaning up
 *        remains the caller's responsibility).
 */
class ImageBuffer {
public:
  /// Creates an empty ImageBuffer.
  ImageBuffer();


  /// Allocates memory to hold a W x H x CH image of the specified type.
  ImageBuffer(int w, int h, int ch, ImageBufferType buf_type);

  ~ImageBuffer();

  //FIXME doc: copies data if other.owns_data; otherwise, *this will also be a shared buffer!
  ImageBuffer(const ImageBuffer &other); // copy c'tor

  //TODO [ ] add C++ test (tests/xxx_test.cpp)
  ImageBuffer(ImageBuffer &&other) noexcept; // move c'tor

  //TODO [ ] add C++ test (tests/xxx_test.cpp)
  ImageBuffer &operator=(const ImageBuffer &other); // copy assignment

  //TODO [ ] add C++ test (tests/xxx_test.cpp)
  ImageBuffer &operator=(ImageBuffer &&other) noexcept; // move assignment

  inline int Width() const { return width; }
  inline int Height() const { return height; }
  inline int Channels() const { return channels; }
  inline int Stride() const { return stride; }
  inline int ItemSize() const { return item_size; }
  inline ImageBufferType BufferType() const { return buffer_type; }
  inline int NumElements() const { return width * height * channels; }
  inline int NumBytes() const { return NumElements() * item_size; }

  inline bool OwnsData() const { return owns_data; }
  inline bool IsContiguous() const { return stride == (width * channels * item_size); }

  inline unsigned char const *ImmutableData() const { return data; }
  inline unsigned char *MutableData() { return data; }

  template<typename _Tp>
  inline _Tp const *ImmutableRowPtr(int row) const { return reinterpret_cast<_Tp*>(&data[row*stride]); }

  template<typename _Tp>
  inline _Tp *MutableRowPtr(int row) { return reinterpret_cast<_Tp*>(&data[row*stride]); }

  template<typename _Tp>
  _Tp &At(int row, int col, int channel=0) {
    return *reinterpret_cast<_Tp*>(&data[row * stride + col * channels * item_size + channel * item_size]);
  }


  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  /**
   * @brief Reuses the given image data.
   *
   * This ImageBuffer will point to the given image data. It will
   * NOT take ownership.
   */
  void CreateSharedBuffer(
      unsigned char *buffer,
      int width, int height, int channels, int stride,
      ImageBufferType buffer_type);


  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  /**
   * @brief Copies the given image data.
   */
  void CreateCopy(unsigned char const *buffer, int width, int height, int channels, int stride, ImageBufferType buffer_type);

  ImageBuffer CreateCopy() const;


  void SwapChannels(int ch1, int ch2);

  // TODO doc - sets owns_data = true; obviously, call it only if you want this buffer to free the memory!
  void TakeOwnership();


  /**
   * TODO documentation!
   *
   * Only the following conversions are supported:
   * * From single-channel to 1-, 3-, or 4-channel output.
   * * From 3-channel to 3- or 4-channel output, *i.e.* adding
   *   an alpha channel.
   * * From 4-channel to 3- or 4-channel output, *i.e.* removing
   *   the alpha channel.
   *
   * Other configurations are **not** supported.
   */
  ImageBuffer ToChannels(int output_channels) const;


  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  //DONE [x] add Python bindings
  //TODO [ ] add Python test (tests/test_xxx.py)
  /** @brief Returns true if this buffer points to a valid memory location. */
  bool IsValid() const;

  /// Returns a readable representation.
  std::string ToString() const;

  /// Overloaded stream operator.
  friend std::ostream &operator<<(std::ostream &os, const ImageBuffer &buffer) {
    os << buffer.ToString();
    return os;
  }

private:
  unsigned char *data;  ///< Pointer to the image data.
  int width;            ///< Width of the image in pixels.
  int height;           ///< Height of the image in pixels.
  int channels;         ///< Number of channels.

  /// Size of a single element in bytes.
  int item_size;
  int stride;           ///< Stride (number of bytes) per row.
  ImageBufferType buffer_type;
  bool owns_data;      ///< Flag indicating if we own the memory (i.e. if we need to clean up).

  void Cleanup();
};


//DONE [x] add documentation
//DONE [x] add C++ test (tests/xxx_test.cpp)
//DONE [x] add Python bindings (via Painter)
/**
 * @brief Loads an image from disk.
 *
 * We use the stb/stb_image library for reading/decoding.
 * Supported formats as of now:
 *   JPEG, PNG, TGA, BMP, PSD, GIF, HDR, PIC, PNM
 *
 * Check for stb updates if your format is missing:
 *   https://github.com/nothings/stb/blob/master/stb_image.h
 *
 * @param image_filename Path to image file
 *
 * @param force_num_channels can be used to force the number of
 *              loaded channels (e.g. load a JPEG as RGBA by
 *              specifying force_num_channels=4).
 *              Supported:
 *                0: load as-is
 *                1: load as grayscale
 *                2: load as grayscale + alpha channel
 *                3: load as rgb
 *                4: load as rgb + alpha channel
 *
 * @return ImageBuffer
 */
ImageBuffer LoadImage(const std::string &image_filename, int force_num_channels=0);


/// Saves the ImageBuffer to disk as either JPEG or PNG.
///
/// We use the stb/stb_image_write library for writing. Note
/// that PNG output is not optimal (20-50% larger file size
/// than using an optimized library).
/// I consider writing to disk only a nice-to-have feature,
/// thus I'm not including any other specialized third-party
/// libraries for that.
void SaveImage(const std::string &image_filename, const ImageBuffer &image);


//TODO(snototter) rgb(a)2gray (single output channel vs 3 channels for drawing)


//DONE [x] add documentation
//TODO [ ] add C++ test (tests/xxx_test.cpp)
//TODO [ ] add Python bindings
//TODO [ ] add Python test (tests/test_xxx.py)
//TODO [ ] add C++ demo
//TODO [ ] add Python demo
/** @brief Converts a grayscale ImageBuffer to RGB. */
ImageBuffer Gray2RGB(const ImageBuffer &img);


//DONE [x] add documentation
//TODO [ ] add C++ test (tests/xxx_test.cpp)
//TODO [ ] add Python bindings
//TODO [ ] add Python test (tests/test_xxx.py)
//TODO [ ] add C++ demo
//TODO [ ] add Python demo
/** @brief Converts a grayscale ImageBuffer to RGBA. */
ImageBuffer Gray2RGBA(const ImageBuffer &img);


//DONE [x] add documentation
//TODO [ ] add C++ test (tests/xxx_test.cpp)
//TODO [ ] add Python bindings
//TODO [ ] add Python test (tests/test_xxx.py)
//TODO [ ] add C++ demo
//TODO [ ] add Python demo
/** @brief Converts a RGBA ImageBuffer to RGB. */
ImageBuffer RGBA2RGB(const ImageBuffer &img);


//DONE [x] add documentation
//TODO [ ] add C++ test (tests/xxx_test.cpp)
//TODO [ ] add Python bindings
//TODO [ ] add Python test (tests/test_xxx.py)
//TODO [ ] add C++ demo
//TODO [ ] add Python demo
/** @brief Converts a RGB ImageBuffer to RGBA. */
ImageBuffer RGB2RGBA(const ImageBuffer &img);

} // namespace viren2d

#endif // __VIREN2D_IMAGEBUFFER_H__
