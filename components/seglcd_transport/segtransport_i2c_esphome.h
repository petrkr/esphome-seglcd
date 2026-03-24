#pragma once

#include "SegTransport.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/component.h"

namespace esphome {
namespace seglcd_transport {

class SegTransportI2CESPHome : public SegTransportI2C {
 public:
  void set_i2c_bus(i2c::I2CBus *bus) { this->bus_ = bus; }
  bool has_i2c_bus() const { return this->bus_ != nullptr; }

  void write(uint8_t address, uint8_t data) override;
  void write(uint8_t address, uint8_t *data, size_t length) override;

 protected:
  i2c::I2CBus *bus_{nullptr};
};

class SegLCDTransport : public Component {
 public:
  SegLCDTransport(uint8_t address) : address_(address) {}

  void set_i2c_bus(i2c::I2CBus *bus) { this->bus_.set_i2c_bus(bus); }
  float get_setup_priority() const override { return setup_priority::BUS; }

  uint8_t get_address() const { return this->address_; }
  SegTransportI2CESPHome &get_bus() { return this->bus_; }
  bool has_i2c_bus() const { return this->bus_.has_i2c_bus(); }

 protected:
  uint8_t address_;
  SegTransportI2CESPHome bus_;
};

}  // namespace seglcd_transport
}  // namespace esphome
