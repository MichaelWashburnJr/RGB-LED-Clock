"""
Author: Michael D Washburn Jr
Send a serial message to the given com port containing the current time
in epoch.
Usage: `python sync.p Com#` (Where Com# is an integer of the com port)
"""
import time
import sys
import serial
import array

#The header leads the epoch time and is used to distinguish a time sync message
#from other serial messages
HEADER = "T"

def print_usage():
	"""
	Print the usage for this script
	"""
	print("Usage: python sync.py C")
	print(" Arguments:")
	print("   - C: An integer representing the COM port to transmit to.")

def to_byte_array(string):
	"""
	Convert a string to a byte array
	"""
	return array.array('B', string)

def main():
	#check the number of args
	if(len(sys.argv) != 2):
		print_usage()
		return 0
	#parse the com port
	com = 0
	try:
		com = int(sys.argv[1])
	except:
		print_usage()
		return 0
	
	#Arguments are good, send the message
	ser = serial.Serial("COM%d" % com)	#connect to the serial port
	#wait until the arduino is ready
	ser.readline()
	epoch = int(time.time())			#get the epoch time
	message = HEADER + str(epoch)		#format the message
	ser.write(message.encode())			#write the message (as byte array)
	ser.close()							#always close when done

if __name__ == '__main__':
	main()
