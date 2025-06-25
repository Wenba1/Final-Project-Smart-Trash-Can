SELECT 
  topic(3) AS thing_name,
  substring(topic(3), 16, 22) AS serial_number,
  substring(topic(3), 0, 15) AS thing_type,
  state.reported.motion_detected AS motion_detected,
  state.reported.automatic_mode AS automatic_mode,
FROM '$aws/things/+/shadow/update'
WHERE 
  state.reported.motion_detected = true AND 
  state.reported.automatic_mode = true

