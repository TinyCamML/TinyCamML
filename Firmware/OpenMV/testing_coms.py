# Testing Coms - By: efarquhar
import sensor, image, time, utime, pyb, tf, machine, gc, os, uselect
from pyb import UART, Pin, ExtInt
from machine import LED


net = tf.load('MNv2Flood_cat_CG.tflite', load_to_fb=True)
labels = ['Flood', 'NoFlood']

print("It does run this part everytime")

def callback(line):
    pass

#make directory to save images
if not "images" in os.listdir():
    os.mkdir("images")  # Make a temp directory

pin = Pin("P7", Pin.IN, Pin.PULL_UP)
ext = ExtInt(pin, ExtInt.IRQ_FALLING, Pin.PULL_UP, callback)

while(True):
    #Reinitialize the sensor after sleep
    uart = pyb.UART(1, 9600, timeout_char=1000) # UART1, adjust baudrate as needed
    sensor.reset() # Initialize the camera sensor.
    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.QVGA)
    sensor.skip_frames(time = 2000)
    pyb.delay(100)

    img = sensor.snapshot()
    TF_objs = net.classify(img)
    Flood = TF_objs[0].output()[0]
    NoFlood = TF_objs[0].output()[1]

    if Flood > NoFlood:
        print('Flood')
        uart.write('Flood')
        floodstate = "Flood"

    else:
        print('NoFlood')
        uart.write('NoFlood')
        floodstate = "NoFlood"

    uart.deinit()
    pyb.delay(1000)
    uart = pyb.UART(1, 9600, timeout_char=1000)
    pyb.delay(100)

    poll = uselect.poll()
    poll.register(uart, uselect.POLLIN)
    poll.poll()
    curr_time = uart.read().decode('utf-8')

    file_name = curr_time + floodstate

    with open("./DataLog.txt", 'a') as file:
        file.write(curr_time + "," + floodstate + "," + str(gc.mem_alloc()) + "\n")

    #print(file_name)
    img.save("./images/" + file_name + ".jpg")

    # run garbarge collection to prevent memory growth
    del img
    gc.collect()

    # Begin power down ------------------------------------
    # Shutdown the sensor (pulls PWDN high).
    sensor.shutdown(True)
    # Enter lightsleep Mode
    machine.lightsleep()
