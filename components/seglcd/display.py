import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome.const import CONF_ADDRESS, CONF_ID, CONF_LAMBDA
from esphome.core import CORE

CONF_MODEL = "model"
CONF_SUBADDRESS = "subaddress"
CONF_LIBRARY_SOURCE = "library_source"
CONF_LIBRARY_VERSION = "library_version"

SUPPORTED_MODELS = {
    "pcf85134_xygax": {
        "label": "PCF85134 Xygax",
        "define": "SEGLCD_ENABLE_PCF85134_XYGAX",
        "enum": "esphome::seglcd::SEGLCD_MODEL_PCF85134_XYGAX",
    },
}

seglcd_ns = cg.esphome_ns.namespace("seglcd")
SegLCDDisplay = seglcd_ns.class_("SegLCDDisplay", cg.PollingComponent)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SegLCDDisplay),
            cv.Required(CONF_MODEL): cv.one_of(*SUPPORTED_MODELS, lower=True),
            cv.Optional(CONF_SUBADDRESS, default=0): cv.int_range(min=0, max=7),
            cv.Optional(
                CONF_LIBRARY_SOURCE,
                default="https://github.com/petrkr/SegLCDLib.git",
            ): cv.Any(cv.string, None),
            cv.Optional(CONF_LIBRARY_VERSION, default="refactor"): cv.Any(cv.string, None),
            cv.Optional(CONF_LAMBDA): cv.lambda_,
        }
    )
    .extend(cv.polling_component_schema("1s"))
    .extend(i2c.i2c_device_schema(0x38))
)


async def to_code(config):
    if not CORE.using_arduino:
        raise cv.Invalid("seglcd requires the Arduino framework")

    model = SUPPORTED_MODELS[config[CONF_MODEL]]

    var = cg.new_Pvariable(config[CONF_ID], config[CONF_ADDRESS], config[CONF_SUBADDRESS])
    await cg.register_component(var, config)
    cg.add(var.set_model(cg.RawExpression(model["enum"])))
    cg.add(var.set_model_name(model["label"]))

    bus = await cg.get_variable(config[i2c.CONF_I2C_ID])
    cg.add(var.set_i2c_bus(bus))

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA],
            [(SegLCDDisplay.operator("ref"), "it")],
            return_type=cg.void,
        )
        cg.add(var.set_writer(lambda_))

    cg.add_build_flag("-DSEGLCD_DISABLE_ALL_LCDS")
    cg.add_build_flag("-DSEGLCD_DISABLE_ARDUINO_TRANSPORT")
    cg.add_build_flag(f"-D{model['define']}")
    if config[CONF_LIBRARY_SOURCE] is not None:
        cg.add_library("SegLCDLib", config[CONF_LIBRARY_VERSION], config[CONF_LIBRARY_SOURCE])
    else:
        cg.add_library("SegLCDLib", None)
