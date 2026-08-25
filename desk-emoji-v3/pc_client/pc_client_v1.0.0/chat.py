import threading

from common import *
from action import *


def main():
    input("\n按回车开始说话 ")
    question = listener.hear()
    answer, emotion = chat(question)
    speaker.say(answer)
    threading.Thread(target=act_emotion, args=(cmd, emotion)).start()


if __name__ == '__main__':
    logger.info('Initializing...')
    cmd = CmdClient()
    cmd.select_port()
    cmd.connect()
    listener = Listener(cmd)
    speaker = Speaker()
    logger.info("Initialization complete.")
    try:
        while True:
            main()
    except KeyboardInterrupt:
        logger.info("Stopped.")
