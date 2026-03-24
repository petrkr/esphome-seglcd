import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome.const import CONF_ID, CONF_ADDRESS

DEPENDENCIES = ["i2c"]
MULTI_CONF = True

CONF_SEGLCD_TRANSPORT_ID = "seglcd_transport_id"

seglcd_transport_ns = cg.esphome_ns.namespace("seglcd_transport")
SegLCDTransport = seglcd_transport_ns.class_("SegLCDTransport", cg.Component)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SegLCDTransport),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(i2c.i2c_device_schema(0x38))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID], config[CONF_ADDRESS])
    await cg.register_component(var, config)

    bus = await cg.get_variable(config[i2c.CONF_I2C_ID])
    cg.add(var.set_i2c_bus(bus))

    cg.add_library("Wire", None)
