# Halloween
A Halloween Decoration app meant to control LEDs in a pumpkin.

This app is split into two parts.  One is an LED controller run on a PIC microcontroller.  Four of these go into pumpkins to form the display.  Serial communications are then run back to a raspberry Pi where the second part of the app live.  The Raspberry Pi runs a python script that plays The Monster Mash and commands the pumpkin LED controllers to flash in time to the music.  
