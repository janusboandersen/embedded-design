from client import MqttClient
import Adafruit_BBIO.GPIO as GPIO

def setup():
    GPIO.setup("USR3", GPIO.OUT)


def on_message_callback(client, userdata, message):
    # The received MQTT message is binary and must be decoded
    msg = message.payload.decode("utf-8")
    print(msg)
    if msg == "ON":
        GPIO.output("USR3", GPIO.HIGH)
    elif msg == "OFF":
        GPIO.output("USR3", GPIO.LOW)
    else:
        print("No action taken!")


def handler_sub_main():
    # Define a subscriber that listens to all subjects
    subscriber = MqttClient("MessageHandler1", on_message_callback)

    # Only subscribe to relevant inbound messages
    subscriber.subscribe("mikroprojekt/inbound")

    print("Starting listening loop")
    subscriber.loop()

def handler_pub_main():
    #Sends off a single message and quits
    publisher = MqttClient("MessagePublisher", on_message_callback)
    publisher.publish("mikroprojekt/outbound", "Hello World!")

if __name__ == '__main__':
    setup()
    handler_sub_main()
