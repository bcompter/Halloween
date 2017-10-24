import pygame, time, serial, math, codecs
from pygame.locals import *

# Test for a float
def isfloat(value):
	try:
		float(value)
		return True
	except:
		return False

# Convert integer to a byte
def int2bytes(i):
    hex_value = '{0:x}'.format(i)
    # make length of hex_value a multiple of two
    hex_value = '0' * (len(hex_value) % 2) + hex_value
    return codecs.decode(hex_value, 'hex_codec')

# Load music file into pygame
pygame.init()
pygame.mixer.music.load("Monster_Mash.ogg")

# Setup serial port for use
serialPort = serial.Serial("/dev/ttyAMA0", 19200)
msg = bytearray(5)

# Open script for reading
#fo = open("commands.csv", "r")
fo = open("testing.csv", "r")

# Start the music
pygame.mixer.music.play()

# Loop until the music is over
while (True):

	# Read and configure the next command from the script
	command = fo.readline()
	
	print("Command is: %s", command)
	
	# Check for the end of commands
	if (command == ""):
		break
	
	# Split input into the component bytes
	command_bytes = command.split(",")
	
	# Skip the header
	if ( isfloat(command_bytes[0]) != True ):
		continue

	# Create our command bytes to send
	time_to_send = float(command_bytes[0])
	
	msg[0] = int2bytes(int(command_bytes[1]))	# Command / ID
	msg[1] = int2bytes(int(command_bytes[2]))	# Payload 1
	msg[2] = int2bytes(int(command_bytes[3]))	# Payload 2
	msg[3] = int2bytes(int(command_bytes[4]))	# Payload 3
	checksum = msg[0] + msg[1] + msg[2] + msg[3]
	msg[4] = checksum % 256
	
	# Wait until the next command needs to be sent
	while (pygame.mixer.music.get_pos() < time_to_send*1000):
		pass		# Busy wait...
		
	# Send new command over serial (twice to be sure to be sure...)
	serialPort.write(msg)

# Fade out
pygame.mixer.music.fadeout(1500)
	
# Close our script file
fo.close()

# Play it again!
# todo...