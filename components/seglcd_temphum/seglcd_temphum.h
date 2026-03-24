#pragma once

#include "esphome/components/i2c/i2c.h"
#include "esphome/components/number/number.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "SegLCD_PCF85176_TempHum.h"
#include "segtransport_i2c_esphome.h"

namespace esphome {
namespace seglcd_temphum {

class SegLCDTempHumComponent : public PollingComponent {
 public:
  SegLCDTempHumComponent(uint8_t address, uint8_t subaddress);

  void setup() override;
  void update() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void set_i2c_bus(i2c::I2CBus *bus) { this->bus_.set_i2c_bus(bus); }

  void set_temperature_sensor(sensor::Sensor *sensor) { this->temperature_sensor_ = sensor; }
  void set_humidity_sensor(sensor::Sensor *sensor) { this->humidity_sensor_ = sensor; }
  void set_battery_level_sensor(sensor::Sensor *sensor) { this->battery_level_sensor_ = sensor; }
  void set_signal_level_sensor(sensor::Sensor *sensor) { this->signal_level_sensor_ = sensor; }
  void set_show_celsius(bool show_celsius);
  void set_show_percent(bool show_percent);
  void set_temperature_value(float value);
  void set_humidity_value(float value);
  void set_battery_level_value(uint8_t value);
  void set_signal_level_value(uint8_t value);
  void clear_temperature_value();
  void clear_humidity_value();
  void clear_battery_level_value();
  void clear_signal_level_value();
  void set_temperature_number(number::Number *number) { this->temperature_number_ = number; }
  void set_humidity_number(number::Number *number) { this->humidity_number_ = number; }
  void set_battery_level_number(number::Number *number) { this->battery_level_number_ = number; }
  void set_signal_level_number(number::Number *number) { this->signal_level_number_ = number; }
  void set_celsius_switch(switch_::Switch *sw) { this->celsius_switch_ = sw; }
  void set_percent_switch(switch_::Switch *sw) { this->percent_switch_ = sw; }

 protected:
  void render_();
  void apply_labels_();
  void write_temperature_();
  void write_humidity_();
  void write_battery_level_();
  void write_signal_level_();
  int clamp_level_(sensor::Sensor *source) const;
  int clamp_level_(int value) const;
  bool get_temperature_value_(float &value) const;
  bool get_humidity_value_(float &value) const;
  bool get_battery_level_value_(uint8_t &value) const;
  bool get_signal_level_value_(uint8_t &value) const;
  void format_temperature_(char *buffer, size_t buffer_size, float value) const;
  void format_humidity_(char *buffer, size_t buffer_size, float value) const;

  sensor::Sensor *temperature_sensor_{nullptr};
  sensor::Sensor *humidity_sensor_{nullptr};
  sensor::Sensor *battery_level_sensor_{nullptr};
  sensor::Sensor *signal_level_sensor_{nullptr};
  number::Number *temperature_number_{nullptr};
  number::Number *humidity_number_{nullptr};
  number::Number *battery_level_number_{nullptr};
  number::Number *signal_level_number_{nullptr};
  switch_::Switch *celsius_switch_{nullptr};
  switch_::Switch *percent_switch_{nullptr};

  bool show_celsius_{true};
  bool show_percent_{true};
  bool has_manual_temperature_{false};
  bool has_manual_humidity_{false};
  bool has_manual_battery_level_{false};
  bool has_manual_signal_level_{false};
  float manual_temperature_{0.0f};
  float manual_humidity_{0.0f};
  uint8_t manual_battery_level_{0};
  uint8_t manual_signal_level_{0};

  uint8_t address_;
  uint8_t subaddress_;
  SegTransportI2CESPHome bus_;
  SegLCD_PCF85176_TempHumidity lcd_;
};

class SegLCDTempHumTemperatureNumber : public number::Number, public Parented<SegLCDTempHumComponent> {
 protected:
  void control(float value) override;
};

class SegLCDTempHumHumidityNumber : public number::Number, public Parented<SegLCDTempHumComponent> {
 protected:
  void control(float value) override;
};

class SegLCDTempHumBatteryLevelNumber : public number::Number, public Parented<SegLCDTempHumComponent> {
 protected:
  void control(float value) override;
};

class SegLCDTempHumSignalLevelNumber : public number::Number, public Parented<SegLCDTempHumComponent> {
 protected:
  void control(float value) override;
};

class SegLCDTempHumCelsiusSwitch : public switch_::Switch, public Parented<SegLCDTempHumComponent> {
 protected:
  void write_state(bool state) override;
};

class SegLCDTempHumPercentSwitch : public switch_::Switch, public Parented<SegLCDTempHumComponent> {
 protected:
  void write_state(bool state) override;
};

}  // namespace seglcd_temphum
}  // namespace esphome
