from openai import OpenAI
import json
import wave

from google import genai
from google.genai import types

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
            # Initialize Google Gemini client for native TTS
            self.gemini_client = genai.Client(api_key=self.api_key)
            logger.info("Connect to Gemini API Success!")
            return True
        except Exception as e:
            error(e, "Connect to Gemini API Failed! Please check the API configuration")
            return False

    def chat(self, message='', model="gemini-2.5-flash", temperature=0):
        messages = [{"role": "system", "content": llm_role},
                    {"role": "user", "content": llm_prompt + message}]
        response = self.client.chat.completions.create(
            model=model,
            messages=messages,
            temperature=temperature,
        )
        return response.choices[0].message.content.strip()


    def speak(self, text="", model="gemini-2.5-flash-preview-tts", voice="Kore", audio_path=""):
        if not hasattr(self, 'gemini_client'):
            raise RuntimeError("Gemini client is not initialized. Call connect() first.")

        if not audio_path:
            audio_path = "out.wav"

        response = self.gemini_client.models.generate_content(
            model=model,
            contents=text,
            config=types.GenerateContentConfig(
                response_modalities=["AUDIO"],
                speech_config=types.SpeechConfig(
                    voice_config=types.VoiceConfig(
                        prebuilt_voice_config=types.PrebuiltVoiceConfig(
                            voice_name=voice,
                        )
                    )
                ),
            )
        )

        data = response.candidates[0].content.parts[0].inline_data.data

        with wave.open(audio_path, "wb") as wf:
            wf.setnchannels(1)
            wf.setsampwidth(2)
            wf.setframerate(24000)
            wf.writeframes(data)

        logger.info(f"Voice: {voice}")


if __name__ == '__main__':
    blt = BluetoothClient()
    llm = GPT()
    llm.connect()