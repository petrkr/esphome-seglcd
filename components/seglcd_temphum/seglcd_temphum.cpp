#include "seglcd_temphum.h"

#include <cmath>
#include <cstdio>

#include "esphome/core/log.h"

namespace esphome {
namespace seglcd_temphum {

static const char *const TAG = "seglcd_temphum";

SegLCDTempHumComponent::SegLCDTempHumComponent(uint8_t address, uint8_t subaddress, uint8_t sda_pin, uint8_t scl_pin)
    : address_(address),
      subaddress_(subaddress),
      sda_pin_(sda_pin),
      scl_pin_(scl_pin),
      lcd_(Wire, address, subaddress) {}

void SegLCDTempHumComponent::setup() {
  ESP_LOGCONFIG(TAG, "Initializing SegLCD TempHum display");
  Wire.begin(this->sda_pin_, this->scl_pin_);
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
  ESP_LOGCONFIG(TAG, "  SDA pin: %u", this->sda_pin_);
  ESP_LOGCONFIG(TAG, "  SCL pin: %u", this->scl_pin_);
  ESP_LOGCONFIG(TAG, "  Show Celsius: %s", this->show_celsius_ ? "true" : "false");
  ESP_LOGCONFIG(TAG, "  Show Percent: %s", this->show_percent_ ? "true" : "false");
}

float SegLCDTempHumComponent::get_setup_priority() const { return setup_priority::HARDWARE; }

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
  if (this->show_celsius_) {
    labels |= SegLCD_PCF85176_TempHumidity::LABEL_DEGREE_C;
  }
  if (this->show_percent_) {
    labels |= SegLCD_PCF85176_TempHumidity::LABEL_PROC;
  }
  this->lcd_.setLabels(labels);
}

void SegLCDTempHumComponent::write_temperature_() {
  if (this->temperature_sensor_ == nullptr || !this->temperature_sensor_->has_state()) {
    return;
  }

  char value[8];
  this->format_temperature_(value, sizeof(value), this->temperature_sensor_->state);
  this->lcd_.setCursor(0, 0);
  this->lcd_.print(value);
}

void SegLCDTempHumComponent::write_humidity_() {
  if (this->humidity_sensor_ == nullptr || !this->humidity_sensor_->has_state()) {
    return;
  }

  char value[8];
  this->format_humidity_(value, sizeof(value), this->humidity_sensor_->state);
  this->lcd_.setCursor(1, 0);
  this->lcd_.print(value);
}

void SegLCDTempHumComponent::write_battery_level_() {
  this->lcd_.setBatteryLevel(this->clamp_level_(this->battery_level_sensor_));
}

void SegLCDTempHumComponent::write_signal_level_() {
  this->lcd_.setSignalLevel(this->clamp_level_(this->signal_level_sensor_));
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
