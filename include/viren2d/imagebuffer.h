#ifndef __VIREN2D_IMAGEBUFFER_H__
#define __VIREN2D_IMAGEBUFFER_H__

#include <string>
#include <ostream>
#include <sstream>
#include <exception>
#include <cstdint>  // For fixed width integer types (stdint.h in C)
#include <type_traits>
#include <algorithm> // std::min
#include <limits> // quiet nan
#include <initializer_list>
#include <utility> // pair

#include <viren2d/primitives.h>


namespace viren2d {

/// Data types supported by the ImageBuffer class.
enum class ImageBufferType : int {
  UInt8 = 0,
  Int16,
  UInt16,
  Int32,
  UInt32,
  Int64,
  UInt64,
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
      T == ImageBufferType::UInt16,
      uint16_t,
      typename std::conditional<
        T == ImageBufferType::Int32,
        int32_t,
        typename std::conditional<
          T == ImageBufferType::UInt32,
          uint32_t,
          typename std::conditional<
            T == ImageBufferType::Int64,
            int64_t,
            typename std::conditional<
              T == ImageBufferType::UInt64,
              uint64_t,
              typename std::conditional<
                T == ImageBufferType::Float,
                float,
                double
              >::type
            >::type
          >::type
        >::type
      >::type
    >::type
  >::type
>::type;


/// Returns the built-in data type's type_info for the corresponding enum value.
const std::type_info &ImageBufferTypeInfo(ImageBufferType t);


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
  /// equals `width * channels * item_size`.
  inline int RowStride() const { return row_stride; }


  /// Number of bytes between subsequent pixels.
  /// On a freshly allocated buffer, this
  /// equals `channels * item_size`.
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
    CheckType<_Tp>();
    CheckIndexedAccess(row, col, channel);
    return AtUnchecked<_Tp>(row, col, channel);
  }


  /// Returns a read-only reference to the specified pixel element.
  template<typename _Tp> inline
  const _Tp& AtChecked(int row, int col, int channel=0) const {
    CheckType<_Tp>();
    CheckIndexedAccess(row, col, channel);
    return AtUnchecked<_Tp>(row, col, channel);
  }


  /// Sets `I(x,y,i)` to `element_i` for all x, y. The number of arguments
  /// must be <= number of channels.
  template <typename _Tp, typename... _Ts> inline
  void SetToPixel(_Tp element0, _Ts... elements) {
    CheckType<_Tp>();

    const int num_el = 1 + sizeof...(elements);
    if (num_el > channels) {
      std::ostringstream msg;
      msg << "Invalid number of template arguments (" << num_el
          << ") to `SetToPixel` for an ImageBuffer with only " << channels
          << " channels!";
      throw std::invalid_argument(msg.str());
    }

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


  /// Sets all components of each pixel to the given scalar value.
  template <typename _Tp> inline
  void SetToScalar(_Tp element) {
    CheckType<_Tp>();
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


  /// Returns a uint8 mask which is set to 255 where the corresponding pixel
  /// components are within the given range.
  /// More specifically, for the i-th component (e.g. red = 0, green = 1, ...),
  /// M(x,y) = 255 iff (min0 <= I(x,y,0) <= max0) && (min1 <= I(x,y,1) <= max1), etc.
  template <typename _Tp, typename... _Ts> inline
  ImageBuffer MaskRange(_Tp min0, _Tp max0, _Ts... min_max_others) const {
    CheckType<_Tp>();

    const int num_inputs = 2 + sizeof...(min_max_others);
    if (num_inputs != 2 * channels) {
      std::ostringstream msg;
      msg << "`MaskRange` expects min/max per channel, i.e. " << (2 * channels)
          << " values, but got " << num_inputs << '!';
      throw std::invalid_argument(msg.str());
    }

    const _Tp min_max[num_inputs] = {
      min0, max0, static_cast<_Tp>(min_max_others)...};
    ImageBuffer mask(height, width, 1, ImageBufferType::UInt8);

    int rows = height;
    int cols = width;

    if (IsContiguous()) {
      cols *= rows;
      rows = 1;
    }

    for (int row = 0; row < rows; ++row) {
      for (int col = 0; col < cols; ++col) {
        bool within_range = true;
        for (int ch = 0; ch < channels; ++ch) {
          const _Tp val = AtUnchecked<_Tp>(row, col, ch);
          if ((val < min_max[ch * 2])
              || (val > min_max[ch * 2 + 1])) {
            within_range = false;
          }
        }
        mask.AtUnchecked<unsigned char>(row, col, 0) = within_range ? 255 : 0;
      }
    }
    return mask;
  }


  //TODO impl & doc (any input type, eps equal 0, 0 --> mask = 255)
//  ImageBuffer Invert() const;


  //TODO doc
  // TODO (pay attention to the signs!) :math:`I_{x,y,i}^{\text{dst}} = \left( I_{x,y,i}^{\text{src}} - \text{shift}_i^{\text{pre}} \right) * \text{scale}_i + \text{shift}_i^{\text{post}}`
  template <ImageBufferType output_type, typename _Tp, typename... _Ts> inline
  ImageBuffer Normalize(
      _Tp shift_pre, _Tp scale, _Tp shift_post, _Ts... sss_others) const {
    CheckType<_Tp>();

    const int num_inputs = 3 + sizeof...(sss_others);
    if (num_inputs != 3 * channels) {
      std::ostringstream msg;
      msg << "`Normalize` expects `shift_pre`, `scale` and `shift_post` per "
             "channel, i.e. " << (3 * channels) << " values, but got "
          << num_inputs << '!';
      throw std::invalid_argument(msg.str());
    }

    const _Tp sss[num_inputs] = {
      shift_pre, scale, shift_post, static_cast<_Tp>(sss_others)...};
    ImageBuffer dst(height, width, channels, output_type);
    using dst_type = image_buffer_t<output_type>;

    int rows = height;
    int cols = width;

    if (IsContiguous()) {
      cols *= rows;
      rows = 1;
    }

    for (int row = 0; row < rows; ++row) {
      for (int col = 0; col < cols; ++col) {
        for (int ch = 0; ch < channels; ++ch) {
          const dst_type val = static_cast<dst_type>(
                ((AtUnchecked<_Tp>(row, col, ch) +  sss[ch * 3]) * sss[(ch * 3) + 1])
              + sss[(ch * 3) + 2]);
          dst.AtUnchecked<dst_type>(row, col, ch) = val;
        }
      }
    }
    return dst;
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
  ///   pixel_stride: Number of bytes between neighboring pixels.
  ///   buffer_type: Element type.
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
  ///   pixel_stride: Number of bytes between neighboring pixels.
  ///   buffer_type: Element type.
  void CreateCopiedBuffer(unsigned char const *buffer,
      int height, int width, int channels, int row_stride,
      int column_stride, int channel_stride, ImageBufferType buffer_type);


  /// Returns a deep copy.
  ImageBuffer DeepCopy() const;


  /// Returns a shared ImageBuffer which points to the specified axis-aligned
  /// region-of-interest. This buffer will usually NOT be contiguous.
  ImageBuffer ROI(int left, int top, int roi_width, int roi_height);


  /// Swaps the specified (0-based) channels *in-place*.
  void SwapChannels(int ch1, int ch2);


  /// Returns a copy of this ImageBuffer with the given number
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


  /// Converts this buffer to `float`.
  /// If the underlying type is integral (`uint8`,
  /// `int16`, etc.), the values will be **divided by 255**.
  /// Number of channels remains the same.
  ImageBuffer ToFloat() const;


  /// Returns a copy of this buffer converted to the given type.
  /// Before type casting (and thus, potential value clipping), the values
  /// will be scaled by the given scaling factor.
  ImageBuffer AsType(
      ImageBufferType type, double scaling_factor=1.0) const;


  //TODO Gradient (sobel, border handling)

//FIXME extend to any/all channels
  /// Computes the magnitude of a dual-channel image, e.g. an optical flow
  /// field or an image gradient. Only implemented for buffers of type float
  /// or double. Output buffer type will be the same as this buffer's.
  ImageBuffer Magnitude() const;


  /// Computes the orientation in radians of a dual-channel image, e.g. an
  /// optical flow field or an image gradient. Only implemented for buffers of
  /// type float or double. Output buffer type will be the same as this
  /// buffer's.
  /// If both .At(r,c,0) and .At(r,c,1) are zero, the output value will be set
  /// to the specified `invalid` value.
  ImageBuffer Orientation(
      float invalid = std::numeric_limits<float>::quiet_NaN()) const;


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


  /// Returns an alpha-blended image.
  ///
  /// Creates a new image as the result of
  /// :math:`(1 - \alpha_{r,c}) * \text{self}_{r,c} + \alpha_{r,c} * \text{other}_{r,c}``,
  /// where :math:`alpha` is a weight mask.
  /// If the mask provides multiple channels, the blending weights will be
  /// taken from the corresponding channel. Otherwise, the blending weights
  /// will be taken from the first mask channel.
  ///
  /// If the number of channels of the two images is not the same, the number
  /// of output channels will be the maximum of ``self.channels``
  /// and ``other.channels``. In this case, *non-blendable* channels
  /// are copied from the input buffer which has more channels.
  ImageBuffer Blend(
      const ImageBuffer &other, const ImageBuffer &weights) const;


  /// Returns a single-channel buffer deeply copied from this ImageBuffer.
  ImageBuffer Channel(int channel) const;


  /// Returns a dimmed version of this image by element-wise
  /// multiplication of alpha and the corresponding pixel value.
  ImageBuffer Dim(double alpha) const;


  /// Returns true if this buffer points to a valid memory location.
  bool IsValid() const;


  /// Computes the minimum & maximum values over a single channel.
  /// To compute only the locations, pass nullptr for min_val/max_val.
  /// A negative channel index is only allowed for single-channel buffers.
  /// Otherwise, std::out_of_range will be thrown, unless you provide a valid
  /// (zero-based) channel index.
  void MinMaxLocation(
      double *min_val, double *max_val,
      Vec2i *min_loc = nullptr, Vec2i *max_loc = nullptr,
      int channel = -1) const;


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
      std::ostringstream msg;
      msg << "Buffer index (row=" << row << ", col="
          << col << ", ch=" << channel
          << ") is out of range for ImageBuffer of size h="
          << height << ", w=" << width << " a " << channels << " channels!";
      throw std::out_of_range(msg.str());
    }
  }


  template <typename _Tp> inline
  void CheckType() const {
    if (typeid(_Tp) != ImageBufferTypeInfo(buffer_type)) {
      std::string msg("Invalid template type with typeid `");
      msg += typeid(_Tp).name();
      msg += "`, but buffer is of type `";
      msg += ImageBufferTypeToString(buffer_type);
      msg += "`!";
      throw std::logic_error(msg);
    }
  }


  /// Returns the offset in bytes to the given indices.
  inline int ByteOffset(int row, int col, int channel) const {
    return (row * row_stride) + (col * pixel_stride) + (channel * element_size);
  }
};


// TODO(interface) - other color conversions (i.e. rgb2gray,
// gray2rgb) should be added here, too
// Then, add "ImageBuffer utils" doc section on RTD


/// Returns a uint8 single channel mask where a pixel is set to 255 iff
/// the corresponding hsv pixel is within the given range.
/// Args:
///   hsv: Color image in HSV format (uint8), see `ConvertRGB2HSV`.
///   hue_range: Hue range as ``(min_hue, max_hue)``, where
///     hue values are in `[0, 360]`.
///   saturation_range: Saturation range as
///     ``(min_saturation, max_saturation)``, where saturation
///     values are in `[0, 1]`.
///   value_range: Similar to saturation range, *i.e.* values in `[0, 1]`.
ImageBuffer MaskHSVRange(
    const ImageBuffer &hsv,
    const std::pair<float, float> &hue_range,
    const std::pair<float, float> &saturation_range,
    const std::pair<float, float> &value_range);


/// Implements a *color pop* effect, *i.e.* colors within the given HSV
/// range remain as-is, whereas all other colors are converted to
/// grayscale.
/// Args:
///   rgb: Color image in RGB(A)/BGR(A) format.
///   hue_range: Hue range as ``(min_hue, max_hue)``, where
///     hue values are in `[0, 360]`.
///   saturation_range: Saturation range as
///     ``(min_saturation, max_saturation)``, where saturation
///     values are in `[0, 1]`.
///   value_range: Similar to saturation range, *i.e.* values in `[0, 1]`.
///   is_bgr: Set to ``true`` if the color image is provided in BGR(A) format.
ImageBuffer ColorPop(const ImageBuffer &image,
    const std::pair<float, float> &hue_range,
    const std::pair<float, float> &saturation_range,
    const std::pair<float, float> &value_range,
    bool is_bgr = false);


/// Returns the grayscale image.
///
/// Args:
///   output_channels: Number of output channels as :class:`int`, must
///     be ``<=4``. The first (up to) 3 channels will contain the repeated
///     luminance, whereas the 4th channel will always be 255 (*i.e.* alpha,
///     fully opaque).
///   is_bgr: Set to ``true`` if the channels of the color image are in BGR format.
ImageBuffer ConvertRGB2Gray(
    const ImageBuffer &color,
    int output_channels = 1,
    bool is_bgr_format = false);


/// Converts a RGB(A)/BGR(A) image to HSV. Input image must be of type uint8.
///
/// Returns:
///   A 3-channel uint8 image, where hue in [0, 180], saturation in [0, 255]
///   and value in [0, 255].
ImageBuffer ConvertRGB2HSV(
    const ImageBuffer &image_rgb,
    bool is_bgr_format = false);


/// Converts a HSV image to RGB(A)/BGR(A).
/// Input image must be of type uint8, where hue in [0, 180], saturation in [0, 255]
/// and value in [0, 255].
///
/// If output_channels is 4, the fourth channel will be set to 255 (i.e. a
/// fully opaque alpha channel).
ImageBuffer ConvertHSV2RGB(
    const ImageBuffer &image_hsv,
    int output_channels = 3,
    bool output_bgr_format = false);


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
