#include "seglcd.h"

#ifdef SEGLCD_ENABLE_PCF85134_XYGAX_SEG_I2C
#include "SegLCD_PCF85134_XygaxSegI2C.h"
#endif

namespace esphome {
namespace seglcd {

SegLCDLib *SegLCDDisplay::create_pcf85134_xygax_seg_i2c_() {
#ifdef SEGLCD_ENABLE_PCF85134_XYGAX_SEG_I2C
  return new SegLCD_PCF85134_XygaxSegI2C(this->transport_, this->address_, this->subaddress_);
#else
  return nullptr;
#endif
}

}  // namespace seglcd
}  // namespace esphome
