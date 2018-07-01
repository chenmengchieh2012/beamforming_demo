# test_sub.py
# run : python test_sub.py ServerIp Topic
import sys
import fcntl
import paho.mqtt.client as mqtt

ServerIP = "localhost"

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe(sys.argv[2])

def on_message(client, userdata, msg):
    print("Topic : " + msg.topic + "\n\tmessage : " + str(msg.payload))
    fd = open('isSwitched.txt', 'a')
    fcntl.lockf(fd, fcntl.LOCK_EX | fcntl.LOCK_NB)
    fd.write(str(msg.payload))
    fd.close()

fd = open('isSwitched.txt', 'w')
fd.close()
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(sys.argv[1], 1883, 60)
client.loop_forever()
