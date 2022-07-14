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
                img = viren2d.colorize(
                    data=data, colormap=fx(cm),
                    low=-6, high=6, bins=bins)
                assert img.width == 600
                assert img.height == 600
                assert img.dtype == np.uint8
                assert img.channels == 3

    # Invalid inputs:
    with pytest.raises(ValueError):
        viren2d.colorize(
            data=data, colormap='gouldian', low=-6, high=6, bins=1)
    
    with pytest.raises(ValueError):
        viren2d.colorize(
            data=data, colormap='gouldian', low=-6, high=6, bins=1,
            output_channels=5)

    # Different data types:
    data = viren2d.peaks(width=800)
    for dt in [np.uint8, np.int32, np.float32]:
        data_np = np.array(data, copy=False).astype(dt)

        img = viren2d.colorize(
            data=data_np,
            colormap='cividis', low=-10, high=20, bins=8,
            output_channels=4)

        assert img.width == 800
        assert img.height == 600
        assert img.dtype == np.uint8
        assert img.channels == 4
