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


**Basic data types:**

  .. autosummary::
     :nosignatures:

     viren2d.Color
     viren2d.RGBa
     viren2d.rgba
     viren2d.color_names
     viren2d.Vec2d
     viren2d.Vec3d
     viren2d.Ellipse
     viren2d.Rect


**Style definitions:**

  .. autosummary::
     :nosignatures:

     viren2d.ArrowStyle
     viren2d.LineCap
     viren2d.LineJoin
     viren2d.LineStyle
     viren2d.marker_codes
     viren2d.Marker
     viren2d.MarkerStyle
     viren2d.TextStyle
     viren2d.BoundingBox2DStyle


**Positioning:**

  .. autosummary::
     :nosignatures:

     viren2d.HorizontalAlignment
     viren2d.VerticalAlignment
     viren2d.TextAnchor
     viren2d.BoundingBoxLabelPosition


**Drawing:**

  .. autosummary::
  
     viren2d.Painter

.. warning::
   Currently, the documentation for static class members is missing due to
   a potential `pybind11 bug <https://github.com/pybind/pybind11/issues/3815>`_.
