# voice_recognition.py
import speech_recognition as sr
import time
from gtts import gTTS
import pygame
import os

class VoiceRecognitionSystem:
    def __init__(self, keyword):
        self.keyword = keyword.lower()
        self.recognizer = sr.Recognizer()
        self.audio_file = "temp_audio.mp3"

    def start_listening(self, audio_lock):
        self.generate_audio("Escuchando... di la palabra clave.", audio_lock)
        with sr.Microphone() as source:
            print("Escuchando... di la palabra clave.")
            self.recognizer.adjust_for_ambient_noise(source, duration=1)
            time.sleep(1)  
            audio = self.recognizer.listen(source)

        try:
            text = self.recognizer.recognize_google(audio, language="es-ES").lower()
            if self.keyword in text:
                print(f'Dijiste: {text}')
                self.generate_audio("Palabra clave reconocida. Acceso concedido.", audio_lock)
                return True
            else:
                print(f'Dijiste: {text}')
                self.generate_audio("Palabra clave incorrecta. Intenta de nuevo.", audio_lock)
                return False
        except sr.UnknownValueError:
            self.generate_audio("Lo siento, no pude entender el audio.", audio_lock)
            return False
        except sr.RequestError as e:
            self.generate_audio(f"Error al recuperar los resultados; {e}", audio_lock)
            return False

    def generate_audio(self, text, audio_lock):
        with audio_lock:
            tts = gTTS(text=text, lang='es')
            tts.save(self.audio_file)

            pygame.mixer.init()
            pygame.mixer.music.load(self.audio_file)
            pygame.mixer.music.play()

            while pygame.mixer.music.get_busy():
                time.sleep(0.1)

            pygame.mixer.quit()
            os.remove(self.audio_file)
            time.sleep(1)  
