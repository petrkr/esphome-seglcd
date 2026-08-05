import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome.const import CONF_ADDRESS, CONF_ID, CONF_LAMBDA
from esphome.core import CORE
from esphome.cpp_generator import LambdaExpression

CONF_MODEL = "model"
CONF_SUBADDRESS = "subaddress"
CONF_LIBRARY_SOURCE = "library_source"

SUPPORTED_MODELS = {
    "pcf85134_xygax_seg_i2c": {
        "label": "PCF85134 Xygax SEG_I2C",
        "define": "SEGLCD_ENABLE_PCF85134_XYGAX_SEG_I2C",
        "class_name": "SegLCDDisplayPcf85134XygaxSegI2c",
        "lcd_class_name": "SegLCD_PCF85134_XygaxSegI2C",
        # No extra symbols on this model.
        "symbols": {},
    },
    "pcf85176_4dr821b": {
        "label": "PCF85176 4DR821B",
        "define": "SEGLCD_ENABLE_PCF85176_4DR821B",
        "class_name": "SegLCDDisplayPcf85176_4dr821b",
        "lcd_class_name": "SegLCD_PCF85176_4DR821B",
        # YAML key -> SymbolFlags enum value on the model's own C++ class.
        # Each becomes a declarative `tilda: true/!lambda` option instead of
        # a dedicated set_tilda()-style method on the component.
        "symbols": {
            "tilda": "SYMBOL_TILDA",
            "arrow": "SYMBOL_ARROW",
        },
    },
}

seglcd_ns = cg.esphome_ns.namespace("seglcd")
# Vendor library classes live in the global C++ namespace, not esphome::seglcd.
global_ns = cg.global_ns


def _model_schema(config):
    model = SUPPORTED_MODELS[config[CONF_MODEL]]
    seglcd_class = seglcd_ns.class_(model["class_name"], cg.PollingComponent)

    return (
        cv.Schema(
            {
                cv.GenerateID(): cv.declare_id(seglcd_class),
                cv.Required(CONF_MODEL): cv.one_of(*SUPPORTED_MODELS, lower=True),
                cv.Optional(CONF_SUBADDRESS, default=0): cv.int_range(min=0, max=7),
                cv.Optional(
                    CONF_LIBRARY_SOURCE,
                    default="https://github.com/petrkr/SegLCDLib.git#develop",
                ): cv.Any(cv.string, None),
                cv.Optional(CONF_LAMBDA): cv.lambda_,
                **{
                    cv.Optional(key): cv.templatable(cv.boolean)
                    for key in model["symbols"]
                },
            }
        )
        .extend(cv.polling_component_schema("1s"))
        .extend(i2c.i2c_device_schema(0x38))
    )


def CONFIG_SCHEMA(config):
    config = cv.Schema(
        {cv.Required(CONF_MODEL): cv.one_of(*SUPPORTED_MODELS, lower=True)},
        extra=cv.ALLOW_EXTRA,
    )(config)
    return _model_schema(config)(config)


async def to_code(config):
    if not CORE.using_arduino:
        raise cv.Invalid("seglcd requires the Arduino framework")

    model = SUPPORTED_MODELS[config[CONF_MODEL]]
    seglcd_ns.class_(model["class_name"], cg.PollingComponent)
    lcd_class = global_ns.class_(model["lcd_class_name"])

    var = cg.new_Pvariable(config[CONF_ID], config[CONF_ADDRESS], config[CONF_SUBADDRESS])
    await cg.register_component(var, config)
    cg.add(var.set_model_name(model["label"]))

    bus = await cg.get_variable(config[i2c.CONF_I2C_ID])
    cg.add(var.set_i2c_bus(bus))

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA],
            [(lcd_class.operator("ref"), "lcd")],
            return_type=cg.void,
        )
        cg.add(var.set_writer(lambda_))

    for key, symbol in model["symbols"].items():
        if key not in config:
            continue
        # cg.templatable() with a non-None output_type always yields a
        # callable (a real lambda for `!lambda`, a wrapped literal for a
        # plain true/false) - call it to get the bool, then feed that into
        # the setSymbol() action lambda that add_update_action() runs.
        state = await cg.templatable(config[key], [], cg.bool_)
        action = LambdaExpression(
            [f"lcd.setSymbol({model['lcd_class_name']}::{symbol}, ({state})());"],
            [(lcd_class.operator("ref"), "lcd")],
            capture="",
            return_type=cg.void,
        )
        cg.add(var.add_update_action(action))

    cg.add_build_flag("-DSEGLCD_DISABLE_ALL_LCDS")
    cg.add_build_flag("-DSEGLCD_DISABLE_ARDUINO_TRANSPORT")
    cg.add_build_flag(f"-D{model['define']}")
    if config[CONF_LIBRARY_SOURCE] is not None:
        cg.add_library("SegLCDLib", None, config[CONF_LIBRARY_SOURCE])
    else:
        cg.add_library("SegLCDLib", None)
