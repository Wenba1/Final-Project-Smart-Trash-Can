SELECT
  topic(3) AS thing_name, 
  substring(topic(3), 16, 22) AS serial_number,
  substring(topic(3), 0, 15) AS thing_type, 
  state.reported.depth_cm AS depth_cm,
  state.reported.led_color AS led_color
FROM 
  '$aws/things/+/shadow/update'
WHERE 
  substring(topic(3), 0, 15) = "smart_trash_can" AND 
  state.reported.depth_cm > -1