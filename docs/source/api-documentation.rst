.. _api-documentation:

=================
API Documentation
=================

.. toctree::
   :maxdepth: 2
   :hidden:

   apidoc/apidoc-basic-shapes
   apidoc/apidoc-color
   apidoc/apidoc-colorgradients
   apidoc/apidoc-colormaps
   apidoc/apidoc-drawing
   apidoc/apidoc-imagebuffer
   apidoc/apidoc-optical-flow
   apidoc/apidoc-positioning
   apidoc/apidoc-styles
   apidoc/apidoc-style-enums
   apidoc/apidoc-vectors


**Colors:**

   .. autosummary::
      :nosignatures:

      viren2d.Color
      viren2d.axis_color
      viren2d.color_from_object_category
      viren2d.color_from_object_id
      viren2d.color_names
      viren2d.fade_out_linear
      viren2d.fade_out_logarithmic
      viren2d.fade_out_quadratic
      viren2d.object_category_names
      viren2d.rgba
      viren2d.RGBa


**Color Gradients:**

   .. autosummary::
      :nosignatures:

      viren2d.ColorGradient
      viren2d.LinearColorGradient
      viren2d.RadialColorGradient


**Color Maps:**

   .. autosummary::
      :nosignatures:

      viren2d.ColorMap
      viren2d.colorize_labels
      viren2d.colorize_scaled
      viren2d.colorize_scalars
      viren2d.get_colormap
      viren2d.set_custom_colormap
      viren2d.StreamColorizer
      viren2d.peaks


.. _api-documentation-drawing-summary:

**Drawing Functionality:**

   The :class:`~viren2d.Painter` provides the following drawing functionality:

   .. viren2d-drawing-summary::


**Image Handling:**

   .. autosummary::
      :nosignatures:

      viren2d.ImageBuffer
      viren2d.collage
      viren2d.color_pop
      viren2d.convert_gray2rgb
      viren2d.convert_hsv2rgb
      viren2d.convert_rgb2gray
      viren2d.convert_rgb2hsv
      viren2d.load_image_uint8
      viren2d.save_image_uint8
      

**Optical Flow:**

   .. autosummary::
      :nosignatures:
      
      viren2d.colorize_optical_flow
      viren2d.load_optical_flow
      viren2d.optical_flow_legend
      viren2d.save_optical_flow


**Points/Vectors:**

   .. autosummary::
      :nosignatures:
      
      viren2d.Vec2d
      viren2d.Vec2i
      viren2d.Vec3d
      viren2d.Vec3i


**Positioning:**

   .. autosummary::
      :nosignatures:

      viren2d.Anchor
      viren2d.HorizontalAlignment
      viren2d.LabelPosition
      viren2d.VerticalAlignment


**Shapes:**

   .. autosummary::
      :nosignatures:

      viren2d.Ellipse
      viren2d.Line2d
      viren2d.Rect
      viren2d.ellipse_from_endpoints
      viren2d.rect_from_cwh
      viren2d.rect_from_lrtb
      viren2d.rect_from_ltwh


**Style Definitions:**

   .. autosummary::
      :nosignatures:

      viren2d.ArrowStyle
      viren2d.BoundingBox2DStyle
      viren2d.LineCap
      viren2d.LineJoin
      viren2d.LineStyle
      viren2d.Marker
      viren2d.MarkerStyle
      viren2d.TextStyle
