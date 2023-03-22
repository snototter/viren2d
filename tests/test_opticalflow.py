import pytest
import viren2d
import numpy as np
import pathlib


def test_flow_colorization():
    data_path = pathlib.Path(__file__).parent.parent.resolve() / 'examples' / 'data'
    with pytest.raises(RuntimeError):
        viren2d.load_optical_flow(str(data_path / 'lunar-farside.jpg'))

    flow = viren2d.load_optical_flow(str(data_path / 'sintel-alley2.flo'))
    assert flow.channels == 2
    assert flow.dtype == np.float32

    # Parameter order
    vis = viren2d.colorize_optical_flow(
        flow, viren2d.ColorMap.ColorBlindOrientation)
    assert flow.width == vis.width
    assert flow.height == vis.height
    assert vis.dtype == np.uint8
    assert vis.channels == 3

    # Parameters by name
    vis = viren2d.colorize_optical_flow(
        flow=flow, colormap='hsv', motion_normalizer=15)
    assert flow.width == vis.width
    assert flow.height == vis.height
    assert vis.dtype == np.uint8
    assert vis.channels == 3

    # Output channels
    vis = viren2d.colorize_optical_flow(
        flow=flow, colormap='optical-flow', motion_normalizer=99,
        output_channels=4)
    assert flow.width == vis.width
    assert flow.height == vis.height
    assert vis.dtype == np.uint8
    assert vis.channels == 4
    amin, amax, _, _ = vis.min_max(channel=3)
    assert amin == amax
    assert amin == 255


def test_flow_legend():
    cmaps = ['orientation-4', 'orientation-6', 'optical-flow', viren2d.ColorMap.HSV]
    for cmap in cmaps:
        for draw_circ in [True, False]:
            for ls in [viren2d.LineStyle.Invalid, None, viren2d.LineStyle(2, 'blue')]:
                # 3 channel output --> clip_circle will be ignored
                vis3 = viren2d.optical_flow_legend(
                    size=300, colormap=cmap, line_style=ls,
                    draw_circle=draw_circ, clip_circle=True,
                    output_channels=3)
                assert vis3.width == 300
                assert vis3.height == 300
                assert vis3.dtype == np.uint8
                assert vis3.channels == 3

                # 4 channel output
                vis4 = viren2d.optical_flow_legend(
                    size=100, colormap=cmap, line_style=ls,
                    draw_circle=draw_circ, clip_circle=False,
                    output_channels=4)
                assert vis4.width == 100
                assert vis4.height == 100
                assert vis4.dtype == np.uint8
                assert vis4.channels == 4
                amin, amax, _, _ = vis4.min_max(channel=3)
                assert amin == amax
                assert amin == 255

                # 4 channel & clip to circle
                vis4 = viren2d.optical_flow_legend(
                    size=200, colormap=cmap, line_style=ls,
                    draw_circle=draw_circ, clip_circle=True,
                    output_channels=4)
                assert vis4.width == 200
                assert vis4.height == 200
                assert vis4.dtype == np.uint8
                assert vis4.channels == 4
                amin, amax, _, _ = vis4.min_max(channel=3)
                assert amin == 0
                assert amax == 255
