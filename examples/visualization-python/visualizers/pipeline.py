import numpy as np
import viren2d


class VisualizationPipeline(object):
    """
    Sets up a visualization pipeline which you can apply to simplify
    visualizing a stream of images.

    For example, overlay frame information for each frame of a camera
    stream or video:

    >>> from visualizers.pipeline import VisualizationPipeline
    >>> from visualizers.text_overlay import TextOverlay, frame_label
    >>> 
    >>> # Set up the visualization pipeline:
    >>> visualizer = VisualizationPipeline()
    >>> overlay = TextOverlay()
    >>> # Now you could adjust the overlay style, position, placement, etc.
    >>> visualizer.append(overlay)
    >>> 
    >>> while True:
    >>>     image = load_next_image()
    >>>     vis = visualizer.visualize(image, ['Some label'])

    #TODO add tracking-by-detection or camera geometry/calibration example
    """
    def __init__(self):
        self._painter = viren2d.Painter()
        self._visualizers = list()
    
    def append(self, visualizer: object) -> None:
        """
        Adds the given visualizer to this pipeline.

        A valid visualizer must have an `apply` method, which takes the
        `viren2d.Painter` (used for drawing) and its additional parameters.
        """
        apply_op = getattr(visualizer, 'apply', None)
        if not callable(apply_op):
            raise ValueError('Visualizer does not have an `apply` method.')
        self._visualizers.append(visualizer)
    
    def visualize(
            self, image: np.ndarray, *args, return_copy: bool=True) -> np.ndarray:
        """
        Applies the configured visualization pipeline on the given image.
        
        TODO document usage of args or refer to the examples in the pipeline's
        docstring.
        """
        if len(args) != len(self._visualizers):
            raise ValueError(
                'Length of argument list does not match visualizers!')
        
        self._painter.set_canvas_image(image)
        for idx in range(len(self._visualizers)):
            self._visualizers[idx].apply(self._painter, args[idx])

        return np.array(self._painter.canvas, copy=return_copy)
