R"example(import viren2d
moon = viren2d.load_image_uint8('examples/data/lunar-farside.jpg')
vis = viren2d.colorize_scaled(
    data=moon, colormap='relief-low-contrast', low=0, high=255)
shaded = viren2d.relief_shading(relief=moon, colorized=vis))example"

