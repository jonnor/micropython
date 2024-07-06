// distance.c
#include "py/dynruntime.h"

#include <stdint.h>


static mp_obj_t
euclidean_argmin(mp_obj_t vectors_obj, mp_obj_t point_obj) {

    // Checking first arg
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(vectors_obj, &bufinfo, MP_BUFFER_RW);

// XXX: switching this to 0 makes issue not reproduce. Unsure why...
#if 1
    if (bufinfo.typecode != 'B') {
        mp_raise_ValueError(MP_ERROR_TEXT("expecting B array (uint8)"));
    }
#endif

    // hardcoded return values
    uint32_t min_dist = 0;
    const uint16_t min_index = 1;

    return mp_obj_new_tuple(2, ((mp_obj_t []) {
        mp_obj_new_int(min_index),
        mp_obj_new_int(min_dist),
    }));
 }
static MP_DEFINE_CONST_FUN_OBJ_2(euclidian_argmin_obj, euclidean_argmin);


// This is the entry point and is called when the module is imported
mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    // This must be first, it sets up the globals dict and other things
    MP_DYNRUNTIME_INIT_ENTRY

    mp_store_global(MP_QSTR_euclidean_argmin, MP_OBJ_FROM_PTR(&euclidian_argmin_obj));

    // This must be last, it restores the globals dict
    MP_DYNRUNTIME_INIT_EXIT
}
