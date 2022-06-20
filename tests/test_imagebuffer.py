import pytest
import numpy as np
import viren2d


def test_buffer_passing():
    # Although ImageBuffer is only used for 1-, 3-,
    # and 4-channel image data so far, we can still
    # test its creation with an arbitrary number of channels:
    for channels in [1, 2, 3, 4, 5, 10]:
        data = (255 * np.random.rand(5, 15, channels)).astype(np.uint8)
        # Ensure that our later constant will not cause a broken test
        data[3, 4, :] = 99

        # Create image buffers (shared & copy)
        buf_shared = viren2d.ImageBuffer(data, copy=False)
        assert buf_shared.shape == (5, 15, channels)
        buf_copied = viren2d.ImageBuffer(data, copy=True)
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


def test_buffer_side_effects():
    data = (255 * np.random.rand(5, 15, 3)).astype(np.uint8)

    painter = viren2d.Painter()
    painter.set_canvas_image(data)

    shared_canvas = painter.get_canvas(copy=False)
    np_shared = np.array(shared_canvas, copy=False)
    np_copied = np.array(shared_canvas, copy=True)

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
