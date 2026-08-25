import time
import pygame
from concurrent.futures import ThreadPoolExecutor
import threading

from common import *



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
        #pygame.mixer.music.unload()

    def say(self, text="", voice="Kore", audio_path="output.wav"):
        try:
            self.gpt.speak(text=text, voice=voice, audio_path=audio_path)
            self.executor.submit(self._play_audio, audio_path)

        except Exception as e:
            error(e, "Speak Failed")


if __name__ == "__main__":
    from gpt import *
    llm = GPT()
    llm.connect()
    speaker = Speaker(llm)
    speaker.say(text="你好呀")
