SELECT 
  topic(3) AS thing_name,
  timestamp() AS timestamp,
  substring(topic(3), 16, 22) AS serial_number,
  substring(topic(3), 0, 15) AS thing_type,
  state.reported.lid_open AS lid_open,
  state.reported.automatic_mode AS automatic_mode,
  state.reported.filling_state AS filling_state,
  state.reported.depth_cm AS depth_cm,
  
FROM '$aws/things/+/shadow/update'
WHERE substring(topic(3), 0, 15) = "smart_trash_can" AND 
  state.reported.lid_open <> NULL AND
  state.reported.automatic_mode <> NULL AND
  state.reported.depth_cm <> NULL AND
  state.reported.filling_state <> NULL