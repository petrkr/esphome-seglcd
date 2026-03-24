#include "seglcd_temphum.h"

#include <cmath>
#include <cstdio>
#include <cstring>

#include "esphome/core/log.h"

namespace esphome {
namespace seglcd_temphum {

static const char *const TAG = "seglcd_temphum";

SegLCDTempHumComponent::SegLCDTempHumComponent(uint8_t subaddress)
    : subaddress_(subaddress) {
  this->temp_field_.row = 0;
  this->temp_field_.width = 4;
  this->temp_field_.decimals = 1;
  this->hum_field_.row = 1;
  this->hum_field_.width = 3;
  this->hum_field_.decimals = 1;
}

void SegLCDTempHumComponent::set_show_celsius(bool v) {
  this->show_celsius_ = v;
  if (this->celsius_switch_ != nullptr)
    this->celsius_switch_->publish_state(v);
  this->apply_labels_();
}

void SegLCDTempHumComponent::set_show_percent(bool v) {
  this->show_percent_ = v;
  if (this->percent_switch_ != nullptr)
    this->percent_switch_->publish_state(v);
  this->apply_labels_();
}

void SegLCDTempHumComponent::set_field_number(DisplayField &field, float value) {
  field.number.set(value);
  if (field.number_entity != nullptr)
    field.number_entity->publish_state(value);
  this->write_display_field_(field);
}

void SegLCDTempHumComponent::set_field_text(DisplayField &field, const std::string &value) {
  field.text.set(value);
  if (field.text_entity != nullptr)
    field.text_entity->publish_state(value);
  this->write_display_field_(field);
}

void SegLCDTempHumComponent::set_battery_level_value(uint8_t value) {
  this->manual_battery_.set(this->clamp_level_(value));
  if (this->battery_level_number_ != nullptr)
    this->battery_level_number_->publish_state(this->manual_battery_.value);
  this->write_battery_level_();
}

void SegLCDTempHumComponent::set_signal_level_value(uint8_t value) {
  this->manual_signal_.set(this->clamp_level_(value));
  if (this->signal_level_number_ != nullptr)
    this->signal_level_number_->publish_state(this->manual_signal_.value);
  this->write_signal_level_();
}

void SegLCDTempHumComponent::setup() {
  ESP_LOGCONFIG(TAG, "Initializing SegLCD TempHum display");
  if (this->transport_ == nullptr) {
    ESP_LOGE(TAG, "No transport configured");
    this->mark_failed();
    return;
  }
  if (!this->transport_->has_i2c_bus()) {
    ESP_LOGE(TAG, "Transport has no I2C bus");
    this->mark_failed();
    return;
  }
  this->lcd_ = new SegLCD_PCF85176_TempHumidity(
      this->transport_->get_bus(), this->transport_->get_address(), this->subaddress_);
  this->lcd_->init();
  this->render_();
}

void SegLCDTempHumComponent::update() {
  ESP_LOGV(TAG, "Refreshing display");
  this->render_();
}

void SegLCDTempHumComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "SegLCD TempHum");
  if (this->transport_ != nullptr)
    ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->transport_->get_address());
  ESP_LOGCONFIG(TAG, "  Subaddress: %u", this->subaddress_);
  ESP_LOGCONFIG(TAG, "  Show Celsius: %s", this->show_celsius_ ? "true" : "false");
  ESP_LOGCONFIG(TAG, "  Show Percent: %s", this->show_percent_ ? "true" : "false");
}

float SegLCDTempHumComponent::get_setup_priority() const { return setup_priority::DATA; }

void SegLCDTempHumComponent::render_() {
  if (this->lcd_ == nullptr)
    return;
  this->apply_labels_();
  this->write_display_field_(this->temp_field_);
  this->write_display_field_(this->hum_field_);
  this->write_battery_level_();
  this->write_signal_level_();
}

void SegLCDTempHumComponent::apply_labels_() {
  if (this->lcd_ == nullptr)
    return;
  uint8_t all_labels = SegLCD_PCF85176_TempHumidity::LABEL_DEGREE_C
                      | SegLCD_PCF85176_TempHumidity::LABEL_PROC;
  uint8_t set = 0;
  if (this->show_celsius_)
    set |= SegLCD_PCF85176_TempHumidity::LABEL_DEGREE_C;
  if (this->show_percent_)
    set |= SegLCD_PCF85176_TempHumidity::LABEL_PROC;
  this->lcd_->clearLabels(all_labels & ~set);
  this->lcd_->setLabels(set);
}

void SegLCDTempHumComponent::write_display_field_(DisplayField &field) {
  if (this->lcd_ == nullptr)
    return;
  // Priority: text > number > sensor
  if (field.text.active) {
    this->write_text_(field.row, field.text.value);
    return;
  }

  if (field.number.active) {
    char buf[16];
    this->format_value_(buf, sizeof(buf), field.number.value, field.width, field.decimals);
    if (field.number_entity != nullptr)
      field.number_entity->publish_state(field.number.value);
    this->write_number_right_aligned_(field.row, field.width, buf);
    return;
  }

  if (field.sensor != nullptr && field.sensor->has_state() && !std::isnan(field.sensor->state)) {
    float value = field.sensor->state;
    char buf[16];
    this->format_value_(buf, sizeof(buf), value, field.width, field.decimals);
    if (field.number_entity != nullptr)
      field.number_entity->publish_state(value);
    this->write_number_right_aligned_(field.row, field.width, buf);
  }
}

void SegLCDTempHumComponent::write_number_right_aligned_(uint8_t row, uint8_t width, const char *formatted) {
  size_t len = std::strlen(formatted);
  size_t display_chars = 0;
  for (size_t i = 0; i < len; i++) {
    if (formatted[i] != '.')
      display_chars++;
  }

  std::string padded;
  if (display_chars < width) {
    padded.append(width - display_chars, ' ');
  }
  padded.append(formatted);

  this->lcd_->setCursor(row, 0);
  this->lcd_->print(padded.c_str());
}

void SegLCDTempHumComponent::write_text_(uint8_t row, const std::string &text) {
  this->lcd_->setCursor(row, 0);
  this->lcd_->print(text.c_str());
}

void SegLCDTempHumComponent::write_battery_level_() {
  if (this->lcd_ == nullptr)
    return;
  uint8_t level;
  if (this->manual_battery_.active) {
    level = this->manual_battery_.value;
  } else {
    level = this->clamp_level_(this->battery_level_sensor_);
  }
  if (this->battery_level_number_ != nullptr)
    this->battery_level_number_->publish_state(level);
  this->lcd_->setBatteryLevel(level);
}

void SegLCDTempHumComponent::write_signal_level_() {
  if (this->lcd_ == nullptr)
    return;
  uint8_t level;
  if (this->manual_signal_.active) {
    level = this->manual_signal_.value;
  } else {
    level = this->clamp_level_(this->signal_level_sensor_);
  }
  if (this->signal_level_number_ != nullptr)
    this->signal_level_number_->publish_state(level);
  this->lcd_->setSignalLevel(level);
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

void SegLCDTempHumComponent::format_value_(char *buffer, size_t buffer_size, float value,
                                            uint8_t width, uint8_t decimals) const {
  if (std::isnan(value)) {
    buffer[0] = '\0';
    return;
  }

  // Format with requested decimals. Reduce only if it doesn't fit in width.
  // "12.40" with decimals=2 stays "12.40", trailing zeros are preserved.
  for (int d = decimals; d >= 0; d--) {
    std::snprintf(buffer, buffer_size, "%.*f", d, value);

    // Count digit positions (everything except '.')
    size_t digits = 0;
    for (const char *p = buffer; *p; p++) {
      if (*p != '.')
        digits++;
    }
    if (digits <= width)
      return;
  }

  // Still doesn't fit at 0 decimals — output as-is (integer)
  std::snprintf(buffer, buffer_size, "%.0f", value);
}

}  // namespace seglcd_temphum
}  // namespace esphome
