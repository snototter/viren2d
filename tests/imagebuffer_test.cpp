#include <exception>

#include <gtest/gtest.h>

#include <viren2d/imagebuffer.h>

TEST(ImageBufferTest, ImageLoading) {
  viren2d::ImageBuffer empty;
  EXPECT_EQ(empty.ImmutableData(), nullptr);
  EXPECT_FALSE(empty.IsValid());

  empty = viren2d::ImageBuffer(10, 20, 1, viren2d::ImageBufferType::UInt8);
  EXPECT_TRUE(empty.IsValid());
  EXPECT_EQ(empty.Width(), 10);
  EXPECT_EQ(empty.Height(), 20);
  EXPECT_EQ(empty.Channels(), 1);
  EXPECT_EQ(empty.ItemSize(), 1);
  EXPECT_EQ(empty.BufferType(), viren2d::ImageBufferType::UInt8);

  // Load non-existing file
  EXPECT_THROW(viren2d::LoadImage("this-file-does-not-exist.png"),
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
  EXPECT_EQ(buffer_rgb.Channels(), 3);
  EXPECT_EQ(buffer_rgba.Channels(), 4);

  std::vector<viren2d::ImageBuffer*> ptrs {&buffer_gray, &buffer_graya, &buffer_rgb, &buffer_rgba};
  for (size_t i = 0; i < ptrs.size(); ++i) {
    // Check that the image dimensions match
    EXPECT_EQ(ptrs[i]->Width(), viren2d_EXAMPLE_IMAGE_WIDTH);
    EXPECT_EQ(ptrs[i]->Height(), viren2d_EXAMPLE_IMAGE_HEIGHT);

    // After creation, the ImageBuffer must have
    // allocated a continuous memory block
    EXPECT_TRUE(ptrs[i]->IsContiguous());
    EXPECT_EQ(ptrs[i]->Stride(),
              ptrs[i]->Width() * ptrs[i]->Channels() * sizeof(unsigned char));
    // ... and RGB images should always be loaded as uint8.
    EXPECT_EQ(ptrs[i]->ItemSize(), 1);
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
          ptrs[i]->MutableData(), ptrs[i]->Width(),
          ptrs[i]->Height(), ptrs[i]->Channels(),
          ptrs[i]->Stride(), ptrs[i]->BufferType());
    EXPECT_FALSE(tmp.OwnsData());
    EXPECT_TRUE(ptrs[i]->OwnsData());

    tmp.MutableData()[0] = ptrs[i]->ImmutableData()[0] + 23;
    tmp.MutableData()[1] = ptrs[i]->ImmutableData()[1] + 77;
    tmp.MutableData()[2] = ptrs[i]->ImmutableData()[2] + 42;
    EXPECT_EQ(ptrs[i]->ImmutableData()[0], tmp.ImmutableData()[0]);
    EXPECT_EQ(ptrs[i]->ImmutableData()[1], tmp.ImmutableData()[1]);
    EXPECT_EQ(ptrs[i]->ImmutableData()[2], tmp.ImmutableData()[2]);

    // Create a copy
    tmp.CreateCopy(
          ptrs[i]->ImmutableData(), ptrs[i]->Width(),
          ptrs[i]->Height(), ptrs[i]->Channels(),
          ptrs[i]->Stride(), ptrs[i]->BufferType());
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


// Helper to verify that all pixels of a channel have
// the same value
void CheckChannelConstant(
    const viren2d::ImageBuffer &buf,
    std::size_t channel, unsigned char value) {
  // Currently, we only need to work with contiguous
  // memory buffers in the test cases which use this
  // verification routine.
  EXPECT_EQ(buf.Stride(),
            buf.Width() * buf.Channels() * sizeof(unsigned char));

  for (int px = 0; px < buf.Width() * buf.Height(); px += buf.Channels()) {
    EXPECT_EQ((buf.ImmutableData())[px + channel], value);
  }
}

void CheckChannelEqual(
    const viren2d::ImageBuffer &buf1, std::size_t ch1,
    const viren2d::ImageBuffer &buf2, std::size_t ch2) {
  EXPECT_EQ(buf1.Width(), buf2.Width());
  EXPECT_EQ(buf1.Height(), buf2.Height());
  EXPECT_EQ(buf1.BufferType(), buf2.BufferType());
  EXPECT_EQ(buf1.BufferType(), viren2d::ImageBufferType::UInt8);//FIXME error message - this check is only supported for uint8 so far

  // FIXME double-check
  // Iterate each row, byte per byte
  for (int row = 0; row < buf1.Height(); ++row) {
//    const unsigned char *p1 = buf1.data + row * buf1.stride;
//    const unsigned char *p2 = buf2.data + row * buf2.stride;
    unsigned char const *p1 = buf1.ImmutableRowPtr<unsigned char>(row);
    unsigned char const *p2 = buf2.ImmutableRowPtr<unsigned char>(row);

    for (int col = 0; col < buf1.Width(); ++col) {
      EXPECT_EQ(
          p1[col * buf1.Channels() + ch1],
          p2[col * buf2.Channels() + ch2]);
    }
  }
}


TEST(ImageBufferTest, Conversion) {
  // Load existing file as grayscale & rgb
  auto buffer_gray = viren2d::LoadImage(viren2d_EXAMPLE_IMAGE_FILE, 1);
  auto buffer_rgb = viren2d::LoadImage(viren2d_EXAMPLE_IMAGE_FILE, 3);

  // Check that the correct number of channels has been loaded
  EXPECT_EQ(buffer_gray.Channels(), 1);
  EXPECT_EQ(buffer_rgb.Channels(), 3);

  // Actual test case: try different conversion scenarios
  // A) From grayscale to other formats
  auto res = buffer_gray.ToChannels(1);
  EXPECT_EQ(res.Channels(), 1);
  EXPECT_TRUE(res.OwnsData());
  CheckChannelEqual(res, 0, buffer_gray, 0);

  EXPECT_THROW(buffer_gray.ToChannels(2), std::invalid_argument);

  res = buffer_gray.ToChannels(3);
  EXPECT_EQ(res.Channels(), 3);
  EXPECT_TRUE(res.OwnsData());
  CheckChannelEqual(res, 0, buffer_gray, 0);
  CheckChannelEqual(res, 1, buffer_gray, 0);
  CheckChannelEqual(res, 2, buffer_gray, 0);

  res = buffer_gray.ToChannels(4);
  EXPECT_EQ(res.Channels(), 4);
  EXPECT_TRUE(res.OwnsData());
  CheckChannelEqual(res, 0, buffer_gray, 0);
  CheckChannelEqual(res, 1, buffer_gray, 0);
  CheckChannelEqual(res, 2, buffer_gray, 0);
  CheckChannelConstant(res, 3, 255);

  // B) From RGB to other formats
  // (only 3 & 4 output channels are
  //  currently supported)
  EXPECT_THROW(buffer_rgb.ToChannels(1), std::invalid_argument);

  EXPECT_THROW(buffer_rgb.ToChannels(2), std::invalid_argument);

  res = buffer_rgb.ToChannels(3);
  EXPECT_EQ(res.Channels(), 3);
  EXPECT_TRUE(res.OwnsData());
  CheckChannelEqual(res, 0, buffer_rgb, 0);
  CheckChannelEqual(res, 1, buffer_rgb, 1);
  CheckChannelEqual(res, 2, buffer_rgb, 2);

  res = buffer_rgb.ToChannels(4);
  EXPECT_EQ(res.Channels(), 4);
  EXPECT_TRUE(res.OwnsData());
  CheckChannelEqual(res, 0, buffer_rgb, 0);
  CheckChannelEqual(res, 1, buffer_rgb, 1);
  CheckChannelEqual(res, 2, buffer_rgb, 2);
  CheckChannelConstant(res, 3, 255);
}


//FIXME test image buffer with different dtypes
//FIXME test image buffer access
TEST(ImageBufferTest, FloatBuffer) {
  viren2d::ImageBuffer buffer(10, 5, 3, viren2d::ImageBufferType::Float);
  float v = 0.0f;

  for (int row = 0; row < buffer.Height(); ++row) {
    for (int col = 0; col < buffer.Width(); ++col) {
      for (int channel = 0; channel < buffer.Channels(); ++channel) {
        buffer.At<float>(row, col, channel) = v;
        v += 1.0;
      }
    }
  }

  EXPECT_EQ(sizeof(float), 4);
  EXPECT_EQ(4, viren2d::ItemSizeFromImageBufferType(viren2d::ImageBufferType::Float));

  union U {
    std::uint8_t as_bytes[4];
    std::float_t as_float;
  } value;
  value.as_float = 1.0f;

  EXPECT_DOUBLE_EQ(buffer.At<float>(0, 1, 0), value.as_float);

  value.as_float = 0.0f;
  unsigned char const *uchar_ptr = buffer.ImmutableData();

  for (int row = 0; row < buffer.Height(); ++row) {
    float const *row_ptr = buffer.ImmutableRowPtr<float>(row);
    for (int col = 0; col < buffer.Width(); ++col) {
      for (int channel = 0; channel < buffer.Channels(); ++channel) {
        EXPECT_DOUBLE_EQ(buffer.At<float>(row, col, channel), value.as_float);

        EXPECT_EQ(row_ptr[col * buffer.Channels() + channel], value.as_float);

        for (int b = 0; b < 4; ++b) {
          EXPECT_EQ(
              uchar_ptr[row * buffer.Stride() + col * buffer.Channels() * buffer.ItemSize() + channel * buffer.ItemSize() + b],
              value.as_bytes[b]);
        }
        value.as_float += 1.0;
      }
    }
  }
}
