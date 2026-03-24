#include "segtransport_i2c_esphome.h"

namespace esphome {
namespace seglcd_temphum {

void SegTransportI2CESPHome::write(uint8_t address, uint8_t data) {
  if (this->bus_ == nullptr) {
    return;
  }

  this->bus_->write_readv(address, &data, 1, nullptr, 0);
}

void SegTransportI2CESPHome::write(uint8_t address, uint8_t *data, size_t length) {
  if (this->bus_ == nullptr) {
    return;
  }

  this->bus_->write_readv(address, data, length, nullptr, 0);
}

}  // namespace seglcd_temphum
}  // namespace esphome
