import serial # import module for serial communcation
import pyautogui # import module for keyboard/mouse control
import io # import module to to use universal newline mode
import time

alive = True

# open Serial Port 5
ser = serial.Serial()
ser.baudrate = 115200 # baud rate
ser.port = 'COM6'# set port
ser.timeout = 0.1 # set timeout in seconds
ser.open() # open serial communication
print('COM 6 Open: ', ser.is_open)
sio = io.TextIOWrapper(io.BufferedRWPair(ser, ser))
ser.flush() # wait until all data is written

pyautogui.FAILSAFE = False # set failsafe for manual exit
width, height = pyautogui.size() # set size of screen
print('Press Ctrl-C to quit.')
pyautogui.moveTo(960, 540); # set mouse to middle

ss = 0
gear = 0
center = 0

flap_up_prev = 0
flap_down_prev = 0
pause_prev = 0
landing_gear_prev = 0
throttle_prev = 0
rudder_prev = 0
xn_prev = 0
yn_prev = 0
brake_left_prev = 0
brake_right_prev = 0
count = 0

while (alive): # kill switch not asserted
    line = sio.readline()
    data = line.split()
    #print(line) 
    print(data) 
    # flaps up                                      
    if data != []:
        if int(data[0])==1:
            pyautogui.keyDown('down')
            pyautogui.keyUp('down')
        elif int(data[0])==2:
            pyautogui.keyDown('up')
            pyautogui.keyUp('up')

        if int(data[1])==1:
            pyautogui.keyDown('shift')
            pyautogui.keyDown('shiftright')
            pyautogui.keyDown('right')
            pyautogui.keyUp('right')
            pyautogui.keyUp('shift')
            pyautogui.keyUp('shiftright') 
        elif int(data[1])==2:   
            pyautogui.keyDown('shift')
            pyautogui.keyDown('shiftright')
            pyautogui.keyDown('left')
            pyautogui.keyUp('left')
            pyautogui.keyUp('shift') 
            pyautogui.keyUp('shiftright')

        if int(data[2])!=ss:
            pyautogui.press('space')
            ss = int(data[2]) 
	  
	  if int(data[4])==1:
		pyautogui.keyDown('shift')
	  else:
		pyautogui.keyUp('shift')
	
        if int(data[3])!=gear:
            pyautogui.press('g')
            gear = int(data[3])

        if int(data[5])!=center:
            pyautogui.press('c')
            center = int(data[5])

        if int(data[6])==2:
            pyautogui.keyDown('pageup')
            pyautogui.keyUp('pageup')
        elif int(data[6])==1:
            pyautogui.keyDown('pagedown')
            pyautogui.keyUp('pagedown')

        if int(data[7])==2:
            pyautogui.press(']')
        elif int(data[7])==1:
            pyautogui.press('[')

        if int(data[8])==1:
            pyautogui.keyDown('.')
        else:
            pyautogui.keyUp('.')

        if int(data[9])==1:
            pyautogui.keyDown(',')
        else:
            pyautogui.keyUp(',')
    
    
