import speech_recognition as sr
import datetime
import requests
import time
from gtts import gTTS
import pygame
import os
import threading

class AssistantSystem:
    def __init__(self, wake_word, esp32_ip):
        self.wake_word = wake_word.lower()
        self.recognizer = sr.Recognizer()
        self.audio_file = "temp_audio.mp3"
        self.esp32_ip = esp32_ip
        self.active = False
        self.commands = {
            "desactivar": self.deactivate_assistant,
            "hora": self.get_current_time,
            "ayuda": self.provide_help,
            "encender led": self.led_on,
            "apagar led": self.led_off,
            "patrón": self.show_pattern,
            "animación": self.start_animation,
            "color": self.set_color,
            "establecer brillo": self.set_brightness,
            "subir brillo": self.increase_brightness,
            "bajar brillo": self.decrease_brightness
        }
        self.predefined_colors = {
            "rojo": (255, 0, 0),
            "verde": (0, 255, 0),
            "azul": (0, 0, 255),
            "amarillo": (255, 255, 0),
            "cian": (0, 255, 255),
            "magenta": (255, 0, 255),
            "blanco": (255, 255, 255),
            "naranja": (255, 165, 0),
            "púrpura": (128, 0, 128),
            "rosa": (255, 192, 203)
        }

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

    def listen_for_wake_word(self, audio_lock):
        print("Escuchando palabra de activación...")
        while True:
            with sr.Microphone() as source:
                self.recognizer.adjust_for_ambient_noise(source, duration=1)
                audio = self.recognizer.listen(source)
                try:
                    text = self.recognizer.recognize_google(audio, language="es-ES").lower()
                    if self.wake_word in text:
                        self.activate_assistant(audio_lock)
                except (sr.UnknownValueError, sr.RequestError):
                    self.handle_errors(audio_lock)

    def process_command(self, audio_lock):
        while self.active:
            with sr.Microphone() as source:
                self.recognizer.adjust_for_ambient_noise(source, duration=0.5)
                audio = self.recognizer.listen(source)
                try:
                    spoken_text = self.recognizer.recognize_google(audio, language="es-ES").lower()
                    print(f'Dijiste: {spoken_text}')
                    self.execute_command(spoken_text, audio_lock)
                except (sr.UnknownValueError, sr.RequestError):
                    self.generate_audio("Repite el comando, por favor.", audio_lock)

    def execute_command(self, spoken_text, audio_lock):
        found_command = False
        for keyword, action in self.commands.items():
            if keyword in spoken_text:
                action(audio_lock)
                found_command = True
                break
        if not found_command:
            self.handle_errors(audio_lock, error_message="Lo siento, no conozco ese comando." )

    def handle_errors(self, audio_lock, error_message="Problema de conexión o audio no claro"):
        self.generate_audio(error_message, audio_lock)

    def activate_assistant(self, audio_lock):
        self.active = True
        self.generate_audio("Asistente activado. Di un comando.", audio_lock)
        self.process_command(audio_lock)

    # COMMAND FUNCTIONS :
    # -------------------
    def deactivate_assistant(self, audio_lock):
        self.active = False
        self.generate_audio("Desactivando asistente.", audio_lock)

    def provide_help(self, audio_lock):
        help_text = "Puedes pedirme que apague el sistema, te salude, diga la hora o el clima."
        self.generate_audio(help_text, audio_lock)

    def get_current_time(self, audio_lock):
        now = datetime.datetime.now()
        current_time = now.strftime("%H:%M")
        self.generate_audio(f"La hora actual es {current_time}", audio_lock)

    def led_on(self, audio_lock):
        try:
            response = requests.get(f"http://{self.esp32_ip}/on", timeout=3)
            if response.status_code == 200:
                self.generate_audio("LED encendido.", audio_lock)
            else:
                self.generate_audio("No se pudo encender el LED, inténtalo de nuevo.", audio_lock)
        except requests.exceptions.RequestException as e:
            self.generate_audio("Error al conectar con el ESP32.", audio_lock)
            print(f"Error: {e}")

    def led_off(self, audio_lock):
        try:
            response = requests.get(f"http://{self.esp32_ip}/off", timeout=3)
            if response.status_code == 200:
                self.generate_audio("LED apagado.", audio_lock)
            else:
                self.generate_audio("No se pudo apagar el LED, inténtalo de nuevo.", audio_lock)
        except requests.exceptions.RequestException as e:
            self.generate_audio("Error al conectar con el ESP32.", audio_lock)
            print(f"Error: {e}")

    def show_pattern(self, audio_lock):
        try:
            response = requests.get(f"http://{self.esp32_ip}/pattern", timeout=3)
            if response.status_code == 200:
                self.generate_audio("Mostrando patrón.", audio_lock)
            else:
                self.generate_audio("No se pudo mostrar el patrón, inténtalo de nuevo.", audio_lock)
        except requests.exceptions.RequestException as e:
            self.generate_audio("Error al conectar con el ESP32.", audio_lock)
            print(f"Error: {e}")

    def start_animation(self, audio_lock):
        try:
            response = requests.get(f"http://{self.esp32_ip}/animate", timeout=3)
            if response.status_code == 200:
                self.generate_audio("Iniciando animación.", audio_lock)
            else:
                self.generate_audio("No se pudo iniciar la animación, inténtalo de nuevo.", audio_lock)
        except requests.exceptions.RequestException as e:
            self.generate_audio("Error al conectar con el ESP32.", audio_lock)
            print(f"Error: {e}")

    def set_color(self, audio_lock):
        try:
            self.generate_audio("Dime el nombre del color. Los colores disponibles son rojo, verde, azul, amarillo, cian, magenta, blanco, naranja, púrpura, y rosa.", audio_lock)
            with sr.Microphone() as source:
                self.recognizer.adjust_for_ambient_noise(source, duration=0.5)
                audio = self.recognizer.listen(source)
                color_name = self.recognizer.recognize_google(audio, language="es-ES").lower()
                if color_name in self.predefined_colors:
                    r, g, b = self.predefined_colors[color_name]
                    response = requests.get(f"http://{self.esp32_ip}/setcolor?r={r}&g={g}&b={b}", timeout=3)
                    if response.status_code == 200:
                        self.generate_audio(f"Color {color_name} establecido.", audio_lock)
                    else:
                        self.generate_audio("No se pudo establecer el color, inténtalo de nuevo.", audio_lock)
                else:
                    self.generate_audio("Color no reconocido.", audio_lock)
        except requests.exceptions.RequestException as e:
            self.generate_audio("Error al conectar con el ESP32.", audio_lock)
            print(f"Error: {e}")
        except (sr.UnknownValueError, sr.RequestError):
            self.generate_audio("No pude entender el color, por favor intenta de nuevo.", audio_lock)

    def set_brightness(self, audio_lock):
        try:
            self.generate_audio("Dime el nivel de brillo entre 0 y 255.", audio_lock)
            with sr.Microphone() as source:
                self.recognizer.adjust_for_ambient_noise(source, duration=0.5)
                audio = self.recognizer.listen(source)
                brightness_text = self.recognizer.recognize_google(audio, language="es-ES").lower()
                if brightness_text.isdigit():
                    brightness = int(brightness_text)
                    response = requests.get(f"http://{self.esp32_ip}/setbrightness?brightness={brightness}", timeout=3)
                    if response.status_code == 200:
                        self.generate_audio("Brillo establecido.", audio_lock)
                    else:
                        self.generate_audio("No se pudo establecer el brillo, inténtalo de nuevo.", audio_lock)
                else:
                    self.generate_audio("Formato de brillo incorrecto.", audio_lock)
        except requests.exceptions.RequestException as e:
            self.generate_audio("Error al conectar con el ESP32.", audio_lock)
            print(f"Error: {e}")
        except (sr.UnknownValueError, sr.RequestError):
            self.generate_audio("No pude entender el nivel de brillo, por favor intenta de nuevo.", audio_lock)

    def increase_brightness(self, audio_lock):
        try:
            response = requests.get(f"http://{self.esp32_ip}/increasebrightness", timeout=3)
            if response.status_code == 200:
                self.generate_audio("Brillo aumentado.", audio_lock)
            else:
                self.generate_audio("No se pudo aumentar el brillo, inténtalo de nuevo.", audio_lock)
        except requests.exceptions.RequestException as e:
            self.generate_audio("Error al conectar con el ESP32.", audio_lock)
            print(f"Error: {e}")

    def decrease_brightness(self, audio_lock):
        try:
            response = requests.get(f"http://{self.esp32_ip}/decreasebrightness", timeout=3)
            if response.status_code == 200:
                self.generate_audio("Brillo disminuido.", audio_lock)
            else:
                self.generate_audio("No se pudo disminuir el brillo, inténtalo de nuevo.", audio_lock)
        except requests.exceptions.RequestException as e:
            self.generate_audio("Error al conectar con el ESP32.", audio_lock)
            print(f"Error: {e}")
