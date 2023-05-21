import paho.mqtt.client as mqtt
from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
from AWSIoTPythonSDK.exception.AWSIoTExceptions import connectError
import json
import paho.mqtt.publish as publish

#AWS Paramater Init
AWS_HOST = "a3b5fvirnosinx-ats.iot.eu-west-1.amazonaws.com" #can be found as parameter '--endpoint' in the last line of the file start.sh

AWS_ROOT_CAP_PATH = "/home/nardo/Desktop/IoTDeviceKey/root-CA.crt"
AWS_PRIVATE_KEY_PATH = "/home/nardo/Desktop/IoTDeviceKey/windforme.private.key"
AWS_CERTIFICATE_PATH = "/home/nardo/Desktop/IoTDeviceKey/windforme.cert.pem"
AWS_PORT = 8883

# MQTT broker details
BROKER_ADDRESS = "localhost"
BROKER_PORT = 1886
# Topic to subscribe to
TOPIC = "my_topic"
TOPIC_TO_AWS = "topic_temp"
TOPIC_FROM_AWS = "topic_from_aws"

def myCallback(client, userdata, message):
    print("Received message on topic:", message.topic)
    print("Message:", message.payload)
    #publish.single(TOPIC,payload=message.payload, hostname="localhost",port="1886")
    mqttClient.publish(TOPIC,message.payload)

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    if rc == 0:
        print("Subscribe to topic : "+TOPIC)
        client.subscribe(TOPIC)
    else:
        print("Connection failed. Return code =", rc)

def on_message(client, userdata, msg):
    print("Received message:", msg.topic, msg.payload)
    
    message = {}
    message['temperature'] = str(msg.payload)[2:-1]
    messageJson = json.dumps(message)

    print("Json inviato  :   "+messageJson)
    # Publishing message
    # myAWSIoTMQTTClient.publish(TOPIC_TO_AWS, messageJson, 1)


print("AWS Client connection")
myAWSIoTMQTTClient = None
myAWSIoTMQTTClient = AWSIoTMQTTClient("windforme")
myAWSIoTMQTTClient.configureEndpoint(AWS_HOST, AWS_PORT)
myAWSIoTMQTTClient.configureCredentials(AWS_ROOT_CAP_PATH, AWS_PRIVATE_KEY_PATH, AWS_CERTIFICATE_PATH)
try:
    myAWSIoTMQTTClient.connect()
    print("Connected to AWS IoT MQTT broker")
except connectError as e:
    print("Connection to AWS IoT MQTT broker failed:", str(e))


print("MQTT Client connection")
# Create MQTT client instance
mqttClient = mqtt.Client()
# Set up callbacks
mqttClient.on_connect = on_connect
mqttClient.on_message = on_message

print("AWSClient Subscription")
myAWSIoTMQTTClient.subscribe(TOPIC_FROM_AWS,1,myCallback)


# Connect to MQTT broker
mqttClient.connect(BROKER_ADDRESS, BROKER_PORT , 60)
# Start the network loop to handle MQTT communication
mqttClient.loop_forever()







def on_aws_message(client, userdata, message):
    print("Received message from AWS IoT Core:", message.topic, message.payload)
    # Process the received message as needed
    # Example: Extract data from the payload
    payload = message.payload.decode('utf-8')
    data = json.loads(payload)
    temperature = data.get('temperature')
    # humidity = data.get('humidity')
    # Perform further processing or actions with the extracted data
    
    # Example: Publish a response back to the local MQTT broker
    response = {
        'status': 'success',
        'message': 'Message received and processed'
    }
    response_payload = json.dumps(response)
    mqttClient.publish('response_topic', response_payload, 1)