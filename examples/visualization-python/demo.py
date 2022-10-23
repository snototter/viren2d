#TODO requires OpenCV
import cv2
import datetime
from visualizers.pipeline import VisualizationPipeline
from visualizers.text_overlay import TextOverlay, frame_label, StaticTextOverlay


def demo_webcam_visualizer():
    cam = cv2.VideoCapture(0)
    if not cam.isOpened():
        raise IOError('Cannot open webcam')

    visualizer = VisualizationPipeline()
    overlay = TextOverlay()
    # TODO configure overlay
    visualizer.append(overlay)

    overlay = StaticTextOverlay()
    overlay.text = 'Pipeline Demo'
    visualizer.append(overlay)
    
    num_frames = 0
    while True:
        retval, frame = cam.read()
        if not retval:
            break

        text = frame_label('Webcam', num_frames, datetime.datetime.now())
        vis = visualizer.visualize(frame, text, None)
        
        cv2.imshow('Image', vis)
        k = cv2.waitKey(10) & 0xff
        if (k == 27) or (k == ord('q')):
            break
        num_frames += 1
    cv2.destroyAllWindows()


if __name__ == '__main__':
    demo_webcam_visualizer()

