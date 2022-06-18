Positioning
-----------

~~~~~~~~~~~~~~~~~~~~
Horizontal Alignment
~~~~~~~~~~~~~~~~~~~~

.. autoclass:: viren2d.HorizontalAlignment
   :autosummary:

   **Example:**

      This figure shows different alignment of multi-line text boxes.
      These text lines are an excerpt from `Revelation, a Visual Poem <https://vimeo.com/57370112>`__.

      .. figure:: ../images/text-multi-line.png
         :width: 750
         :alt: Multi-line Text Alignment

   **Corresponding Python Code:**

      Note that the :attr:`viren2d.TextStyle.alignment` property provides
      string conversion to :class:`~viren2d.HorizontalAlignment` enum values.

      .. literalinclude:: ../../../examples/rtd_demo_images/positioning.py
         :language: python
         :linenos:
         :lines: 83-99


~~~~~~~~~~~~~~~~~~
Vertical Alignment
~~~~~~~~~~~~~~~~~~

.. autoclass:: viren2d.VerticalAlignment


~~~~~~~~~~~
Text Anchor
~~~~~~~~~~~

.. autoclass:: viren2d.TextAnchor

   **Example:**

      This figure shows differently anchored text boxes.

      .. figure:: ../images/text-anchors.png
         :width: 440
         :alt: Text anchors

   **Corresponding Python Code:**

      .. literalinclude:: ../../../examples/rtd_demo_images/positioning.py
         :language: python
         :linenos:
         :lines: 35-51



~~~~~~~~~~~~~~~~~~~~~~~~~~~
Bounding Box Label Position
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autoclass:: viren2d.BoundingBoxLabelPosition


   **Example:**

      TODO add example bbox figure


   **Corresponding Python Code:**

      TODO implement in examples/rtd/positioning

