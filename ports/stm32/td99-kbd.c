#include "py/runtime.h"
#include "py/mphal.h"

#include "kbd.h"
#include "bufhelper.h"
#include "td99-kbd.h"
#include "ti-2def2.h"

#if MICROPY_HW_HAS_LCD

// Puffer aus kbd.c verwenden
extern struct fifo_descriptor kbd_buf;

// Objektstruktur für das Touch-Keyboard
typedef struct _tiger_kbd_obj_t {
    mp_obj_base_t base;
    // TODO: Hardware-Parameter hier einfügen (Pins usw.)
} tiger_kbd_obj_t;

// Konstruktor
static mp_obj_t tiger_kbd_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Bei Bedarf Parameter auswerten und Hardware initialisieren
    // TODO: Benötigte Argumente definieren und auswerten

    kbd_init();

    tiger_kbd_obj_t *self = mp_obj_malloc(tiger_kbd_obj_t, type);
    // Weitere Initialisierungen können hier erfolgen
    return MP_OBJ_FROM_PTR(self);
}

// \method setKeyTable(table)
// Erwartet ein Bytes-Objekt mit 128 Einträgen und setzt die Key-Codes.
static mp_obj_t tiger_kbd_set_key_table(mp_obj_t self_in, mp_obj_t table) {
    if (!mp_obj_is_type(table, &mp_type_bytes)) {
        mp_raise_TypeError(MP_ERROR_TEXT("parameter must be bytes"));
    }

    uint8_t data[1];
    mp_buffer_info_t bufinfo;
    pyb_buf_get_for_send(table, &bufinfo, data);
    if (bufinfo.len != 128) {
        mp_raise_ValueError(MP_ERROR_TEXT("table must have 128 bytes"));
    }

    kbd_set_key_table((uint8_t *)bufinfo.buf);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(tiger_kbd_set_key_table_obj, tiger_kbd_set_key_table);

// \method setKeyShiftTable(table)
static mp_obj_t tiger_kbd_set_key_shift_table(mp_obj_t self_in, mp_obj_t table) {
    if (!mp_obj_is_type(table, &mp_type_bytes)) {
        mp_raise_TypeError(MP_ERROR_TEXT("parameter must be bytes"));
    }

    uint8_t data[1];
    mp_buffer_info_t bufinfo;
    pyb_buf_get_for_send(table, &bufinfo, data);
    if (bufinfo.len != 128) {
        mp_raise_ValueError(MP_ERROR_TEXT("table must have 128 bytes"));
    }

    kbd_set_key_shift_table((uint8_t *)bufinfo.buf);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(tiger_kbd_set_key_shift_table_obj, tiger_kbd_set_key_shift_table);

// \method setKeyAttributeCode(table)
static mp_obj_t tiger_kbd_set_key_attr_table(mp_obj_t self_in, mp_obj_t table) {
    if (!mp_obj_is_type(table, &mp_type_bytes)) {
        mp_raise_TypeError(MP_ERROR_TEXT("parameter must be bytes"));
    }

    uint8_t data[1];
    mp_buffer_info_t bufinfo;
    pyb_buf_get_for_send(table, &bufinfo, data);
    if (bufinfo.len != 128) {
        mp_raise_ValueError(MP_ERROR_TEXT("table must have 128 bytes"));
    }

    kbd_set_key_attribute_table((uint8_t *)bufinfo.buf);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(tiger_kbd_set_key_attr_table_obj, tiger_kbd_set_key_attr_table);

// \method keyClickOn()
static mp_obj_t tiger_kbd_key_click_on(mp_obj_t self_in) {
    RAM1_KEY_CLICK = true;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(tiger_kbd_key_click_on_obj, tiger_kbd_key_click_on);

// \method keyClickOff()
static mp_obj_t tiger_kbd_key_click_off(mp_obj_t self_in) {
    RAM1_KEY_CLICK = false;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(tiger_kbd_key_click_off_obj, tiger_kbd_key_click_off);

// \method setRepeat(delay, freq)
static mp_obj_t tiger_kbd_set_repeat(mp_obj_t self_in, mp_obj_t d, mp_obj_t f) {
    uint8_t delay = mp_obj_get_int(d);
    uint8_t freq = mp_obj_get_int(f);
    kbd_set_auto_repeat(delay, freq);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(tiger_kbd_set_repeat_obj, tiger_kbd_set_repeat);

// \method setDipSwitch(list_of_int)
static mp_obj_t tiger_kbd_set_dip_switches(mp_obj_t self_in, mp_obj_t dips) {
    if (!mp_obj_is_type(dips, &mp_type_list)) {
        mp_raise_TypeError(MP_ERROR_TEXT("parameter must be a list"));
    }
    mp_obj_list_t *list = MP_OBJ_TO_PTR(dips);
    if (list->len != 16) {
        mp_raise_ValueError(MP_ERROR_TEXT("list must have length 16"));
    }
    uint8_t col[16];
    for (size_t i = 0; i < 16; ++i) {
        col[i] = mp_obj_get_int(list->items[i]);
    }
    kbd_set_dip_switches(16, col);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(tiger_kbd_set_dip_switches_obj, tiger_kbd_set_dip_switches);

// \method getBufferSize()
static mp_obj_t tiger_kbd_get_buf_size(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(kbd_buf.cnt);
}
static MP_DEFINE_CONST_FUN_OBJ_1(tiger_kbd_get_buf_size_obj, tiger_kbd_get_buf_size);

// \method read(bytes)
static mp_obj_t tiger_kbd_read(mp_obj_t self_in, mp_obj_t bytes) {
    uint8_t len = mp_obj_get_int(bytes);
    if (kbd_buf.cnt == 0) {
        return mp_const_none;
    }
    if (kbd_buf.cnt < len) {
        len = kbd_buf.cnt;
    }
    kbd_buf.busy = true;
    vstr_t vstr;
    vstr_init(&vstr, len);
    if (!read_from_kbd_buf((uint8_t *)vstr.buf, len)) {
        kbd_buf.busy = false;
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("buffer error"));
    }
    kbd_buf.busy = false;
    return mp_obj_new_bytes_from_vstr(&vstr);
}
static MP_DEFINE_CONST_FUN_OBJ_2(tiger_kbd_read_obj, tiger_kbd_read);

// Lokales Methodentableau
static const mp_rom_map_elem_t tiger_kbd_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_setKeyTable), MP_ROM_PTR(&tiger_kbd_set_key_table_obj) },
    { MP_ROM_QSTR(MP_QSTR_setKeyShiftTable), MP_ROM_PTR(&tiger_kbd_set_key_shift_table_obj) },
    { MP_ROM_QSTR(MP_QSTR_setKeyAttributeCode), MP_ROM_PTR(&tiger_kbd_set_key_attr_table_obj) },
    { MP_ROM_QSTR(MP_QSTR_keyClickOn), MP_ROM_PTR(&tiger_kbd_key_click_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_keyClickOff), MP_ROM_PTR(&tiger_kbd_key_click_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_setRepeat), MP_ROM_PTR(&tiger_kbd_set_repeat_obj) },
    { MP_ROM_QSTR(MP_QSTR_setDipSwitch), MP_ROM_PTR(&tiger_kbd_set_dip_switches_obj) },
    { MP_ROM_QSTR(MP_QSTR_getBufferSize), MP_ROM_PTR(&tiger_kbd_get_buf_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&tiger_kbd_read_obj) },
};
static MP_DEFINE_CONST_DICT(tiger_kbd_locals_dict, tiger_kbd_locals_dict_table);

// Type Objekt
MP_DEFINE_CONST_OBJ_TYPE(
    tiger_kbd_type,
    MP_QSTR_Keyboard,
    MP_TYPE_FLAG_NONE,
    make_new, tiger_kbd_make_new,
    locals_dict, &tiger_kbd_locals_dict
    );

#endif // MICROPY_HW_HAS_LCD