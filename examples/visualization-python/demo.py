#TODO requires OpenCV
import cv2
import datetime
from visualizers.pipeline import VisualizationPipeline
from visualizers.text_overlay import TextOverlay, frame_label, StaticTextOverlay
from vito import imutils

def demo_webcam_visualizer():
    cam = cv2.VideoCapture(0)
    if not cam.isOpened():
        raise IOError('Cannot open webcam')

    visualizer = VisualizationPipeline()
    overlay = TextOverlay()
    overlay.text_style.color = 'navyblue'
    visualizer.add('frame-label', overlay)

    overlay = StaticTextOverlay()
    overlay.text = 'Pipeline Demo'
    visualizer.add('static-text', overlay)
    
    num_frames = 0
    while True:
        retval, frame = cam.read()
        if not retval:
            break

        # Convert from OpenCV BGR to RGB (used by viren2d)
        #TODO check why flip_layers, ::-1, etc. yields different result with imshow than OpenCV's cvtColor...
        # rgb = frame[:, :, ::-1]
        rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        # rgb = imutils.flip_layers(frame)
        # Prepare parameters for the configured visualizers
        text = frame_label('Webcam', num_frames, datetime.datetime.now())
        # Apply the visualization pipeline
        vis = visualizer.visualize(rgb, {'frame-label': text})
        vis = cv2.cvtColor(vis, cv2.COLOR_RGB2BGR)
        # vis = imutils.flip_layers(vis)

        cv2.imshow('Image', vis)#vis[:, :, ::-1])
        k = cv2.waitKey(10) & 0xff
        if (k == 27) or (k == ord('q')):
            break
        num_frames += 1
    cv2.destroyAllWindows()


if __name__ == '__main__':
    demo_webcam_visualizer()

