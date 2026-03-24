import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import CONF_ID, CONF_HUMIDITY, CONF_TEMPERATURE

DEPENDENCIES = ["i2c"]

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
            cv.Required(CONF_TEMPERATURE): cv.use_id(sensor.Sensor),
            cv.Required(CONF_HUMIDITY): cv.use_id(sensor.Sensor),
            cv.Optional(CONF_BATTERY_LEVEL): cv.use_id(sensor.Sensor),
            cv.Optional(CONF_SIGNAL_LEVEL): cv.use_id(sensor.Sensor),
            cv.Optional(CONF_SHOW_CELSIUS, default=True): cv.boolean,
            cv.Optional(CONF_SHOW_PERCENT, default=True): cv.boolean,
            cv.Optional(CONF_SUBADDRESS, default=0): cv.int_range(min=0, max=7),
        }
    )
    .extend(cv.polling_component_schema("10s"))
    .extend(i2c.i2c_device_schema(0x38))
)


async def to_code(config):
    var = cg.new_Pvariable(
        config[CONF_ID],
        config[i2c.CONF_ADDRESS],
        config[CONF_SUBADDRESS],
    )
    await cg.register_component(var, config)

    bus = await cg.get_variable(config[i2c.CONF_I2C_ID])
    cg.add(var.set_i2c_bus(bus))

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

    cg.add_library("https://github.com/petrkr/SegLCDLib.git#a53ea3394b5cb5746d289159de1a925ad5904fff", None)
