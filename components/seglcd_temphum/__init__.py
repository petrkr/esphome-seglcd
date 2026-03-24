import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number, sensor, switch, text
from esphome.components.seglcd_transport import SegLCDTransport, CONF_SEGLCD_TRANSPORT_ID
from esphome.const import CONF_ID, CONF_HUMIDITY, CONF_TEMPERATURE

DEPENDENCIES = ["seglcd_transport"]
AUTO_LOAD = ["sensor", "number", "switch", "text"]

CONF_BATTERY_LEVEL = "battery_level"
CONF_SIGNAL_LEVEL = "signal_level"
CONF_SHOW_CELSIUS = "show_celsius"
CONF_SHOW_PERCENT = "show_percent"
CONF_SUBADDRESS = "subaddress"
CONF_TEMPERATURE_NUMBER = "temperature_number"
CONF_HUMIDITY_NUMBER = "humidity_number"
CONF_TEMPERATURE_TEXT = "temperature_text"
CONF_HUMIDITY_TEXT = "humidity_text"
CONF_BATTERY_LEVEL_NUMBER = "battery_level_number"
CONF_SIGNAL_LEVEL_NUMBER = "signal_level_number"
CONF_CELSIUS_SWITCH = "celsius_switch"
CONF_PERCENT_SWITCH = "percent_switch"
CONF_TEMPERATURE_DECIMALS = "temperature_decimals"
CONF_HUMIDITY_DECIMALS = "humidity_decimals"

seglcd_temphum_ns = cg.esphome_ns.namespace("seglcd_temphum")
SegLCDTempHumComponent = seglcd_temphum_ns.class_("SegLCDTempHumComponent", cg.PollingComponent)
SegLCDTempHumTemperatureNumber = seglcd_temphum_ns.class_("SegLCDTempHumTemperatureNumber", number.Number)
SegLCDTempHumHumidityNumber = seglcd_temphum_ns.class_("SegLCDTempHumHumidityNumber", number.Number)
SegLCDTempHumBatteryLevelNumber = seglcd_temphum_ns.class_("SegLCDTempHumBatteryLevelNumber", number.Number)
SegLCDTempHumSignalLevelNumber = seglcd_temphum_ns.class_("SegLCDTempHumSignalLevelNumber", number.Number)
SegLCDTempHumTemperatureText = seglcd_temphum_ns.class_("SegLCDTempHumTemperatureText", text.Text)
SegLCDTempHumHumidityText = seglcd_temphum_ns.class_("SegLCDTempHumHumidityText", text.Text)
SegLCDTempHumCelsiusSwitch = seglcd_temphum_ns.class_("SegLCDTempHumCelsiusSwitch", switch.Switch)
SegLCDTempHumPercentSwitch = seglcd_temphum_ns.class_("SegLCDTempHumPercentSwitch", switch.Switch)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SegLCDTempHumComponent),
            cv.GenerateID(CONF_SEGLCD_TRANSPORT_ID): cv.use_id(SegLCDTransport),
            cv.Optional(CONF_SUBADDRESS, default=0): cv.int_range(min=0, max=7),
            cv.Optional(CONF_TEMPERATURE): cv.use_id(sensor.Sensor),
            cv.Optional(CONF_HUMIDITY): cv.use_id(sensor.Sensor),
            cv.Optional(CONF_BATTERY_LEVEL): cv.use_id(sensor.Sensor),
            cv.Optional(CONF_SIGNAL_LEVEL): cv.use_id(sensor.Sensor),
            cv.Optional(CONF_TEMPERATURE_DECIMALS, default=1): cv.int_range(min=0, max=3),
            cv.Optional(CONF_HUMIDITY_DECIMALS, default=1): cv.int_range(min=0, max=3),
            cv.Optional(CONF_SHOW_CELSIUS, default=False): cv.boolean,
            cv.Optional(CONF_SHOW_PERCENT, default=False): cv.boolean,
            cv.Optional(CONF_TEMPERATURE_NUMBER, default={"name": "LCD Temperature"}): number.number_schema(
                SegLCDTempHumTemperatureNumber, icon="mdi:thermometer"
            ),
            cv.Optional(CONF_HUMIDITY_NUMBER, default={"name": "LCD Humidity"}): number.number_schema(
                SegLCDTempHumHumidityNumber, icon="mdi:water-percent"
            ),
            cv.Optional(CONF_TEMPERATURE_TEXT): text.text_schema(
                SegLCDTempHumTemperatureText, icon="mdi:thermometer", mode="text"
            ),
            cv.Optional(CONF_HUMIDITY_TEXT): text.text_schema(
                SegLCDTempHumHumidityText, icon="mdi:water-percent", mode="text"
            ),
            cv.Optional(CONF_BATTERY_LEVEL_NUMBER, default={"name": "LCD Battery"}): number.number_schema(
                SegLCDTempHumBatteryLevelNumber, icon="mdi:battery"
            ),
            cv.Optional(CONF_SIGNAL_LEVEL_NUMBER, default={"name": "LCD Signal"}): number.number_schema(
                SegLCDTempHumSignalLevelNumber, icon="mdi:wifi"
            ),
            cv.Optional(CONF_CELSIUS_SWITCH, default={"name": "LCD Celsius Flag"}): switch.switch_schema(
                SegLCDTempHumCelsiusSwitch, icon="mdi:temperature-celsius"
            ),
            cv.Optional(CONF_PERCENT_SWITCH, default={"name": "LCD Percent Flag"}): switch.switch_schema(
                SegLCDTempHumPercentSwitch, icon="mdi:percent"
            ),
        }
    )
    .extend(cv.polling_component_schema("10s"))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID], config[CONF_SUBADDRESS])
    await cg.register_component(var, config)

    transport = await cg.get_variable(config[CONF_SEGLCD_TRANSPORT_ID])
    cg.add(var.set_transport(transport))

    if CONF_TEMPERATURE in config:
        temperature = await cg.get_variable(config[CONF_TEMPERATURE])
        cg.add(var.set_temperature_sensor(temperature))

    if CONF_HUMIDITY in config:
        humidity = await cg.get_variable(config[CONF_HUMIDITY])
        cg.add(var.set_humidity_sensor(humidity))

    if CONF_BATTERY_LEVEL in config:
        battery_level = await cg.get_variable(config[CONF_BATTERY_LEVEL])
        cg.add(var.set_battery_level_sensor(battery_level))

    if CONF_SIGNAL_LEVEL in config:
        signal_level = await cg.get_variable(config[CONF_SIGNAL_LEVEL])
        cg.add(var.set_signal_level_sensor(signal_level))

    cg.add(var.set_temperature_decimals(config[CONF_TEMPERATURE_DECIMALS]))
    cg.add(var.set_humidity_decimals(config[CONF_HUMIDITY_DECIMALS]))
    cg.add(var.set_show_celsius(config[CONF_SHOW_CELSIUS]))
    cg.add(var.set_show_percent(config[CONF_SHOW_PERCENT]))

    temperature_number = await number.new_number(
        config[CONF_TEMPERATURE_NUMBER], min_value=-40, max_value=99.9, step=0.1
    )
    await cg.register_parented(temperature_number, config[CONF_ID])
    cg.add(var.set_temperature_number(temperature_number))

    humidity_number = await number.new_number(
        config[CONF_HUMIDITY_NUMBER], min_value=0, max_value=100, step=0.1
    )
    await cg.register_parented(humidity_number, config[CONF_ID])
    cg.add(var.set_humidity_number(humidity_number))

    if CONF_TEMPERATURE_TEXT in config:
        temperature_text = await text.new_text(config[CONF_TEMPERATURE_TEXT], min_length=0, max_length=5)
        await cg.register_parented(temperature_text, config[CONF_ID])
        cg.add(var.set_temperature_text(temperature_text))

    if CONF_HUMIDITY_TEXT in config:
        humidity_text = await text.new_text(config[CONF_HUMIDITY_TEXT], min_length=0, max_length=3)
        await cg.register_parented(humidity_text, config[CONF_ID])
        cg.add(var.set_humidity_text(humidity_text))

    battery_level_number = await number.new_number(
        config[CONF_BATTERY_LEVEL_NUMBER], min_value=0, max_value=4, step=1
    )
    await cg.register_parented(battery_level_number, config[CONF_ID])
    cg.add(var.set_battery_level_number(battery_level_number))

    signal_level_number = await number.new_number(
        config[CONF_SIGNAL_LEVEL_NUMBER], min_value=0, max_value=4, step=1
    )
    await cg.register_parented(signal_level_number, config[CONF_ID])
    cg.add(var.set_signal_level_number(signal_level_number))

    celsius_switch = await switch.new_switch(config[CONF_CELSIUS_SWITCH])
    await cg.register_parented(celsius_switch, config[CONF_ID])
    cg.add(var.set_celsius_switch(celsius_switch))

    percent_switch = await switch.new_switch(config[CONF_PERCENT_SWITCH])
    await cg.register_parented(percent_switch, config[CONF_ID])
    cg.add(var.set_percent_switch(percent_switch))

    cg.add_library("https://github.com/petrkr/SegLCDLib.git#develop", None)
