import pygame, time, serial, math, codecs, RPi.GPIO as GPIO
from pygame.locals import *

# Load music file into pygame
pygame.init()
pygame.mixer.music.load("Monster_Mash.ogg")

# Open script for writing
fo = open("output.txt", "w")

# Setup GPIO input
GPIO.setmode(GPIO.BOARD)
GPIO.setup(7, GPIO.IN, pull_up_down=GPIO.PUD_UP)

# Start the music
pygame.mixer.music.play()

current_state = 0
last_state = 0
wait_time = 0
while(pygame.mixer.music.get_busy()):
	
	current_state = GPIO.input(7)
	if ( current_state != last_state ):
		current_time = pygame.mixer.music.get_pos()
		print(current_time, ", ", current_state, "\n")
		fo.write(str(current_time))
		fo.write(",")
		fo.write(str(current_state))
		fo.write("\n")
		wait_time = current_time + 0.001
		last_state = current_state
	
	# Debounce
	while (pygame.mixer.music.get_pos() < wait_time and pygame.mixer.music.get_busy()):
		pass
		
	
	