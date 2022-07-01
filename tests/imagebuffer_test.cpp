#include <exception>

#include <gtest/gtest.h>
#include <werkzeugkiste/geometry/utils.h>

#include <viren2d/imagebuffer.h>

namespace wgu = werkzeugkiste::geometry;


template <typename _Tp>
::testing::AssertionResult CheckChannelConstantHelper(
    const viren2d::ImageBuffer &buf, int channel, _Tp value) {
  for (int row = 0; row < buf.Height(); ++row) {
    for (int col = 0; col < buf.Width(); ++col) {
      if ((buf.BufferType() == viren2d::ImageBufferType::Float)
          || (buf.BufferType() == viren2d::ImageBufferType::Double)) {
        if (!wgu::eps_equal(buf.AtChecked<_Tp>(row, col, channel), value)) {
          return ::testing::AssertionFailure()
              << "`CheckChannelConstant` (float/double): " << buf.ToString()
              << ", channel=" << channel << ", value=" << value
              << " differs at row=" << row << ", col=" << col;
        }
      } else {
        if (buf.AtChecked<_Tp>(row, col, channel) != value) {
          return ::testing::AssertionFailure()
              << "`CheckChannelConstant` (integral type): " << buf.ToString()
              << ", channel=" << channel << ", value=" << value
              << " differs at row=" << row << ", col=" << col
              << ", actual value=" << buf.AtChecked<_Tp>(row, col, channel);
        }
      }
    }
  }
  return ::testing::AssertionSuccess();
}


::testing::AssertionResult CheckChannelConstant(
    const viren2d::ImageBuffer &buf, int channel, double value) {
  switch (buf.BufferType()) {
    case viren2d::ImageBufferType::UInt8:
      return CheckChannelConstantHelper<uint8_t>(
            buf, channel, static_cast<uint8_t>(value));

    case viren2d::ImageBufferType::Int16:
      return CheckChannelConstantHelper<int16_t>(
            buf, channel, static_cast<int16_t>(value));

    case viren2d::ImageBufferType::Int32:
      return CheckChannelConstantHelper<int32_t>(
            buf, channel, static_cast<int32_t>(value));

    case viren2d::ImageBufferType::Float:
      return CheckChannelConstantHelper<float>(
            buf, channel, static_cast<float>(value));

    case viren2d::ImageBufferType::Double:
      return CheckChannelConstantHelper<double>(
            buf, channel, value);
  }

  return ::testing::AssertionFailure() << "ImageBufferType "
      << viren2d::ImageBufferTypeToString(buf.BufferType())
      << " was not handled in `CheckChannelConstant` switch!";
}



template<typename _Tp>
::testing::AssertionResult CheckChannelEqualsHelper(
    const viren2d::ImageBuffer &buf1, int ch1,
    const viren2d::ImageBuffer &buf2, int ch2) {
  for (int row = 0; row < buf1.Height(); ++row) {
    for (int col = 0; col < buf1.Width(); ++col) {
      if ((buf1.BufferType() == viren2d::ImageBufferType::Float)
          || (buf1.BufferType() == viren2d::ImageBufferType::Double)) {
        if (!wgu::eps_equal(
              buf1.AtChecked<_Tp>(row, col, ch1),
              buf2.AtChecked<_Tp>(row, col, ch2))) {
          return ::testing::AssertionFailure()
              << "`CheckChannelEquals` (float/double): " << buf1.ToString()
              << ", channel=" << ch1 << ", vs. " << buf2.ToString()
              << ", channel=" << ch2 << " differs at row=" << row << ", col=" << col
              << ". Values: " << static_cast<double>(buf1.AtChecked<_Tp>(row, col, ch1)) << " vs. "
              << static_cast<double>(buf2.AtChecked<_Tp>(row, col, ch2)) << '!';
        }
      } else {
        if (buf1.AtChecked<_Tp>(row, col, ch1) != buf2.AtChecked<_Tp>(row, col, ch2)) {
          return ::testing::AssertionFailure()
              << "`CheckChannelEquals` (integral type): " << buf1.ToString()
              << ", channel=" << ch1 << ", vs. " << buf2.ToString()
              << ", channel=" << ch2 << " differs at row=" << row << ", col=" << col
              << ". Values: " << static_cast<double>(buf1.AtChecked<_Tp>(row, col, ch1)) << " vs. "
              << static_cast<double>(buf2.AtChecked<_Tp>(row, col, ch2)) << '!';
        }
      }
    }
  }

  return ::testing::AssertionSuccess();
}


::testing::AssertionResult CheckChannelEquals(
    const viren2d::ImageBuffer &buf1, int ch1,
    const viren2d::ImageBuffer &buf2, int ch2) {
  if (buf1.BufferType() != buf2.BufferType()) {
    return ::testing::AssertionFailure()
        << "ImageBufferType differs: " << buf1.ToString()
        << " vs. " << buf2.ToString() << '!';
  }

  if ((buf1.Width() != buf2.Width())
      || (buf1.Height() != buf2.Height())) {
    return ::testing::AssertionFailure()
        << "ImageBuffer width/height mismatch: " << buf1.ToString()
        << " vs. " << buf2.ToString() << '!';
  }

  switch (buf1.BufferType()) {
    case viren2d::ImageBufferType::UInt8:
      return CheckChannelEqualsHelper<uint8_t>(buf1, ch1, buf2, ch2);

    case viren2d::ImageBufferType::Int16:
      return CheckChannelEqualsHelper<int16_t>(buf1, ch1, buf2, ch2);

    case viren2d::ImageBufferType::Int32:
      return CheckChannelEqualsHelper<int32_t>(buf1, ch1, buf2, ch2);

    case viren2d::ImageBufferType::Float:
      return CheckChannelEqualsHelper<float>(buf1, ch1, buf2, ch2);

    case viren2d::ImageBufferType::Double:
      return CheckChannelEqualsHelper<double>(buf1, ch1, buf2, ch2);
  }

  return ::testing::AssertionFailure() << "ImageBufferType "
      << viren2d::ImageBufferTypeToString(buf1.BufferType())
      << " was not handled in `CheckChannelEquals` switch!";
}


TEST(ImageBufferTest, ImageLoading) {
  viren2d::ImageBuffer empty;
  EXPECT_EQ(empty.ImmutableData(), nullptr);
  EXPECT_FALSE(empty.IsValid());

  empty = viren2d::ImageBuffer(20, 10, 1, viren2d::ImageBufferType::UInt8);
  EXPECT_TRUE(empty.IsValid());
  EXPECT_EQ(empty.Width(), 10);
  EXPECT_EQ(empty.Height(), 20);
  EXPECT_EQ(empty.Channels(), 1);
  EXPECT_EQ(empty.ElementSize(), 1);
  EXPECT_EQ(empty.BufferType(), viren2d::ImageBufferType::UInt8);

  // Load non-existing file
  EXPECT_THROW(
        viren2d::LoadImage("this-file-does-not-exist.png"),
        std::runtime_error);

  // Load existing file in different modes
  auto buffer_gray = viren2d::LoadImage(viren2d_EXAMPLE_IMAGE_FILE, 1);
  EXPECT_TRUE(buffer_gray.IsValid());
  auto buffer_graya = viren2d::LoadImage(viren2d_EXAMPLE_IMAGE_FILE, 2);
  EXPECT_TRUE(buffer_graya.IsValid());
  auto buffer_rgb = viren2d::LoadImage(viren2d_EXAMPLE_IMAGE_FILE, 3);
  EXPECT_TRUE(buffer_rgb.IsValid());
  auto buffer_rgba = viren2d::LoadImage(viren2d_EXAMPLE_IMAGE_FILE, 4);
  EXPECT_TRUE(buffer_rgba.IsValid());

  // Check that the correct number of channels has been loaded
  EXPECT_EQ(buffer_gray.Channels(), 1);
  EXPECT_EQ(buffer_graya.Channels(), 2);
  EXPECT_TRUE(CheckChannelConstant(buffer_graya, 1, 255));
  EXPECT_TRUE(CheckChannelEquals(buffer_gray, 0, buffer_graya, 0));

  EXPECT_EQ(buffer_rgb.Channels(), 3);
  EXPECT_EQ(buffer_rgba.Channels(), 4);
  EXPECT_TRUE(CheckChannelEquals(buffer_rgb, 0, buffer_rgba, 0));
  EXPECT_TRUE(CheckChannelEquals(buffer_rgb, 1, buffer_rgba, 1));
  EXPECT_TRUE(CheckChannelEquals(buffer_rgb, 2, buffer_rgba, 2));
  EXPECT_TRUE(CheckChannelConstant(buffer_rgba, 3, 255));

  // Check single channel extraction
  viren2d::ImageBuffer single_channel = buffer_gray.Channel(0);
  EXPECT_EQ(single_channel.Channels(), 1);
  EXPECT_TRUE(CheckChannelEquals(single_channel, 0, buffer_gray, 0));
  EXPECT_THROW(buffer_gray.Channel(-1), std::invalid_argument);
  EXPECT_THROW(buffer_gray.Channel(2), std::invalid_argument);

  single_channel = buffer_graya.Channel(0);
  EXPECT_EQ(single_channel.Channels(), 1);
  EXPECT_TRUE(CheckChannelEquals(single_channel, 0, buffer_graya, 0));
  EXPECT_TRUE(CheckChannelEquals(single_channel, 0, buffer_gray, 0));

  single_channel = buffer_graya.Channel(1);
  EXPECT_EQ(single_channel.Channels(), 1);
  EXPECT_TRUE(CheckChannelEquals(single_channel, 0, buffer_graya, 1));
  EXPECT_TRUE(CheckChannelConstant(single_channel, 0, 255));


  std::vector<viren2d::ImageBuffer*> ptrs {&buffer_gray, &buffer_graya, &buffer_rgb, &buffer_rgba};
  for (size_t i = 0; i < ptrs.size(); ++i) {
    // Check that the image dimensions match
    EXPECT_EQ(ptrs[i]->Width(), viren2d_EXAMPLE_IMAGE_WIDTH);
    EXPECT_EQ(ptrs[i]->Height(), viren2d_EXAMPLE_IMAGE_HEIGHT);

    // After creation, the ImageBuffer must have
    // allocated a continuous memory block
    EXPECT_TRUE(ptrs[i]->IsContiguous());
    EXPECT_EQ(ptrs[i]->RowStride(),
              ptrs[i]->Width() * ptrs[i]->Channels() * sizeof(unsigned char));
    // ... and RGB images should always be loaded as uint8.
    EXPECT_EQ(ptrs[i]->ElementSize(), 1);
    EXPECT_EQ(ptrs[i]->BufferType(), viren2d::ImageBufferType::UInt8);

    // The ImageBuffer must be the owner of the data
    EXPECT_TRUE(ptrs[i]->OwnsData());

    // Try invalid channel swaps (valid swaps are
    // tested later on)
    EXPECT_THROW(ptrs[i]->SwapChannels(0, 4), std::invalid_argument);
    EXPECT_THROW(ptrs[i]->SwapChannels(4, 0), std::invalid_argument);
    EXPECT_THROW(ptrs[i]->SwapChannels(0, -1), std::invalid_argument);
    EXPECT_THROW(ptrs[i]->SwapChannels(-1, 0), std::invalid_argument);

    // Create a shared buffer
    viren2d::ImageBuffer tmp;
    tmp.CreateSharedBuffer(
          ptrs[i]->MutableData(), ptrs[i]->Height(),
          ptrs[i]->Width(), ptrs[i]->Channels(),
          ptrs[i]->RowStride(), ptrs[i]->PixelStride(),
          ptrs[i]->BufferType());
    EXPECT_FALSE(tmp.OwnsData());
    EXPECT_TRUE(ptrs[i]->OwnsData());

    tmp.MutableData()[0] = ptrs[i]->ImmutableData()[0] + 23;
    tmp.MutableData()[1] = ptrs[i]->ImmutableData()[1] + 77;
    tmp.MutableData()[2] = ptrs[i]->ImmutableData()[2] + 42;
    EXPECT_EQ(ptrs[i]->ImmutableData()[0], tmp.ImmutableData()[0]);
    EXPECT_EQ(ptrs[i]->ImmutableData()[1], tmp.ImmutableData()[1]);
    EXPECT_EQ(ptrs[i]->ImmutableData()[2], tmp.ImmutableData()[2]);

    // Create a copy
    tmp.CreateCopiedBuffer(
          ptrs[i]->ImmutableData(), ptrs[i]->Height(),
          ptrs[i]->Width(), ptrs[i]->Channels(),
          ptrs[i]->RowStride(), ptrs[i]->PixelStride(),
          ptrs[i]->BufferType());
    // Both must now own their allocated data
    EXPECT_TRUE(tmp.OwnsData());
    EXPECT_TRUE(ptrs[i]->OwnsData());

    // Test in-place channel flipping for RGB & RGBA images:
    if (tmp.Channels() > 2) {
      // 1) Check if all pixels are equal
      for (int px = 0; px < tmp.Width() * tmp.Height(); px+=tmp.Channels()) {
        EXPECT_EQ(tmp.ImmutableData()[px], ptrs[i]->ImmutableData()[px]);
        EXPECT_EQ(tmp.ImmutableData()[px + 1], ptrs[i]->ImmutableData()[px + 1]);
        EXPECT_EQ(tmp.ImmutableData()[px + 2], ptrs[i]->ImmutableData()[px + 2]);
      }
      // 2) Flip channels in place
      ptrs[i]->SwapChannels(0, 2);
      // 3) Verify all pixels
      for (int px = 0; px < tmp.Width() * tmp.Height(); px+=tmp.Channels()) {
        EXPECT_EQ(tmp.ImmutableData()[px], ptrs[i]->ImmutableData()[px + 2]);
        EXPECT_EQ(tmp.ImmutableData()[px + 1], ptrs[i]->ImmutableData()[px + 1]);
        EXPECT_EQ(tmp.ImmutableData()[px + 2], ptrs[i]->ImmutableData()[px]);
      }
    } else {
      // Swapping red and blue is impossible for grayscale buffers:
      EXPECT_THROW(ptrs[i]->SwapChannels(0, 2), std::invalid_argument);

      // Verify that changing a few pixels of the copied buffer
      // does not affect the original buffer:
      tmp.MutableData()[0] = ptrs[i]->ImmutableData()[0] + 23;
      tmp.MutableData()[1] = ptrs[i]->ImmutableData()[1] + 77;
      tmp.MutableData()[2] = ptrs[i]->ImmutableData()[2] + 42;
      EXPECT_NE(ptrs[i]->ImmutableData()[0], tmp.ImmutableData()[0]);
      EXPECT_NE(ptrs[i]->ImmutableData()[1], tmp.ImmutableData()[1]);
      EXPECT_NE(ptrs[i]->ImmutableData()[2], tmp.ImmutableData()[2]);
    }
  }
}



TEST(ImageBufferTest, Conversion) {
  // Load existing file as grayscale & rgb
  viren2d::ImageBuffer buffer_gray = viren2d::LoadImage(viren2d_EXAMPLE_IMAGE_FILE, 1);
  viren2d::ImageBuffer buffer_rgb = viren2d::LoadImage(viren2d_EXAMPLE_IMAGE_FILE, 3);

  // Check that the correct number of channels has been loaded
  EXPECT_EQ(buffer_gray.Channels(), 1);
  EXPECT_EQ(buffer_rgb.Channels(), 3);

  // Actual test case: try different conversion scenarios
  // A) From grayscale to other formats
  auto res = buffer_gray.ToChannels(1);
  EXPECT_EQ(res.Channels(), 1);
  EXPECT_TRUE(res.OwnsData());
  EXPECT_TRUE(CheckChannelEquals(res, 0, buffer_gray, 0));

  EXPECT_THROW(buffer_gray.ToChannels(2), std::invalid_argument);

  res = buffer_gray.ToChannels(3);
  EXPECT_EQ(res.Channels(), 3);
  EXPECT_TRUE(res.OwnsData());
  EXPECT_TRUE(CheckChannelEquals(res, 0, buffer_gray, 0));
  EXPECT_TRUE(CheckChannelEquals(res, 1, buffer_gray, 0));
  EXPECT_TRUE(CheckChannelEquals(res, 2, buffer_gray, 0));

  res = buffer_gray.ToChannels(4);
  EXPECT_EQ(res.Channels(), 4);
  EXPECT_TRUE(res.OwnsData());
  EXPECT_TRUE(CheckChannelEquals(res, 0, buffer_gray, 0));
  EXPECT_TRUE(CheckChannelEquals(res, 1, buffer_gray, 0));
  EXPECT_TRUE(CheckChannelEquals(res, 2, buffer_gray, 0));
  EXPECT_TRUE(CheckChannelConstant(res, 3, 255));

  // B) From RGB to other formats
  // (only 3 & 4 output channels are
  //  currently supported)
  EXPECT_THROW(buffer_rgb.ToChannels(1), std::invalid_argument);

  EXPECT_THROW(buffer_rgb.ToChannels(2), std::invalid_argument);

  res = buffer_rgb.ToChannels(3);
  EXPECT_EQ(res.Channels(), 3);
  EXPECT_TRUE(res.OwnsData());
  EXPECT_TRUE(CheckChannelEquals(res, 0, buffer_rgb, 0));
  EXPECT_TRUE(CheckChannelEquals(res, 1, buffer_rgb, 1));
  EXPECT_TRUE(CheckChannelEquals(res, 2, buffer_rgb, 2));

  res = buffer_rgb.ToChannels(4);
  EXPECT_EQ(res.Channels(), 4);
  EXPECT_TRUE(res.OwnsData());
  EXPECT_TRUE(CheckChannelEquals(res, 0, buffer_rgb, 0));
  EXPECT_TRUE(CheckChannelEquals(res, 1, buffer_rgb, 1));
  EXPECT_TRUE(CheckChannelEquals(res, 2, buffer_rgb, 2));
  EXPECT_TRUE(CheckChannelConstant(res, 3, 255));
}


TEST(ImageBufferTest, FloatBuffer) {
  viren2d::ImageBuffer buffer(5, 10, 3, viren2d::ImageBufferType::Float);
  float v = 0.0f;

  for (int row = 0; row < buffer.Height(); ++row) {
    for (int col = 0; col < buffer.Width(); ++col) {
      for (int channel = 0; channel < buffer.Channels(); ++channel) {
        buffer.AtChecked<float>(row, col, channel) = v;
        v += 1.0;
      }
    }
  }

  EXPECT_EQ(sizeof(float), 4);
  EXPECT_EQ(4, viren2d::ElementSizeFromImageBufferType(viren2d::ImageBufferType::Float));

  // MinMaxLocation test
  double minval, maxval;
  viren2d::Vec2i minloc, maxloc;
  buffer.MinMaxLocation(&minval, &maxval, &minloc, &maxloc, 2);
  EXPECT_EQ(minloc, viren2d::Vec2i(0, 0));
  EXPECT_EQ(maxloc, viren2d::Vec2i(9, 4));
  EXPECT_DOUBLE_EQ(minval, 2.0);
  EXPECT_DOUBLE_EQ(maxval, buffer.NumElements() - 1.0);


  // Check access via uchar vs float
  union U {
    std::uint8_t as_bytes[4];
    std::float_t as_float;
  } value;
  value.as_float = static_cast<float>(buffer.Channels());

  EXPECT_DOUBLE_EQ(buffer.AtChecked<float>(0, 1, 0), value.as_float);

  value.as_float = 0.0f;
  unsigned char const *uchar_ptr = buffer.ImmutableData();

  for (int row = 0; row < buffer.Height(); ++row) {
    float const *row_ptr = buffer.ImmutablePtr<float>(row, 0, 0);
    for (int col = 0; col < buffer.Width(); ++col) {
      for (int channel = 0; channel < buffer.Channels(); ++channel) {
        EXPECT_DOUBLE_EQ(buffer.AtChecked<float>(row, col, channel), value.as_float);

        EXPECT_EQ(row_ptr[col * buffer.Channels() + channel], value.as_float);

        for (int b = 0; b < 4; ++b) {
          EXPECT_EQ(
              uchar_ptr[row * buffer.RowStride() + col * buffer.Channels() * buffer.ElementSize() + channel * buffer.ElementSize() + b],
              value.as_bytes[b]);
        }
        value.as_float += 1.0;
      }
    }
  }

  viren2d::ImageBuffer copy = buffer.DeepCopy();
  EXPECT_TRUE(CheckChannelEquals(copy, 0, buffer, 0));
  EXPECT_TRUE(CheckChannelEquals(copy, 1, buffer, 1));
  EXPECT_TRUE(CheckChannelEquals(copy, 2, buffer, 2));

  copy.SwapChannels(0, 1);
  EXPECT_TRUE(CheckChannelEquals(copy, 1, buffer, 0));
  EXPECT_TRUE(CheckChannelEquals(copy, 0, buffer, 1));
  EXPECT_TRUE(CheckChannelEquals(copy, 2, buffer, 2));

  copy.SwapChannels(1, 2);
  EXPECT_TRUE(CheckChannelEquals(copy, 2, buffer, 0));
  EXPECT_TRUE(CheckChannelEquals(copy, 0, buffer, 1));
  EXPECT_TRUE(CheckChannelEquals(copy, 1, buffer, 2));


  viren2d::ImageBuffer single_channel = buffer.Channel(0);
  EXPECT_EQ(single_channel.Channels(), 1);
  EXPECT_TRUE(CheckChannelEquals(single_channel, 0, buffer, 0));

  single_channel = buffer.Channel(1);
  EXPECT_EQ(single_channel.Channels(), 1);
  EXPECT_EQ(single_channel.BufferType(), buffer.BufferType());
  EXPECT_TRUE(CheckChannelEquals(single_channel, 0, buffer, 1));

  single_channel = buffer.Channel(2);
  EXPECT_EQ(single_channel.Channels(), 1);
  EXPECT_TRUE(CheckChannelEquals(single_channel, 0, buffer, 2));

  EXPECT_THROW(buffer.Channel(-1), std::invalid_argument);
  EXPECT_THROW(buffer.Channel(3), std::invalid_argument);
}


inline double GrayReference(double r, double g, double b) {
  return (0.2989 * r) + (0.5870 * g) + (0.1141 * b);
}


TEST(ImageBufferTest, GrayscaleDouble) {
  viren2d::ImageBuffer buf(3, 1, 3, viren2d::ImageBufferType::Double);
  EXPECT_EQ(buf.Width(), 1);
  EXPECT_EQ(buf.Height(), 3);
  EXPECT_EQ(buf.Channels(), 3);
  buf.AtChecked<double>(0, 0, 0) = 1.0;
  buf.AtChecked<double>(0, 0, 1) = 1.0;
  buf.AtChecked<double>(0, 0, 2) = 1.0;

  buf.AtChecked<double>(1, 0, 0) = 0.1;
  buf.AtChecked<double>(1, 0, 1) = 10.0;
  buf.AtChecked<double>(1, 0, 2) = 100.0;

  buf.AtChecked<double>(2, 0, 0) = 1234.0;
  buf.AtChecked<double>(2, 0, 1) = 5678.0;
  buf.AtChecked<double>(2, 0, 2) = 9.0;

  // Sidetrack: MinMaxLocation for double precision buffer
  double minval, maxval;
  viren2d::Vec2i minloc, maxloc;
  buf.MinMaxLocation(&minval, &maxval, &minloc, &maxloc, 0);
  EXPECT_EQ(minloc, viren2d::Vec2i(0, 1));
  EXPECT_EQ(maxloc, viren2d::Vec2i(0, 2));
  EXPECT_DOUBLE_EQ(minval, 0.1);
  EXPECT_DOUBLE_EQ(maxval, 1234.0);

  buf.MinMaxLocation(&minval, &maxval, &minloc, &maxloc, 2);
  EXPECT_EQ(minloc, viren2d::Vec2i(0, 0));
  EXPECT_EQ(maxloc, viren2d::Vec2i(0, 1));
  EXPECT_DOUBLE_EQ(minval, 1.0);
  EXPECT_DOUBLE_EQ(maxval, 100.0);

  // Back to grayscale conversion
  viren2d::ImageBuffer gray = buf.ToGrayscale(4, false);
  EXPECT_EQ(gray.Width(), buf.Width());
  EXPECT_EQ(gray.Height(), buf.Height());
  EXPECT_EQ(gray.Channels(), 4);
  EXPECT_EQ(gray.BufferType(), buf.BufferType());

  // Check first layer
  EXPECT_DOUBLE_EQ(gray.AtChecked<double>(0, 0, 0), GrayReference(1.0, 1.0, 1.0));
  EXPECT_DOUBLE_EQ(gray.AtChecked<double>(1, 0, 0), GrayReference(0.1, 10.0, 100.0));
  EXPECT_DOUBLE_EQ(gray.AtChecked<double>(2, 0, 0), GrayReference(1234.0, 5678.0, 9.0));
  // Next two layers must be the same
  EXPECT_TRUE(CheckChannelEquals(gray, 0, gray, 1));
  EXPECT_TRUE(CheckChannelEquals(gray, 0, gray, 2));
  // Alpha channel check
  EXPECT_TRUE(CheckChannelConstant(gray, 3, 255.0));


  // Repeat with BGR format
  gray = buf.ToGrayscale(3, true);
  EXPECT_EQ(gray.Width(), buf.Width());
  EXPECT_EQ(gray.Height(), buf.Height());
  EXPECT_EQ(gray.Channels(), 3);

  // Check first layer
  EXPECT_DOUBLE_EQ(
        gray.AtChecked<double>(0, 0, 0),
        GrayReference(1.0, 1.0, 1.0));
  EXPECT_DOUBLE_EQ(
        gray.AtChecked<double>(1, 0, 0),
        GrayReference(100.0, 10.0, 0.1));
  EXPECT_DOUBLE_EQ(
        gray.AtChecked<double>(2, 0, 0),
        GrayReference(9.0, 5678.0, 1234.0));
  // Next two layers must be the same
  EXPECT_TRUE(CheckChannelEquals(gray, 0, gray, 1));
  EXPECT_TRUE(CheckChannelEquals(gray, 0, gray, 2));


  // Check that single-channel output yields the same conversion results
  viren2d::ImageBuffer gray2 = buf.ToGrayscale(1, true);
  EXPECT_EQ(gray2.Width(), buf.Width());
  EXPECT_EQ(gray2.Height(), buf.Height());
  EXPECT_EQ(gray2.Channels(), 1);
  EXPECT_EQ(gray2.BufferType(), buf.BufferType());

  EXPECT_TRUE(CheckChannelEquals(gray2, 0, gray, 0));
}


TEST(ImageBufferTest, GrayscaleUInt8) {
  viren2d::ImageBuffer buf(1, 5, 4, viren2d::ImageBufferType::UInt8);
  EXPECT_EQ(buf.Width(), 5);
  EXPECT_EQ(buf.Height(), 1);
  EXPECT_EQ(buf.Channels(), 4);
  buf.AtChecked<uint8_t>(0, 0, 0) = 1;
  buf.AtChecked<uint8_t>(0, 0, 1) = 1;
  buf.AtChecked<uint8_t>(0, 0, 2) = 1;
  buf.AtChecked<uint8_t>(0, 0, 3) = 100;

  buf.AtChecked<uint8_t>(0, 1, 0) = 100;
  buf.AtChecked<uint8_t>(0, 1, 1) = 1;
  buf.AtChecked<uint8_t>(0, 1, 2) = 10;
  buf.AtChecked<uint8_t>(0, 1, 3) = 200;

  buf.AtChecked<uint8_t>(0, 2, 0) = 10;
  buf.AtChecked<uint8_t>(0, 2, 1) = 1;
  buf.AtChecked<uint8_t>(0, 2, 2) = 100;
  buf.AtChecked<uint8_t>(0, 2, 3) = 255;

  buf.AtChecked<uint8_t>(0, 3, 0) = -1;
  buf.AtChecked<uint8_t>(0, 3, 1) = -1;
  buf.AtChecked<uint8_t>(0, 3, 2) = -1;
  buf.AtChecked<uint8_t>(0, 3, 3) = -1;

  buf.AtChecked<uint8_t>(0, 4, 0) = 255;
  buf.AtChecked<uint8_t>(0, 4, 1) = 255;
  buf.AtChecked<uint8_t>(0, 4, 2) = 255;
  buf.AtChecked<uint8_t>(0, 4, 3) = 5;

  viren2d::ImageBuffer gray = buf.ToGrayscale(4, false);
  EXPECT_EQ(gray.Width(), buf.Width());
  EXPECT_EQ(gray.Height(), buf.Height());
  EXPECT_EQ(gray.Channels(), 4);

  // Check first layer
  EXPECT_EQ(
        gray.AtChecked<uint8_t>(0, 0, 0),
        static_cast<uint8_t>(GrayReference(1.0, 1.0, 1.0)));
  EXPECT_EQ(
        gray.AtChecked<uint8_t>(0, 1, 0),
        static_cast<uint8_t>(GrayReference(100.0, 1.0, 10.0)));
  EXPECT_EQ(
        gray.AtChecked<uint8_t>(0, 2, 0),
        static_cast<uint8_t>(GrayReference(10.0, 1.0, 100.0)));
  EXPECT_EQ(
        gray.AtChecked<uint8_t>(0, 3, 0),
        static_cast<uint8_t>(GrayReference(255.0, 255.0, 255.0)));
  EXPECT_EQ(
        gray.AtChecked<uint8_t>(0, 4, 0),
        static_cast<uint8_t>(GrayReference(255.0, 255.0, 255.0)));
  // Next two layers must be the same
  EXPECT_TRUE(CheckChannelEquals(gray, 0, gray, 1));
  EXPECT_TRUE(CheckChannelEquals(gray, 0, gray, 2));
  // Alpha channel check
  EXPECT_EQ(gray.AtChecked<uint8_t>(0, 0, 3), 100);
  EXPECT_EQ(gray.AtChecked<uint8_t>(0, 1, 3), 200);
  EXPECT_EQ(gray.AtChecked<uint8_t>(0, 2, 3), 255);
  EXPECT_EQ(gray.AtChecked<uint8_t>(0, 3, 3), 255);
  EXPECT_EQ(gray.AtChecked<uint8_t>(0, 4, 3), 5);

  // Check that single-channel output yields the same conversion results
  viren2d::ImageBuffer gray2 = buf.ToGrayscale(1, false);
  EXPECT_EQ(gray2.Width(), buf.Width());
  EXPECT_EQ(gray2.Height(), buf.Height());
  EXPECT_EQ(gray2.Channels(), 1);
  EXPECT_EQ(gray2.BufferType(), buf.BufferType());

  EXPECT_TRUE(CheckChannelEquals(gray2, 0, gray, 0));
}


TEST(ImageBufferTest, ROIInt16) {
  viren2d::ImageBuffer buf(2, 5, 3, viren2d::ImageBufferType::Int16);
  EXPECT_EQ(buf.Channels(), 3);
  EXPECT_EQ(buf.NumElements(), 2 * 5 * 3);
  EXPECT_EQ(buf.NumBytes(), 2 * 5 * 3 * sizeof(short));
  EXPECT_EQ(buf.NumPixels(), 2 * 5);
  EXPECT_EQ(buf.ElementSize(), sizeof(short));
  EXPECT_EQ(buf.ElementSize(), sizeof(signed short int));
  EXPECT_EQ(
        buf.ElementSize(),
        sizeof(viren2d::image_buffer_t<viren2d::ImageBufferType::Int16>));


  for (int row = 0; row < buf.Height(); ++row) {
    for (int col = 0; col < buf.Width(); ++col) {
      short val = row * buf.Width() + col;
      for (int ch = 0; ch < buf.Channels(); ++ch) {
        buf.AtChecked<int16_t>(row, col, ch) = val + (ch * buf.NumPixels());
      }
    }
  }

  EXPECT_THROW(buf.ROI(5, 5, 10, 2), std::out_of_range);
  EXPECT_THROW(buf.ROI(1, 0, 1, 10), std::out_of_range);
  EXPECT_THROW(buf.ROI(1, 0, 1, 0), std::invalid_argument);
  //TODO check other invalid rois (negative numbers, etc)

  viren2d::ImageBuffer roi = buf.ROI(1, 0, 2, 1);
  EXPECT_EQ(roi.Width(), 2);
  EXPECT_EQ(roi.Height(), 1);
  EXPECT_EQ(roi.Channels(), buf.Channels());
  EXPECT_FALSE(roi.IsContiguous());
  EXPECT_FALSE(roi.OwnsData());

  const int num_pixels = buf.NumPixels();
  EXPECT_EQ(roi.AtChecked<short>(0, 0, 0), 1);
  EXPECT_EQ(roi.AtChecked<short>(0, 0, 1), 1 + num_pixels);
  EXPECT_EQ(roi.AtChecked<short>(0, 0, 2), 1 + 2 * num_pixels);

  EXPECT_THROW(roi.AtChecked<short>(1, 0, 0), std::out_of_range);
  EXPECT_NO_THROW(roi.AtUnchecked<short>(1, 0, 0));
  EXPECT_THROW(roi.AtChecked<short>(0, 2, 0), std::out_of_range);

  EXPECT_EQ(roi.AtChecked<short>(0, 1, 0), 2);
  EXPECT_EQ(roi.AtChecked<short>(0, 1, 1), 2 + num_pixels);
  EXPECT_EQ(roi.AtChecked<short>(0, 1, 2), 2 + 2 * num_pixels);


  roi = buf.ROI(1, 0, 1, 2);
  EXPECT_EQ(roi.Width(), 1);
  EXPECT_EQ(roi.Height(), 2);
  EXPECT_FALSE(roi.IsContiguous());
  EXPECT_FALSE(roi.OwnsData());

  EXPECT_EQ(roi.AtChecked<short>(0, 0, 0), 1);
  EXPECT_EQ(roi.AtChecked<short>(0, 0, 1), 1 + num_pixels);
  EXPECT_EQ(roi.AtChecked<short>(0, 0, 2), 1 + 2 * num_pixels);

  EXPECT_THROW(roi.AtChecked<short>(0, 1, 0), std::out_of_range);
  EXPECT_NO_THROW(roi.AtUnchecked<short>(0, 1, 0));
  EXPECT_THROW(roi.AtChecked<short>(0, 1, 0), std::out_of_range);

  EXPECT_EQ(roi.AtChecked<short>(1, 0, 0), 6);
  EXPECT_EQ(roi.AtChecked<short>(1, 0, 1), 6 + num_pixels);
  EXPECT_EQ(roi.AtChecked<short>(1, 0, 2), 6 + 2 * num_pixels);


  roi.SetToScalar<short>(0);
  for (int ch = 0; ch < buf.Channels(); ++ch) {
    EXPECT_TRUE(CheckChannelConstant(roi, ch, 0));
  }
  roi.SetToPixel<short>(3, 42);
  EXPECT_TRUE(CheckChannelConstant(roi, 0, 3));
  EXPECT_TRUE(CheckChannelConstant(roi, 1, 42));
  EXPECT_TRUE(CheckChannelConstant(roi, 2, 0));
}
