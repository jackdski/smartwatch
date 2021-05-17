SRC_FILES += lv_circ.c
SRC_FILES += lv_area.c
SRC_FILES += lv_task.c
SRC_FILES += lv_fs.c
SRC_FILES += lv_anim.c
SRC_FILES += lv_mem.c
SRC_FILES += lv_ll.c
SRC_FILES += lv_color.c
SRC_FILES += lv_txt.c
SRC_FILES += lv_math.c
SRC_FILES += lv_log.c
SRC_FILES += lv_gc.c
SRC_FILES += lv_utils.c
SRC_FILES += lv_async.c
SRC_FILES += lv_printf.c
SRC_FILES += lv_bidi.c


DEPPATH += --dep-path $(LVGL_DIR)/lvgl/src/lv_misc
VPATH += :$(LVGL_DIR)/lvgl/src/lv_misc

CFLAGS += "-I$(LVGL_DIR)/lvgl/src/lv_misc"
