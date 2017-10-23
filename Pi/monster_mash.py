import pygame, time, serial
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
while (pygame.mixer.music.get_pos() < 10000):
	
	# testing...
	time.sleep(1)

	# Read and configure the next command from the script
	command = fo.readline()
	bytes = command.split(",")
	time_to_send = float(bytes[0])
	msg[0] = byte(bytes[1])
	msg[1] = byte(bytes[2])
	msg[2] = byte(bytes[3])
	msg[3] = byte(bytes[4])
	msg[4] = msg[0] + msg[1] + msg[2] + msg[3]
	
	# Sleep while we are not ready for the command
	while (pygame.mixer.music.get_pos() < time_to_send):
		time.sleep(1)
		
	# Send new command over serial (twice to be sure to be sure...)
	serialPort.write(msg)


# Close our script file

