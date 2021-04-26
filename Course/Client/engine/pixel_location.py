import cv2


def get_keypoint_from_image(img):
    hsv_img = cv2.cvtColor(img, cv2.COLOR_RGB2HSV)
    lower = (99, 254, 100)
    upper = (100, 255, 101)
    mask = cv2.inRange(hsv_img, lower, upper)

    params = cv2.SimpleBlobDetector_Params()

    params.minThreshold = 10
    params.maxThreshold = 255

    params.filterByColor = True
    params.blobColor = 255

    params.filterByCircularity = False
    params.filterByConvexity = False
    params.filterByInertia = False

    params.filterByArea = True
    params.minArea = 10.0

    detector = cv2.SimpleBlobDetector_create(params)

    key_points = detector.detect(mask)

    if len(key_points) <= 0:
        return None

    return int(key_points[0].pt[0]), int(key_points[0].pt[1])


class PixelLocation:
    Value = None

    @staticmethod
    def config():
        PixelLocation.Value = (0, 0, 1, 1)

        return True
