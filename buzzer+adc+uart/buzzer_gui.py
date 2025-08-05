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

# GUI root 열기
tk = Tk()
tk.title("시리얼 버튼 테스트")

#  시리얼 포트 열기
ser = openSerial('COM3')


def send_note(note_name):
    data = f'{note_name}'
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


playBtn = Button(tk, text = "Play", width=10, command=lambda : send_note('p'))
playBtn.grid(row=2, column=3, padx=20, pady=10)
stopBtn = Button(tk, text="Stop", width=10, command=lambda : send_note('s'))
stopBtn.grid(row=2, column=2, padx=20, pady=10)

volume = 100

# def setVolume(n):
#     if n == '0':
#         
#         if (volume - 10) >= 0:
#             volume-=10
#     elif n == '9':
#         send_note('9')
#         if (volume + 10) <= 100:
#             volume += 10



volumeDownBtn = Button(tk, text= "🔉", width=10, command=lambda: send_note('0'))
volumeUpBtn = Button(tk, text= "🔊", width=10, command=lambda: send_note('9'))
#volumeOffBtn = Button(tk, text= "🔇", width=10, command=lambda: send_note('.'))
volumeDownBtn.grid(row=3, column=3, padx=20, pady=10)
volumeUpBtn.grid(row=3, column=4, padx=20, pady=10)
#volumeOffBtn.grid(row=3, column=2, padx=20, pady=10)

#volumeText = Label(tk, text={volume})
#volumeText.grid(row=3, column=4, padx=20, pady=10)

ADCBtn = Button(tk, text= "volume according to lux", width=40, command=lambda: send_note('8'))
ADCBtn.grid(row=3, column=8, padx=20, pady=10)


tk.mainloop()

ser.close()
