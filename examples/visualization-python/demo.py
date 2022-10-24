#TODO requires OpenCV
import cv2
import numpy as np
import datetime
import cvvis2d


def demo_webcam_visualizer():
    cam = cv2.VideoCapture(0)
    if not cam.isOpened():
        raise IOError('Cannot open webcam')

    visualizer = cvvis2d.VisualizationPipeline()
    overlay = cvvis2d.TextOverlay()
    overlay.text_style.color = 'navyblue'
    visualizer.add('frame-label', overlay)

    overlay = cvvis2d.StaticTextOverlay()
    overlay.text = 'Pipeline Demo'
    visualizer.add('static-text', overlay)
    
    num_frames = 0
    while True:
        retval, frame = cam.read()
        if not retval:
            break

        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        gx = cv2.Sobel(gray, ddepth=cv2.CV_32F, dx=1, dy=0, ksize=3)
        gy = cv2.Sobel(gray, ddepth=cv2.CV_32F, dx=0, dy=1, ksize=3)
        mag = cv2.convertScaleAbs(np.sqrt(gx**2 + gy**2))
        frame = cv2.cvtColor(mag, cv2.COLOR_GRAY2BGR)
        # Convert from OpenCV BGR to RGB (used by viren2d)
        rgb = frame[:, :, ::-1]
        # Prepare parameters for the configured visualizers
        text = cvvis2d.frame_label('Webcam', num_frames, datetime.datetime.now())
        # Apply the visualization pipeline
        vis = visualizer.visualize(rgb, {'frame-label': text})
        # Convert back to BGR for display
        vis = vis[:, :, ::-1]

        cv2.imshow('Image', vis)
        k = cv2.waitKey(10) & 0xff
        if (k == 27) or (k == ord('q')):
            break
        num_frames += 1
    cv2.destroyAllWindows()


if __name__ == '__main__':
    demo_webcam_visualizer()

