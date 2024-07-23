# Untitled - By: ebgoldstein - Wed Feb 14 2024
## Adding edits by combining senior design teams and EBG's - Liz Farquhar 6/18/2024
# EBG clean up 7/23/24

import sensor, image, time, utime, pyb, tf, machine, gc, os, uselect
from pyb import UART, Pin, ExtInt
from machine import LED

#load the TF lite micro file
net = tf.load('MNv2Flood_cat_CG.tflite', load_to_fb=True)
labels = ['Flood', 'NoFlood']

#make directory to save images
if not "images" in os.listdir():
    os.mkdir("images")  # Make a temp directory

#ExtInt setup
uart = UART(1, 9600) # UART1, adjust baudrate as needed

def callback(line):
    pass

pin = Pin("P7", Pin.IN, Pin.PULL_UP)
ext = ExtInt(pin, ExtInt.IRQ_FALLING, Pin.PULL_UP, callback)



while(True):
    #Reinitialize the sensor after sleep
    sensor.reset() # Initialize the camera sensor.
    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.QVGA)
    sensor.skip_frames(time = 2000)
    
    #TAKE AND CLASSIFY PIC
    img = sensor.snapshot()
    TF_objs = net.classify(img)
    Flood = TF_objs[0].output()[0]
    NoFlood = TF_objs[0].output()[1]

    if Flood > NoFlood:
        floodstate = "Flood"
    else:
        floodstate = "NoFlood"
    print(floodstate)

    #COMMS via UART
    uart.write(floodstate)
    pyb.delay(1000);
    poll = uselect.poll()
    poll.register(uart, uselect.POLLIN)
    poll.poll()
    curr_time = uart.read().decode('utf-8')
    
    #SAVE LOG AND PIC
    filename = str(curr_time) + '_' + floodstate

    with open("./DataLog.txt", 'a') as file:
        file.write(curr_time + "," + floodstate + "," + file_name + "," + str(gc.mem_alloc()) + "\n")

    img.save("./images/" + file_name + ".jpg")

    # run garbarge collection to prevent memory growth
    gc.collect()

    # Begin power down ------------------------------------
    # Shutdown the sensor (pulls PWDN high).
    sensor.shutdown(True)
    # Enter lightsleep Mode
    machine.lightsleep()
