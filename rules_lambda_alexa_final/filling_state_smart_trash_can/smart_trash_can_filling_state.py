import json
import boto3

client = boto3.client('iot-data', region_name='us-east-1')

def lambda_handler(event, context):
    try:
        thing_name = event.get('thing_name')
        depth_cm = event['depth_cm']

        if thing_name is None or depth_cm is None:
            raise ValueError("Missing data")


        if depth_cm <= 3:
            filling_state = "full"
            led_color = "red"
        elif depth_cm <= 10:
            filling_state = "medium"
            led_color = "blue"
        else:
            filling_state = "empty"
            led_color = "green"

        payload = {
            "state": {
                "desired": {
                    "filling_state": filling_state,
                    "led_color": led_color
                }
            }
        }

        topic = f"$aws/things/{thing_name}/shadow/update"
        client.publish(topic=topic, qos=1, payload=json.dumps(payload))

        return { "statusCode": 200 }

    except Exception as e:
        print(f"Error: {e}")
        return { "statusCode": 500, "error": str(e) }
