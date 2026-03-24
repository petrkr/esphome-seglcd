#pragma once

#include "esphome/components/i2c/i2c.h"
#include "esphome/components/number/number.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/text/text.h"
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "SegLCD_PCF85176_TempHum.h"
#include "../seglcd_transport/segtransport_i2c_esphome.h"

#include <string>

namespace esphome {
namespace seglcd_temphum {

template<typename T> struct ManualOverride {
  bool active{false};
  T value{};
  void set(T v) { active = true; value = v; }
  void clear() { active = false; }
};

struct DisplayField {
  ManualOverride<float> number;
  ManualOverride<std::string> text;
  sensor::Sensor *sensor{nullptr};
  number::Number *number_entity{nullptr};
  text::Text *text_entity{nullptr};
  uint8_t row;
  uint8_t width;  // digit positions (4 for temp, 3 for hum)
};

class SegLCDTempHumComponent : public PollingComponent {
 public:
  SegLCDTempHumComponent(uint8_t address, uint8_t subaddress);

  void setup() override;
  void update() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void set_i2c_bus(i2c::I2CBus *bus) { this->bus_.set_i2c_bus(bus); }

  // Sensor inputs
  void set_temperature_sensor(sensor::Sensor *sensor) { this->temp_field_.sensor = sensor; }
  void set_humidity_sensor(sensor::Sensor *sensor) { this->hum_field_.sensor = sensor; }
  void set_battery_level_sensor(sensor::Sensor *sensor) { this->battery_level_sensor_ = sensor; }
  void set_signal_level_sensor(sensor::Sensor *sensor) { this->signal_level_sensor_ = sensor; }

  // Entity setters
  void set_temperature_number(number::Number *n) { this->temp_field_.number_entity = n; }
  void set_humidity_number(number::Number *n) { this->hum_field_.number_entity = n; }
  void set_temperature_text(text::Text *t) { this->temp_field_.text_entity = t; }
  void set_humidity_text(text::Text *t) { this->hum_field_.text_entity = t; }
  void set_battery_level_number(number::Number *n) { this->battery_level_number_ = n; }
  void set_signal_level_number(number::Number *n) { this->signal_level_number_ = n; }
  void set_celsius_switch(switch_::Switch *sw) { this->celsius_switch_ = sw; }
  void set_percent_switch(switch_::Switch *sw) { this->percent_switch_ = sw; }

  // Label flags
  void set_show_celsius(bool v);
  void set_show_percent(bool v);

  // Manual value setters (called from entities)
  void set_field_number(DisplayField &field, float value);
  void set_field_text(DisplayField &field, const std::string &value);
  void set_battery_level_value(uint8_t value);
  void set_signal_level_value(uint8_t value);

  DisplayField &temp_field() { return this->temp_field_; }
  DisplayField &hum_field() { return this->hum_field_; }

 protected:
  void render_();
  void apply_labels_();
  void write_display_field_(DisplayField &field);
  void write_number_right_aligned_(uint8_t row, uint8_t width, const char *formatted);
  void write_text_(uint8_t row, const std::string &text);
  void write_battery_level_();
  void write_signal_level_();
  int clamp_level_(sensor::Sensor *source) const;
  int clamp_level_(int value) const;
  void format_temperature_(char *buffer, size_t buffer_size, float value) const;
  void format_humidity_(char *buffer, size_t buffer_size, float value) const;

  DisplayField temp_field_{};
  DisplayField hum_field_{};

  sensor::Sensor *battery_level_sensor_{nullptr};
  sensor::Sensor *signal_level_sensor_{nullptr};
  number::Number *battery_level_number_{nullptr};
  number::Number *signal_level_number_{nullptr};
  switch_::Switch *celsius_switch_{nullptr};
  switch_::Switch *percent_switch_{nullptr};

  ManualOverride<uint8_t> manual_battery_;
  ManualOverride<uint8_t> manual_signal_;

  bool show_celsius_{true};
  bool show_percent_{true};

  uint8_t address_;
  uint8_t subaddress_;
  seglcd_transport::SegTransportI2CESPHome bus_;
  SegLCD_PCF85176_TempHumidity lcd_;
};

// --- Number entities ---

class SegLCDTempHumTemperatureNumber : public number::Number, public Parented<SegLCDTempHumComponent> {
 protected:
  void control(float value) override { this->parent_->set_field_number(this->parent_->temp_field(), value); }
};

class SegLCDTempHumHumidityNumber : public number::Number, public Parented<SegLCDTempHumComponent> {
 protected:
  void control(float value) override { this->parent_->set_field_number(this->parent_->hum_field(), value); }
};

class SegLCDTempHumBatteryLevelNumber : public number::Number, public Parented<SegLCDTempHumComponent> {
 protected:
  void control(float value) override {
    this->parent_->set_battery_level_value(static_cast<uint8_t>(std::lround(value)));
  }
};

class SegLCDTempHumSignalLevelNumber : public number::Number, public Parented<SegLCDTempHumComponent> {
 protected:
  void control(float value) override {
    this->parent_->set_signal_level_value(static_cast<uint8_t>(std::lround(value)));
  }
};

// --- Text entities ---

class SegLCDTempHumTemperatureText : public text::Text, public Parented<SegLCDTempHumComponent> {
 protected:
  void control(const std::string &value) override { this->parent_->set_field_text(this->parent_->temp_field(), value); }
};

class SegLCDTempHumHumidityText : public text::Text, public Parented<SegLCDTempHumComponent> {
 protected:
  void control(const std::string &value) override { this->parent_->set_field_text(this->parent_->hum_field(), value); }
};

// --- Switch entities ---

class SegLCDTempHumCelsiusSwitch : public switch_::Switch, public Parented<SegLCDTempHumComponent> {
 protected:
  void write_state(bool state) override { this->parent_->set_show_celsius(state); }
};

class SegLCDTempHumPercentSwitch : public switch_::Switch, public Parented<SegLCDTempHumComponent> {
 protected:
  void write_state(bool state) override { this->parent_->set_show_percent(state); }
};

}  // namespace seglcd_temphum
}  // namespace esphome
