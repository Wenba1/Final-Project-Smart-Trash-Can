import boto3
from decimal import Decimal
import json
import time

dynamodb = boto3.resource('dynamodb')
table = dynamodb.Table('db_smart_trash_can')

def lambda_handler(event, context):
    print("Raw Event:", event)

    try:
        thing_name = event.get('thing_name')
        if not thing_name:
            print("Missing thing_name. Skipping insert.")
            return {'statusCode': 400, 'message': 'Missing thing_name'}

        timestamp = event.get('timestamp', int(time.time()))
        
        # Primero verificamos si existe depth_cm
        if 'depth_cm' not in event:
            print("depth_cm not present. Skipping insert.")
            return {'statusCode': 400, 'message': 'Missing depth_cm'}

        raw_depth = event.get('depth_cm')
        try:
            depth = round(float(raw_depth), 1)
        except (TypeError, ValueError):
            print(f"Invalid depth_cm type: {raw_depth}")
            return {'statusCode': 400, 'message': 'Invalid depth_cm type'}

        if depth <= 0 or depth > 15:
            print(f"Invalid depth_cm value: {depth}. Skipping insert.")
            return {'statusCode': 400, 'message': f'Invalid depth_cm value: {depth}'}

        depth_decimal = Decimal(str(depth))

        lid_open = bool(event.get('lid_open', False))
        filling_state = str(event.get('filling_state', 'unknown'))
        automatic_mode = bool(event.get('automatic_mode', False))
        serial_number = str(event.get('serial_number', 'unknown'))
        thing_type = str(event.get('thing_type', 'unknown'))

        last_item = get_last_item(thing_name)
        print("Last item:", last_item)

        should_save = (
            not last_item or
            abs(depth_decimal - last_item.get('depth_cm', Decimal('0'))) >= Decimal('1.0') or
            filling_state != last_item.get('filling_state') or
            lid_open != last_item.get('lid_open') or
            automatic_mode != last_item.get('automatic_mode')
        )

        if should_save:
            item = {
                'thing_name': thing_name,
                'thing_type': thing_type,
                'timestamp': timestamp,
                'depth_cm': depth_decimal,
                'lid_open': lid_open,
                'filling_state': filling_state,
                'automatic_mode': automatic_mode,
                'serial_number': serial_number
            }

            print("Saving to DynamoDB:", item)
            table.put_item(Item=item)
            return {'statusCode': 200, 'message': 'Inserted'}
        else:
            print("No significant change. Skipping insert.")
            return {'statusCode': 200, 'message': 'Skipped'}

    except Exception as e:
        print(f"Error: {e}")
        return {'statusCode': 500, 'message': str(e)}

def get_last_item(thing_name):
    try:
        response = table.query(
            KeyConditionExpression=boto3.dynamodb.conditions.Key('thing_name').eq(thing_name),
            ScanIndexForward=False,
            Limit=1
        )
        items = response.get('Items')
        return items[0] if items else None
    except Exception as e:
        print(f"Error getting last item: {e}")
        return None
