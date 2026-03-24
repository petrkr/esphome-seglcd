#pragma once

#include "SegTransport.h"
#include "esphome/components/i2c/i2c.h"

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

}  // namespace seglcd_transport
}  // namespace esphome
