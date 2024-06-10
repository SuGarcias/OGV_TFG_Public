# facial_recognition.py
import cv2
import face_recognition
from firebase_service import FirebaseService

class FacialRecognitionSystem:
    def __init__(self, credentials_path, camera_lock):
        self.credentials_path = credentials_path
        self.camera_lock = camera_lock
        self.names = []
        self.face_encodings_db = []
        self.load_known_faces_from_db()

    def load_known_faces_from_db(self):
        try:
            firebase_service = FirebaseService(self.credentials_path)
            users_data = firebase_service.fetch_user_encodings()
            self.names = [name for name, _ in users_data]
            self.face_encodings_db = [encoding for _, encoding in users_data]
        except Exception as e:
            print(f"Error carregant dades de Firebase: {e}")


    def recognize_and_display_faces(self, camera_lock):
        with camera_lock:
            camera = cv2.VideoCapture(0)
            if not camera.isOpened():
                print("No s'ha pogut accedir a la càmera.")
                yield "Desconocido"

            while True:
                ret, frame = camera.read()
                if not ret:
                    continue
                frame_copy = frame.copy()
                frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
                face_locations = face_recognition.face_locations(frame_rgb, model="hog")
                face_encodings = face_recognition.face_encodings(frame_rgb, face_locations)

                name = "Desconocido"
                for (top, right, bottom, left), face_encoding in zip(face_locations, face_encodings):
                    matches = face_recognition.compare_faces(self.face_encodings_db, face_encoding, tolerance=0.5)
                    if any(matches):
                        name = self.names[matches.index(True)]
                        color = (0, 255, 0)  
                        label = f"{name}"
                    else:
                        color = (0, 0, 255)  
                        label = "Desconocido"
                    
                    cv2.rectangle(frame_copy, (left, top), (right, bottom), color, 2)
                    (w, h), _ = cv2.getTextSize(label, cv2.FONT_HERSHEY_SIMPLEX, 0.5, 1)
                    cv2.rectangle(frame_copy, (left, bottom - h - 10), (left + w, bottom), color, cv2.FILLED)
                    cv2.putText(frame_copy, label, (left + 5, bottom - 5), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)

                cv2.imshow('Reconocimiento Facial', frame_copy)
                if cv2.waitKey(1) & 0xFF == ord('q'):
                    break
                
                yield name
            
            camera.release()
            cv2.destroyAllWindows()


