import threading
import serial
import uinput

port = '/dev/ttyACM0'
baud = 115200

serial_port = serial.Serial(port, baud, timeout=None)

def read_from_port(ser):
    keys = [
      uinput.KEY_UP,
      uinput.KEY_DOWN,
      uinput.KEY_RIGHT,
      uinput.KEY_LEFT,
      uinput.KEY_X,
      uinput.KEY_C,
      uinput.KEY_G,
      uinput.KEY_Z,
      uinput.KEY_S,
      uinput.KEY_J,
      uinput.KEY_A,
      uinput.KEY_D,
      uinput.KEY_ENTER,
      uinput.KEY_LEFTSHIFT,
    ]
    with uinput.Device(keys) as device:
        while True:
          data = ser.readline()
          if len(data) > 13:
            for i, k in enumerate(keys):
              device.emit(k, 1 if data[i] == 49 else 0)


thread = threading.Thread(target=read_from_port, args=(serial_port,))
thread.start()
