#include "seglcd.h"

#ifdef SEGLCD_ENABLE_PCF85176_4DR821B
#include "SegLCD_PCF85176_4DR821B.h"
#endif

namespace esphome {
namespace seglcd {

SegLCDLib *SegLCDDisplay::create_pcf85176_4dr821b_() {
#ifdef SEGLCD_ENABLE_PCF85176_4DR821B
  return new SegLCD_PCF85176_4DR821B(this->transport_, this->address_, this->subaddress_);
#else
  return nullptr;
#endif
}

void SegLCDDisplay::set_arrow(bool state) {
#ifdef SEGLCD_ENABLE_PCF85176_4DR821B
  if (this->model_ == SEGLCD_MODEL_PCF85176_4DR821B && this->lcd_ != nullptr)
    static_cast<SegLCD_PCF85176_4DR821B *>(this->lcd_)->setSymbol(
        SegLCD_PCF85176_4DR821B::SYMBOL_ARROW, state);
#endif
}

void SegLCDDisplay::set_tilde(bool state) {
#ifdef SEGLCD_ENABLE_PCF85176_4DR821B
  if (this->model_ == SEGLCD_MODEL_PCF85176_4DR821B && this->lcd_ != nullptr)
    static_cast<SegLCD_PCF85176_4DR821B *>(this->lcd_)->setSymbol(
        SegLCD_PCF85176_4DR821B::SYMBOL_TILDA, state);
#endif
}

}  // namespace seglcd
}  // namespace esphome
