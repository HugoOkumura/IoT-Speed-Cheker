import serial

ser = serial.Serial('/dev/ttyUCB0', 500000)

def read_from_serial():
    try:
        data = ser.readline().decode('utf-8').strip()
        
        return data
    except Exception as e:
        return None