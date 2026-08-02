#include "seglcd.h"

namespace esphome {
namespace seglcd {

void SegLCDI2CTransport::write(uint8_t address, uint8_t data) {
  if (this->bus_ == nullptr)
    return;

  this->bus_->write_readv(address, &data, 1, nullptr, 0);
}

void SegLCDI2CTransport::write(uint8_t address, uint8_t *data, size_t length) {
  if (this->bus_ == nullptr)
    return;

  this->bus_->write_readv(address, data, length, nullptr, 0);
}

bool seglcd_setup(PollingComponent *component, SegLCDI2CTransport &transport, SegLCDLib &lcd) {
  ESP_LOGCONFIG(TAG, "Initializing SegLCD");

  if (!transport.has_i2c_bus()) {
    ESP_LOGE(TAG, "No I2C bus configured");
    component->mark_failed();
    return false;
  }

  lcd.init();
  lcd.setAutoFlush(false);
  return true;
}

void seglcd_update(SegLCDLib &lcd) { lcd.flush(); }

void seglcd_dump_config(const char *model_name, uint8_t address, uint8_t subaddress, PollingComponent *component) {
  ESP_LOGCONFIG(TAG, "SegLCD Display");
  ESP_LOGCONFIG(TAG, "  Model: %s", model_name);
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", address);
  ESP_LOGCONFIG(TAG, "  Subaddress: %u", subaddress);
  LOG_UPDATE_INTERVAL(component);
}

}  // namespace seglcd
}  // namespace esphome
