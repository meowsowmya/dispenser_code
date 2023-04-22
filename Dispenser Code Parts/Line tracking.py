import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BOARD)

LOWER_DIST = 9 #cm
UPPER_DIST = 15 #cm
TRIG_PIN = 24   #18 for general, 24 for gpio.board
ECHO_PIN = 8   #24 for general, 8 for gpio.board

GPIO.setup(TRIG_PIN,GPIO.OUT)
GPIO.setup(ECHO_PIN,GPIO.IN)
GPIO.output(TRIG_PIN, False)
time.sleep(1) #1 second delay

def get_distance():
    GPIO.output(TRIG_PIN, True)
    time.sleep(0.00001)
    GPIO.output(TRIG_PIN, False)

    while GPIO.input(ECHO_PIN)==0:
        pulse_start = time.time()

    while GPIO.input(ECHO_PIN)==1:
          pulse_end = time.time()

    pulse_duration = pulse_end - pulse_start

    distance = pulse_duration * 17150 #speed of sound is 34300 cm/s
    distance = round(distance+1.15, 2) #some correctionfactor, eneds to b etested
    return distance
 


def wait_for_can_mode():
    while(True):
        distance = get_distance()
        if (distance < LOWER_DIST) or (distance > UPPER_DIST):
            #can is present 
            return 1 #returns 1 if can is present
    return 0 #returns zero if no can
