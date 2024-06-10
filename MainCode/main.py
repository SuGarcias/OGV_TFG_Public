from facial_recognition import FacialRecognitionSystem
from voice_recognition import VoiceRecognitionSystem
from assistant import AssistantSystem
import multiprocessing
import time
from collections import deque

def welcome(name, audio_lock, voice_recognition_system, assistant_system):
    print(f"Bienvenido, {name}!")
    voice_recognition_system.generate_audio(f"Bienvenido, {name}!", audio_lock)
    time.sleep(2)  # Pausa para evitar la retroalimentación de audio
    while not voice_recognition_system.start_listening(audio_lock):
        print("Palabra clave incorrecta o no reconocida, inténtalo de nuevo")
    assistant_system.listen_for_wake_word(audio_lock)

def start_process(name, audio_lock, voice_recognition_system, assistant_system):
    process = multiprocessing.Process(target=welcome, args=(name, audio_lock, voice_recognition_system, assistant_system))
    process.start()  # Inicia el proceso
    return process

if __name__ == "__main__":
    voice_keyword = 'alabanza'
    esp32_ip = '172.20.10.3'
    wake_word = 'activar'
    credentials = 'cred.json'
    
    multiprocessing.set_start_method('spawn')

    audio_lock = multiprocessing.Lock()
    camera_lock = multiprocessing.Lock()

    voice_recognition_system = VoiceRecognitionSystem(voice_keyword)
    assistant_system = AssistantSystem(wake_word, esp32_ip)
    facial_recognition_system = FacialRecognitionSystem(credentials, camera_lock)

    user_generator = facial_recognition_system.recognize_and_display_faces(camera_lock)
    user_history = deque(maxlen=10) 
    current_user = None
    process = None

    while True:
        detected_user = next(user_generator)
        user_history.append(detected_user)
        most_common_user = max(set(user_history), key=user_history.count)  

        if user_history.count(most_common_user) >= 7:
            if most_common_user != current_user:
                
                if process:
                    process.terminate()
                    process.join()
                    process = None

                if most_common_user != "Desconocido":
                    print(f"Starting interaction for {most_common_user}")
                    process = start_process(most_common_user, audio_lock, voice_recognition_system, assistant_system)
                
                current_user = most_common_user
                print(f"Current user updated to {current_user}")
            elif most_common_user == "Desconocido" and process:
                print("No user detected consistently. Stopping interaction.")
                process.terminate()
                process.join()
                process = None
                current_user = "Desconocido"

        if most_common_user == "Desconocido":
            print("Esperando a reconocer un usuario...")
            time.sleep(1)
