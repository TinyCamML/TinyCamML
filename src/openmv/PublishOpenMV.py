# Untitled - By: ebgoldstein - Wed Feb 14 2024
import sensor, image, time, utime, pyb, tf, machine, gc
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

net = tf.load('MNv2Flood_cat (3).tflite', load_to_fb=True)
labels = ['Flood', 'NoFlood']

def callback(line):
    pass


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

    else:
        print('No Flood')
        uart.write('No Flood')

    #Debugging code
    #led.on()
    #time.sleep_ms(500)
    #led.off()
    #time.sleep_ms(500)

    #debugging to see memory growth
    # print(gc.mem_alloc())
    
    # run garbarge collection to prevent memory growth
    gc.collect()

    machine.sleep()