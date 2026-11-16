/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Jon Nordby
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/mphal.h"
#include "py/runtime.h"
#include "extmod/vfs.h"

#include <stddef.h>
#include <gpio.h>
#include <timer.h>
#include <stdio.h>
#include <csr.h>
#include <uart.h>
#include <math.h>

#define printf uart_printf

// PDM audio input
// base address is 0x800_0280 (0x800_0000 + 0x40*10)
volatile uint32_t* pdm_enable = (volatile uint32_t*)0x8000280;
volatile uint32_t* pdm_period = (volatile uint32_t*)0x8000284;
volatile uint32_t* pdm_select = (volatile uint32_t*)0x8000288;
volatile uint32_t* pdm_sample = (volatile uint32_t*)0x800028c;
volatile uint16_t* pdm_sample16 = (volatile uint16_t*)0x800028c;
volatile uint8_t* pdm_sample8 = (volatile uint8_t*)0x800028c;

// XXX: buffer used in tinyqv_pdm_isr.s
#define BUFFER_LENGTH (1*16000)
volatile int16_t buffer[BUFFER_LENGTH];
volatile uint32_t buffer_idx;



    printf("init start\n-----------------------------------\n");

    // Set all outputs to regular mode (not debug)
    set_debug_sel(0xff);

    // gpio 4
    enable_all_outputs();

    // Set all output 6 to peripheral 10
    set_gpio_func(1, 10);

    for(int i=0; i<BUFFER_LENGTH; ++i)
    	    buffer[i] = 22223;

    *pdm_period = 64;
    *pdm_select = 0;
    *pdm_enable = 1;

#if 1
    uint32_t t0 = read_time();
    uint32_t t1 = t0 + 100 * 1000;
    while (read_time() < t1)
    	    ;

    enable_interrupt(10);

    while (buffer_idx < BUFFER_LENGTH-100)
	    ;
    *pdm_enable = 0;

    printf("BEGIN\n");
    for(int i=0; i<BUFFER_LENGTH; i+=8) {
	    for(int j=0; j<8; ++j) {
		    printf("%6d", buffer[i+j]);
	    }
	    printf("\n");
    }
    printf("END\n");

#endif

}



typedef struct _tinyqv_psram_obj_t {
    mp_obj_base_t base;
    uint32_t base_addr;
    uint32_t size;
} tinyqv_psram_obj_t;

const mp_obj_type_t tinyqv_psram_type;

static const tinyqv_psram_obj_t tinyqv_psram_fs_obj = {
    .base = { &tinyqv_psram_type },
    .base_addr = TINYQV_PSRAM_FS_BASE_ADDR,
    .size = TINYQV_PSRAM_FS_DEFAULT_SIZE,
};

static mp_obj_t tinyqv_psram_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Parse arguments
    enum { ARG_start, ARG_len };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_start, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_len,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_start].u_int == -1 && args[ARG_len].u_int == -1) {
        // Default singleton object that accesses default filesystem
        return MP_OBJ_FROM_PTR(&tinyqv_psram_fs_obj);
    }

    tinyqv_psram_obj_t *self = mp_obj_malloc(tinyqv_psram_obj_t, &tinyqv_psram_type);

    mp_int_t start = args[ARG_start].u_int;
    if (start == -1) {
        start = 0;
    } else if (start < 0 || start >= TINYQV_PSRAM_FS_MAX_SIZE || start % TINYQV_PSRAM_FS_BLOCK_SIZE != 0) {
        mp_raise_ValueError(NULL);
    }

    mp_int_t len = args[ARG_len].u_int;
    if (len == -1) {
        len = TINYQV_PSRAM_FS_DEFAULT_SIZE;
    } else if (len < 0 || start + len > TINYQV_PSRAM_FS_MAX_SIZE || len % TINYQV_PSRAM_FS_BLOCK_SIZE != 0) {
        mp_raise_ValueError(NULL);
    }

    self->base_addr = TINYQV_PSRAM_FS_BASE_ADDR + start;
    self->size = len;

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t tinyqv_psram_readblocks(size_t n_args, const mp_obj_t *args) {
    tinyqv_psram_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t offset = mp_obj_get_int(args[1]) * TINYQV_PSRAM_FS_BLOCK_SIZE;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);
    if (n_args == 4) {
        offset += mp_obj_get_int(args[3]);
    }
    memcpy(bufinfo.buf, (const void*)(self->base_addr + offset), bufinfo.len);
    mp_event_handle_nowait();
    return MP_OBJ_NEW_SMALL_INT(0);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(tinyqv_psram_readblocks_obj, 3, 4, tinyqv_psram_readblocks);


static const mp_rom_map_elem_t tinyqv_psram_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&tinyqv_psram_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&tinyqv_psram_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&tinyqv_psram_ioctl_obj) },
};
static MP_DEFINE_CONST_DICT(tinyqv_psram_locals_dict, tinyqv_psram_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    tinyqv_psram_type,
    MP_QSTR_PSRAMBlockDevice,
    MP_TYPE_FLAG_NONE,
    make_new, tinyqv_psram_make_new,
    buffer, tinyqv_psram_get_buffer,
    locals_dict, &tinyqv_psram_locals_dict
    );

static const mp_rom_map_elem_t psram_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),            MP_ROM_QSTR(MP_QSTR__psram) },
    { MP_ROM_QSTR(MP_QSTR_PSRAMBlockDevice),    MP_ROM_PTR(&tinyqv_psram_type) },
};
static MP_DEFINE_CONST_DICT(psram_module_globals, psram_module_globals_table);

const mp_obj_module_t mp_module_psram = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&psram_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR__psram, mp_module_psram);
