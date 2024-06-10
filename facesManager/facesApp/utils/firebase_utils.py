from facesManager.firebase_config import db, bucket
from io import BytesIO
from PIL import Image
import numpy as np

def upload_to_firebase(image, doc_id):
    """Puja una imatge al Firebase Storage i retorna l'URL pública."""
    blob = bucket.blob(f'images/{doc_id}.jpg')
    img_byte_arr = BytesIO()
    
    # Comprobar si la imagen es una matriz NumPy y convertirla a objeto Image de PIL si es necesario
    if isinstance(image, np.ndarray):
        # Asumimos que la imagen está en formato RGB
        image = Image.fromarray(image.astype('uint8'), 'RGB')
    
    # Guardar la imagen en el buffer de bytes como JPEG
    image.save(img_byte_arr, format='JPEG')
    blob.upload_from_string(img_byte_arr.getvalue(), content_type='image/jpeg')
    blob.make_public()
    return blob.public_url

def save_user_data(name, face_encoding, image):
    """Desa les dades de l'usuari a Firestore i puja la imatge relacionada."""
    doc_ref = db.collection('users').document()
    image_url = upload_to_firebase(image, doc_ref.id)  # Envía la imagen (que puede ser una matriz NumPy o una imagen PIL)
    doc_ref.set({
        'name': name,
        'face_encoding': face_encoding.tolist(),  # Convertir las codificaciones de cara a lista para el almacenamiento en Firestore
        'image_url': image_url
    })
    return doc_ref.id
