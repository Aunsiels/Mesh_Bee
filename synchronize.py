"""" This script synchronize a MeshBee connected with a wire to the RP"""

import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BCM)
GPIO.setup(23, GPIO.OUT, initial=GPIO.LOW)
GPIO.setup(18, GPIO.OUT)
p = GPIO.PWM(18, 0.5)

p.start(50.0)

temp = raw_input("Reset the meshbee and press a touch")

GPIO.output(23, 1)

temp = raw_input("Press a key to stop")

p.stop()
GPIO.output(23, 0)
GPIO.cleanup()

