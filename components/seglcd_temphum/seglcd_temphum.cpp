#include "seglcd_temphum.h"

#include <cmath>
#include <cstdio>

#include "esphome/core/log.h"

namespace esphome {
namespace seglcd_temphum {

static const char *const TAG = "seglcd_temphum";

SegLCDTempHumComponent::SegLCDTempHumComponent(uint8_t address, uint8_t subaddress)
    : address_(address), subaddress_(subaddress), lcd_(bus_, address, subaddress) {}

void SegLCDTempHumComponent::set_show_celsius(bool show_celsius) {
  this->show_celsius_ = show_celsius;
  if (this->celsius_switch_ != nullptr) {
    this->celsius_switch_->publish_state(show_celsius);
  }
  this->render_();
}

void SegLCDTempHumComponent::set_show_percent(bool show_percent) {
  this->show_percent_ = show_percent;
  if (this->percent_switch_ != nullptr) {
    this->percent_switch_->publish_state(show_percent);
  }
  this->render_();
}

void SegLCDTempHumComponent::set_temperature_value(float value) {
  this->manual_temperature_ = value;
  this->has_manual_temperature_ = true;
  if (this->temperature_number_ != nullptr) {
    this->temperature_number_->publish_state(value);
  }
  this->render_();
}

void SegLCDTempHumComponent::set_humidity_value(float value) {
  this->manual_humidity_ = value;
  this->has_manual_humidity_ = true;
  if (this->humidity_number_ != nullptr) {
    this->humidity_number_->publish_state(value);
  }
  this->render_();
}

void SegLCDTempHumComponent::set_battery_level_value(uint8_t value) {
  this->manual_battery_level_ = this->clamp_level_(value);
  this->has_manual_battery_level_ = true;
  if (this->battery_level_number_ != nullptr) {
    this->battery_level_number_->publish_state(this->manual_battery_level_);
  }
  this->render_();
}

void SegLCDTempHumComponent::set_signal_level_value(uint8_t value) {
  this->manual_signal_level_ = this->clamp_level_(value);
  this->has_manual_signal_level_ = true;
  if (this->signal_level_number_ != nullptr) {
    this->signal_level_number_->publish_state(this->manual_signal_level_);
  }
  this->render_();
}

void SegLCDTempHumComponent::clear_temperature_value() {
  this->has_manual_temperature_ = false;
  this->render_();
}

void SegLCDTempHumComponent::clear_humidity_value() {
  this->has_manual_humidity_ = false;
  this->render_();
}

void SegLCDTempHumComponent::clear_battery_level_value() {
  this->has_manual_battery_level_ = false;
  this->render_();
}

void SegLCDTempHumComponent::clear_signal_level_value() {
  this->has_manual_signal_level_ = false;
  this->render_();
}

void SegLCDTempHumComponent::setup() {
  ESP_LOGCONFIG(TAG, "Initializing SegLCD TempHum display");
  if (!this->bus_.has_i2c_bus()) {
    ESP_LOGE(TAG, "No ESPHome I2C bus configured");
    this->mark_failed();
    return;
  }
  this->lcd_.init();
  this->render_();
}

void SegLCDTempHumComponent::update() {
  ESP_LOGV(TAG, "Refreshing display");
  this->render_();
}

void SegLCDTempHumComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "SegLCD TempHum");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);
  ESP_LOGCONFIG(TAG, "  Subaddress: %u", this->subaddress_);
  ESP_LOGCONFIG(TAG, "  Show Celsius: %s", this->show_celsius_ ? "true" : "false");
  ESP_LOGCONFIG(TAG, "  Show Percent: %s", this->show_percent_ ? "true" : "false");
}

float SegLCDTempHumComponent::get_setup_priority() const { return setup_priority::DATA; }

void SegLCDTempHumComponent::render_() {
  this->lcd_.clear();
  this->apply_labels_();
  this->write_temperature_();
  this->write_humidity_();
  this->write_battery_level_();
  this->write_signal_level_();
}

void SegLCDTempHumComponent::apply_labels_() {
  uint8_t labels = 0;
  uint8_t clear_labels = 0;
  if (this->show_celsius_) {
    labels |= SegLCD_PCF85176_TempHumidity::LABEL_DEGREE_C;
  } else {
    clear_labels |= SegLCD_PCF85176_TempHumidity::LABEL_DEGREE_C;
  }
  if (this->show_percent_) {
    labels |= SegLCD_PCF85176_TempHumidity::LABEL_PROC;
  } else {
    clear_labels |= SegLCD_PCF85176_TempHumidity::LABEL_PROC;
  }
  this->lcd_.clearLabels(clear_labels);
  this->lcd_.setLabels(labels);
}

void SegLCDTempHumComponent::write_temperature_() {
  float temperature = 0.0f;
  if (!this->get_temperature_value_(temperature)) {
    return;
  }
  if (this->temperature_number_ != nullptr) {
    this->temperature_number_->publish_state(temperature);
  }

  char value[8];
  this->format_temperature_(value, sizeof(value), temperature);
  this->lcd_.setCursor(0, 0);
  this->lcd_.print(value);
}

void SegLCDTempHumComponent::write_humidity_() {
  float humidity = 0.0f;
  if (!this->get_humidity_value_(humidity)) {
    return;
  }
  if (this->humidity_number_ != nullptr) {
    this->humidity_number_->publish_state(humidity);
  }

  char value[8];
  this->format_humidity_(value, sizeof(value), humidity);
  this->lcd_.setCursor(1, 0);
  this->lcd_.print(value);
}

void SegLCDTempHumComponent::write_battery_level_() {
  uint8_t level = 0;
  if (this->get_battery_level_value_(level)) {
    if (this->battery_level_number_ != nullptr) {
      this->battery_level_number_->publish_state(level);
    }
    this->lcd_.setBatteryLevel(level);
  }
}

void SegLCDTempHumComponent::write_signal_level_() {
  uint8_t level = 0;
  if (this->get_signal_level_value_(level)) {
    if (this->signal_level_number_ != nullptr) {
      this->signal_level_number_->publish_state(level);
    }
    this->lcd_.setSignalLevel(level);
  }
}

int SegLCDTempHumComponent::clamp_level_(sensor::Sensor *source) const {
  if (source == nullptr || !source->has_state() || std::isnan(source->state)) {
    return 0;
  }

  int level = static_cast<int>(std::lround(source->state));
  if (level < 0) {
    return 0;
  }
  if (level > 4) {
    return 4;
  }
  return level;
}

int SegLCDTempHumComponent::clamp_level_(int value) const {
  if (value < 0) {
    return 0;
  }
  if (value > 4) {
    return 4;
  }
  return value;
}

bool SegLCDTempHumComponent::get_temperature_value_(float &value) const {
  if (this->has_manual_temperature_) {
    value = this->manual_temperature_;
    return true;
  }

  if (this->temperature_sensor_ == nullptr || !this->temperature_sensor_->has_state()) {
    return false;
  }

  value = this->temperature_sensor_->state;
  return !std::isnan(value);
}

bool SegLCDTempHumComponent::get_humidity_value_(float &value) const {
  if (this->has_manual_humidity_) {
    value = this->manual_humidity_;
    return true;
  }

  if (this->humidity_sensor_ == nullptr || !this->humidity_sensor_->has_state()) {
    return false;
  }

  value = this->humidity_sensor_->state;
  return !std::isnan(value);
}

bool SegLCDTempHumComponent::get_battery_level_value_(uint8_t &value) const {
  if (this->has_manual_battery_level_) {
    value = this->manual_battery_level_;
    return true;
  }

  value = this->clamp_level_(this->battery_level_sensor_);
  return true;
}

bool SegLCDTempHumComponent::get_signal_level_value_(uint8_t &value) const {
  if (this->has_manual_signal_level_) {
    value = this->manual_signal_level_;
    return true;
  }

  value = this->clamp_level_(this->signal_level_sensor_);
  return true;
}

void SegLCDTempHumTemperatureNumber::control(float value) { this->parent_->set_temperature_value(value); }

void SegLCDTempHumHumidityNumber::control(float value) { this->parent_->set_humidity_value(value); }

void SegLCDTempHumBatteryLevelNumber::control(float value) {
  this->parent_->set_battery_level_value(static_cast<uint8_t>(std::lround(value)));
}

void SegLCDTempHumSignalLevelNumber::control(float value) {
  this->parent_->set_signal_level_value(static_cast<uint8_t>(std::lround(value)));
}

void SegLCDTempHumCelsiusSwitch::write_state(bool state) { this->parent_->set_show_celsius(state); }

void SegLCDTempHumPercentSwitch::write_state(bool state) { this->parent_->set_show_percent(state); }

void SegLCDTempHumComponent::format_temperature_(char *buffer, size_t buffer_size, float value) const {
  if (std::isnan(value)) {
    buffer[0] = '\0';
    return;
  }

  const float abs_value = std::fabs(value);
  if (abs_value < 10.0f) {
    std::snprintf(buffer, buffer_size, "%.1f", value);
    return;
  }

  if (abs_value < 100.0f) {
    std::snprintf(buffer, buffer_size, "%.1f", value);
    return;
  }

  std::snprintf(buffer, buffer_size, "%.0f", value);
}

void SegLCDTempHumComponent::format_humidity_(char *buffer, size_t buffer_size, float value) const {
  if (std::isnan(value)) {
    buffer[0] = '\0';
    return;
  }

  int rounded = static_cast<int>(std::lround(value));
  if (rounded < 0) {
    rounded = 0;
  }
  if (rounded > 100) {
    rounded = 100;
  }
  std::snprintf(buffer, buffer_size, "%d", rounded);
}

}  // namespace seglcd_temphum
}  // namespace esphome
