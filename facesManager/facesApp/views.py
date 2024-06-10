# facesApp/views.py
from django.shortcuts import render, redirect
from django.http import HttpResponse
from facesManager.firebase_config import db, bucket
from .utils.image_processing import obtain_faceprint
from .utils.firebase_utils import save_user_data
import face_recognition
from PIL import Image
import numpy as np
from io import BytesIO
import base64
from google.api_core.exceptions import NotFound

def index(request):
    users_ref = db.collection('users')
    users = users_ref.stream()
    user_list = [{**user.to_dict(), 'id': user.id} for user in users]
    return render(request, 'index.html', {'users': user_list})

def upload_image(request):
    if request.method == 'POST':
        name = request.POST.get('name', 'Unnamed')
        image_file = request.FILES.get('image') or request.POST.get('camera_image')

        if not image_file:
            return HttpResponse("Please provide an image.")

        if isinstance(image_file, str):  # Camera data comes as base64
            format, imgstr = image_file.split(';base64,')
            image = Image.open(BytesIO(base64.b64decode(imgstr)))
        else:
            image = Image.open(image_file)

        # Process image and detect faces
        face_encodings, face_locations, processed_image = obtain_faceprint(image)
        if not face_encodings:
            return render(request, 'error.html', {'error_message': "No recognizable faces found in the image."})

        existing_users = db.collection('users').stream()
        for user in existing_users:
            user_data = user.to_dict()
            if np.any(face_recognition.compare_faces([np.array(user_data['face_encoding'])], face_encodings[0], tolerance=0.5)):
                face_distance = face_recognition.face_distance([np.array(user_data['face_encoding'])], face_encodings[0])[0]
                match_percentage = (1 - face_distance) * 100
                return render(request, 'error.html', {'error_message': f"This face is already registered with {user_data['name']}. Match Confidence: {match_percentage:.2f}%."})

        # Save the face and user data
        doc_id = save_user_data(name, face_encodings[0], image)
        return redirect('index')
    else:
        return render(request, 'upload_image.html')


def edit_user(request, doc_id):
    if request.method == 'POST':
        name = request.POST.get('name')
        user_ref = db.collection('users').document(doc_id)
        user_ref.update({'name': name})
        return redirect('index')
    user_ref = db.collection('users').document(doc_id)
    user = user_ref.get()
    return render(request, 'edit_user.html', {'user': user.to_dict(), 'id': doc_id})

def delete_user(request, doc_id):
    try:
        # Intenta eliminar el blob
        blob = bucket.blob(f'images/{doc_id}.jpg')
        blob.delete()
        # Elimina la informació de l'usuari de Firestore
        db.collection('users').document(doc_id).delete()
        return redirect('index')
    except NotFound:
        # Tracta l'excepció si l'objecte no existeix
        print(f"The file images/{doc_id}.jpg does not exist.")
        # Encara elimina l'entrada de l'usuari de Firestore, si així ho desitges
        db.collection('users').document(doc_id).delete()
        return HttpResponse("User data deleted, but the image was not found.", status=404)
    except Exception as e:
        # Maneig d'altres possibles excepcions
        return HttpResponse(f"An error occurred: {str(e)}", status=500)
