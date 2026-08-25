from openai import OpenAI
import json

from common import *
from connect import *


class GPT(BaseLLM):

    def __init__(self):
        super().__init__("GPT")
        self.json_path = 'gpt_api.json'
        self._create_empty_json()

    def connect(self, api_url="", api_key=""):
        if not api_url or not api_key:
            api_url, api_key = self.read_json()
        self.api_url = api_url
        self.api_key = api_key
        try:
            self.client = OpenAI(
                base_url = self.api_url,
                api_key = self.api_key
            )
            self.client.models.list()
            logger.info("Connect to OpenAI API Success!")
            return True
        except Exception as e:
            error(e, "Connect to OpenAI API Failed! Please check the API configuration")
            return False

    def chat(self, message='', model="gpt-4o-mini", temperature=0):
        messages = [{"role": "system", "content": llm_role},
                    {"role": "user", "content": llm_prompt + message}]
        response = self.client.chat.completions.create(
            model=model,
            messages=messages,
            temperature=temperature,
        )
        return response.choices[0].message.content.strip()

    def speech(self, model="whisper-1", audio_path=""):
        audio_file= open(audio_path, "rb")
        transcription = self.client.audio.transcriptions.create(
            model=model,
            file=audio_file
        )
        return transcription.text

    def speak(self, text="", model="tts-1", voice="onyx", audio_path=""):
        response = self.client.audio.speech.create(
            model=model,
            voice=voice,
            input=text
        )
        logger.info(f"Voice: {voice}")
        response.stream_to_file(audio_path)


if __name__ == '__main__':
    blt = BluetoothClient()
    llm = GPT()
    llm.connect()
    # print(llm.chat("who are you?"))
    # devices = blt.list_devices()
    # if devices:
    #     device_address = devices[0]
    #     blt.connect(device_address)
    #     while True:
    #         question = input('Type command (type "exit" to exit): ')
    #         if question == 'exit':
    #             blt.disconnect()
    #             break
    #         response = llm.chat(question)
    #         blt.send(response)
    # else:
    #     print("No devices found.")