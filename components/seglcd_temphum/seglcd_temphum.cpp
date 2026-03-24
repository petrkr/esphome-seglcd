#include "seglcd_temphum.h"

#include <cmath>
#include <cstdio>
#include <cstring>

#include "esphome/core/log.h"

namespace esphome {
namespace seglcd_temphum {

static const char *const TAG = "seglcd_temphum";

SegLCDTempHumComponent::SegLCDTempHumComponent(uint8_t address, uint8_t subaddress)
    : address_(address), subaddress_(subaddress), lcd_(bus_, address, subaddress) {
  this->temp_field_.row = 0;
  this->temp_field_.width = 4;
  this->hum_field_.row = 1;
  this->hum_field_.width = 3;
}

void SegLCDTempHumComponent::set_show_celsius(bool v) {
  this->show_celsius_ = v;
  if (this->celsius_switch_ != nullptr)
    this->celsius_switch_->publish_state(v);
  this->render_();
}

void SegLCDTempHumComponent::set_show_percent(bool v) {
  this->show_percent_ = v;
  if (this->percent_switch_ != nullptr)
    this->percent_switch_->publish_state(v);
  this->render_();
}

void SegLCDTempHumComponent::set_field_number(DisplayField &field, float value) {
  field.number.set(value);
  if (field.number_entity != nullptr)
    field.number_entity->publish_state(value);
  this->render_();
}

void SegLCDTempHumComponent::set_field_text(DisplayField &field, const std::string &value) {
  field.text.set(value);
  if (field.text_entity != nullptr)
    field.text_entity->publish_state(value);
  this->render_();
}

void SegLCDTempHumComponent::set_battery_level_value(uint8_t value) {
  this->manual_battery_.set(this->clamp_level_(value));
  if (this->battery_level_number_ != nullptr)
    this->battery_level_number_->publish_state(this->manual_battery_.value);
  this->render_();
}

void SegLCDTempHumComponent::set_signal_level_value(uint8_t value) {
  this->manual_signal_.set(this->clamp_level_(value));
  if (this->signal_level_number_ != nullptr)
    this->signal_level_number_->publish_state(this->manual_signal_.value);
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
  this->write_display_field_(this->temp_field_);
  this->write_display_field_(this->hum_field_);
  this->write_battery_level_();
  this->write_signal_level_();
}

void SegLCDTempHumComponent::apply_labels_() {
  uint8_t labels = 0;
  if (this->show_celsius_)
    labels |= SegLCD_PCF85176_TempHumidity::LABEL_DEGREE_C;
  if (this->show_percent_)
    labels |= SegLCD_PCF85176_TempHumidity::LABEL_PROC;
  this->lcd_.setLabels(labels);
}

void SegLCDTempHumComponent::write_display_field_(DisplayField &field) {
  // Priority: last manual write wins — text over number over sensor
  // Text entity has highest priority if active
  if (field.text.active) {
    this->write_text_(field.row, field.text.value);
    return;
  }

  // Number entity override
  if (field.number.active) {
    char buf[8];
    if (field.row == 0)
      this->format_temperature_(buf, sizeof(buf), field.number.value);
    else
      this->format_humidity_(buf, sizeof(buf), field.number.value);
    if (field.number_entity != nullptr)
      field.number_entity->publish_state(field.number.value);
    this->write_number_right_aligned_(field.row, field.width, buf);
    return;
  }

  // Sensor fallback
  if (field.sensor != nullptr && field.sensor->has_state() && !std::isnan(field.sensor->state)) {
    float value = field.sensor->state;
    char buf[8];
    if (field.row == 0)
      this->format_temperature_(buf, sizeof(buf), value);
    else
      this->format_humidity_(buf, sizeof(buf), value);
    if (field.number_entity != nullptr)
      field.number_entity->publish_state(value);
    this->write_number_right_aligned_(field.row, field.width, buf);
  }
}

void SegLCDTempHumComponent::write_number_right_aligned_(uint8_t row, uint8_t width, const char *formatted) {
  // Count display characters (digits + minus, excluding dot)
  size_t len = std::strlen(formatted);
  size_t display_chars = 0;
  for (size_t i = 0; i < len; i++) {
    if (formatted[i] != '.')
      display_chars++;
  }

  // For temp row (row 0): extra minus segment handled by lib, so effective width
  // for padding is based on digit positions
  std::string padded;
  if (display_chars < width) {
    padded.append(width - display_chars, ' ');
  }
  padded.append(formatted);

  this->lcd_.setCursor(row, 0);
  this->lcd_.print(padded.c_str());
}

void SegLCDTempHumComponent::write_text_(uint8_t row, const std::string &text) {
  this->lcd_.setCursor(row, 0);
  this->lcd_.print(text.c_str());
}

void SegLCDTempHumComponent::write_battery_level_() {
  uint8_t level;
  if (this->manual_battery_.active) {
    level = this->manual_battery_.value;
  } else {
    level = this->clamp_level_(this->battery_level_sensor_);
  }
  if (this->battery_level_number_ != nullptr)
    this->battery_level_number_->publish_state(level);
  this->lcd_.setBatteryLevel(level);
}

void SegLCDTempHumComponent::write_signal_level_() {
  uint8_t level;
  if (this->manual_signal_.active) {
    level = this->manual_signal_.value;
  } else {
    level = this->clamp_level_(this->signal_level_sensor_);
  }
  if (this->signal_level_number_ != nullptr)
    this->signal_level_number_->publish_state(level);
  this->lcd_.setSignalLevel(level);
}

int SegLCDTempHumComponent::clamp_level_(sensor::Sensor *source) const {
  if (source == nullptr || !source->has_state() || std::isnan(source->state))
    return 0;
  return this->clamp_level_(static_cast<int>(std::lround(source->state)));
}

int SegLCDTempHumComponent::clamp_level_(int value) const {
  if (value < 0) return 0;
  if (value > 4) return 4;
  return value;
}

void SegLCDTempHumComponent::format_temperature_(char *buffer, size_t buffer_size, float value) const {
  if (std::isnan(value)) {
    buffer[0] = '\0';
    return;
  }

  const float abs_value = std::fabs(value);
  if (abs_value < 100.0f) {
    std::snprintf(buffer, buffer_size, "%.1f", value);
  } else {
    std::snprintf(buffer, buffer_size, "%.0f", value);
  }
}

void SegLCDTempHumComponent::format_humidity_(char *buffer, size_t buffer_size, float value) const {
  if (std::isnan(value)) {
    buffer[0] = '\0';
    return;
  }

  int rounded = static_cast<int>(std::lround(value));
  if (rounded < 0) rounded = 0;
  if (rounded > 100) rounded = 100;
  std::snprintf(buffer, buffer_size, "%d", rounded);
}

}  // namespace seglcd_temphum
}  // namespace esphome
