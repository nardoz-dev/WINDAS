import paho.mqtt.client as mqtt #provide function to interact with mosquitto rsmb
#from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
import json
import paho.mqtt.publish as publish

#AWS Paramater Init


#MQTT Broker Parameters
MQTT_SERVER = "localhost"
MQTT_PORT = 1886
MQTT_KEEP_ALIVE = 60

                    #change in according to my topic name board.
TOPIC_FROM_BOARD = "my_topic_out"


def mqtt_publish(mqtt_client, topic, message):
    mqtt_client.publish(topic, message)


# Function called to send messages to AWS, invoked when a msg is received from the MQTT Broker
# def aws_publish(aws_client, message):
def aws_publish(message):
    message_out = {}
    #handling messages to send to different AWS topic
    if(message[0] == "t"): #msg received is from temperature and humidity sensor
        message = message.split("h")
        message_out['temperature'] =  message[0][1:len(message[0])-1]+"."+ message[0][len(message[0])-1:]
        message_out['humidity'] = message[1][:len(message[1])-1]+"."+ message[1][len(message[1])-1:]
        messageJson = json.dumps(message_out)
        #aws_client.publish(TOPIC_TEMP_TO_AWS, messageJson, 1)

#MQTT Broker Interfaction

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    #subscription to topic in MQTT broker 
    client.subscribe(TOPIC_FROM_BOARD) 

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))

    message = str(msg.payload)[2:len(str(msg.payload))-1]

    #calling function to publish on AWS
    aws_publish(AWS_CLIENT, message)

print("Transparent Bridge Starting\n")
#AWS and MQTT Objects
AWS_CLIENT = None
MQTT_CLIENT = None

#AWS Start Init

#AWS Finish Init

#MQTT Start Init 
MQTT_CLIENT = mqtt.Client()
MQTT_CLIENT.on_connect = on_connect
MQTT_CLIENT.on_message = on_message
MQTT_CLIENT.connect(MQTT_SERVER, MQTT_PORT, MQTT_KEEP_ALIVE) #connection to broker (subscription is done in the on_connect function)
#add on connection fail?
MQTT_CLIENT.loop_forever()
#MQTT Finish Init