#include <exception>

#include <gtest/gtest.h>

#include <viren2d/primitives.h>

TEST(ImageBufferTest, ImageLoading) {
  viren2d::ImageBuffer empty;
  EXPECT_EQ(empty.data, nullptr);
  EXPECT_FALSE(empty.IsValid());

  empty = viren2d::ImageBuffer(10, 20, 1);
  EXPECT_TRUE(empty.IsValid());
  EXPECT_EQ(empty.width, 10);
  EXPECT_EQ(empty.height, 20);
  EXPECT_EQ(empty.channels, 1);

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
  EXPECT_EQ(buffer_gray.channels, 1);
  EXPECT_EQ(buffer_graya.channels, 2);
  EXPECT_EQ(buffer_rgb.channels, 3);
  EXPECT_EQ(buffer_rgba.channels, 4);

  std::vector<viren2d::ImageBuffer*> ptrs {&buffer_gray, &buffer_graya, &buffer_rgb, &buffer_rgba};
  for (size_t i = 0; i < ptrs.size(); ++i) {
    // Check that the image dimensions match
    EXPECT_EQ(ptrs[i]->width, viren2d_EXAMPLE_IMAGE_WIDTH);
    EXPECT_EQ(ptrs[i]->height, viren2d_EXAMPLE_IMAGE_HEIGHT);

    // After creation, the ImageBuffer must have
    // allocated a continuous memory block
    EXPECT_EQ(ptrs[i]->stride,
              ptrs[i]->width * ptrs[i]->channels * sizeof(unsigned char));

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
    tmp.CreateSharedBuffer(ptrs[i]->data, ptrs[i]->width,
                           ptrs[i]->height, ptrs[i]->channels,
                           ptrs[i]->stride);
    EXPECT_FALSE(tmp.OwnsData());
    EXPECT_TRUE(ptrs[i]->OwnsData());

    tmp.data[0] = ptrs[i]->data[0] + 23;
    tmp.data[1] = ptrs[i]->data[1] + 77;
    tmp.data[2] = ptrs[i]->data[2] + 42;
    EXPECT_TRUE(ptrs[i]->data[0] == tmp.data[0]);
    EXPECT_TRUE(ptrs[i]->data[1] == tmp.data[1]);
    EXPECT_TRUE(ptrs[i]->data[2] == tmp.data[2]);

    // Create a copy
    tmp.CreateCopy(ptrs[i]->data, ptrs[i]->width,
                   ptrs[i]->height, ptrs[i]->channels,
                   ptrs[i]->stride);
    // Both must now own their allocated data
    EXPECT_TRUE(tmp.OwnsData());
    EXPECT_TRUE(ptrs[i]->OwnsData());

    // Test in-place channel flipping for RGB & RGBA images:
    if (tmp.channels > 2) {
      // 1) Check if all pixels are equal
      for (int px = 0; px < tmp.width * tmp.height; px+=tmp.channels) {
        EXPECT_TRUE(tmp.data[px] == ptrs[i]->data[px]);
        EXPECT_TRUE(tmp.data[px + 1] == ptrs[i]->data[px + 1]);
        EXPECT_TRUE(tmp.data[px + 2] == ptrs[i]->data[px + 2]);
      }
      // 2) Flip channels in place
      ptrs[i]->SwapChannels(0, 2);
      // 3) Verify all pixels
      for (int px = 0; px < tmp.width * tmp.height; px+=tmp.channels) {
        EXPECT_TRUE(tmp.data[px] == ptrs[i]->data[px + 2]);
        EXPECT_TRUE(tmp.data[px + 1] == ptrs[i]->data[px + 1]);
        EXPECT_TRUE(tmp.data[px + 2] == ptrs[i]->data[px]);
      }
    } else {
      // Swapping red and blue is impossible for grayscale buffers:
      EXPECT_THROW(ptrs[i]->SwapChannels(0, 2), std::invalid_argument);

      // Verify that changing a few pixels of the copied buffer
      // does not affect the original buffer:
      tmp.data[0] = ptrs[i]->data[0] + 23;
      tmp.data[1] = ptrs[i]->data[1] + 77;
      tmp.data[2] = ptrs[i]->data[2] + 42;
      EXPECT_FALSE(ptrs[i]->data[0] == tmp.data[0]);
      EXPECT_FALSE(ptrs[i]->data[1] == tmp.data[1]);
      EXPECT_FALSE(ptrs[i]->data[2] == tmp.data[2]);
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
  EXPECT_EQ(buf.stride,
            buf.width * buf.channels * sizeof(unsigned char));

  for (int px = 0; px < buf.width * buf.height; px += buf.channels) {
    EXPECT_TRUE(buf.data[px + channel] == value);
  }
}

void CheckChannelEqual(
    const viren2d::ImageBuffer &buf1, std::size_t ch1,
    const viren2d::ImageBuffer &buf2, std::size_t ch2) {
  EXPECT_EQ(buf1.width, buf2.width);
  EXPECT_EQ(buf1.height, buf2.height);

  // Currently, ImageBuffer doesn't support random acces,
  // so we have to iterate the uint8 data manually.
  for (int row = 0; row < buf1.height; ++row) {
    const unsigned char *p1 = buf1.data + row * buf1.stride;
    const unsigned char *p2 = buf2.data + row * buf2.stride;

    for (int col = 0; col < buf1.width; ++col) {
      EXPECT_EQ(
          p1[col * buf1.channels + ch1],
          p2[col * buf2.channels + ch2]);
    }
  }
}


TEST(ImageBufferTest, Conversion) {
  // Load existing file as grayscale & rgb
  auto buffer_gray = viren2d::LoadImage(viren2d_EXAMPLE_IMAGE_FILE, 1);
  auto buffer_rgb = viren2d::LoadImage(viren2d_EXAMPLE_IMAGE_FILE, 3);

  // Check that the correct number of channels has been loaded
  EXPECT_EQ(buffer_gray.channels, 1);
  EXPECT_EQ(buffer_rgb.channels, 3);

  // Actual test case: try different conversion scenarios
  // A) From grayscale to other formats
  auto res = buffer_gray.ToChannels(1);
  EXPECT_EQ(res.channels, 1);
  EXPECT_TRUE(res.OwnsData());
  CheckChannelEqual(res, 0, buffer_gray, 0);

  EXPECT_THROW(buffer_gray.ToChannels(2), std::invalid_argument);

  res = buffer_gray.ToChannels(3);
  EXPECT_EQ(res.channels, 3);
  EXPECT_TRUE(res.OwnsData());
  CheckChannelEqual(res, 0, buffer_gray, 0);
  CheckChannelEqual(res, 1, buffer_gray, 0);
  CheckChannelEqual(res, 2, buffer_gray, 0);

  res = buffer_gray.ToChannels(4);
  EXPECT_EQ(res.channels, 4);
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
  EXPECT_EQ(res.channels, 3);
  EXPECT_TRUE(res.OwnsData());
  CheckChannelEqual(res, 0, buffer_rgb, 0);
  CheckChannelEqual(res, 1, buffer_rgb, 1);
  CheckChannelEqual(res, 2, buffer_rgb, 2);

  res = buffer_rgb.ToChannels(4);
  EXPECT_EQ(res.channels, 4);
  EXPECT_TRUE(res.OwnsData());
  CheckChannelEqual(res, 0, buffer_rgb, 0);
  CheckChannelEqual(res, 1, buffer_rgb, 1);
  CheckChannelEqual(res, 2, buffer_rgb, 2);
  CheckChannelConstant(res, 3, 255);
}
