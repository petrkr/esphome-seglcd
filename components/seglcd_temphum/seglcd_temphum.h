#pragma once

#include <Wire.h>

#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"
#include "SegTransport.h"
#include "SegLCD_PCF85176_TempHum.h"

namespace esphome {
namespace seglcd_temphum {

class SegLCDTempHumComponent : public PollingComponent {
 public:
  SegLCDTempHumComponent(uint8_t address, uint8_t subaddress, uint8_t sda_pin, uint8_t scl_pin);

  void setup() override;
  void update() override;
  void dump_config() override;
  float get_setup_priority() const override;

  void set_temperature_sensor(sensor::Sensor *sensor) { this->temperature_sensor_ = sensor; }
  void set_humidity_sensor(sensor::Sensor *sensor) { this->humidity_sensor_ = sensor; }
  void set_battery_level_sensor(sensor::Sensor *sensor) { this->battery_level_sensor_ = sensor; }
  void set_signal_level_sensor(sensor::Sensor *sensor) { this->signal_level_sensor_ = sensor; }
  void set_show_celsius(bool show_celsius) { this->show_celsius_ = show_celsius; }
  void set_show_percent(bool show_percent) { this->show_percent_ = show_percent; }

 protected:
  void render_();
  void apply_labels_();
  void write_temperature_();
  void write_humidity_();
  void write_battery_level_();
  void write_signal_level_();
  int clamp_level_(sensor::Sensor *source) const;
  void format_temperature_(char *buffer, size_t buffer_size, float value) const;
  void format_humidity_(char *buffer, size_t buffer_size, float value) const;

  sensor::Sensor *temperature_sensor_{nullptr};
  sensor::Sensor *humidity_sensor_{nullptr};
  sensor::Sensor *battery_level_sensor_{nullptr};
  sensor::Sensor *signal_level_sensor_{nullptr};

  bool show_celsius_{true};
  bool show_percent_{true};

  uint8_t address_;
  uint8_t subaddress_;
  uint8_t sda_pin_;
  uint8_t scl_pin_;
  SegTransportI2CArduino bus_;
  SegLCD_PCF85176_TempHumidity lcd_;
};

}  // namespace seglcd_temphum
}  // namespace esphome
