# ESPHome - SegLCD

Integration of SegLCDLib to ESPHome.

First target:
- `seglcd_temphum`
- controller: `PCF85176`
- display: `SegLCD_PCF85176_TempHumidity`

Status:
- base PoC structure
- external component layout
- first fixed-purpose component for temperature/humidity display
- custom ESPHome I2C transport for SegLCDLib

Example:

```yaml
external_components:
  - source:
      type: local
      path: .

i2c:
  id: bus_a
  sda: GPIO21
  scl: GPIO22
  scan: true

sensor:
  - platform: bme280_i2c
    temperature:
      id: room_temperature
    humidity:
      id: room_humidity
    address: 0x76

  - platform: template
    id: display_battery_level
    lambda: return 3;

  - platform: template
    id: display_signal_level
    lambda: return 4;

seglcd_temphum:
  id: my_lcd
  i2c_id: bus_a
  address: 0x38
  subaddress: 0
  temperature: room_temperature
  humidity: room_humidity
  battery_level: display_battery_level
  signal_level: display_signal_level
  show_celsius: true
  show_percent: true
  update_interval: 10s

number:
  - platform: template
    name: LCD Temperature
    min_value: -40
    max_value: 99.9
    step: 0.1
    optimistic: true
    set_action:
      - lambda: |-
          id(my_lcd).set_temperature_value(x);

  - platform: template
    name: LCD Humidity
    min_value: 0
    max_value: 100
    step: 1
    optimistic: true
    set_action:
      - lambda: |-
          id(my_lcd).set_humidity_value(x);

  - platform: template
    name: LCD Battery
    min_value: 0
    max_value: 4
    step: 1
    optimistic: true
    set_action:
      - lambda: |-
          id(my_lcd).set_battery_level_value((uint8_t) x);

  - platform: template
    name: LCD Signal
    min_value: 0
    max_value: 4
    step: 1
    optimistic: true
    set_action:
      - lambda: |-
          id(my_lcd).set_signal_level_value((uint8_t) x);

switch:
  - platform: template
    name: LCD Celsius Flag
    optimistic: true
    turn_on_action:
      - lambda: |-
          id(my_lcd).set_show_celsius(true);
    turn_off_action:
      - lambda: |-
          id(my_lcd).set_show_celsius(false);

  - platform: template
    name: LCD Percent Flag
    optimistic: true
    turn_on_action:
      - lambda: |-
          id(my_lcd).set_show_percent(true);
    turn_off_action:
      - lambda: |-
          id(my_lcd).set_show_percent(false);
```

Notes:
- this PoC uses ESPHome `i2c_id`
- intended as the first external-components prototype before upstreaming
