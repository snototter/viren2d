----------------
Basic Data Types
----------------

~~~~~
Color
~~~~~

.. autoclass:: viren2d.Color
   :autosummary:
   :autosummary-nosignatures:
   :members:
   :undoc-members:


~~~~~~~~~~~
Color Names
~~~~~~~~~~~

.. autofunction:: viren2d.color_names

   **Available predefined colors:**

   .. figure:: ../images/color-names.png
      :width: 750
      :alt: Color names
      :align: center


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


~~~~~~~
Ellipse
~~~~~~~

.. autoclass:: viren2d.Ellipse
   :autosummary:
   :autosummary-nosignatures:
   :members:

   **Example:**

      .. figure:: ../images/ellipse.png
         :width: 600
         :alt: Exemplary ellipses
         :align: center

   **Corresponding Python Code:**

      .. literalinclude:: ../../../examples/rtd_demo_images/primitives.py
         :language: python
         :lines: 22-60
         :emphasize-lines: 2-4, 9-11, 19-21


~~~~~~~~~
Rectangle
~~~~~~~~~

.. autoclass:: viren2d.Rect
   :autosummary:
   :autosummary-nosignatures:
   :members:


~~~~~~~~~~~
ImageBuffer
~~~~~~~~~~~

TODO add conversion examples to the tutorial (where they actually belong) & only include class documentation here!
TODO :ref:`convert into types used in common image processing libraries <tutorial-conversion>`

.. autoclass:: viren2d.ImageBuffer
   :autosummary:
   :autosummary-nosignatures:
   :members:

