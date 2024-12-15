import serial

ser = serial.Serial('/dev/ttyUSB2', 500000)
# ser = serial.Serial()

def read_from_serial():
    try:
        data = ser.readline().decode('utf-8').strip()
        return data
    except Exception as e:
        return None