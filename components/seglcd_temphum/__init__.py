import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ADDRESS, CONF_ID, CONF_HUMIDITY, CONF_SCL, CONF_SDA, CONF_TEMPERATURE

CONF_BATTERY_LEVEL = "battery_level"
CONF_SIGNAL_LEVEL = "signal_level"
CONF_SHOW_CELSIUS = "show_celsius"
CONF_SHOW_PERCENT = "show_percent"
CONF_SUBADDRESS = "subaddress"

seglcd_temphum_ns = cg.esphome_ns.namespace("seglcd_temphum")
SegLCDTempHumComponent = seglcd_temphum_ns.class_("SegLCDTempHumComponent", cg.PollingComponent)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SegLCDTempHumComponent),
            cv.Optional(CONF_ADDRESS, default=0x38): cv.int_range(min=0x00, max=0x7F),
            cv.Required(CONF_TEMPERATURE): cv.use_id(sensor.Sensor),
            cv.Required(CONF_HUMIDITY): cv.use_id(sensor.Sensor),
            cv.Optional(CONF_BATTERY_LEVEL): cv.use_id(sensor.Sensor),
            cv.Optional(CONF_SIGNAL_LEVEL): cv.use_id(sensor.Sensor),
            cv.Optional(CONF_SHOW_CELSIUS, default=True): cv.boolean,
            cv.Optional(CONF_SHOW_PERCENT, default=True): cv.boolean,
            cv.Optional(CONF_SUBADDRESS, default=0): cv.int_range(min=0, max=7),
            cv.Required(CONF_SDA): cv.int_range(min=0, max=48),
            cv.Required(CONF_SCL): cv.int_range(min=0, max=48),
        }
    )
    .extend(cv.polling_component_schema("10s"))
)


async def to_code(config):
    var = cg.new_Pvariable(
        config[CONF_ID],
        config[CONF_ADDRESS],
        config[CONF_SUBADDRESS],
        config[CONF_SDA],
        config[CONF_SCL],
    )
    await cg.register_component(var, config)

    temperature = await cg.get_variable(config[CONF_TEMPERATURE])
    cg.add(var.set_temperature_sensor(temperature))

    humidity = await cg.get_variable(config[CONF_HUMIDITY])
    cg.add(var.set_humidity_sensor(humidity))

    if CONF_BATTERY_LEVEL in config:
        battery_level = await cg.get_variable(config[CONF_BATTERY_LEVEL])
        cg.add(var.set_battery_level_sensor(battery_level))

    if CONF_SIGNAL_LEVEL in config:
        signal_level = await cg.get_variable(config[CONF_SIGNAL_LEVEL])
        cg.add(var.set_signal_level_sensor(signal_level))

    cg.add(var.set_show_celsius(config[CONF_SHOW_CELSIUS]))
    cg.add(var.set_show_percent(config[CONF_SHOW_PERCENT]))

    cg.add_library("Wire", None)
    cg.add_library("https://github.com/petrkr/SegLCDLib.git", None)
