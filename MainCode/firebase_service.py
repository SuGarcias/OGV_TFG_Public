# firebase_service.py
import firebase_admin
from firebase_admin import credentials, firestore
import numpy as np
    
class FirebaseService:
    def __init__(self, credentials_path):
        self.cred = credentials.Certificate(credentials_path)
        firebase_admin.initialize_app(self.cred)
        self.db = firestore.client()

    def fetch_user_encodings(self):
        users_ref = self.db.collection('users').stream()
        users_data = []
        for user in users_ref:
            data = user.to_dict()
            name = data['name']
            face_encoding = np.array(data['face_encoding'])
            users_data.append((name, face_encoding))
        return users_data
