# ESPHome SegLCD

ESPHome display component for [SegLCDLib](https://github.com/petrkr/SegLCDLib).

This repository is a staging component for ESPHome integration. It currently targets
the Arduino framework.

## Installation

For real-world use, add this repository as a GitHub external component:

```yaml
external_components:
  - source: github://petrkr/esphome-seglcd
    components: [seglcd]
```

For local development against a checked-out copy:

```yaml
external_components:
  - source:
      type: local
      path: ../components
    components: [seglcd]
```

## Supported models

| Model option | SegLCDLib class | Build macro |
| --- | --- | --- |
| `pcf85134_xygax_seg_i2c` | `SegLCD_PCF85134_XygaxSegI2C` | `SEGLCD_ENABLE_PCF85134_XYGAX_SEG_I2C` |
| `pcf85176_4dr821b` | `SegLCD_PCF85176_4DR821B` | `SEGLCD_ENABLE_PCF85176_4DR821B` |

The component passes SegLCDLib build flags itself:

- `SEGLCD_DISABLE_ALL_LCDS`
- `SEGLCD_DISABLE_ARDUINO_TRANSPORT`
- the selected LCD model macro

## Basic example

In `lambda:`, the display entity is exposed as `lcd` and is typed as the concrete
SegLCDLib model class for the configured `model:` (not a generic wrapper), so
model-specific methods are available directly, with no casting required.

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
    model: pcf85134_xygax_seg_i2c
    library_source: file:///home/user/git/SegLCDLib
    library_version: null
    address: 0x38
    subaddress: 0
    update_interval: never
    lambda: |-
      lcd.write("1122");
```

Use `lcd.write(const char *)` to write text — it is SegLCDLib's framework-independent
API and works identically under Arduino and non-Arduino targets (e.g. ESP-IDF), unlike
the Arduino-only `print()` inherited from `Print`. This component currently requires
the Arduino framework itself, but using `write()` keeps lambdas portable for when
ESP-IDF support lands.

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
    model: pcf85134_xygax_seg_i2c
    library_source: file:///home/user/git/SegLCDLib
    library_version: null
    update_interval: never
    lambda: |-
      lcd.write(id(lcd_text).state.c_str());
```

## 4DR821B symbols

`pcf85176_4dr821b` has two extra segment symbols (arrow, tilda). They can be set
directly from a lambda, calling the SegLCDLib model class API on `lcd`:

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
      lcd.write("+12:34");
      lcd.setSymbol(SegLCD_PCF85176_4DR821B::SYMBOL_ARROW, true);
      lcd.setSymbol(SegLCD_PCF85176_4DR821B::SYMBOL_TILDA, true);
```

Alternatively, `tilda:` and `arrow:` are available as declarative config keys for
this model, applied on every `update()` after the lambda writer runs. Each accepts
either a plain boolean or a `!lambda` returning one:

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
    tilda: true
    arrow: !lambda "return id(my_switch).state;"
    lambda: |-
      lcd.write("+12:34");
```

`tilda:`/`arrow:` and `lambda:` can be combined freely; there is no conflict
validation between them, so combining a declarative key with a lambda that sets
the same symbol will just apply both in sequence.

See `examples/` for complete ESPHome configurations.

## License

See [LICENSE](LICENSE).

## Contributing

Issues and pull requests are welcome on this repository's GitHub page.
