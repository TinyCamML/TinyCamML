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

net = tf.load('MNv2Flood_cat_CG.tflite', load_to_fb=True)
labels = ['Flood', 'NoFlood']

def callback(line):
    pass

#function to save images to sd card
def saveimage(file_name, image):
    if not "images" in os.listdir():
        os.mkdir("images")  # Make a temp directory

    image.save('images/' + file_name, quality=90)


pin = Pin("P7", Pin.IN, Pin.PULL_UP)
ext = ExtInt(pin, ExtInt.IRQ_FALLING, Pin.PULL_UP, callback)

while(True):

    img = sensor.snapshot()

    TF_objs = net.classify(img)

    Flood = TF_objs[0].output()[0]
    NoFlood = TF_objs[0].output()[1]

    if Flood > NoFlood:
        print('Flood')
        uart.write('Flood')
        pyb.delay(1000);
        poll = uselect.poll()
        poll.register(uart, uselect.POLLIN)
        poll.poll()
        curr_time = uart.read().decode('utf-8')
        file_name = curr_time +"_FLOOD"
        f = open("DataLog.txt", "a")
        f.write(curr_time+",Flood\n")
        f.close()

    else:
        print('No Flood')
        uart.write('No Flood')
        pyb.delay(1000);
        poll = uselect.poll()
        poll.register(uart, uselect.POLLIN)
        poll.poll()
        curr_time = uart.read().decode('utf-8')
        file_name = curr_time +"_NOFLOOD"
        f = open("DataLog.txt", "a")
        f.write(curr_time+",NoFlood\n")
        f.close()

    #print(file_name)
    saveimage(file_name, img)


    #Debugging code
    #led.on()
    #time.sleep_ms(500)
    #led.off()
    #time.sleep_ms(500)

    #debugging to see memory growth
    # print(gc.mem_alloc())

    # run garbarge collection to prevent memory growth
    gc.collect()

    # Begin power down ------------------------------------
    # Enable sensor softsleep
    #sensor.sleep(True)
    # Shutdown the sensor (pulls PWDN high).
    #sensor.shutdown(True)
    # Enable RTC interrupts every 6 minutes (in microseconds)
    # camera will RESET after wakeup from deepsleep Mode.
    #rtc.wakeup(334 * 1000)
    # Enter Deepsleep Mode (i.e. the OpenMV Cam effectively turns itself off except for the RTC).
    machine.sleep() #not sure if deepsleep can be awoken via toggle pin
