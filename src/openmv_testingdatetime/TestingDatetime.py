# Untitled - By: ebgoldstein - Wed Feb 14 2024
## Adding edits by combining senior design teams and evan's - Liz Farquhar 6/18/2024
import sensor, image, time, utime, pyb, tf, machine, gc, os, uselect
from pyb import UART, Pin, ExtInt
from machine import LED

uart = machine.UART(1, baudrate=9600)
uart = UART(1, 9600) # UART1, adjust baudrate as needed

led = LED("LED_BLUE")

#rtc = pyb.RTC()
sensor.reset() # Initialize the camera sensor.
sensor.set_pixformat(sensor.RGB565) # or sensor.GRAYSCALE
sensor.set_framesize(sensor.QVGA)

sensor.skip_frames(time = 2000)


#function to save images to sd card
def saveimage(file_name, image):
    if not "images" in os.listdir():
        os.mkdir("images")  # Make a temp directory

    image.save('images/' + file_name, quality=90)


while(True):

    img = sensor.snapshot()

    uart.write('Testsend')

    pyb.delay(2000)

    poll = uselect.poll()
    poll.register(uart, uselect.POLLIN)
    poll.poll()

    curr_time = uart.read().decode('utf-8')
    print(curr_time)

    f = open("DataLog.txt", "a")
    f.write(curr_time+",NoFlood\n")
    f.close()analogRead(A1) * ((3.3/4096)*((2000000+1300000)/2000000))*((2000000+1300000)/2000000);

    #if curr_time == None:
     #   time = "None"
    #else:
     #   time = curr_time
    #wait until uart.read() != None:
     #   curr_time = "None"
    #else: select.POLLIN()
    #uart.wait_for()
    #curr_time = str(uart.readline())

    file_name = curr_time +"_FLOOD"

    print(file_name)
    saveimage(file_name, img)

    gc.collect()
