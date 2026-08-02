#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <utility>

#include "SegLCDLib.h"
#include "SegTransport.h"
#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace seglcd {

static const char *const TAG = "seglcd";

class SegLCDI2CTransport : public SegTransportI2C {
 public:
  void set_i2c_bus(i2c::I2CBus *bus) { this->bus_ = bus; }
  bool has_i2c_bus() const { return this->bus_ != nullptr; }

  void write(uint8_t address, uint8_t data) override;
  void write(uint8_t address, uint8_t *data, size_t length) override;

 protected:
  i2c::I2CBus *bus_{nullptr};
};

// Shared PollingComponent lifecycle steps, identical for every SegLCD model.
// Each per-model display class calls these from its own setup()/update()/
// dump_config() overrides instead of duplicating the logic; the writer
// lambda itself (which needs the concrete model type) stays in the caller.
bool seglcd_setup(PollingComponent *component, SegLCDI2CTransport &transport, SegLCDLib &lcd);
void seglcd_update(SegLCDLib &lcd);
void seglcd_dump_config(const char *model_name, uint8_t address, uint8_t subaddress, PollingComponent *component);

}  // namespace seglcd
}  // namespace esphome
