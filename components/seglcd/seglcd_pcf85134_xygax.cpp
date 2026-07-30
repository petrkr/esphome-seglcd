#include "seglcd.h"

#ifdef SEGLCD_ENABLE_PCF85134_XYGAX
#include "SegLCD_PCF85134_Xygax.h"
#endif

namespace esphome {
namespace seglcd {

SegLCDLib *SegLCDDisplay::create_pcf85134_xygax_() {
#ifdef SEGLCD_ENABLE_PCF85134_XYGAX
  return new SegLCD_PCF85134_Xygax(this->transport_, this->address_, this->subaddress_);
#else
  return nullptr;
#endif
}

}  // namespace seglcd
}  // namespace esphome
