import time
import pygame
import speech_recognition as sr
from concurrent.futures import ThreadPoolExecutor
import threading

from common import *


class Listener(object):

    def __init__(self, gpt=None):
        self.recognizer = sr.Recognizer()
        self.executor = ThreadPoolExecutor(max_workers=1)
        self.gpt = gpt

    def hear(self, audio_path="input.wav", timeout=8):
        try:
            with sr.Microphone() as source:
                print("开始说话...")
                audio_data = self.recognizer.listen(source, timeout=timeout)
                print("录音已完成")
                with open(audio_path, "wb") as audio_file:
                    audio_file.write(audio_data.get_wav_data())
                return self.gpt.speech(audio_path=audio_path)

        except Exception as e:
            error(e, "Speech recognition Failed")


class Speaker(object):
    def __init__(self, gpt=None):
        self.executor = ThreadPoolExecutor(max_workers=1)
        pygame.mixer.init()
        self.gpt = gpt

    def _play_audio(self, audio_path):
        pygame.mixer.music.load(audio_path)
        pygame.mixer.music.play()
        while pygame.mixer.music.get_busy():
            time.sleep(1)

    def say(self, text="", voice="onyx", audio_path="output.mp3"):
        try:
            self.gpt.speak(text=text, voice=voice, audio_path=audio_path)
            self.executor.submit(self._play_audio, audio_path)

        except Exception as e:
            error(e, "Speak Failed")


if __name__ == "__main__":
    from gpt import *
    llm = GPT()
    llm.connect()
    # listener = Listener(llm)
    # print(listener.hear())
    speaker = Speaker(llm)
    speaker.say(text="你好呀")
