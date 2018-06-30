# test_pub.py
# run : python test_sub.py ServerIP Topic
import sys
import paho.mqtt.client as mqtt

ServerIP = "localhost"
ServerPort = 1883
SubscribeTopic = sys.argv[2]
Message = sys.argv[2]

mqtt = mqtt.Client("test_pub")
mqtt.connect(sys.argv[1], ServerPort)
mqtt.publish(SubscribeTopic, Message)
# with open("queue.txt", "a") as text_file:
#     text_file.write(SubscribeTopic)
