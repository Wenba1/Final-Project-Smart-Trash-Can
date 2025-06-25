import json
import boto3

client = boto3.client('iot-data', region_name='us-east-1')

def lambda_handler(event, context):
    print("Event received:", json.dumps(event))

    try:
        thing_name = event.get("thing_name")
        if not thing_name:
            raise ValueError("Missing thing_name")

        response_open = client.update_thing_shadow(
            thingName=thing_name,
            payload=json.dumps({
                "state": {
                    "desired": {
                        "lid_open": True
                    }
                }
            })
        )
        print("Lid opened:", response_open)

        client.update_thing_shadow(
            thingName=thing_name,
            payload=json.dumps({
                "state": {
                    "desired": {
                        "motion_detected": False
                    }
                }
            })
        )

        return {
            'statusCode': 200,
            'body': json.dumps("Lid opened and motion reset.")
        }

    except Exception as e:
        print(f"Error in Lambda: {e}")
        return {
            'statusCode': 500,
            'body': json.dumps(f"Error: {str(e)}")
        }
