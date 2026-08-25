import os
import time
import logging
import inquirer
import pygame
import json
import platform
import random
import serial
import serial.tools.list_ports
import speech_recognition as sr
import asyncio
import threading
from bleak import BleakClient, BleakScanner
from concurrent.futures import ThreadPoolExecutor
from openai import OpenAI
from langchain.memory import ConversationBufferMemory


# Set conversation memory
memory = ConversationBufferMemory(buffer_key="chat_history")
max_memory = 30

## Set logger
log_directory = 'logs'
if not os.path.exists(log_directory):
    os.makedirs(log_directory)
logger = logging.getLogger('Logger')
logger.setLevel(logging.DEBUG)

debug_handler = logging.FileHandler(os.path.join(log_directory, 'debug.log'))
debug_handler.setLevel(logging.DEBUG)
debug_handler.setFormatter(logging.Formatter('%(asctime)s - %(levelname)s - %(message)s'))

error_handler = logging.FileHandler(os.path.join(log_directory, 'error.log'))
error_handler.setLevel(logging.ERROR)
error_handler.setFormatter(logging.Formatter('%(asctime)s - %(levelname)s - %(message)s'))

info_handler = logging.FileHandler(os.path.join(log_directory, 'info.log'))
info_handler.setLevel(logging.INFO)
info_handler.setFormatter(logging.Formatter('%(asctime)s - %(levelname)s - %(message)s'))

stream_handler = logging.StreamHandler()
stream_handler.setLevel(logging.INFO)
stream_handler.setFormatter(logging.Formatter('%(asctime)s - %(message)s'))

logger.addHandler(debug_handler)
logger.addHandler(error_handler)
logger.addHandler(info_handler)
logger.addHandler(stream_handler)


# Command button list
eye_button_list = [
    ("眨眼", "eye_blink"),
    ("快乐", "eye_happy"),
    ("难过", "eye_sad"),
    ("生气气", "eye_anger"),
    ("惊讶", "eye_surprise"),
    ("向左看", "eye_left"),
    ("向右看", "eye_right"),
]


head_button_list = [
    ("左转", "head_left"),
    ("右转", "head_right"),
    ("抬头", "head_up"),
    ("低头", "head_down"),
    ("点头", "head_nod"),
    ("摇头", "head_shake"),
    ("向左环绕", "head_roll_left"),
    ("向右环绕", "head_roll_right"),
    ("归中", "head_center"),
]


def error(e="", msg=""):
    print(f"[Error] {msg}. See details in logs/error.log")
    logger.error(msg)
    logger.error(e)


class ChatGPT(object):

    def __init__(self):
        self.json_path = 'api.json'
        self.client = None
        self.__create_empty_json()
        self.connect()

    def __create_empty_json(self):
        data = {
            "api_url": "",
            "api_key": ""
        }
        if not os.path.exists(self.json_path):
            with open(self.json_path, 'w') as fp:
                json.dump(data, fp, indent=4)

    def read_json(self):
        with open(self.json_path, 'r') as json_file:
            data = json.load(json_file)
        return data.get('api_url', ''), data.get('api_key', '')

    def write_json(self, data):
        with open(self.json_path, 'w') as fp:
            json.dump(data, fp, indent=4)

    def connect(self, api_url="", api_key=""):
        if not api_url or not api_key:
            api_url, api_key = self.read_json()
        try:
            self.client = OpenAI(
                base_url = api_url,
                api_key = api_key
            )
            self.client.models.list()
            logger.info("Connect to OpenAI API Success!")
            return True
        except Exception as e:
            error(e, "Connect to OpenAI API Failed! Please check the API configuration")
            return False


class SerialClient(object):

    def __init__(self):
        self.port = ''
        self.ser = None
        self.connected = False

    def __unique_ports(self, ports):
        port_list = []
        for port in ports:
            if port not in port_list:
                port_list.append(port)
        return port_list

    def list_ports(self):
        ports = serial.tools.list_ports.comports()
        matching_ports = [port.device for port in ports if platform.system() == 'Windows' or "serial" in port.device.lower()]
        non_matching_ports = [port.device for port in ports if port.device not in matching_ports]
        return self.__unique_ports(matching_ports + non_matching_ports)

    def select_port(self):
        ports = self.list_ports()
        if len(ports) == 1:
            return ports[0]
        questions = [
            inquirer.List('port',
                          message="Select a port",
                          choices=ports,
                          carousel=True)
        ]
        answers = inquirer.prompt(questions)
        self.port = answers['port']
        return self.port

    def connect(self, port="", baud_rate=115200):
        try:
            port = port if port else self.port
            self.ser = serial.Serial(port, baud_rate, timeout=1)
            logger.info(f"Connected to {port} at {baud_rate} baud rate.")
            self.connected = True
            return True
        except Exception as e:
            error(e, f"Connect to {port} Failed")
            self.connected = False
            return False

    def read(self, port):
        while True:
            if port.in_waiting:
                data = port.read(port.in_waiting)
                result = data.decode('utf-8', errors='ignore')
                logger.debug("\nReceived:", result.strip('\n').strip())

    def send(self, msg):
        try:
            encode_msg = msg.encode('utf-8')
            self.ser.write(encode_msg)
            logger.debug(f"Sent: {msg}")
            start_time = time.time()
            received_msg = ""
            while True:
                if self.ser.in_waiting > 0:
                    received_msg += self.ser.read(self.ser.in_waiting).decode('utf-8')

                if time.time() - start_time > 10: return

                if received_msg and msg in received_msg:
                    logger.debug(f"Received: {received_msg}")
                    return received_msg

                time.sleep(0.1)
        except Exception as e:
            error(e, "Serial port send message Failed!")


class BaseBluetoothClient(object):
    def __init__(self, device_name="", service_uuid="", characteristic_uuid=""):
        self.device_name = device_name
        self.service_uuid = service_uuid
        self.characteristic_uuid = characteristic_uuid
        self.client = None
        self.connected = False

    async def list_devices(self):
        logger.info("Scanning devices...")
        device_list = []
        devices = await BleakScanner.discover()
        for device in devices:
            if device.name == self.device_name:
                device_list.append(device.address)
        return device_list

    async def connect(self, device_address):
        self.client = BleakClient(device_address)
        try:
            await self.client.connect()
            logger.info(f"Connected to {self.device_name} at {device_address}")
            self.connected = True
            return True
        except Exception as e:
            logger.error(f"Failed to connect to {self.device_name}: {e}")
            self.connected = False
            return False

    async def send(self, data):
        if self.client and self.client.is_connected:
            try:
                await self.client.write_gatt_char(self.characteristic_uuid, data.encode('utf-8'))
                logger.info(f"Sent to {self.device_name}: {data}")
            except Exception as e:
                logger.error(f"Failed to send data: {e}")
        else:
            logger.info("Not connected to any device.")

    async def disconnect(self):
        if self.client and self.client.is_connected:
            await self.client.disconnect()
            logger.info(f"Disconnected from {self.device_name}")


class BluetoothClient(BaseBluetoothClient):
    def __init__(self, device_name="Desk-Emoji",
                 service_uuid="4db9a22d-6db4-d9fe-4d93-38e350abdc3c",
                 characteristic_uuid="ff1cdaef-0105-e4fb-7be2-018500c2e927"):
        super().__init__(device_name, service_uuid, characteristic_uuid)
        self.loop_thread = threading.Thread(target=self._run_event_loop)
        self.loop_thread.daemon = True
        self.loop = asyncio.new_event_loop()
        self.loop_thread.start()

    def _run_event_loop(self):
        asyncio.set_event_loop(self.loop)
        self.loop.run_forever()

    def list_devices(self):
        return asyncio.run_coroutine_threadsafe(super().list_devices(), self.loop).result()

    def connect(self, device_address):
        return asyncio.run_coroutine_threadsafe(super().connect(device_address), self.loop).result()

    def send(self, data):
        asyncio.run_coroutine_threadsafe(super().send(data), self.loop).result()

    def disconnect(self):
        asyncio.run_coroutine_threadsafe(super().disconnect(), self.loop).result()


class Listener(object):

    def __init__(self):
        self.recognizer = sr.Recognizer()
        self.executor = ThreadPoolExecutor(max_workers=1)

    def hear(self, audio_path='input.wav', timeout=8):
        try:
            with sr.Microphone() as source:
                print("开始说话...")
                self.executor.submit(act_random)
                audio_data = self.recognizer.listen(source, timeout=timeout)
                print("录音已完成")
                with open(audio_path, "wb") as audio_file:
                    audio_file.write(audio_data.get_wav_data())

                audio_file= open(audio_path, "rb")
                transcription = chatgpt.client.audio.transcriptions.create(
                    model="whisper-1",
                    file=audio_file
                )
                return transcription.text

        except Exception as e:
            error(e, "Speech recognition Failed")


class Speaker(object):
    def __init__(self):
        self.executor = ThreadPoolExecutor(max_workers=1)
        pygame.mixer.init()
    
    def play_audio(self, audio_path):
        pygame.mixer.music.load(audio_path)
        pygame.mixer.music.play()
        while pygame.mixer.music.get_busy():
            time.sleep(1)

    def say(self, text="", model="tts-1", voice="onyx", audio_path='output.mp3'):
        try:
            response = chatgpt.client.audio.speech.create(
                model=model,
                voice=voice,
                input=text
            )
            response.stream_to_file(audio_path)
            self.executor.submit(self.play_audio, audio_path)

        except Exception as e:
            error(e, "Speak Failed")


# Instantiation
chatgpt = ChatGPT()
ser = SerialClient()
blt = BluetoothClient()
listener = Listener()
speaker = Speaker()


def send_cmd(command):
    if ser.connected:
        ser.send(command)
    if blt.connected:
        blt.send(command)


def get_completion(model="gpt-4o-mini", temperature=0, history_messages=[], prompt=''):
    # Define the rule text for the system message
    rule_text = """
You are a small size desktop robot.
You are funny and lovely.
Your name is 'Desk-Emoji'.
Your gender is male.
Your age is 1.
Your eyes are blue.
You have no hands and legs, stick on the desk.
I am your master.
Responses in short, with variety emotions, without emoji.
"""

    # Add a request to analyze the sentiment immediately after providing the response
    sentiment_request = """
After providing your response, analyze its sentiment and specify one of the following sentiments: 
'happy', 'sad', 'normal', 'curious', 'surprised', or 'anger'.
Ensure to separate the response and sentiment by clearly labeling them.
"""

    # Construct the messages
    messages = [{"role": "system", "content": rule_text}]
    for message in history_messages:
        if message.startswith('Human:'):
            role = "user"
            content = message.replace('Human:', '').strip()
        elif message.startswith('AI:'):
            role = "assistant"
            content = message.replace('AI:', '').strip()
        else:
            continue
        messages.append({"role": role, "content": content})
    
    prompt_with_analysis = f"{prompt}\n\n{sentiment_request}"
    messages.append({"role": "user", "content": prompt_with_analysis})

    response = chatgpt.client.chat.completions.create(
        model=model,
        messages=messages,
        temperature=temperature,
    ).choices[0].message.content.strip()

    try:
        chat_response, emotion_response = response.split("Sentiment: ")
    except ValueError:
        chat_response = response
        emotion_response = "undetected"

    chat_response = chat_response.split('**')[0].strip()
    emotion_response = emotion_response.lower().strip()

    return chat_response, emotion_response


def chat(question):
    try:
        if not question: return None, None
        logger.info(f"You: {question}")
        memory_content = memory.load_memory_variables(inputs={})
        history = memory_content.get('chat_history', [])
        history_messages = history[max_memory*-2:]
        answer, emotion = get_completion(temperature=1, history_messages=history_messages, prompt=question)
        memory.chat_memory.add_user_message(question)
        memory.chat_memory.add_ai_message(answer)
        logger.info(f"Bot: {answer}")
        logger.info(f'Emo: {emotion}')
        return answer, emotion
    except Exception as e:
        error(e, "Chat Failed!")
        return "OpenAI 连接失败！请检查 API 配置", "(x_x)"


def act_introduce():
    text = '''
Hello 你好呀！我是Desk-Emoji，你的友好桌面机器人，致力于为你的工作空间带来快乐和效率。
凭借我充满活力的个性和实用的功能，我的目标是在辅助你完成日常任务的同时，为你带来乐趣。
无论你需要讲个笑话、一句励志名言，还是仅仅一个微笑，我都在这里为你增添一天的光彩，让你的桌面体验更加愉悦。
准备好与我一起探索 emoji 的创意世界了吗？Let's roll!。
'''
    speaker.say(text)
    send_cmd('eye_happy')
    send_cmd('head_roll')
    send_cmd('eye_blink')
    send_cmd('head_move 20, 15, 30')
    send_cmd('eye_right')
    send_cmd('head_move -40, 0, 20')
    send_cmd('eye_blink')
    send_cmd('head_center')
    send_cmd('eye_blink')
    send_cmd('eye_happy')
    send_cmd('head_shake')
    send_cmd('head_roll')
    send_cmd('head_nod')
    send_cmd('head_center')
    send_cmd('eye_blink')


def act_random(loop=False, sleep_min=0, sleep_max=8):
    def act():
        random_num = random.random()
        x = random.randint(-25, 25)
        y = random.randint(-20, 50)
        if random_num < 0.2:
            selected_cmd = "head_roll"
        else:
            selected_cmd = f"head_move {x} {y} 10"

        send_cmd(selected_cmd)
        if "head_move" in selected_cmd:
            if x > 0 and random_num < 0.8:
                send_cmd('eye_right')
            elif x < 0 and random_num < 0.8:
                send_cmd('eye_left')
            elif random_num < 0.4:
                send_cmd('eye_happy')
            else:
                send_cmd('eye_blink')
    act()
    send_cmd('head_center')
    while loop:
        act()
        time.sleep(random.randint(sleep_min, sleep_max))


def act_happy():
    send_cmd('eye_happy')
    command = random.choice(['head_nod', 'head_shake'])
    send_cmd(command)
    send_cmd('eye_blink')


def act_sad():
    send_cmd('head_move 0 100 10')
    send_cmd('eye_sad')
    send_cmd('head_shake')
    send_cmd('head_center')
    send_cmd('eye_blink')


def act_anger():
    send_cmd('eye_anger')
    for i in range(2):
        command = random.choice(['head_nod', 'head_shake'])
        send_cmd(command)
    send_cmd('eye_blink')


def act_surprise():
    send_cmd('eye_surprise')
    send_cmd('head_shake')
    time.sleep(1)
    send_cmd('eye_blink')


def act_curiosity():
    def look_side(x_offset):
        if x_offset > 0:
            send_cmd('eye_right')
        else:
            send_cmd('eye_left')

    x_value = random.randint(15, 25)
    y_value = random.randint(15, 30)
    x_offset = random.choice([x_value * -1, x_value])
    y_offset = random.choice([y_value * -1, y_value])
    send_cmd(f'head_move {x_offset} {y_offset} 20')
    look_side(x_offset)
    send_cmd(f'head_move {x_offset * -2} 10 15')
    send_cmd('head_center')
    send_cmd('eye_blink')


def act_emotion(emotion):
    if emotion:
        if 'happy' in emotion:
            act_happy()
        elif 'sad' in emotion:
            act_sad()
        elif 'anger' in emotion:
            act_anger()
        elif 'surprise' in emotion or 'undetected' in emotion:
            act_surprise()
        elif 'curious' in emotion:
            act_curiosity()
        else:
            act_random()
        send_cmd('head_center')


# Test
if __name__ == '__main__':
    devices = blt.list_devices()
    if devices:
        device_address = devices[0]
        blt.connect(device_address)
        while True:
            command = input('Type command (type "exit" to exit): ')
            blt.send(command)
            if command == 'exit':
                blt.disconnect()
                break
    else:
        print("No devices found.")