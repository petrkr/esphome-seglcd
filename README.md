# ESPHome - SegLCD

Integration of SegLCDLib to ESPHome

First target:
- `seglcd_temphum`
- controller: `PCF85176`
- display: `SegLCD_PCF85176_TempHumidity`

Status:
- base PoC structure
- external component layout
- first fixed-purpose component for temperature/humidity display

Example:

```yaml
external_components:
  - source:
      type: local
      path: .

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
  sda: GPIO21
  scl: GPIO22
  address: 0x38
  subaddress: 0
  temperature: room_temperature
  humidity: room_humidity
  battery_level: display_battery_level
  signal_level: display_signal_level
  show_celsius: true
  show_percent: true
  update_interval: 10s
```

Notes:
- this PoC uses Arduino `Wire` directly and does not use ESPHome `i2c:` for the display bus
- intended as the first external-components prototype before upstreaming
