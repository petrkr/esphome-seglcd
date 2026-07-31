# ESPHome SegLCD

ESPHome display component for [SegLCDLib](https://github.com/petrkr/SegLCDLib).

This repository is a staging component for ESPHome integration. It currently targets
the Arduino framework.

## Supported models

| Model option | SegLCDLib class | Build macro |
| --- | --- | --- |
| `pcf85134_xygax` | `SegLCD_PCF85134_Xygax` | `SEGLCD_ENABLE_PCF85134_XYGAX` |
| `pcf85176_4dr821b` | `SegLCD_PCF85176_4DR821B` | `SEGLCD_ENABLE_PCF85176_4DR821B` |

The component passes SegLCDLib build flags itself:

- `SEGLCD_DISABLE_ALL_LCDS`
- `SEGLCD_DISABLE_ARDUINO_TRANSPORT`
- the selected LCD model macro

## Basic example

```yaml
external_components:
  - source:
      type: local
      path: ../components
    components: [seglcd]

esp32:
  board: esp32-s3-devkitc-1
  framework:
    type: arduino

i2c:
  id: bus_a
  sda: GPIO1
  scl: GPIO2
  scan: true

display:
  - platform: seglcd
    id: lcd
    i2c_id: bus_a
    model: pcf85134_xygax
    library_source: file:///home/user/git/SegLCDLib
    library_version: null
    address: 0x38
    subaddress: 0
    update_interval: never
    lambda: |-
      it.print("1122");
```

## Updating from web entities

Use ESPHome entities to hold state and call `component.update` on change.

```yaml
text:
  - platform: template
    name: LCD Text
    id: lcd_text
    optimistic: true
    min_length: 0
    max_length: 8
    mode: text
    restore_value: true
    on_value:
      then:
        - component.update: lcd

web_server:
  port: 80
  version: 3

display:
  - platform: seglcd
    id: lcd
    i2c_id: bus_a
    model: pcf85134_xygax
    library_source: file:///home/user/git/SegLCDLib
    library_version: null
    update_interval: never
    lambda: |-
      it.print(id(lcd_text).state.c_str());
```

## 4DR821B symbols

`pcf85176_4dr821b` exposes model-specific helpers in the display lambda:

```yaml
display:
  - platform: seglcd
    id: lcd
    i2c_id: bus_a
    model: pcf85176_4dr821b
    library_source: file:///home/user/git/SegLCDLib
    library_version: null
    address: 0x39
    subaddress: 0x03
    update_interval: never
    lambda: |-
      it.print("+12:34");
      it.set_arrow(true);
      it.set_tilde(true);
```

See `examples/` for complete ESPHome configurations.
