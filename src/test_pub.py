# test_pub.py
# run : python test_sub.py Topic
import sys
import paho.mqtt.client as mqtt

ServerIP = "localhost"
ServerPort = 1883
SubscribeTopic = sys.argv[1]
Message = "KW is faaaaaaat."

mqtt = mqtt.Client("test_pub")
mqtt.connect(ServerIP, ServerPort)
mqtt.publish(SubscribeTopic, Message)
with open("isSwitched.txt", "w") as text_file:
    text_file.write("0")
