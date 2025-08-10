include("$(MPY_DIR)/extmod/asyncio")

require("upysh")
require("aioble")
require("neopixel")
require("aiorepl")

base = './../../emlearn-micropython/src/'
module("emlearn_trees.py", base_path=base+'emlearn_trees')
module("emlearn_fft.py", base_path=base+'emlearn_fft')
