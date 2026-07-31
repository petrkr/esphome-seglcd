#pragma once

#include <cstddef>
#include <cstdarg>
#include <cstdint>
#include <string>
#include <utility>

#include "SegLCDLib.h"
#include "SegTransport.h"
#include "esphome/components/display/display.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/component.h"

namespace esphome {
namespace seglcd {

class SegLCDDisplay;

using seglcd_writer_t = display::DisplayWriter<SegLCDDisplay>;

enum SegLCDModel {
  SEGLCD_MODEL_PCF85134_XYGAX,
  SEGLCD_MODEL_PCF85176_4DR821B,
};

class SegLCDI2CTransport : public SegTransportI2C {
 public:
  void set_i2c_bus(i2c::I2CBus *bus) { this->bus_ = bus; }
  bool has_i2c_bus() const { return this->bus_ != nullptr; }

  void write(uint8_t address, uint8_t data) override;
  void write(uint8_t address, uint8_t *data, size_t length) override;

 protected:
  i2c::I2CBus *bus_{nullptr};
};

class SegLCDDisplay : public PollingComponent {
 public:
  SegLCDDisplay(uint8_t address, uint8_t subaddress);
  ~SegLCDDisplay();

  void set_model(SegLCDModel model) { this->model_ = model; }
  void set_model_name(const char *model_name) { this->model_name_ = model_name; }
  void set_i2c_bus(i2c::I2CBus *bus) { this->transport_.set_i2c_bus(bus); }
  void set_writer(seglcd_writer_t &&writer) {
    this->writer_ = std::move(writer);
    this->writer_configured_ = true;
  }

  void setup() override;
  void update() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void clear();
  void home();
  void set_cursor(uint8_t column);
  void set_cursor(uint8_t column, uint8_t row);
  void print(const char *str);
  void print(const std::string &str);
  void print(uint8_t column, const char *str);
  void print(uint8_t column, const std::string &str);
  void print(uint8_t column, uint8_t row, const char *str);
  void print(uint8_t column, uint8_t row, const std::string &str);
  void printf(const char *format, ...) __attribute__((format(printf, 2, 3)));
  void printf(uint8_t column, const char *format, ...) __attribute__((format(printf, 3, 4)));
  void printf(uint8_t column, uint8_t row, const char *format, ...) __attribute__((format(printf, 4, 5)));
  void set_arrow(bool state);
  void set_tilde(bool state);
  void on();
  void off();

 protected:
  SegLCDLib *create_lcd_();
  SegLCDLib *create_pcf85134_xygax_();
  SegLCDLib *create_pcf85176_4dr821b_();
  void print_va_(uint8_t column, uint8_t row, const char *format, va_list args);

  uint8_t address_;
  uint8_t subaddress_;
  SegLCDModel model_{SEGLCD_MODEL_PCF85134_XYGAX};
  const char *model_name_{"unknown"};
  SegLCDI2CTransport transport_;
  SegLCDLib *lcd_{nullptr};
  seglcd_writer_t writer_;
  bool writer_configured_{false};
};

}  // namespace seglcd
}  // namespace esphome
