import os
import logging
import json


# Set Version
VERSION = "v3.1.0"

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


# LLM voices
# OpenAI legacy voices (kept for backward compatibility)
gpt_voice_list = [
    {'Onyx', 'onyx'},
    {'Alloy', 'alloy'},
    {'Echo', 'echo'},
    {'Fable', 'fable'},
    {'Nova', 'nova'},
    {'Shimmer', 'shimmer'}]

# Gemini TTS prebuilt voice names (ref: Google Gemini TTS docs)
gemini_voice_list = [
    ("Kore", "Kore"), ("Puck", "Puck"), ("Zephyr", "Zephyr"), ("Charon", "Charon"), ("Fenrir", "Fenrir"),
    ("Leda", "Leda"), ("Orus", "Orus"), ("Aoede", "Aoede"), ("Callirrhoe", "Callirrhoe"), ("Autonoe", "Autonoe"),
    ("Enceladus", "Enceladus"), ("Iapetus", "Iapetus"), ("Umbriel", "Umbriel"), ("Algieba", "Algieba"), ("Despina", "Despina"),
    ("Erinome", "Erinome"), ("Algenib", "Algenib"), ("Rasalgethi", "Rasalgethi"), ("Laomedeia", "Laomedeia"), ("Achernar", "Achernar"),
    ("Alnilam", "Alnilam"), ("Schedar", "Schedar"), ("Gacrux", "Gacrux"), ("Pulcherrima", "Pulcherrima"), ("Achird", "Achird"),
    ("Zubenelgenubi", "Zubenelgenubi"), ("Vindemiatrix", "Vindemiatrix"), ("Sadachbia", "Sadachbia"), ("Sadaltager", "Sadaltager"), ("Sulafat", "Sulafat")
]

volcano_voice_list = [
    {"灿灿 2.0", "BV700_V2_streaming"}, 
    {"炀炀", "BV705_streaming"}, 
    {"擎苍 2.0", "BV701_V2_streaming"}, 
    {"通用女声 2.0", "BV001_V2_streaming"}, 
    {"灿灿", "BV700_streaming"}, 
    {"超自然音色-梓梓2.0", "BV406_V2_streaming"}, 
    {"超自然音色-梓梓", "BV406_streaming"}, 
    {"超自然音色-燃燃2.0", "BV407_V2_streaming"}, 
    {"超自然音色-燃燃", "BV407_streaming"}, 
    {"通用女声", "BV001_streaming"}, 
    {"通用男声", "BV002_streaming"}, 
    {"擎苍", "BV701_streaming"}, 
    {"阳光青年", "BV123_streaming"}, 
    {"反卷青年", "BV120_streaming"}, 
    {"通用赘婿", "BV119_streaming"}, 
    {"古风少御", "BV115_streaming"}, 
    {"霸气青叔", "BV107_streaming"}, 
    {"质朴青年", "BV100_streaming"}, 
    {"温柔淑女", "BV104_streaming"}, 
    {"开朗青年", "BV004_streaming"}, 
    {"甜宠少御", "BV113_streaming"}, 
    {"儒雅青年", "BV102_streaming"}, 
    {"甜美小源", "BV405_streaming"}, 
    {"亲切女声", "BV007_streaming"}, 
    {"知性女声", "BV009_streaming"}, 
    {"诚诚", "BV419_streaming"}, 
    {"童童", "BV415_streaming"}, 
    {"亲切男声", "BV008_streaming"}, 
    {"译制片男声", "BV408_streaming"}, 
    {"懒小羊", "BV426_streaming"}, 
    {"清新文艺女声", "BV428_streaming"}, 
    {"鸡汤女声", "BV403_streaming"}, 
    {"智慧老者", "BV158_streaming"}, 
    {"慈爱姥姥", "BV157_streaming"}, 
    {"说唱小哥", "BR001_streaming"}, 
    {"活力解说男", "BV410_streaming"}, 
    {"影视解说小帅", "BV411_streaming"}, 
    {"解说小帅-多情感", "BV437_streaming"}, 
    {"影视解说小美", "BV412_streaming"}, 
    {"纨绔青年", "BV159_streaming"}, 
    {"直播一姐", "BV418_streaming"}, 
    {"反卷青年", "BV120_streaming"}, 
    {"沉稳解说男", "BV142_streaming"}, 
    {"潇洒青年", "BV143_streaming"}, 
    {"阳光男声", "BV056_streaming"}, 
    {"活泼女声", "BV005_streaming"}, 
    {"小萝莉", "BV064_streaming"}, 
    {"奶气萌娃", "BV051_streaming"}, 
    {"动漫海绵", "BV063_streaming"},
    {"动漫海星", "BV417_streaming"}, 
    {"动漫小新", "BV050_streaming"}, 
    {"天才童声", "BV061_streaming"}, 
    {"促销男声", "BV401_streaming"}, 
    {"促销女声", "BV402_streaming"}, 
    {"磁性男声", "BV006_streaming"}, 
    {"新闻女声", "BV011_streaming"}, 
    {"新闻男声", "BV012_streaming"}, 
    {"知性姐姐-双语", "BV034_streaming"}, 
    {"温柔小哥", "BV033_streaming"}, 
    {"东北老铁", "BV021_streaming"}, 
    {"东北丫头", "BV020_streaming"}, 
    {"方言灿灿", "BV704_streaming"}, 
    {"西安佟掌柜", "BV210_streaming"}, 
    {"沪上阿姐", "BV217_streaming"}, 
    {"广西表哥", "BV213_streaming"}, 
    {"甜美台妹", "BV025_streaming"}, 
    {"台普男声", "BV227_streaming"}, 
    {"港剧男神", "BV026_streaming"}, 
    {"广东女仔", "BV424_streaming"}, 
    {"相声演员", "BV212_streaming"}, 
    {"重庆小伙", "BV019_streaming"}, 
    {"四川甜妹儿", "BV221_streaming"}, 
    {"重庆幺妹儿", "BV423_streaming"}, 
    {"乡村企业家", "BV214_streaming"}, 
    {"湖南妹坨", "BV226_streaming"}, 
    {"长沙靓女", "BV216_streaming"}
]


# Command button list
eye_button_list = [
    ("眨眼", "eye_blink"),
    ("快乐", "eye_happy"),
    ("难过", "eye_sad"),
    ("生气", "eye_angry"),
    ("惊讶", "eye_surprised"),
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