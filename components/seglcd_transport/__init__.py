import esphome.codegen as cg
import esphome.config_validation as cv

DEPENDENCIES = ["i2c"]

CONFIG_SCHEMA = cv.Schema({})


async def to_code(config):
    cg.add_library("Wire", None)
