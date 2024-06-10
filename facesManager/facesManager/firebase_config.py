# FIREBASE

import firebase_admin
from firebase_admin import credentials, firestore, storage

credJSON = './cred.json'
cred = credentials.Certificate(credJSON)
firebase_admin.initialize_app(cred, {
    'storageBucket': 'ogv-tfg.appspot.com'
})

db = firestore.client()
bucket = storage.bucket()