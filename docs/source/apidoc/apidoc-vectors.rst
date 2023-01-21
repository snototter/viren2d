--------------
Points/Vectors
--------------

.. important::
   Vectors can be implicitly created from :class:`tuple`, :class:`list` and
   :class:`numpy.ndarray` objects, as long as the number of elements matches
   the dimensionality of the vector.

.. autosummary::
   :nosignatures:

   viren2d.Vec2i
   viren2d.Vec2d
   viren2d.Vec3i
   viren2d.Vec3d

~~~~~~~~~
Vector 2D
~~~~~~~~~

.. autoclass:: viren2d.Vec2i

   .. warning::
      TODO With the latest werkzeugkiste changes, there are differences
      between the integral and floating point vectors!

      **Deprecated doc:**
      The only difference between :class:`~viren2d.Vec2d` and
      :class:`~viren2d.Vec2i` is that the latter holds elements of type
      :class:`int`.
      It is only a template specialization for integral types and thus,
      provides exactly the same attributes and methods as its
      double-precision counterpart.

.. autoclass:: viren2d.Vec2d
   :autosummary:
   :autosummary-nosignatures:
   :members:

~~~~~~~~~
Vector 3D
~~~~~~~~~

.. autoclass:: viren2d.Vec3i

   .. note::
      The only difference between :class:`~viren2d.Vec3d` and
      :class:`~viren2d.Vec3i` is that the latter holds elements of type
      :class:`int`.
      It is only a template specialization for integral types and thus,
      provides exactly the same attributes and methods as its
      double-precision counterpart.

.. autoclass:: viren2d.Vec3d
   :autosummary:
   :autosummary-nosignatures:
   :members:
