import json
import boto3

client = boto3.client('iot-data', region_name='your region name') #example eu-west-1

def lambda_handler(event, context):
    
    response = client.publish(
        topic='your_topic_name',
        qos=1,
        payload=event['body']
    )
    
    return {
        'statusCode': 200,
        'body': json.dumps('Published to topic')
    }