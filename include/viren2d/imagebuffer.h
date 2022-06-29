#ifndef __VIREN2D_IMAGEBUFFER_H__
#define __VIREN2D_IMAGEBUFFER_H__

#include <string>
#include <ostream>
#include <sstream>
#include <exception>

namespace viren2d {

/// Data types supported by the ImageBuffer class.
enum class ImageBufferType : int {
  UInt8 = 0,
  Int16,
  Int32,
  Float,
  Double
};


/// Returns the string representation.
std::string ImageBufferTypeToString(ImageBufferType t);


/// Returns the ImageBufferType corresponding to the given string representation.
ImageBufferType ImageBufferTypeFromString(const std::string &s);


/// Returns the size of the corresponding data type in bytes.
int ItemSizeFromImageBufferType(ImageBufferType t);


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
  inline int ColumnStride() const { return column_stride; }


  /// Returns the size in bytes of a single element/value.
  /// Multiply by Channels() to get the memory consumption per pixel.
  inline int ItemSize() const { return item_size; }


  /// Returns this buffers data type.
  inline ImageBufferType BufferType() const { return buffer_type; }


  /// Returns the number of elements (i.e. values of the
  /// chosen data type) in this buffer.
  inline int NumElements() const { return width * height * channels; }


  /// Returns the number of bytes.
  inline int NumBytes() const { return NumElements() * item_size; }


  /// Returns true if this ImageBuffer is responsible for
  /// cleaning up the corresponding data.
  inline bool OwnsData() const { return owns_data; }


  /// Sets this->owns_data = true;
  /// Obviously to be used only, if the calling code ensures that
  /// the former `data` owner will NOT free the memory.
  void TakeOwnership();


  /// Returns true if the underlying `data` memory is contiguous.
  inline bool IsContiguous() const {
    return (row_stride == width * channels * item_size)
        && (column_stride == channels * item_size);
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
    CheckAccess(row, col, channel);
    return AtUnchecked<_Tp>(row, col, channel);
  }


  /// Returns a read-only reference to the specified pixel element.
  template<typename _Tp> inline
  const _Tp& AtChecked(int row, int col, int channel=0) const {
    CheckAccess(row, col, channel);
    return AtUnchecked<_Tp>(row, col, channel);
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
      ImageBufferType buffer_type, int column_stride);


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
      ImageBufferType buffer_type, int column_stride);


  /// Returns a deep copy.
  ImageBuffer DeepCopy() const;


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


  //TODO
  ImageBuffer ToGrayscale(int output_channels, bool is_bgr_format=false) const;


  /// Returns a single-channel buffer deeply copied from this ImageBuffer.
  ImageBuffer Channel(int channel) const;


  /// Returns true if this buffer points to a valid memory location.
  bool IsValid() const;


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

  /// Size of a single element in bytes.
  int item_size;

  /// Number of bytes between subsequent rows.
  int row_stride;

  /// Number of bytes between subsequent columns.
  int column_stride;

  /// This buffer's data type.
  ImageBufferType buffer_type;

  /// Flag which indicates if this buffer owns the
  /// memory, i.e. if it is responsible for cleaning up.
  bool owns_data;


  /// Frees the memory if needed and resets
  /// the members accordingly.
  void Cleanup();


  inline void CheckAccess(int row, int col, int channel) const {
    if ((row < 0) || (row >= height)
        || (col < 0) || (col >= width)
        || (channel < 0) || (channel >= channels)) {
      std::ostringstream s;
      s << "Buffer index (row=" << row << ", col="
        << col << ", ch=" << channel
        << ") is out of range for ImageBuffer of size: "
        << height << "x" << width << "x" << channels << '!';
      throw std::out_of_range(s.str()); //FIXME change c'tor to H!!, W, C!!!
    }
  }


  inline int ByteOffset(int row, int col, int channel) const {
    return (row * row_stride) + (col * column_stride) + (channel * item_size);
  }
};


//TODO test with 16bit png - should work according to stb, need to check
// the imagebuffer setup
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


//TODO need to check 16-bit png support:
/// Saves the ImageBuffer to disk as either JPEG or PNG.
///
/// We use the stb/stb_image_write library for writing. Note
/// that PNG output is not optimal (20-50% larger file size
/// than using an optimized library).
/// I consider writing to disk only a nice-to-have feature,
/// thus I'm not including any other specialized third-party
/// libraries for that.
void SaveImage(const std::string &image_filename, const ImageBuffer &image);


//TODO(extension) rgb(a)2gray & bgr(a)2gray


/// Converts a grayscale ImageBuffer to RGB.
ImageBuffer Gray2RGB(const ImageBuffer &img);


/// Converts a grayscale ImageBuffer to RGBA.
ImageBuffer Gray2RGBA(const ImageBuffer &img);

//TODO refactor RGBx2RGB
/// Converts a RGBA ImageBuffer to RGB.
ImageBuffer RGBA2RGB(const ImageBuffer &img);

//TODO refactor RGBx2RGBA
/// Converts a RGB ImageBuffer to RGBA.
ImageBuffer RGB2RGBA(const ImageBuffer &img);

//TODO implement RGBx2Gray(buf, is_bgr_format, num_output_channels)

} // namespace viren2d

#endif // __VIREN2D_IMAGEBUFFER_H__
