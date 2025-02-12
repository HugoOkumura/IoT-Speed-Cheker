import serial
import api_send as s

def read_from_serial():
    ser = serial.Serial('/dev/ttyUSB0', 115200)

    while True:
        try:
            if ser.in_waiting > 0:
                data = ser.readline().decode('utf-8').split()
                # print(len(data))
                
                print(f"Received: {data}")
                s.send_to_api(list(data))
        except Exception as e:
            print(f"ERR: {e}")
        