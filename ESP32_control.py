import serial
import time
import argparse

global Read_response_now

parser = argparse.ArgumentParser()
parser.add_argument('--ESP32_comport', type=int, required=True)
parser.add_argument('--ESP32_command', type=str, required=True)

args = parser.parse_args()

ESP32_comport = args.ESP32_comport

#Comm_port, Relay_state = input("Comm Port: "), input("On or Off relay: ")

# Define the serial port and baud rate.
# Ensure the 'COM#' corresponds to what was seen in the Windows Device Manager
ser = serial.Serial("COM" + str(ESP32_comport), 115200)

def ESP32_command_function():

    #Send command
    Count_send = ser.write(str(args.ESP32_command+'\r\n').encode('ascii'))
    ser.flush()
    #print(Count_send)

    while ser.in_waiting==0:

        if(ser.in_waiting > 0):
            #Read data out of the buffer until a carraige return / new line is found
            break
        
    serialString = ser.read_until('\n',4)

    print(serialString.decode('ascii'))

ESP32_command_function()

