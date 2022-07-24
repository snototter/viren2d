
--------------
Points/Vectors
--------------

.. important::
   Vectors can be implicitly created from :class:`tuple`, :class:`list` and
   :class:`numpy.ndarray`, as long as the number of elements matches the
   dimensionality of the vector.


.. note::
   In addition to the documented :class:`~viren2d.Vec2d` and
   :class:`viren2d.Vec3d` classes, there exist bindings for 2- and
   3-dimension vectors which hold elements of type :class:`int`,
   *i.e.* :class:`viren2d.Vec2i` and :class:`viren2d.Vec3i`.
   These are not explicitly documented as they are just a C++ template
   specialization for integral types and thus, provide exactly the same
   attributes and methods as their double-precision counterparts.


~~~~~~~~~
Vector 2D
~~~~~~~~~

.. autoclass:: viren2d.Vec2d
   :autosummary:
   :autosummary-nosignatures:
   :members:


~~~~~~~~~
Vector 3D
~~~~~~~~~

.. autoclass:: viren2d.Vec3d
   :autosummary:
   :autosummary-nosignatures:
   :members:
