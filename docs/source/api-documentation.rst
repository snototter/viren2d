.. _api-documentation:

=================
API Documentation
=================

.. toctree::
   :maxdepth: 2
   :hidden:

   apidoc/apidoc-basic-types
   apidoc/apidoc-styles
   apidoc/apidoc-style-enums
   apidoc/apidoc-positioning
   apidoc/apidoc-drawing


**Basic Shapes & Data Types:**

   .. autosummary::
      :nosignatures:

      viren2d.Color
      viren2d.Ellipse
      viren2d.ImageBuffer
      viren2d.Rect
      viren2d.Vec2d
      viren2d.Vec3d


**Basic Shape Utilities:**

   .. autosummary:: 
      :nosignatures:

      viren2d.ellipse_from_endpoints
      viren2d.rect_from_cwh
      viren2d.rect_from_lrtb
      viren2d.rect_from_ltwh


**Color Utilitites:**

   .. autosummary::
      :nosignatures:

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


.. _api-documentation-drawing-summary:

**Drawing Functionality:**

   The :class:`~viren2d.Painter` provides the following drawing functionality:

   .. viren2d-drawing-summary::

**Image I/O:**

   .. autosummary::
      :nosignatures:

      viren2d.load_image_uint8
      viren2d.save_image_uint8


**Style Definitions:**

   .. autosummary::
      :nosignatures:

      viren2d.ArrowStyle
      viren2d.BoundingBox2DStyle
      viren2d.LineCap
      viren2d.LineJoin
      viren2d.LineStyle
      viren2d.marker_codes
      viren2d.Marker
      viren2d.MarkerStyle
      viren2d.TextStyle


**Positioning:**

   .. autosummary::
      :nosignatures:

      viren2d.Anchor
      viren2d.HorizontalAlignment
      viren2d.LabelPosition
      viren2d.VerticalAlignment
