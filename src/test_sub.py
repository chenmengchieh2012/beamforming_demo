# test_sub.py
# run : python test_sub.py ServerIp Topic
import sys
import paho.mqtt.client as mqtt

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe(sys.argv[2])

def on_message(client, userdata, msg):
    print("Topic : " + msg.topic + "\n\tmessage : " + str(msg.payload))
    with open("isSwitched.txt", "w") as text_file:
        text_file.write("1")

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(sys.argv[1], 1883, 10)
client.loop_forever()
