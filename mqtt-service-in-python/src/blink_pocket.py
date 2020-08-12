import Adafruit_BBIO.GPIO as GPIO
import time

def setup():
    for i in ["USR0", "USR1", "USR2", "USR3"]:
        GPIO.setup(i, GPIO.OUT)

def pulse():
    setup()
    counter = 0

    while True:
        GPIO.output("USR3", GPIO.HIGH)
        print("High")
        time.sleep(0.5)
        GPIO.output("USR3", GPIO.LOW)
        print("Low")
        time.sleep(0.5)
        counter += 1

if __name__ == '__main__':
    pulse()
