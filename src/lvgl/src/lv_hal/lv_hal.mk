SRC_FILES += lv_hal_disp.c
SRC_FILES += lv_hal_indev.c
SRC_FILES += lv_hal_tick.c

DEPPATH += --dep-path $(LVGL_DIR)/lvgl/src/lv_hal
VPATH += :$(LVGL_DIR)/lvgl/src/lv_hal

CFLAGS += "-I$(LVGL_DIR)/lvgl/src/lv_hal"
