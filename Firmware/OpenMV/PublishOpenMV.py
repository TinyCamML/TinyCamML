# Untitled - By: ebgoldstein - Wed Feb 14 2024
## Adding edits by combining senior design teams and EBG's - Liz Farquhar 6/18/2024
# EBG clean up 7/23/24
# LF turned off the external pin interrupt 8/1/2024

import sensor, image, time, utime, pyb, tf, machine, gc, os, uselect
from pyb import UART, Pin, ExtInt
from machine import LED

# Set the threshold for detecting dark images
brightness_threshold = 10  # Adjust this value as needed

#load the TF lite micro file
net = tf.load('MNv2Flood_cat_Sept2024.tflite', load_to_fb=True)
labels = ['Flood', 'NoFlood']

#make directory to save images
#if not "images" in os.listdir():
#    os.mkdir("images")  # Make a temp directory

uart = UART(1, 9600) # UART1, adjust baudrate as needed

def callback(line):
    pass

def get_today_folder():
    ts = int(curr_time) #time from boron
    tsdt = list(time.localtime(ts)) #convert from tuple to list
    tsdt[0] = (int(tsdt[0])-30) #subtract 30 years bc micropython starts timestamps at 2000
    date_str = f"{tsdt[0]}-{tsdt[1]}-{tsdt[2]}" #extract date
    folder_path = f"{date_str}"
    if not folder_path in os.listdir():
        os.mkdir(folder_path)
    return folder_path

while(True):
    #Reinitialize the sensor after sleep
    sensor.reset() # Initialize the camera sensor.
    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.QVGA)
    sensor.skip_frames(time = 2000)

    #TAKE PIC
    img = sensor.snapshot()

    # Calculate the average brightness of the image
    avg_brightness = img.get_statistics().mean()

    # Check if the image is too dark. if not, classify
    if avg_brightness > brightness_threshold:
        TF_objs = net.classify(img)
        Flood = TF_objs[0].output()[0]
        NoFlood = TF_objs[0].output()[1]
        if Flood > NoFlood:
            #print('Flood')
            uart.write('Flood.')
            pyb.delay(1500)
            floodstate = "Flood"
        else:
            #print('No Flood')
            uart.write('No Flood.')
            pyb.delay(1500)
            floodstate = "NoFlood"
    else:
        #print('Too Dark')
        uart.write('Too Dark.')
        pyb.delay(1500)
        floodstate = "TooDark"

    print(floodstate)

    #COMMS via UART
    pyb.delay(1000);

    poll = uselect.poll()
    poll.register(uart, uselect.POLLIN)
    poll.poll()
    curr_time = uart.read().decode('utf-8')
    file_name = curr_time + floodstate

    today_folder = get_today_folder()

    with open("./DataLog.txt", 'a') as file:
        file.write(curr_time + "," + floodstate + "," + str(gc.mem_alloc()) + "\n")

    img.save("./" + today_folder + "/" + file_name + ".jpg")

    # run garbarge collection to prevent memory growth
    #del img
    gc.collect()

    # Begin power down ------------------------------------
    # Shutdown the sensor (pulls PWDN high).
    sensor.shutdown(True)
    # Enter lightsleep Mode
    #machine.lightsleep()
