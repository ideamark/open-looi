import os
import threading
from PIL import Image
import tkinter as tk
import customtkinter as ctk

from common import *


class App(ctk.CTk):
    def __init__(self):

        super().__init__()
        title = "Desk-Emoji v1.1.0"

        # flags
        self.checked = False
        self.api_connected = False
        self.usb_connected = False
        self.blt_connected = False

        # init window
        self.title(title)
        self.window_width = 700
        self.window_height = 510
        self.geometry(f"{self.window_width}x{self.window_height}")
        self.resizable(False, False)
        self.center_window()

        # set grid layout 1x2
        self.grid_rowconfigure(0, weight=1)
        self.grid_columnconfigure(1, weight=1)

        # load images with light and dark mode image
        icon_path = os.path.join(os.path.dirname(os.path.realpath(__file__)), "icons")
        self.logo_image = ctk.CTkImage(Image.open(os.path.join(icon_path, "main_icon.png")), size=(26, 26))
        self.chat_image = ctk.CTkImage(light_image=Image.open(os.path.join(icon_path, "chat_dark.png")),
                                       dark_image=Image.open(os.path.join(icon_path, "chat_light.png")), size=(20, 20))
        self.act_image = ctk.CTkImage(light_image=Image.open(os.path.join(icon_path, "act_dark.png")),
                                      dark_image=Image.open(os.path.join(icon_path, "act_light.png")), size=(20, 20))
        self.usb_icon = ctk.CTkImage(light_image=Image.open(os.path.join(icon_path, "usb_dark.png")),
                                     dark_image=Image.open(os.path.join(icon_path, "usb_light.png")), size=(20, 20))
        self.api_icon = ctk.CTkImage(light_image=Image.open(os.path.join(icon_path, "api_dark.png")),
                                     dark_image=Image.open(os.path.join(icon_path, "api_light.png")), size=(20, 20))
        self.help_icon = ctk.CTkImage(light_image=Image.open(os.path.join(icon_path, "help_dark.png")),
                                      dark_image=Image.open(os.path.join(icon_path, "help_light.png")), size=(20, 20))

        # create navigation frame
        self.navigation_frame = ctk.CTkFrame(self, corner_radius=0)
        self.navigation_frame.grid(row=0, column=0, sticky="nsew")
        self.navigation_frame.grid_rowconfigure(5, weight=1)

        self.navigation_frame_label = ctk.CTkLabel(self.navigation_frame, text="  Desk-Emoji", image=self.logo_image,
                                                             compound="left", font=ctk.CTkFont(size=15, weight="bold"))
        self.navigation_frame_label.grid(row=0, column=0, padx=20, pady=20)

        self.chat_button = ctk.CTkButton(self.navigation_frame, corner_radius=0, height=40, border_spacing=10, text="对话",
                                         fg_color="transparent", text_color=("gray10", "gray90"), hover_color=("gray70", "gray30"),
                                         image=self.chat_image, anchor="w", command=self.chat_button_event)
        self.chat_button.grid(row=1, column=0, sticky="ew")

        self.act_button = ctk.CTkButton(self.navigation_frame, corner_radius=0, height=40, border_spacing=10, text="动作",
                                         fg_color="transparent", text_color=("gray10", "gray90"), hover_color=("gray70", "gray30"),
                                         image=self.act_image, anchor="w", command=self.act_button_event)
        self.act_button.grid(row=2, column=0, sticky="ew")

        self.connect_button = ctk.CTkButton(self.navigation_frame, corner_radius=0, height=40, border_spacing=10, text="串口",
                                        fg_color="transparent", text_color=("gray10", "gray90"), hover_color=("gray70", "gray30"),
                                        image=self.usb_icon, anchor="w", command=self.connect_button_event)
        self.connect_button.grid(row=3, column=0, sticky="ew")

        self.api_button = ctk.CTkButton(self.navigation_frame, corner_radius=0, height=40, border_spacing=10, text="API",
                                        fg_color="transparent", text_color=("gray10", "gray90"), hover_color=("gray70", "gray30"),
                                        image=self.api_icon, anchor="w", command=self.api_button_event)
        self.api_button.grid(row=4, column=0, sticky="ew")

        self.help_button = ctk.CTkButton(self.navigation_frame, corner_radius=0, height=40, border_spacing=10, text="帮助",
                                         fg_color="transparent", text_color=("gray10", "gray90"), hover_color=("gray70", "gray30"),
                                         image=self.help_icon, anchor="w", command=self.help_button_event)
        self.help_button.grid(row=5, column=0, sticky="ew")

        self.appearance_mode_menu = ctk.CTkOptionMenu(self.navigation_frame, values=["System", "Light", "Dark"],
                                                      command=self.change_appearance_mode_event)
        self.appearance_mode_menu.grid(row=6, column=0, padx=20, pady=20, sticky="s")

        # create chat frame
        self.chat_frame = ctk.CTkFrame(self, corner_radius=0, fg_color="transparent")
        self.chat_frame.grid_columnconfigure(0, weight=1)
        self.chat_frame.grid_columnconfigure(1, weight=1)
        self.chat_frame.grid_columnconfigure(2, weight=1)

        self.textbox = ctk.CTkTextbox(self.chat_frame, height=300)
        self.textbox.grid(row=0, column=0, columnspan=3, padx=(20, 20), pady=(20, 20), sticky="nsew")

        self.chat_msg = ctk.CTkEntry(self.chat_frame)
        self.chat_msg.grid(row=1, column=0, columnspan=2, padx=20, pady=0, sticky="ew")
        self.chat_msg.bind("<Return>", self.chat_msg_event)

        self.send_button = ctk.CTkButton(self.chat_frame, text="发送", height=40,
                                         command=self.chat_msg_event)
        self.send_button.grid(row=1, column=2, padx=20, pady=20, sticky='e')

        self.speaker_switch = ctk.CTkSwitch(self.chat_frame, text="扬声器")
        self.speaker_switch.grid(row=2, column=0, padx=20, pady=20, sticky="nsew")
        self.speaker_switch.select()

        self.voice_combobox = ctk.CTkComboBox(self.chat_frame, values=['onyx', 'alloy', 'echo', 'fable', 'nova', 'shimmer'])
        self.voice_combobox.grid(row=2, column=1, padx=20, pady=20, sticky="ew")
        self.voice_combobox.set('onyx')

        self.speech_button = ctk.CTkButton(self.chat_frame, text="语音", height=40,
                                           command=self.speech_button_event)
        self.speech_button.grid(row=2, column=2, padx=20, pady=20, sticky='e')
        self.origin_fg_color = self.speech_button.cget("fg_color")
        self.origin_hover_color = self.speech_button.cget("hover_color")
        self.origin_text_color = self.speech_button.cget("text_color")

        # create act frame
        self.act_frame = ctk.CTkFrame(self, corner_radius=0, fg_color="transparent")
        self.act_frame.grid_columnconfigure(0, weight=1)
        self.act_frame.grid_columnconfigure(1, weight=1)

        for i, (button_name, button_command) in enumerate(eye_button_list):
            button = ctk.CTkButton(
                self.act_frame, 
                text=button_name,
                command=lambda cmd=button_command: send_cmd(cmd)
            )
            button.grid(row=i, column=0, padx=10, pady=10, sticky='w')

        for i, (button_name, button_command) in enumerate(head_button_list):
            button = ctk.CTkButton(
                self.act_frame, 
                text=button_name,
                command=lambda cmd=button_command: send_cmd(cmd)
            )
            button.grid(row=i, column=1, padx=10, pady=10, sticky='w')

        # create connect frame
        self.connect_frame = ctk.CTkFrame(self, corner_radius=0, fg_color="transparent")
        self.connect_frame.grid_columnconfigure(0, weight=1)

        self.connect_tabview = ctk.CTkTabview(self.connect_frame)
        self.connect_tabview.grid(row=0, column=0, padx=20, pady=20, sticky="nsew")
        self.connect_tabview.add("蓝牙")
        self.connect_tabview.tab("蓝牙").grid_columnconfigure(0, weight=1)
        self.connect_tabview.add("USB")
        self.connect_tabview.tab("USB").grid_columnconfigure(0, weight=1)

        self.blt_combobox = ctk.CTkComboBox(self.connect_tabview.tab("蓝牙"), values=[])
        self.blt_combobox.grid(row=0, column=0, columnspan=2, padx=20, pady=20, sticky="nsew")
        self.blt_combobox.set("")

        self.blt_scan_button = ctk.CTkButton(self.connect_tabview.tab("蓝牙"), text="扫描", command=self.blt_scan_button_event)
        self.blt_scan_button.grid(row=1, column=1, padx=20, pady=10)

        self.blt_connect_button = ctk.CTkButton(self.connect_tabview.tab("蓝牙"), text="连接", command=self.blt_connect_button_event)
        self.blt_connect_button.grid(row=2, column=1, padx=20, pady=10)

        self.blt_flag_label = ctk.CTkLabel(self.connect_tabview.tab("蓝牙"), text="")
        self.blt_flag_label.grid(row=2, column=0, padx=20, pady=10)

        self.usb_combobox = ctk.CTkComboBox(self.connect_tabview.tab("USB"), values=ser.list_ports())
        self.usb_combobox.grid(row=0, column=0, columnspan=2, padx=20, pady=20, sticky="nsew")
        self.usb_combobox.set("")

        self.usb_scan_button = ctk.CTkButton(self.connect_tabview.tab("USB"), text="扫描", command=self.usb_scan_button_event)
        self.usb_scan_button.grid(row=1, column=1, padx=20, pady=10)

        self.usb_connect_button = ctk.CTkButton(self.connect_tabview.tab("USB"), text="连接", command=self.usb_connect_button_event)
        self.usb_connect_button.grid(row=2, column=1, padx=20, pady=10)

        self.usb_flag_label = ctk.CTkLabel(self.connect_tabview.tab("USB"), text="")
        self.usb_flag_label.grid(row=2, column=0, padx=20, pady=10)

        # create api frame
        self.api_frame = ctk.CTkFrame(self, corner_radius=0, fg_color="transparent")
        self.api_frame.grid_columnconfigure(0, weight=1)

        self.api_tabview = ctk.CTkTabview(self.api_frame)
        self.api_tabview.grid(row=0, column=0, padx=20, pady=20, sticky="nsew")
        self.api_tabview.add("OpenAI")
        self.api_tabview.tab("OpenAI").grid_columnconfigure(0, weight=1)
        self.api_tabview.tab("OpenAI").grid_columnconfigure(1, weight=6)

        self.api_url_label = ctk.CTkLabel(self.api_tabview.tab("OpenAI"), text="API URL: ")
        self.api_url_label.grid(row=0, column=0, padx=20, pady=20, sticky="w")
        self.api_url = ctk.CTkEntry(self.api_tabview.tab("OpenAI"))
        self.api_url.grid(row=0, column=1, padx=20, pady=20, sticky="nsew")

        self.api_key_label = ctk.CTkLabel(self.api_tabview.tab("OpenAI"), text="API Key: ")
        self.api_key_label.grid(row=1, column=0, padx=20, pady=20, sticky="w")
        self.api_key = ctk.CTkEntry(self.api_tabview.tab("OpenAI"))
        self.api_key.grid(row=1, column=1, padx=20, pady=20, sticky="nsew")

        self.save_flag_label = ctk.CTkLabel(self.api_tabview.tab("OpenAI"), text="")
        self.save_flag_label.grid(row=2, column=0, padx=20, pady=20)

        self.api_save_button = ctk.CTkButton(self.api_tabview.tab("OpenAI"), text="连接", command=self.api_save_button_event)
        self.api_save_button.grid(row=2, column=1, padx=20, pady=10)

        # create help frame
        self.help_frame = ctk.CTkFrame(self, corner_radius=0, fg_color="transparent")
        self.help_frame.grid_columnconfigure(0, weight=1)

        help_text = f"""
{title} 桌面陪伴机器人

初次配置：
1. 连接机器人 -> 点击“串口” -> 选择 蓝牙 或 USB -> “扫描” -> “连接”
2. 点击“API” -> 配置 URL 网址和 Key（支持中转）-> ”连接“

使用说明：
“对话”界面用于对话互动，可以发文字也可以语音，可以开关扬声器、更改声音
“动作”界面用于测试动作，点击不同按钮触发不同动作


杭州易问科技版权所有 2024.11
联系邮箱：mark.yang@ewen.ltd
"""
        self.help_text_lable = ctk.CTkLabel(self.help_frame, text=help_text, anchor="w", justify="left", wraplength=380)
        self.help_text_lable.grid(row=0, column=0, padx=20, pady=20)

        self.select_frame_by_name("connect")

    def center_window(self):
        screen_width = self.winfo_screenwidth()
        screen_height = self.winfo_screenheight()
        x = (screen_width // 2) - (self.window_width // 2)
        y = (screen_height // 2) - (self.window_height // 2)
        self.geometry(f"{self.window_width}x{self.window_height}+{x}+{y}")

    def load_api_key(self):
        try:
            url, key = chatgpt.read_json()
            if not self.api_url.get():
                self.api_url.insert(0, url)
            if not self.api_key.get():
                self.api_key.insert(0, key)
        except Exception:
            pass

    def save_api_key(self):
        data = {
            'api_url': self.api_url.get(),
            'api_key': self.api_key.get()
        }
        chatgpt.write_json(data)
        logger.info(f"Saved API Key to {chatgpt.json_path}")

    def print_textbox(self, text):
        self.textbox.insert(tk.END, f"{text}\n")
        self.textbox.see(tk.END)

    def select_frame_by_name(self, name):
        self.chat_button.configure(fg_color=("gray75", "gray25") if name == "chat" else "transparent")
        self.act_button.configure(fg_color=("gray75", "gray25") if name == "act" else "transparent")
        self.connect_button.configure(fg_color=("gray75", "gray25") if name == "connect" else "transparent")
        self.api_button.configure(fg_color=("gray75", "gray25") if name == "api" else "transparent")
        self.help_button.configure(fg_color=("gray75", "gray25") if name == "help" else "transparent")

        if name == "chat":
            self.chat_frame.grid(row=0, column=1, sticky="nsew")
        else:
            self.chat_frame.grid_forget()
        if name == "act":
            self.act_frame.grid(row=0, column=1, sticky="nsew")
        else:
            self.act_frame.grid_forget()
        if name == "connect":
            self.connect_frame.grid(row=0, column=1, sticky="nsew")
        else:
            self.connect_frame.grid_forget()
        if name == "api":
            self.api_frame.grid(row=0, column=1, sticky="nsew")
        else:
            self.api_frame.grid_forget()
        if name == "help":
            self.help_frame.grid(row=0, column=1, sticky="nsew")
        else:
            self.help_frame.grid_forget()

    def change_appearance_mode_event(self, new_appearance_mode):
        ctk.set_appearance_mode(new_appearance_mode)

    def chat_button_event(self):
        self.select_frame_by_name("chat")
        self.check_connections()

    def act_button_event(self):
        self.select_frame_by_name("act")

    def connect_button_event(self):
        self.select_frame_by_name("connect")
        self.blt_flag_label.configure(text="", fg_color="transparent")
        self.usb_flag_label.configure(text="", fg_color="transparent")

    def blt_scan_button_event(self):
        devices = blt.list_devices()
        if not devices:
            self.blt_flag_label.configure(text="无可用设备", text_color="red")
            return
        self.blt_combobox.configure(values=devices)
        if devices:
            self.blt_combobox.set(devices[0])

    def blt_connect_button_event(self):
        device_address = self.blt_combobox.get()
        if not device_address: return
        if blt.connect(device_address):
            self.blt_connected = True
            self.blt_flag_label.configure(text="连接成功", text_color="green")
        else:
            self.blt_connected = False
            self.blt_flag_label.configure(text="连接失败", text_color="red")

    def usb_scan_button_event(self):
        ports = ser.list_ports()
        if not ports:
            self.usb_flag_label.configure(text="无可用设备", text_color="red")
            return
        self.usb_combobox.configure(values=ports)
        if ports:
            self.usb_combobox.set(ports[0])

    def usb_connect_button_event(self):
        port = self.usb_combobox.get()
        if not port: return
        if ser.connect(port):
            self.usb_connected = True
            self.usb_flag_label.configure(text="连接成功", text_color="green")
        else:
            self.usb_connected = False
            self.usb_flag_label.configure(text="连接失败", text_color="red")

    def api_button_event(self):
        self.select_frame_by_name("api")
        self.save_flag_label.configure(text="", fg_color="transparent")
        self.load_api_key()

    def api_save_button_event(self):
        self.save_api_key()
        if chatgpt.connect():
            self.save_flag_label.configure(text="连接成功", text_color="green")
        else:
            self.save_flag_label.configure(text="连接失败", text_color="red")

    def help_button_event(self):
        self.select_frame_by_name("help")

    def __chat_LLM(self, question):
        self.print_textbox(f"You:\t{question}")
        answer, emotion = chat(question)
        self.print_textbox(f"Bot:\t{answer}")
        self.print_textbox(f"Emo:\t{emotion}\n")
        if bool(self.speaker_switch.get()):
            speaker.say(text=answer, voice=self.voice_combobox.get())
        threading.Thread(target=act_emotion, args=(emotion,)).start()

    def chat_msg_event(self, event=None):
        question = self.chat_msg.get()
        if question:
            self.chat_msg.delete(0, tk.END)
            threading.Thread(target=self.__chat_LLM, args=(question,)).start()

    def speech_button_event(self):
        self.speech_button.configure(fg_color="#00FF00", 
                                     hover_color="#00FF00", 
                                     text_color="#000000",
                                     text="正在录音")
        threading.Thread(target=self.__process_speech).start()

    def __process_speech(self):
        question = listener.hear()
        self.speech_button.configure(fg_color=self.origin_fg_color,
                                     hover_color=self.origin_hover_color,
                                     text_color=self.origin_text_color,
                                     text="语音")
        self.__chat_LLM(question)

    def check_connections(self):
        success = True
        if not self.checked:    
            if self.api_connected or chatgpt.connect():
                self.print_textbox("API 连接成功")
            else:
                success = False
                self.print_textbox("API 未连接，请检查配置")

            if self.usb_connected:
                self.print_textbox(f"USB 连接成功")
            elif self.blt_connected:
                self.print_textbox(f"蓝牙 连接成功")
            else:
                success = False
                self.print_textbox(f"USB 或 蓝牙 未连接，请检查配置")

            if success:
                self.print_textbox("初始化成功\n")
                self.checked = True
            else:
                self.print_textbox("初始化失败\n")


if __name__ == "__main__":
    app = App()
    app.mainloop()
