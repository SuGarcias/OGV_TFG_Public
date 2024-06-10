import cv2
import numpy as np
import face_recognition
from PIL import Image

def obtain_faceprint(image, upsamples=1, jitters=10):
    final_image = cv2.cvtColor(np.array(image), cv2.COLOR_BGR2RGB)

    # Detect faces and encode them
    face_locations = face_recognition.face_locations(final_image, model="hog", number_of_times_to_upsample=upsamples)
    face_encodings = face_recognition.face_encodings(final_image, face_locations, num_jitters=jitters)
    return face_encodings, face_locations, final_image
