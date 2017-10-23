import pygame, time, serial, math
from pygame.locals import *

# Load music file into pygame
pygame.init()
pygame.mixer.music.load("Monster_Mash.ogg")

# Setup serial port for use
serialPort = serial.Serial("/dev/ttyAMA0", 19200)
msg = bytearray(5)

# Open script for reading
fo = open("commands.csv", "r")

# Start the music
pygame.mixer.music.play()

# Loop until the music is over
while (True):

	# Read and configure the next command from the script
	command = fo.readline()
	
	# Check for the end of commands
	if (command == "")
		break
	
	# Split input into the component bytes
	command_bytes = command.split(",")
	time_to_send = float(command_bytes[0])
	
	# Skip the header
	if ( math.isnan(time_to_send) ):
		continue
	
	# Create our command bytes to send
	msg[0] = bytes(command_bytes[1])			# Command / ID
	msg[1] = bytes(command_bytes[2])			# Payload 1
	msg[2] = bytes(command_bytes[3])			# Payload 2
	msg[3] = bytes(command_bytes[4])			# Payload 3
	msg[4] = msg[0] + msg[1] + msg[2] + msg[3]	# Checksum
	
	# Wait until the next command needs to be sent
	while (pygame.mixer.music.get_pos() < time_to_send):
		pass		# Busy wait...
		
	# Send new command over serial (twice to be sure to be sure...)
	serialPort.write(msg)

# Fade out
pygame.mixer.music.fadeout(1500)
	
# Close our script file
fo.close()

# Play it again!
# todo...