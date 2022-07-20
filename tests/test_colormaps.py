import pytest
import viren2d
import numpy as np


def test_colormap_usage():
    data = viren2d.peaks(height=600, width=600)

    # We'll try to colorize using the enum values and the string representation
    param_fx1 = lambda c: c
    param_fx2 = lambda c: str(c).upper()

    for bins in [500, 256, 8]:
        for cm in viren2d.ColorMap.list_all():
            for fx in [param_fx1, param_fx2]:
                img = viren2d.colorize_scaled(
                    data=data, colormap=fx(cm),
                    low=-6, high=6, bins=bins)
                assert img.width == 600
                assert img.height == 600
                assert img.dtype == np.uint8
                assert img.channels == 3

    # Invalid inputs:
    with pytest.raises(ValueError):
        viren2d.colorize_scaled(
            data=data, colormap='gouldian', low=-6, high=6, bins=1)
    
    with pytest.raises(ValueError):
        viren2d.colorize_scaled(
            data=data, colormap='gouldian', low=-6, high=6, bins=1,
            output_channels=5)

    # Different data types:
    data = viren2d.peaks(width=800)
    for dt in [np.uint8, np.int32, np.float32]:
        data_np = np.array(data, copy=False).astype(dt)

        img = viren2d.colorize_scaled(
            data=data_np,
            colormap='cividis', low=-10, high=20, bins=8,
            output_channels=4)

        assert img.width == 800
        assert img.height == 600
        assert img.dtype == np.uint8
        assert img.channels == 4


def test_label_colorization():
    data = np.array(
        [[       -10,        -1,     0,   1,  10],
         [2**31 - 10, 2**31 - 1, 2**31, 257, 266],
         [135, 1234567 % 256, 1234567, 391, 25479]], dtype=np.int32)
    # Use a colormap with 256 bins:
    vis = viren2d.colorize_labels(
        labels=data, colormap='glasbey-dark', output_channels=4)
    assert vis.dtype == np.uint8
    assert vis.channels == 4
    assert vis.width == data.shape[1]
    assert vis.height == data.shape[0]

    vis_np = np.array(vis, copy=False)
    for c in range(vis.width):
        assert np.array_equal(vis_np[0, c, :], vis_np[1, c, :])
        if c > 0:
            assert np.array_equal(vis_np[2, c - 1, :], vis_np[2, c, :])
        assert vis_np[0, c, 3] == 255
    
    # Default should be 3 channels:
    vis = viren2d.colorize_labels(
        labels=data, colormap='glasbey-dark', output_channels=3)
    assert vis.channels == 3

    with pytest.raises(ValueError):
        vis = viren2d.colorize_labels(
            labels=data, colormap='glasbey-dark', output_channels=5)
    
    # Try different integral types:
    for dt in [np.uint8, np.int16, np.uint16, np.int32, np.uint32,
               np.int64, np.uint64]:
        data = (np.random.randn(3,4) * 30e3).astype(dt)
        viren2d.colorize_labels(labels=data)

    # Labels must be integral:
    for dt in [np.float32, np.float64]:
        with pytest.raises(ValueError):
            data = np.random.randn(3,4).astype(dt)
            viren2d.colorize_labels(labels=data)
