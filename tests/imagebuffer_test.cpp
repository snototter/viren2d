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
    EXPECT_TRUE(ptrs[i]->owns_data_);

    // Create a shared buffer
    viren2d::ImageBuffer tmp;
    tmp.CreateSharedBuffer(ptrs[i]->data, ptrs[i]->width,
                           ptrs[i]->height, ptrs[i]->channels,
                           ptrs[i]->stride);
    EXPECT_FALSE(tmp.owns_data_);
    EXPECT_TRUE(ptrs[i]->owns_data_);

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
    EXPECT_TRUE(tmp.owns_data_);
    EXPECT_TRUE(ptrs[i]->owns_data_);

    // Test in-place channel flipping for RGB & RGBA images:
    if (tmp.channels > 2) {
      // 1) Check if all pixels are equal
      for (int px = 0; px < tmp.width * tmp.height; px+=tmp.channels) {
        EXPECT_TRUE(tmp.data[px] == ptrs[i]->data[px]);
        EXPECT_TRUE(tmp.data[px + 1] == ptrs[i]->data[px + 1]);
        EXPECT_TRUE(tmp.data[px + 2] == ptrs[i]->data[px + 2]);
      }
      // 2) Flip channels in place
      ptrs[i]->RGB2BGR();
      // 3) Verify all pixels
      for (int px = 0; px < tmp.width * tmp.height; px+=tmp.channels) {
        EXPECT_TRUE(tmp.data[px] == ptrs[i]->data[px + 2]);
        EXPECT_TRUE(tmp.data[px + 1] == ptrs[i]->data[px + 1]);
        EXPECT_TRUE(tmp.data[px + 2] == ptrs[i]->data[px]);
      }
    } else {
      // Swapping red and blue is impossible for grayscale buffers:
      EXPECT_THROW(ptrs[i]->RGB2BGR(), std::logic_error);

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


TEST(ImageBufferTest, Conversion) {
  // TODO create a dummy image via a fixture!

  // TODO ImageBuffer.RGB, ImageBuffer.RGBA, ...
  // Helpers: Gray2RGB(A), RGB(A)2RGB(A)
}

//TODO add pytests

