import pytest
import numpy as np
import viren2d


def test_buffer_conversion():
    with pytest.raises(TypeError):
        viren2d.ImageBuffer(None)

    with pytest.raises(ValueError):
        viren2d.ImageBuffer(np.array([], dtype=np.uint8))

    # Although ImageBuffer is only used for 1-, 3-,
    # and 4-channel image data so far, we can still
    # test its creation with an arbitrary number of channels:
    for dt in [np.uint8, np.int16, np.int32, np.float32, np.float64]:
        for channels in [1, 2, 3, 4, 5, 10]:
            data = (255 * np.random.rand(5, 15, channels)).astype(dt)
            # Ensure that our later constant will not cause a broken test
            data[3, 4, :] = 99

            # Create image buffers (shared & copy)
            buf_shared = viren2d.ImageBuffer(data, copy=False)
            assert not buf_shared.owns_data
            assert buf_shared.shape == (5, 15, channels)
            assert buf_shared.channels == channels
            assert buf_shared.width == data.shape[1]
            assert buf_shared.height == data.shape[0]
            assert buf_shared.is_valid()

            buf_copied = viren2d.ImageBuffer(data, copy=True)
            assert buf_copied.owns_data
            assert buf_copied.shape == (5, 15, channels)
            
            # Convert back (the numpy arrays both *share* the memory
            # with their corresponding ImageBuffer)
            res_shared = np.array(buf_shared, copy=False)
            assert np.array_equal(data, res_shared)
            res_copied = np.array(buf_copied, copy=False)
            assert np.array_equal(data, res_copied)

            # Check if the memory is actually shared
            data[3, 4, :] = 42
            assert np.array_equal(data, res_shared)
            assert not np.array_equal(data, res_copied)

            # If we have a grayscale, rgb, or rgba image, also
            # test that to_rgb/a works as expected:
            if channels in [1, 3, 4]:
                as_rgb = buf_shared.to_rgb()
                assert as_rgb.channels == 3
                assert as_rgb.owns_data

                as_rgba = buf_shared.to_rgba()
                assert as_rgba.channels == 4
                assert as_rgba.owns_data

    # Create from a 2D array explicitly
    data = (255 * np.random.rand(5, 15)).astype(np.uint8)
    assert data.ndim == 2
    # Ensure that our later constant will not cause a broken test
    data[1, 2] = 99
    buf = viren2d.ImageBuffer(data, copy=False)
    assert buf.channels == 1
    assert buf.width == 15
    assert buf.height == 5
    assert buf.shape == (5, 15, 1)

    # For the sake of completeness, again convert back to
    # numpy and check that passing the data actually worked
    res_shared = np.array(buf, copy=False)
    # One caveat: an ImageBuffer will always result in
    # numpy arrays with ndim == 3
    assert res_shared.ndim == 3
    assert res_shared.shape == (5, 15, 1)
    # ... thus, we need to reshape the buffer to compare it's equal to the
    # input data:
    assert np.array_equal(data, res_shared.reshape((buf.height, -1)))
    # Finally, check that memory sharing also works as expected:
    data[1, 2] = 42
    assert np.array_equal(data, res_shared.reshape((buf.height, -1)))


def test_channel_swapping():
    data = (255 * np.random.rand(5, 15, 3)).astype(np.uint8)
    buf = viren2d.ImageBuffer(data, copy=True)

    # Sanity checks
    with pytest.raises(ValueError):
        buf.swap_channels(-1, 0)

    with pytest.raises(ValueError):
        buf.swap_channels(1, -1)

    with pytest.raises(ValueError):
        buf.swap_channels(0, buf.channels)

    with pytest.raises(ValueError):
        buf.swap_channels(buf.channels, 1)

    # Swap first two channels
    buf.swap_channels(0, 1)
    res = np.array(buf, copy=False)

    assert np.array_equal(data[:, :, 0], res[:, :, 1])
    assert np.array_equal(data[:, :, 1], res[:, :, 0])
    assert np.array_equal(data[:, :, 2], res[:, :, 2])

    # Change back
    buf.swap_channels(0, 1)
    assert np.array_equal(data, res)

    # Swap first and third channels, i.e. RGB <--> BGR
    buf.swap_channels(2, 0)
    assert np.array_equal(data[:, :, 0], res[:, :, 2])
    assert np.array_equal(data[:, :, 1], res[:, :, 1])
    assert np.array_equal(data[:, :, 0], res[:, :, 2])
    

def test_buffer_side_effects():
    data = (255 * np.random.rand(5, 15, 3)).astype(np.uint8)

    painter = viren2d.Painter()
    painter.set_canvas_image(data)

    shared_canvas = painter.get_canvas(copy=False)
    shared_canvas_copy = shared_canvas.copy()
    np_shared = np.array(shared_canvas, copy=False)
    np_copied = np.array(shared_canvas, copy=True)

    # We need to check that .copy() returned a deep copy (part 1)
    assert not shared_canvas.owns_data
    assert shared_canvas_copy.owns_data
    assert shared_canvas.shape == shared_canvas_copy.shape

    np_initial_canvas = np.array(painter.get_canvas(copy=False), copy=True)

    # The canvas should always be an RGBA image
    assert np_copied.ndim == 3
    assert np_copied.shape[2] == 4
    assert np.all(np_copied[:, :, 3] == 255)
    assert np.array_equal(np_copied[:, :, :3], data)

    # Did setting/getting the canvas work?
    assert np.array_equal(np_shared, np_copied)
    assert np.array_equal(np_initial_canvas, np_copied)

    # Now, change the canvas
    painter.draw_rect((0, 0, 100, 100), viren2d.LineStyle.Invalid, 'blue')
    # This mustn't change the copied buffer
    assert np.array_equal(np_copied[:, :, :3], data)
    # ... but the shared one should have changed
    assert not np.array_equal(np_shared[:, :, :3], data)
    assert np.all(np_shared[:, :, 0] == 0)
    assert np.all(np_shared[:, :, 1] == 0)
    assert np.all(np_shared[:, :, 2] == 255)
    assert np.all(np_shared[:, :, 3] == 255)

    # ... and of course, the initial canvas also
    # should not have changed
    assert np.array_equal(np_initial_canvas, np_copied)

    # Finally, part 2 of checking that .copy() returned a deep copy:
    old_deep_copy = np.array(shared_canvas_copy, copy=False)
    assert not np.array_equal(data, old_deep_copy)
    assert not np.array_equal(np_shared, old_deep_copy)
    assert np.array_equal(np_copied, old_deep_copy)


def test_dtypes():
    for channels in [1, 2, 3]:
        for tp in [np.uint8, np.int16, np.int32, np.float32, np.float64]:
            buf_np = np.ones((3, 5, channels), dtype=tp)
            buf_vi = viren2d.ImageBuffer(buf_np, copy=False)
            assert buf_vi.width == 5
            assert buf_vi.height == 3
            assert buf_vi.channels == channels
            #TODO idea: test with randomly initialized np array
            # convert back (with deep copy)
            # check format, shape, dtype
            # check values for equality

    for tp in [np.int8, np.uint16, np.uint32, np.uint64, np.int64, np.float16]:
        buf_np = np.ones((3, 5), dtype=tp)
        with pytest.raises(ValueError):
            viren2d.ImageBuffer(buf_np)

    invalid = np.asfortranarray(np.ones((3, 5), dtype=np.uint8))
    with pytest.raises(ValueError):
        viren2d.ImageBuffer(invalid)


def test_pixelation():
    img_np = np.ones((7, 5, 2), dtype=np.int32)
    img_np[:, :, 1] = 17
    img_np[3:6, 2:3, :] = 99
    buf = viren2d.ImageBuffer(img_np, copy=False)
    buf.pixelate(block_width=4, block_height=3)
    assert np.all(img_np[:3, :, 0] == 1)
    assert np.all(img_np[3:, :, 0] == 99)
    assert np.all(img_np[:3, :, 1] == 17)
    assert np.all(img_np[3:, :, 0] == 99)

    img_np = np.ones((7, 5, 2), dtype=np.uint8)
    img_np[:, :, 1] = 42
    img_np[3:6, 2:4, :] = 33
    buf = viren2d.ImageBuffer(img_np, copy=False)
    buf.pixelate(block_width=2, block_height=3)
    assert np.all(img_np[:3, :, 0] == 1)
    assert np.all(img_np[:, :2, 0] == 1)
    assert np.all(img_np[3:, 2:, 0] == 33)
    assert np.all(img_np[:3, :, 1] == 42)
    assert np.all(img_np[:, :2, 1] == 42)
    assert np.all(img_np[3:, 2:, 1] == 33)
