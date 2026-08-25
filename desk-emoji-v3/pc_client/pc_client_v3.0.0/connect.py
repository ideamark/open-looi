import inquirer
import platform
import serial
import serial.tools.list_ports
import asyncio
import threading
import time
from bleak import BleakClient, BleakScanner

from common import *


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

    def disconnect(self):
        if self.ser and self.ser.is_open:
            try:
                self.ser.close()
                logger.info(f"Disconnected from {self.port}.")
            except Exception as e:
                error(e, "Failed to disconnect the serial port.")
        else:
            logger.info("Serial port is already closed or was not connected.")
        self.connected = False
        self.ser = None

    def read(self, port):
        while True:
            if port.in_waiting:
                data = port.read(port.in_waiting)
                result = data.decode('utf-8', errors='ignore')
                logger.debug("\nReceived:", result.strip('\n').strip())

    def send(self, msg):
        try:
            if not msg.endswith('\n'):
                msg += '\n'
            encode_msg = msg.encode('utf-8')
            self.ser.write(encode_msg)
            self.ser.flush()
            logger.debug(f"Sent: {msg}")

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

    async def disconnect(self):
        if self.client and self.client.is_connected:
            await self.client.disconnect()
            self.connected = False
            logger.info(f"Disconnected from {self.device_name}")

    async def send(self, data):
        if self.client and self.client.is_connected:
            try:
                await self.client.write_gatt_char(self.characteristic_uuid, data.encode('utf-8'))
                logger.info(f"Sent to {self.device_name}: {data}")
            except Exception as e:
                logger.error(f"Failed to send data: {e}")
        else:
            logger.info("Not connected to any device.")


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
        # Python 3.13 compatibility: use asyncio.run_coroutine_threadsafe with proper error handling
        try:
            future = asyncio.run_coroutine_threadsafe(super().list_devices(), self.loop)
            return future.result()
        except Exception as e:
            logger.error(f"Error in list_devices: {e}")
            return []

    def connect(self, device_address):
        try:
            future = asyncio.run_coroutine_threadsafe(super().connect(device_address), self.loop)
            return future.result()
        except Exception as e:
            logger.error(f"Error in connect: {e}")
            return False

    def disconnect(self):
        try:
            future = asyncio.run_coroutine_threadsafe(super().disconnect(), self.loop)
            future.result()
        except Exception as e:
            logger.error(f"Error in disconnect: {e}")

    def send(self, data):
        try:
            future = asyncio.run_coroutine_threadsafe(super().send(data), self.loop)
            future.result()
        except Exception as e:
            logger.error(f"Error in send: {e}")
