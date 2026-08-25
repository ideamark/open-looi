import os
import subprocess
import platform
from PIL import Image
import tkinter as tk
import customtkinter as ctk
import webbrowser
import time
import threading

from common import *
from connect import *
from audio import *
from gpt import *


blt = BluetoothClient()
ser = SerialClient()
llm = GPT()
speaker = Speaker(llm)


class App(ctk.CTk):
    def __init__(self):

        super().__init__()
        title = f"Desk-Emoji {VERSION}（Gemini API特别版）"

        # flags
        self.checked = False
        self.api_connected = False
        self.usb_connected = False
        self.blt_connected = False
        self.firmware = ""

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

        # ------------------------------------------------------------
        # Create Navigation Frame
        # ------------------------------------------------------------
        self.navigation_frame = ctk.CTkFrame(self, corner_radius=0)
        self.navigation_frame.grid(row=0, column=0, sticky="nsew")
        self.navigation_frame.grid_rowconfigure(7, weight=1)

        self.navigation_frame_label = ctk.CTkLabel(self.navigation_frame, text="  Desk-Emoji", image=self.logo_image,
                                                             compound="left", font=ctk.CTkFont(size=15, weight="bold"))
        self.navigation_frame_label.grid(row=0, column=0, padx=20, pady=20)

        self.chat_button = ctk.CTkButton(self.navigation_frame, corner_radius=0, height=40, border_spacing=10, text="对话",
                                         fg_color="transparent", text_color=("gray10", "gray90"), hover_color=("gray70", "gray30"),
                                         image=self.chat_image, anchor="w", command=self.chat_button_event)
        self.chat_button.grid(row=1, column=0, sticky="ew")

        self.act_button = ctk.CTkButton(self.navigation_frame, corner_radius=0, height=40, border_spacing=10, text="测试",
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
        self.help_button.grid(row=6, column=0, sticky="ew")

        self.appearance_mode_menu = ctk.CTkOptionMenu(self.navigation_frame, values=["System", "Light", "Dark"],
                                                      command=self.change_appearance_mode_event)
        self.appearance_mode_menu.grid(row=7, column=0, padx=20, pady=20, sticky="s")

        # ------------------------------------------------------------
        # Create Chat Frame
        # ------------------------------------------------------------
        self.chat_frame = ctk.CTkFrame(self, corner_radius=0, fg_color="transparent")
        self.chat_tabview = ctk.CTkTabview(self.chat_frame)
        self.chat_tabview.grid(row=0, column=0, padx=20, pady=20, sticky="nsew")
        self.chat_tabview.add("对话")
        self.text_chat_tab = self.chat_tabview.tab("对话")
        self.text_chat_tab.grid_columnconfigure(0, weight=1)

        # tab 1
        self.text_chat_tab.grid_columnconfigure(0, weight=1)
        self.text_chat_tab.grid_columnconfigure(1, weight=1)
        self.text_chat_tab.grid_columnconfigure(2, weight=1)

        self.textbox = ctk.CTkTextbox(self.text_chat_tab, height=280)
        self.textbox.grid(row=0, column=0, columnspan=3, padx=(20, 20), pady=(20, 10), sticky="nsew")

        self.chat_msg = ctk.CTkEntry(self.text_chat_tab)
        self.chat_msg.grid(row=1, column=0, columnspan=3, padx=20, pady=0, sticky="ew")
        self.chat_msg.bind("<Return>", self.chat_msg_event)

        self.send_button = ctk.CTkButton(self.text_chat_tab, text="发送", height=40,
                                         command=self.chat_msg_event)
        self.send_button.grid(row=2, column=2, padx=20, pady=10, sticky='e')

        self.speaker_switch = ctk.CTkSwitch(self.text_chat_tab, text="语音回复")
        self.speaker_switch.grid(row=2, column=0, padx=20, pady=10, sticky="nsew")
        self.speaker_switch.select()

        # voice combobox for TTS
        self.voice_combobox = ctk.CTkComboBox(self.text_chat_tab, values=[name for name, _ in gemini_voice_list])
        self.voice_combobox.grid(row=2, column=1, padx=20, pady=10, sticky="ew")
        self.voice_combobox.set('Kore')


        # ------------------------------------------------------------
        # Create Act Frame
        # ------------------------------------------------------------
        self.act_frame = ctk.CTkFrame(self, corner_radius=0, fg_color="transparent")
        self.act_frame.grid_columnconfigure(0, weight=1)

        self.act_tabview = ctk.CTkTabview(self.act_frame)
        self.act_tabview.grid(row=0, column=0, padx=20, pady=20, sticky="nsew")
        self.act_tabview.add("动作测试")
        self.act_tabview.tab("动作测试").grid_columnconfigure(0, weight=1)
        self.act_tabview.tab("动作测试").grid_columnconfigure(1, weight=1)
        self.act_tabview.add("工厂模式")
        self.act_tabview.tab("工厂模式").grid_columnconfigure(0, weight=1)
        self.act_tabview.tab("工厂模式").grid_columnconfigure(1, weight=1)

        # 动作测试 tab content
        for i, (button_name, button_command) in enumerate(eye_button_list):
            button = ctk.CTkButton(
                self.act_tabview.tab("动作测试"), 
                text=button_name,
                command=lambda cmd=button_command: self.send_actions_cmd(cmd)
            )
            button.grid(row=i, column=0, padx=10, pady=10, sticky='we')
        
        test_animation_button = ctk.CTkButton(self.act_tabview.tab("动作测试"), text="测试动画", command=lambda: self.send_actions_cmd("random_gif"))
        test_animation_button.grid(row=len(eye_button_list) + 1, column=0, padx=10, pady=10, sticky='we')

        for i, (button_name, button_command) in enumerate(head_button_list):
            button = ctk.CTkButton(
                self.act_tabview.tab("动作测试"), 
                text=button_name,
                command=lambda cmd=button_command: self.send_actions_cmd(cmd)
            )
            button.grid(row=i, column=1, padx=10, pady=10, sticky='we')

        # 工厂模式 tab content
        angle_set_label = ctk.CTkLabel(self.act_tabview.tab("工厂模式"), text="初始角度调整", font=ctk.CTkFont(size=16, weight="bold"))
        angle_set_label.grid(row=0, column=0, columnspan=4, padx=20, pady=20, sticky="w")
        
        self.act_tabview.tab("工厂模式").grid_columnconfigure(0, weight=1)
        self.act_tabview.tab("工厂模式").grid_columnconfigure(1, weight=1)
        self.act_tabview.tab("工厂模式").grid_columnconfigure(2, weight=1)
        self.act_tabview.tab("工厂模式").grid_columnconfigure(3, weight=1)
        
        self.head_left_textbox = ctk.CTkEntry(self.act_tabview.tab("工厂模式"), textvariable=ctk.StringVar(value="5"))
        self.head_left_textbox.grid(row=1, column=0, padx=10, pady=10, sticky="ew")
        
        self.head_left_button = ctk.CTkButton(self.act_tabview.tab("工厂模式"), text="左转", width=60, command=lambda: self.send_factory_cmd("head_left " + self.head_left_textbox.get()))
        self.head_left_button.grid(row=1, column=1, padx=10, pady=10, sticky="w")
        
        self.head_right_textbox = ctk.CTkEntry(self.act_tabview.tab("工厂模式"), textvariable=ctk.StringVar(value="5"))
        self.head_right_textbox.grid(row=1, column=2, padx=10, pady=10, sticky="ew")
        
        self.head_right_button = ctk.CTkButton(self.act_tabview.tab("工厂模式"), text="右转", width=60, command=lambda: self.send_factory_cmd("head_right " + self.head_right_textbox.get()))
        self.head_right_button.grid(row=1, column=3, padx=10, pady=10, sticky="w")

        self.head_up_textbox = ctk.CTkEntry(self.act_tabview.tab("工厂模式"), textvariable=ctk.StringVar(value="5"))
        self.head_up_textbox.grid(row=2, column=0, padx=10, pady=10, sticky="ew")
        
        self.head_up_button = ctk.CTkButton(self.act_tabview.tab("工厂模式"), text="上仰", width=60, command=lambda: self.send_factory_cmd("head_up " + self.head_up_textbox.get()))
        self.head_up_button.grid(row=2, column=1, padx=10, pady=10, sticky="w")
        
        self.head_down_textbox = ctk.CTkEntry(self.act_tabview.tab("工厂模式"), textvariable=ctk.StringVar(value="5"))
        self.head_down_textbox.grid(row=2, column=2, padx=10, pady=10, sticky="ew")
        
        self.head_down_button = ctk.CTkButton(self.act_tabview.tab("工厂模式"), text="下俯", width=60, command=lambda: self.send_factory_cmd("head_down " + self.head_down_textbox.get()))
        self.head_down_button.grid(row=2, column=3, padx=10, pady=10, sticky="w")
        
        other_set_label = ctk.CTkLabel(self.act_tabview.tab("工厂模式"), text="其他设置", font=ctk.CTkFont(size=16, weight="bold"))
        other_set_label.grid(row=3, column=0, columnspan=4, padx=20, pady=20, sticky="w")
        
        self.wifi_reset_button = ctk.CTkButton(self.act_tabview.tab("工厂模式"), text="重置 Wi-Fi", width=60, command=lambda: self.send_factory_cmd("reset_wifi"))
        self.wifi_reset_button.grid(row=4, column=0, padx=10, pady=10, sticky="w")

        self.wifi_reset_button = ctk.CTkButton(self.act_tabview.tab("工厂模式"), text="获取Mac地址", width=60, command=lambda: self.send_factory_cmd("mac_address"))
        self.wifi_reset_button.grid(row=4, column=1, padx=10, pady=10, sticky="w")

        self.wifi_reset_button = ctk.CTkButton(self.act_tabview.tab("工厂模式"), text="舵机归中", width=60, command=lambda: self.send_actions_cmd("head_center"))
        self.wifi_reset_button.grid(row=4, column=2, padx=10, pady=10, sticky="w")

        # Add a multiline textbox to display serial port return data in real time
        self.serial_output_textbox = ctk.CTkTextbox(self.act_tabview.tab("工厂模式"), width=400, height=120)
        self.serial_output_textbox.grid(row=5, column=0, columnspan=4, padx=10, pady=20, sticky="nsew")
        self.serial_output_textbox.configure(state="disabled")

        # Define a method to append serial port data to the textbox
        def append_serial_output(data):
            self.serial_output_textbox.configure(state="normal")
            self.serial_output_textbox.insert("end", data + "\n")
            self.serial_output_textbox.see("end")
            self.serial_output_textbox.configure(state="disabled")

        # Start a background thread to read serial port data in real time
        def serial_reader():
            while True:
                if hasattr(ser, "ser") and ser.ser and ser.ser.is_open and ser.ser.in_waiting:
                    try:
                        data = ser.ser.read(ser.ser.in_waiting)
                        if data:
                            text = data.decode("utf-8", errors="ignore").strip()
                            if text:
                                self.serial_output_textbox.after(0, append_serial_output, text)
                    except Exception as e:
                        pass
                time.sleep(0.1)

        threading.Thread(target=serial_reader, daemon=True).start()

        # ------------------------------------------------------------
        # Create Connect Frame
        # ------------------------------------------------------------
        self.connect_frame = ctk.CTkFrame(self, corner_radius=0, fg_color="transparent")
        self.connect_frame.grid_columnconfigure(0, weight=1)

        self.connect_tabview = ctk.CTkTabview(self.connect_frame)
        self.connect_tabview.grid(row=0, column=0, padx=20, pady=20, sticky="nsew")
        self.connect_tabview.add("USB")
        self.connect_tabview.tab("USB").grid_columnconfigure(0, weight=1)

        self.usb_combobox = ctk.CTkComboBox(self.connect_tabview.tab("USB"), values=[])
        self.usb_combobox.grid(row=0, column=0, columnspan=2, padx=20, pady=20, sticky="nsew")
        self.usb_combobox.set("")

        self.usb_refresh_button = ctk.CTkButton(self.connect_tabview.tab("USB"), text="刷新", command=self.usb_refresh_button_event)
        self.usb_refresh_button.grid(row=1, column=1, padx=20, pady=10)

        self.usb_connect_button = ctk.CTkButton(self.connect_tabview.tab("USB"), text="连接", command=self.usb_connect_button_event)
        self.usb_connect_button.grid(row=2, column=1, padx=20, pady=10)

        self.usb_flag_label = ctk.CTkLabel(self.connect_tabview.tab("USB"), text="")
        self.usb_flag_label.grid(row=2, column=0, padx=20, pady=10)

        # ------------------------------------------------------------
        # Create API Frame
        # ------------------------------------------------------------
        self.api_frame = ctk.CTkFrame(self, corner_radius=0, fg_color="transparent")
        self.api_frame.grid_columnconfigure(0, weight=1)

        self.api_tabview = ctk.CTkTabview(self.api_frame)
        self.api_tabview.grid(row=0, column=0, padx=20, pady=20, sticky="nsew")


        self.api_tabview.add("Gemini")
        self.openai_api_tab = self.api_tabview.tab("Gemini")
        self.openai_api_tab.grid_columnconfigure(0, weight=1)
        self.openai_api_tab.grid_columnconfigure(1, weight=6)

        self.openai_api_url_label = ctk.CTkLabel(self.openai_api_tab, text="API URL: ")
        self.openai_api_url_label.grid(row=0, column=0, padx=20, pady=20, sticky="w")
        self.openai_api_url_entry = ctk.CTkEntry(self.openai_api_tab)
        self.openai_api_url_entry.grid(row=0, column=1, padx=20, pady=20, sticky="nsew")

        self.openai_api_key_label = ctk.CTkLabel(self.openai_api_tab, text="API Key: ")
        self.openai_api_key_label.grid(row=1, column=0, padx=20, pady=20, sticky="w")
        self.openai_api_key_entry = ctk.CTkEntry(self.openai_api_tab)
        self.openai_api_key_entry.grid(row=1, column=1, padx=20, pady=20, sticky="nsew")

        self.openai_api_connect_flag_label = ctk.CTkLabel(self.openai_api_tab, text="")
        self.openai_api_connect_flag_label.grid(row=2, column=0, padx=20, pady=20)

        self.openai_api_connect_button = ctk.CTkButton(self.openai_api_tab, text="连接", command=self.openai_connect_button_event)
        self.openai_api_connect_button.grid(row=2, column=1, padx=20, pady=10)

        # ------------------------------------------------------------
        # Create Help Frame
        # ------------------------------------------------------------
        self.help_frame = ctk.CTkFrame(self, corner_radius=0, fg_color="transparent")
        self.help_frame.grid_columnconfigure(0, weight=1)

        help_text = f"""
{title} 桌面机器人

初次配置：
1. 连接机器人 -> 点击“串口” -> 点击“刷新” -> 找到合适的串口 -> “连接”
2. 点击“API” -> 配置 URL 网址和 Key-> ”连接“

使用说明：
“对话”界面用于对话互动，可以发文字也可以语音，可以开关扬声器、更改声音
“测试”界面用于测试表情和动作，点击不同按钮触发不同表情和动作


杭州易问科技版权所有 2024.11
联系邮箱：mark.yang@ewen.ltd

Gemini API适配由杭州GDG志愿者黄思源、晓乐乐共同完成 2025.10
联系邮箱：jiahaolan1@gmail.com
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
            url, key = llm.read_json()
            if not self.openai_api_url_entry.get():
                self.openai_api_url_entry.insert(0, url)
            if not self.openai_api_key_entry.get():
                self.openai_api_key_entry.insert(0, key)
        except Exception:
            pass

    def save_api_key(self):
        llm.write_json(self.openai_api_url_entry.get(), self.openai_api_key_entry.get())
        logger.info(f"Saved API Key to {llm.json_path}")

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

    def chat(self, question):
        try:
            if not question: return None, None
            logger.info(f"You: {question}")
            response = llm.chat(question)
            logger.info(f"Bot: {response}")
            return response
        except Exception as e:
            error(e, "Chat Failed!")
            return "OpenAI 连接失败！请检查 API 配置"

    def send_response(self, cmd):
        print(cmd)
        if blt.connected:
            blt.send(cmd)
        if ser.connected:
            ser.send(cmd)

    def send_factory_cmd(self, cmd):
        cmd = json.dumps({"factory": cmd})
        self.send_response(cmd)

    def send_actions_cmd(self, cmd):
        cmd = json.dumps({"actions": [cmd]})
        self.send_response(cmd)

    def chat_button_event(self):
        self.select_frame_by_name("chat")
        self.check_connections()

    def act_button_event(self):
        self.select_frame_by_name("act")

    def connect_button_event(self):
        self.select_frame_by_name("connect")
        self.usb_flag_label.configure(text="", fg_color="transparent")



    def usb_refresh_button_event(self):
        ports = ser.list_ports()
        if ports:
            self.usb_combobox.configure(values=ports)
            self.usb_combobox.set(ports[0])
        else:
            self.usb_flag_label.configure(text="无可用设备", text_color="red")

    def usb_connect_button_event(self):
        port = self.usb_combobox.get()
        if not port: return
        if blt.connected: blt.disconnect()
        if ser.connect(port):
            self.usb_connected = True
            self.usb_flag_label.configure(text="连接成功", text_color="green")
        else:
            self.usb_connected = False
            self.usb_flag_label.configure(text="连接失败", text_color="red")

    def api_button_event(self):
        self.select_frame_by_name("api")
        self.openai_api_connect_flag_label.configure(text="", fg_color="transparent")
        self.load_api_key()
    """
    def volcano_connect_button_event(self):
        self.save_api_key()
        if llm.connect():
            self.openai_api_connect_flag_label.configure(text="连接成功", text_color="green")
        else:
            self.openai_api_connect_flag_label.configure(text="连接失败", text_color="red")
    """
    def openai_connect_button_event(self):
        self.save_api_key()
        if llm.connect():
            self.openai_api_connect_flag_label.configure(text="连接成功", text_color="green")
        else:
            self.openai_api_connect_flag_label.configure(text="连接失败", text_color="red")


    def help_button_event(self):
        self.select_frame_by_name("help")

    def __chat_LLM(self, question):
        self.print_textbox(f"You:\t{question}")
        response = self.chat(question)
        answer = json.loads(response)["answer"]
        self.print_textbox(f"Bot:\t{answer}\n")
        if bool(self.speaker_switch.get()):
            voice = self.voice_combobox.get()
            speaker.say(text=answer, voice=voice)
        threading.Thread(target=self.send_response, args=(response,)).start()

    def chat_msg_event(self, event=None):
        question = self.chat_msg.get()
        if question:
            self.chat_msg.delete(0, tk.END)
            threading.Thread(target=self.__chat_LLM, args=(question,)).start()

    def check_connections(self):
        if not self.checked:
            if self.api_connected or llm.connect():
                self.print_textbox("API 连接成功")
            else:
                self.print_textbox("API 未连接")

            if self.usb_connected:
                self.print_textbox(f"USB 连接成功")
            elif self.blt_connected:
                self.print_textbox(f"蓝牙 连接成功")
            else:
                self.print_textbox(f"蓝牙 或 USB 未连接")
            self.print_textbox("\n")

    def import_firmware(self):
        file_path = tk.filedialog.askopenfilename(filetypes=[("Binary Files", "*.bin")])
        if file_path:
            self.firmware = file_path
            self.firmware_entry.delete(0, "end")
            self.firmware_entry.insert(0, self.firmware)

    def burn_firmware(self):
        if not self.firmware:
            self.terminal_textbox.insert("end", "请先导入固件文件\n")
            return

        esptool = "esptool.py"
        if platform.system() == 'Windows':
            esptool = "esptool"

        chip = "esp32"
        if "esp32s3" in self.firmware:
            chip = "esp32s3"

        selected_port = self.serial_combobox.get()

        command = [
            esptool,
            "--chip", chip,
            "--port", selected_port,
            "--baud", "460800",
            "--before", "default_reset",
            "--after", "hard_reset",
            "write_flash",
            "-z",
            "--flash_mode", "keep",
            "--flash_freq", "keep",
            "--flash_size", "keep",
            "0x0", self.firmware
        ]

        threading.Thread(target=self.run_command, args=(command,), daemon=True).start()

    def run_command(self, command):
        try:
            # Python 3.13 compatibility: ensure proper encoding and error handling
            process = subprocess.Popen(
                command, 
                stdout=subprocess.PIPE, 
                stderr=subprocess.PIPE, 
                text=True,
                encoding='utf-8',
                errors='replace'
            )

            # Read stdout and stderr simultaneously
            def read_output(pipe, is_stderr=False):
                try:
                    for line in iter(pipe.readline, ""):
                        if line:
                            self.terminal_textbox.insert("end", line)
                            self.terminal_textbox.see("end")
                            self.terminal_textbox.update_idletasks()
                except Exception as e:
                    logger.error(f"Error reading {'stderr' if is_stderr else 'stdout'}: {e}")

            # Start reading threads
            stdout_thread = threading.Thread(target=read_output, args=(process.stdout, False))
            stderr_thread = threading.Thread(target=read_output, args=(process.stderr, True))
            
            stdout_thread.daemon = True
            stderr_thread.daemon = True
            stdout_thread.start()
            stderr_thread.start()

            # Wait for process to complete
            process.wait()

            # Wait for threads to finish
            stdout_thread.join(timeout=1)
            stderr_thread.join(timeout=1)

            if process.returncode == 0:
                self.terminal_textbox.insert("end", "\n烧录完成！\n\n")
            else:
                self.terminal_textbox.insert("end", f"\n烧录失败，错误码：{process.returncode}\n")
            self.terminal_textbox.see("end")
            self.terminal_textbox.update_idletasks()

        except Exception as e:
            self.terminal_textbox.insert("end", f"\n运行出错：{e}\n")
            logger.error(f"Command execution error: {e}")

    def open_url(self):
            url = "https://gitee.com/ideamark/desk-emoji/releases"
            webbrowser.open(url)


if __name__ == "__main__":
    app = App()
    app.mainloop()
