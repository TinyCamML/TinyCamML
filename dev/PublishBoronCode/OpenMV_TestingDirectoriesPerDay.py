# Untitled - By: efarquh - Mon Oct 28 2024

import sensor, image, time, utime, pyb, tf, machine, gc, os, uselect
from pyb import UART, Pin, ExtInt
from machine import LED

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 2000)

#make directory to save images
#if not "images" in os.listdir():
#    os.mkdir("images")  # Make a directory

def get_today_folder():
    #ts = int(curr_time) #time from boron
    #tsdt = list(time.localtime(ts)) #convert from tuple to list
    #tsdt[0] = (int(tsdt[0])-30) #subtract 30 years bc micropython starts timestamps at 2000
    #date_str = f"{tsdt[0]}-{tsdt[1]}-{tsdt[2]}" #extract date
    date_str = utime.strftime("%Y-%m-%d")
    folder_path = f"{date_str}"
    if not folder_path in os.listdir():
        os.mkdir(folder_path)
    return folder_path

while(True):
    img = sensor.snapshot()
    pyb.delay(2000)
    curr_time = "1729870206"

    file_name = f"{curr_time}.jpg"
    print(file_name)

    pyb.delay(500)
    with open("./DataLog.txt", 'a') as file:
        log_entry = f"{curr_time},{gc.mem_alloc()}\n"
        file.write(log_entry)

    today_folder = get_today_folder()
    print(today_folder)
    pyb.delay(500)
    img.save("./" + today_folder + "/" + file_name + ".jpg")

    gc.collect()
