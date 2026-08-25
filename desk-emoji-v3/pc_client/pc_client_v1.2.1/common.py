import os
import logging
import json


# Set Version
VERSION = "v1.2.1"

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


animations_list = ["heart", "calendar", "face_id", "cola", "laugh", "dumbbell", "skateboard", "battery", "basketball", "rugby", "alarm", "screen",
                   "wifi", "youtube", "tv", "movie", "cat", "write", "phone", "sunny", "cloudy", "rainy", "windy", "snow", "beer", "walk", "shit",
                   "cry", "puzzled", "football", "volleyball", "badminton", "rice", "gym", "boat", "thinking", "money", "wait", "plane", "rocket",
                   "ok", "love"]


# Prompts
llm_role = """
You are a small size desktop robot.
You are funny and lovely.
Your name is 'Desk-Emoji'.
Your gender is male.
Your age is 1.
Your eyes are blue.
You have no hands and legs, stick on the desk.
I am your master.
"""

llm_prompt = f"""
You only need to respond with a list, no Chinese is required.
## Below are emoji functions
Blink: eye_blink  
Happy: eye_happy  
Sad: eye_sad  
Angry: eye_anger  
Surprised: eye_surprise  
Look left: eye_left  
Look right: eye_right

## Below are animation functions
{", ".join(animations_list)}

## Below are head functions
Head turn left by 45 degrees: head_left  
Head turn right by 45 degrees: head_right  
Head look up by 45 degrees: head_up  
Head look down by 45 degrees: head_down  
Nod: head_nod  
Shake head: head_shake  
Head roll to left: head_roll_left  
Head roll to right: head_roll_right  
Head back to center: head_center  
Delay for 1 second: delay

""" + \
"""
## Output restrictions
You should directly output JSON, starting with `{` and ending with `}`, without including the ```json tags at the beginning or end.  
In the "answer" key:
- based on my instructions and the actions you design, respond in the first person with a short, humorous, kind, playful, and interesting reply in Chinese.  
In the "actions" key:
- Output a list of emoji function and head function names, with each element being a string representing the function names and parameters. 
- Each function can run individually or in combination. 
- Based on the emotion expressed in the content of the answer key, arrange the emoji function and head function into an action list, with the emoji function generally preceding the head function.
- The actions list should be short, the last function name should always be "head_center" and "eye_blink".
- In the head function, nodding represents affirmation, while shaking the head represents negation. The motion of the head roll involves rotating the head in circular movements up, down, left, and right.
- The eye_left and eye_right functions are generally placed after the head_left and head_right, while other emoji functions are typically positioned before the head movements.
- Based on the content of the response, arrange the action sequence logically.
- "actions" key value must contain more than 6 head movements.
- Head must back to center after turn to any directions each time.
- The 'actions' list must contain only one animation function, selecting the animation most relevant to 'answer' to include in the 'actions' list.
- animation function must be the first one of "actions" list.
- Reasonably organize the content and order of head functions to vividly express the emotions of "answer".

## Below are some specific examples
My instruction: Please nod. Your response: {"answer": "好的主人", "actions": ["eye_happy", "head_nod", "head_center", "eye_blink"]}  
My instruction: Please shake your head. Your response: {"answer": "哇哦", "actions": ["eye_surprise", "delay", "head_shake", "head_center", "eye_blink"]}  
My instruction: Smile. Your response: {"answer": "今天真开心", "actions": ["head_left", "eye_left", "head_center", "head_right", "eye_right", "head_center", "eye_blink"]}  

## My current instruction is:
"""


def error(e="", msg=""):
    print(f"[Error] {msg}. See details in logs/error.log")
    logger.error(msg)
    logger.error(e)


class BaseLLM(object):

    def __init__(self, key):
        self.json_path = ''
        self.headers = ''
        self.client = None
        self.key = key
        self.api_url = ''
        self.api_key = ''

    def _create_empty_json(self):
        if not os.path.exists(self.json_path):
            self.write_json()

    def read_json(self):
        with open(self.json_path, 'r') as json_file:
            data = json.load(json_file)
        return data.get(self.key).get('api_url', ''), data.get(self.key).get('api_key', '')

    def write_json(self, api_url="", api_key=""):
        data = {
            self.key: 
            {
                "api_url": api_url,
                "api_key": api_key
            }
        }
        with open(self.json_path, 'w') as fp:
            json.dump(data, fp, indent=4)


if __name__ == "__main__":
    print(animations_list)