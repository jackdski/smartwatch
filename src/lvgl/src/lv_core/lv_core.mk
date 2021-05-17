SRC_FILES += lv_group.c
SRC_FILES += lv_indev.c
SRC_FILES += lv_disp.c
SRC_FILES += lv_obj.c
SRC_FILES += lv_refr.c
SRC_FILES += lv_style.c
SRC_FILES += lv_debug.c

DEPPATH += --dep-path $(LVGL_DIR)/lvgl/src/lv_core
VPATH += :$(LVGL_DIR)/lvgl/src/lv_core

CFLAGS += "-I$(LVGL_DIR)/lvgl/src/lv_core"
