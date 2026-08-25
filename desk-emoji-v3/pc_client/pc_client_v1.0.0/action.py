from common import *


def act_introduce(cmd, speaker):
    text = '''
Hello 你好呀！我是Desk-Emoji，你的友好桌面机器人，致力于为你的工作空间带来快乐和效率。
凭借我充满活力的个性和实用的功能，我的目标是在辅助你完成日常任务的同时，为你带来乐趣。
无论你需要讲个笑话、一句励志名言，还是仅仅一个微笑，我都在这里为你增添一天的光彩，让你的桌面体验更加愉悦。
准备好与我一起探索 emoji 的创意世界了吗？让我们开始吧。
'''
    speaker.say(text)
    cmd.send('eye_happy')
    cmd.send('head_roll')
    cmd.send('eye_blink')
    cmd.send('head_move 20, 15, 30')
    cmd.send('eye_right')
    cmd.send('head_move -40, 0, 20')
    cmd.send('eye_blink')
    cmd.send('head_center')
    cmd.send('eye_blink')
    cmd.send('eye_happy')
    cmd.send('head_shake')
    cmd.send('head_roll')
    cmd.send('head_nod')
    cmd.send('head_center')
    cmd.send('eye_blink')


if __name__ == '__main__':
    cmd = CmdClient()
    cmd.select_port()
    cmd.connect()
    speaker = Speaker()
    print('\nPerforming...\n')
    act_introduce(cmd, speaker)
    act_random(cmd, loop=True, sleep_min=0, sleep_max=3)
