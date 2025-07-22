from tkinter import *
import serial

def openSerial(port, baudrate=9600, bytesize=serial.EIGHTBITS, parity=serial.PARITY_ODD, stopbits=serial.STOPBITS_ONE, timeout=1, xonxoff=False, rtscts=False, dsrdtr=False):
    ser = serial.Serial()

    ser.port = port
    ser.baudrate = baudrate
    ser.bytesize = bytesize
    ser.parity = parity
    ser.stopbits = stopbits
    ser.timeout = timeout
    ser.xonxoff = xonxoff
    ser.rtscts = rtscts
    ser.dsrdtr = dsrdtr

    ser.open()
    return ser


def writePort(ser, data):
    if isinstance(data, str):
        data = data.encode()
        print(data)
    ser.write(data)

# 1. 루트화면 (root window) 생성
tk = Tk()
tk.title("시리얼 버튼 테스트")
# 2. 시리얼 포트 열기
ser = openSerial('COM3')


def send_note(note_name):
    data = f'{note_name}\n'
    writePort(ser, data)
    print(f"전송: {data.strip()}")

songs = ['1', '2', '3']

for i, song in enumerate(songs):
    btn_song = Button(tk, text=song, width=10, command=lambda n=song: send_note(n))
    btn_song.grid(row=0, column=i+2, padx=20, pady=10)

notes = ['C', 'D', 'E', 'F', 'G', 'A', 'B']
for i, note in enumerate(notes):
    btn = Button(tk, text=note, width=10, command=lambda n=note: send_note(n))
    btn.grid(row=1, column=i, padx=20, pady=10)



tk.mainloop()

ser.close()
