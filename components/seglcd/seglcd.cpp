#include "seglcd.h"

#include <cstdarg>
#include <cstdio>

#include "esphome/core/log.h"

namespace esphome {
namespace seglcd {

static const char *const TAG = "seglcd";

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

SegLCDDisplay::SegLCDDisplay(uint8_t address, uint8_t subaddress)
    : address_(address), subaddress_(subaddress) {}

SegLCDDisplay::~SegLCDDisplay() {
  delete this->lcd_;
  this->lcd_ = nullptr;
}

void SegLCDDisplay::setup() {
  ESP_LOGCONFIG(TAG, "Initializing SegLCD");

  if (!this->transport_.has_i2c_bus()) {
    ESP_LOGE(TAG, "No I2C bus configured");
    this->mark_failed();
    return;
  }

  this->lcd_ = this->create_lcd_();

  if (this->lcd_ == nullptr) {
    ESP_LOGE(TAG, "Configured SegLCD model is not enabled");
    this->mark_failed();
    return;
  }
  this->lcd_->init();
  this->lcd_->setAutoFlush(false);
  this->update();
}

void SegLCDDisplay::update() {
  if (this->lcd_ == nullptr)
    return;

  this->lcd_->clear();
  if (this->writer_configured_)
    this->writer_(*this);
  this->lcd_->flush();
}

void SegLCDDisplay::dump_config() {
  ESP_LOGCONFIG(TAG, "SegLCD Display");
  ESP_LOGCONFIG(TAG, "  Model: %s", this->model_name_);
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);
  ESP_LOGCONFIG(TAG, "  Subaddress: %u", this->subaddress_);
  LOG_UPDATE_INTERVAL(this);
}

void SegLCDDisplay::clear() {
  if (this->lcd_ != nullptr)
    this->lcd_->clear();
}

void SegLCDDisplay::home() {
  if (this->lcd_ != nullptr)
    this->lcd_->home();
}

void SegLCDDisplay::set_cursor(uint8_t column) { this->set_cursor(column, 0); }

void SegLCDDisplay::set_cursor(uint8_t column, uint8_t row) {
  if (this->lcd_ != nullptr)
    this->lcd_->setCursor(row, column);
}

void SegLCDDisplay::print(const char *str) {
  if (this->lcd_ != nullptr)
    this->lcd_->print(str);
}

void SegLCDDisplay::print(const std::string &str) { this->print(str.c_str()); }

void SegLCDDisplay::print(uint8_t column, const char *str) { this->print(column, 0, str); }

void SegLCDDisplay::print(uint8_t column, const std::string &str) { this->print(column, str.c_str()); }

void SegLCDDisplay::print(uint8_t column, uint8_t row, const char *str) {
  if (this->lcd_ == nullptr)
    return;

  this->lcd_->setCursor(row, column);
  this->lcd_->print(str);
}

void SegLCDDisplay::print(uint8_t column, uint8_t row, const std::string &str) {
  this->print(column, row, str.c_str());
}

void SegLCDDisplay::printf(const char *format, ...) {
  va_list args;
  va_start(args, format);
  this->print_va_(0, 0, format, args);
  va_end(args);
}

void SegLCDDisplay::printf(uint8_t column, const char *format, ...) {
  va_list args;
  va_start(args, format);
  this->print_va_(column, 0, format, args);
  va_end(args);
}

void SegLCDDisplay::printf(uint8_t column, uint8_t row, const char *format, ...) {
  va_list args;
  va_start(args, format);
  this->print_va_(column, row, format, args);
  va_end(args);
}

void SegLCDDisplay::on() {
  if (this->lcd_ != nullptr)
    this->lcd_->on();
}

void SegLCDDisplay::off() {
  if (this->lcd_ != nullptr)
    this->lcd_->off();
}

void SegLCDDisplay::print_va_(uint8_t column, uint8_t row, const char *format, va_list args) {
  char buffer[64];
  std::vsnprintf(buffer, sizeof(buffer), format, args);
  this->print(column, row, buffer);
}

SegLCDLib *SegLCDDisplay::create_lcd_() {
  switch (this->model_) {
#ifdef SEGLCD_ENABLE_PCF85134_XYGAX_SEG_I2C
    case SEGLCD_MODEL_PCF85134_XYGAX_SEG_I2C:
      return this->create_pcf85134_xygax_seg_i2c_();
#endif
#ifdef SEGLCD_ENABLE_PCF85176_4DR821B
    case SEGLCD_MODEL_PCF85176_4DR821B:
      return this->create_pcf85176_4dr821b_();
#endif
  }
  return nullptr;
}

}  // namespace seglcd
}  // namespace esphome
