#ifndef __VIREN2D_IMAGEBUFFER_H__
#define __VIREN2D_IMAGEBUFFER_H__

#include <string>
#include <ostream>
#include <sstream>
#include <exception>
#include <cstdint>  // For fixed width integer types (stdint.h in C)
#include <type_traits>
#include <algorithm> // std::min
#include <initializer_list>

//#include <viren2d/colors.h>
#include <viren2d/primitives.h>

namespace viren2d {

/// Data types supported by the ImageBuffer class.
enum class ImageBufferType : int {
  UInt8 = 0,
  Int16,
  Int32,
  Float,
  Double
};


/// Templated type alias which provides the
/// underlying type (either fixed width integer
/// or single/double precision float) for an ImageBuffer.
template<ImageBufferType T>
using image_buffer_t = typename std::conditional<
  T == ImageBufferType::UInt8,
  uint8_t,
  typename std::conditional<
    T == ImageBufferType::Int16,
    int16_t,
    typename std::conditional<
      T == ImageBufferType::Int32,
      int32_t,
      typename std::conditional<
        T == ImageBufferType::Float,
        float,
        double
      >::type
    >::type
  >::type
>::type;


/// Returns the string representation.
std::string ImageBufferTypeToString(ImageBufferType t);


/// Returns the ImageBufferType corresponding to the given string representation.
ImageBufferType ImageBufferTypeFromString(const std::string &s);


/// Returns the size of the corresponding data type in bytes.
int ElementSizeFromImageBufferType(ImageBufferType t);


/// Output stream operator to print an ImageBufferType.
std::ostream &operator<<(std::ostream &os, ImageBufferType t);


//---------------------------------------------------- Image buffer

/// Holds image data. For supported data types, see `ImageBufferType`.
///
/// Usage: Either copy existing image data via `CreateCopiedBuffer`, or
///   share the same memory via `CreateSharedBuffer`. The latter
///   does NOT take ownership of the memory (i.e. cleaning up
///   remains the caller's responsibility).
class ImageBuffer {
public:
  /// Creates an empty ImageBuffer.
  ImageBuffer();


  /// Allocates memory to hold a H x W x CH image of the specified type.
  ImageBuffer(int h, int w, int ch, ImageBufferType buf_type);


  /// Destructor frees the memory, if it was allocated
  /// by this ImageBuffer.
  ~ImageBuffer();


  /// Copy c'tor: Copies the data IFF other.owns_data is true.
  /// Otherwise, this ImageBuffer will also be a shared buffer.
  /// For a guaranteed deep copy, use `DeepCopy`!
  ImageBuffer(const ImageBuffer &other) noexcept;


  /// Move constructor.
  ImageBuffer(ImageBuffer &&other) noexcept;


  /// Copy assignment operator.
  ImageBuffer &operator=(const ImageBuffer &other);


  /// Move assignment operator.
  ImageBuffer &operator=(ImageBuffer &&other) noexcept;

  inline Vec2i Size() const { return Vec2i(width, height); }

  /// Returns the number of rows.
  inline int Height() const { return height; }


  /// Returns the number of pixels in each row.
  inline int Width() const { return width; }


  /// Number of values per pixel.
  inline int Channels() const { return channels; }


  /// Number of bytes per subsequent rows in memory.
  /// On a freshly allocated buffer, this
  /// equals `width * num_channels * item_size`.
  inline int RowStride() const { return row_stride; }


  /// Number of bytes between subsequent pixels.
  /// On a freshly allocated buffer, this
  /// equals `num_channels * item_size`.
  inline int PixelStride() const { return pixel_stride; }


  /// Returns the size in bytes of a single element/value.
  /// Multiply by Channels() to get the memory consumption per pixel.
  inline int ElementSize() const { return element_size; }


  /// Returns this buffers data type.
  inline ImageBufferType BufferType() const { return buffer_type; }


  /// Returns the number of pixels, i.e. W*H.
  inline int NumPixels() const { return width * height; }


  /// Returns the number of elements (i.e. values of the
  /// chosen data type), i.e. W*H*C.
  inline int NumElements() const { return NumPixels() * channels; }


  /// Returns the number of bytes.
  inline int NumBytes() const { return NumElements() * element_size; }


  /// Returns true if this ImageBuffer is responsible for
  /// cleaning up the corresponding data.
  inline bool OwnsData() const { return owns_data; }


  /// Sets this->owns_data = true;
  /// Obviously to be used only, if the calling code ensures that
  /// the former `data` owner will NOT free the memory.
  void TakeOwnership();


  /// Returns true if the underlying `data` memory is contiguous.
  inline bool IsContiguous() const {
    return (row_stride == width * channels * element_size)
        && (pixel_stride == channels * element_size);
  }


  /// Returns a mutable pointer to the underlying `data` memory.
  inline unsigned char *MutableData() { return data; }


  /// Returns an immutable pointer to the underlying `data` memory.
  inline unsigned char const *ImmutableData() const { return data; }


  /// Returns a mutable pointer of the specified type to the
  /// underlying `data` memory.
  template<typename _Tp> inline
  _Tp *MutablePtr(int row, int col, int channel=0) {
    return reinterpret_cast<_Tp *>(data + ByteOffset(row, col, channel));
  }


  /// Returns an immutable pointer of the specified type to the
  /// underlying `data` memory.
  template<typename _Tp> inline
  _Tp const *ImmutablePtr(int row, int col, int channel=0) const {
    return reinterpret_cast<_Tp const *>(data + ByteOffset(row, col, channel));
  }


  /// Returns a reference to modify the specified pixel element.
  template<typename _Tp> inline
  _Tp& AtUnchecked(int row, int col, int channel=0) {
    return *MutablePtr<_Tp>(row, col, channel);
  }


  /// Returns a read-only reference to the specified pixel element.
  template<typename _Tp> inline
  const _Tp& AtUnchecked(int row, int col, int channel=0) const {
    return *ImmutablePtr<_Tp>(row, col, channel);
  }


  /// Returns a reference to modify the specified pixel element.
  template<typename _Tp> inline
  _Tp& AtChecked(int row, int col, int channel=0) {
    CheckIndexedAccess(row, col, channel);
    return AtUnchecked<_Tp>(row, col, channel);
  }


  /// Returns a read-only reference to the specified pixel element.
  template<typename _Tp> inline
  const _Tp& AtChecked(int row, int col, int channel=0) const {
    CheckIndexedAccess(row, col, channel);
    return AtUnchecked<_Tp>(row, col, channel);
  }


  //TODO doc
  template <typename _Tp, typename... _Ts> inline
  void SetToPixel(_Tp element0, _Ts... elements) {
    const int num_el = 1 + sizeof...(elements);
    const int num_channels = std::min(channels, num_el);
    const _Tp lst[num_el] = {element0, static_cast<_Tp>(elements)...};

    int rows = height;
    int cols = width;

    if (IsContiguous()) {
      cols *= rows;
      rows = 1;
    }

    for (int row = 0; row < rows; ++row) {
      for (int col = 0; col < cols; ++col) {
        for (int ch = 0; ch < num_channels; ++ch) {
          AtUnchecked<_Tp>(row, col, ch) = lst[ch];
        }
      }
    }
  }

  //TODO doc
  template <typename _Tp> inline
  void SetToScalar(_Tp element) {
    int rows = height;
    int cols = width;

    if (IsContiguous()) {
      cols *= rows;
      rows = 1;
    }

    for (int row = 0; row < rows; ++row) {
      for (int col = 0; col < cols; ++col) {
        for (int ch = 0; ch < channels; ++ch) {
          AtUnchecked<_Tp>(row, col, ch) = element;
        }
      }
    }
  }


  /// Reuses the given image data, i.e. this ImageBuffer will point
  /// to the given image data - it will NOT take ownership.
  ///
  /// Args:
  ///   buffer: Image data
  ///   height: Number of rows
  ///   width: Number of columns
  ///   channels: Number of elements at each (row, column) location.
  ///   row_stride: Number of bytes between consecutive rows.
  ///   buffer_type: Element type.
  /// TODO doc col stride
  void CreateSharedBuffer(
      unsigned char *buffer,
      int height, int width, int channels, int row_stride,
      int pixel_stride, ImageBufferType buffer_type);


  /// Copies the given image data.
  ///
  /// Args:
  ///   buffer: Image data
  ///   height: Number of rows
  ///   width: Number of columns
  ///   channels: Number of elements at each (row, column) location.
  ///   row_stride: Number of bytes between consecutive rows.
  ///   buffer_type: Element type.
  /// TODO doc col stride
  void CreateCopiedBuffer(
      unsigned char const *buffer,
      int height, int width, int channels, int row_stride,
      int pixel_stride, ImageBufferType buffer_type);


  /// Returns a deep copy.
  ImageBuffer DeepCopy() const;


  // TODO shared buffer
  ImageBuffer ROI(int left, int top, int roi_width, int roi_height);


  /// Swaps the specified (0-based) channels *in-place*.
  void SwapChannels(int ch1, int ch2);


  /// Returns an ImageBuffer with the given number
  /// of channels. Note that only the following channel
  /// conversions are supported:
  /// * From single-channel to 1-, 3-, or 4-channel output.
  /// * From 3-channel to 3- or 4-channel output, *i.e.* adding
  ///   an alpha channel.
  /// * From 4-channel to 3- or 4-channel output, *i.e.* removing
  ///   the alpha channel.
  ///
  /// Other configurations are **not** supported.
  ImageBuffer ToChannels(int output_channels) const;


  /// Converts this buffer to `uint8_t`.
  /// If the underlying type is `float` or `double`,
  /// the values will be **multiplied by 255**. Otherwise,
  /// the values will be clamped into [0, 255].
  ///
  /// The following channel configurations are supported:
  /// * 1-channel buffer: output_channels either 1, 3, or 4
  /// * 3-channel buffer: output_channels either 3 or 4
  /// * 4-channel buffer: output_channels either 3 or 4
  ImageBuffer ToUInt8(int output_channels) const;


  /// Returns the grayscale image.
  ///
  /// Args:
  ///   num_channels: Number of output channels as :class:`int`, must be ``<=4``.
  ///     The first (up to) 3 channels will contain the repeated luminance,
  ///     whereas the 4th channel will always be 255 (*i.e.* alpha, fully opaque).
  ///   is_bgr: Set to ``true`` if the channels of this image are in BGR format.
  ImageBuffer ToGrayscale(int output_channels, bool is_bgr_format=false) const;


  /// Performs **in-place** pixelation of images with **up to 4**
  /// :attr:`channels`. All pixels within a *block* will be set to
  /// the value of the block's center pixel.
  ///
  /// If the chosen block size does not align with the region of interest,
  /// the size of the outer blocks (left, right, top and bottom) will be
  /// increased to ensure proper pixelation of these areas.
  ///
  /// If ``left``, ``top``, ``width`` **and** ``height`` are all ``-1``,
  /// the whole image will be pixelated.
  void Pixelate(
      int block_width, int block_height,
      int roi_left, int roi_top, int roi_width, int roi_height);


  /// Returns an alpha-blended image.
  ///
  /// Computes ``((1 - alpha) * this) + (alpha * other)``.
  /// If the number of channels is not the same, the number of
  /// output channels will be the maximum of ``this.channels``
  /// and ``other.channels``. In this case, *non-blendable* channels
  /// are copied from the input buffer which has more channels.
  ImageBuffer Blend(const ImageBuffer &other, double alpha_other) const;


  /// Returns a single-channel buffer deeply copied from this ImageBuffer.
  ImageBuffer Channel(int channel) const;


  /// Returns true if this buffer points to a valid memory location.
  bool IsValid() const;


  /// Computes the minimum & maximum values over a single channel.
  /// To compute only the locations, pass nullptr for min_val/max_val.
  void MinMaxLocation(
      double *min_val, double *max_val,
      Vec2i *min_loc = nullptr, Vec2i *max_loc = nullptr,
      int channel = 0) const;


  /// Returns a human readable representation.
  std::string ToString() const;


  /// Overloaded stream operator.
  friend std::ostream &operator<<(std::ostream &os, const ImageBuffer &buffer) {
    os << buffer.ToString();
    return os;
  }


private:
  /// Pointer to the image data.
  unsigned char *data;

  /// Number of rows.
  int height;

  /// Number of pixels in each row.
  int width;

  /// Number of channels (i.e. elements of
  /// the specified buffer type per pixel).
  int channels;

  /// Size of a single element in bytes, *i.e.*
  /// a 3-channel image would hold 3 "elements"
  /// per (x,y) pixel position.
  int element_size;

  /// Number of bytes between subsequent rows.
  int row_stride;

  /// Number of bytes between subsequent pixels.
  int pixel_stride;

  /// This buffer's data type.
  ImageBufferType buffer_type;

  /// Flag which indicates if this buffer owns the
  /// memory, i.e. if it is responsible for cleaning up.
  bool owns_data;


  /// Frees the memory if needed and resets
  /// the members accordingly.
  void Cleanup();


  /// Checks that the given indices are valid.
  inline void CheckIndexedAccess(int row, int col, int channel) const {
    if ((row < 0) || (row >= height)
        || (col < 0) || (col >= width)
        || (channel < 0) || (channel >= channels)) {
      std::ostringstream s;
      s << "Buffer index (row=" << row << ", col="
        << col << ", ch=" << channel
        << ") is out of range for ImageBuffer of size h="
        << height << ", w=" << width << " a " << channels << " channels!";
      throw std::out_of_range(s.str());
    }
  }


  /// Returns the offset in bytes to the given indices.
  inline int ByteOffset(int row, int col, int channel) const {
    return (row * row_stride) + (col * pixel_stride) + (channel * element_size);
  }
};


/// Loads an 8-bit image from disk.
///
/// We use the stb/stb_image library for reading/decoding.
/// Supported formats as of now:
///   JPEG, PNG, TGA, BMP, PSD, GIF, HDR, PIC, PNM
///
/// Check for stb updates if your format is missing:
///   https://github.com/nothings/stb/blob/master/stb_image.h
///
/// Args:
///   image_filename: Path to image file
///   force_num_channels: can be used to force the number of
///     loaded channels (e.g. load a JPEG as RGBA by
///     specifying force_num_channels=4). Supported:
///     * 0: load as-is
///     * 1: load as grayscale
///     * 2: load as grayscale + alpha channel
///     * 3: load as rgb
///     * 4: load as rgb + alpha channel
ImageBuffer LoadImageUInt8(const std::string &image_filename, int force_num_channels=0);


/// Saves an 8-bit image to disk as either JPEG or PNG.
///
/// We use the stb/stb_image_write library for writing. Note
/// that PNG output is not optimal (20-50% larger file size
/// than using an optimized library).
/// I consider writing to disk only a nice-to-have feature,
/// thus I'm not including any other specialized third-party
/// libraries for that.
void SaveImageUInt8(const std::string &image_filename, const ImageBuffer &image);


} // namespace viren2d

#endif // __VIREN2D_IMAGEBUFFER_H__