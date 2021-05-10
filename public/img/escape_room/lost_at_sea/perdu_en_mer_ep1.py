# CONFIGURATIONS

name_ans = "test"
password_ans = "test"

gas_code = "1/2/3/4/5/6"












import sys
import os
import time
import signal
import pygame
from serial import Serial
import sys, termios
# os.system("paplay ./denied.wav &")


connected = False
DEVICE = "/dev/ttyACM0"
BAUD = "9600"

while (connected == False):
	try:
		ser = Serial(DEVICE, BAUD)
		ser.timeout = 2
		ser.write("G/" + gas_code + "\n")
		connected = True
	except:
		print "------------ YA UNE ERREUR, FAIT PAS PARTIE DU JEU ------------"
		connected = False
		DEVICE = raw_input("Device: ")
		BAUD = raw_input("Baud: ")



def handler(signal, frame):
	print ""
	# sys.exit(1)

signal.signal(signal.SIGINT, handler)


pygame.init()
denied_sound = pygame.mixer.Sound("./denied.wav")
granted_sound = pygame.mixer.Sound("./granted.wav")
gas_error = pygame.mixer.Sound("./gas_error.wav")

os.system('clear')

name = ""
password = ""

step_one = False
data = ""
while step_one == False:
	partial_data = ""
	try:
		partial_data += ser.readline()
	except: pass

	if partial_data != "":
		data += partial_data
		if (data.find("\n") != -1):
			if data.rstrip() == "THEY GOT THE GAS":
				step_one = True
			if data.rstrip() == "GAS ERROR":
				gas_error.play()
			data = ""

os.system('clear')

granted_sound.play()
# POUR EFFACER LE BUFFER D'INPUT ---- LINUX/WINDOWS
try:
	import sys, termios
	termios.tcflush(sys.stdin, termios.TCIOFLUSH)
except ImportError:
	import msvcrt
	while msvcrt.kbhit():
		msvcrt.getch()
# ------------------------------------
while (name != name_ans or password != password_ans):
	name = raw_input("User name: ")
	password = raw_input("Password: ")
	if (name != name_ans or password != password_ans):
		print "Access denied\n"
		denied_sound.play()
	if (name == "admin" and password == "admin"):
		print "Yeah, good try...\n"
		denied_sound.play()

pygame.mixer.music.load("./startmotor.mp3")
pygame.mixer.music.play()
granted_sound.play()

print "-----------Acces granted-----------"



time.sleep(1)
os.system('clear')
print "Connecting..."
time.sleep(1)
print "Connecting..."
time.sleep(1)
print "Connecting..."
time.sleep(1)

for i in range (0, 40):
	os.system('clear')
	if i%4 == 0:
		print 'Restarting systems ---'
	elif i%4 == 1:
		print 'Restarting systems /'
	elif i%4 == 2:
		print 'Restarting systems |'
	elif i%4 == 3:
		print 'Restarting systems \\'
	time.sleep(0.2)

print "---------------------------------------------------------------"
print "Process ID:   2296"
print "Parent Process: emergency_protocol.bin"
print "Command Line: cmd"
print "Start Time: 1985"
print "Visible: False"
print "---------------------------------------------------------------"
time.sleep(2)
print "Connecting to : http://main.system.com:8856"
print "-----------------------------------------------------"
print "[10.02.1984 19:06:00] - Accepted - - 168 KH"
print "[10.02.1984 19:06:02] - Accepted - - 168 KH"
print "[10.02.1984 19:06:10] - Accepted - - 168 KH"
print "[10.02.1984 19:06:20] - Accepted - - 168 KH"
print "[10.02.1984 19:10:05] - Accepted - - 168 KH"
print "[10.02.1984 20:06:02] - Accepted - - 168 KH"
print "[10.02.1984 21:06:88] - Refused  - - 168 KH"
print "[10.02.1984 22:07:90] - Accepted - - 168 KH"
print "[10.02.1984 22:07:96] - Accepted - - 168 KH"
print "[10.02.1984 23:00:00] - Accepted - - 168 KH"
print "[10.02.1984 28:58:23] - Accepted - - 168 KH"
print "[10.02.1984 28:06:00] - Accepted - - 168 KH"
print "[10.02.1984 28:06:58] - Accepted - - 168 KH"
print "[10.02.1984 30:28:22] - Accepted - - 168 KH"
print "[10.02.1984 19:06:00] - Accepted - - 168 KH"
print "[10.02.1984 19:06:00] - Accepted - - 168 KH"
print "[10.02.1984 19:06:00] - Accepted - - 168 KH"
print "[10.02.1984 19:06:00] - Accepted - - 168 KH"
print "-----------------------------------------------------"


print "[ERROR] System reboot finished with errors, manual overide needed."
print "See you in episode 2!"
ser.write("display_end_code\n")

while (name != "exit"):
	name = raw_input("")
