SRC_FILES += lv_font.c
SRC_FILES += lv_font_fmt_txt.c
SRC_FILES += lv_font_roboto_12.c
SRC_FILES += lv_font_roboto_16.c
SRC_FILES += lv_font_roboto_22.c
SRC_FILES += lv_font_roboto_28.c
SRC_FILES += lv_font_unscii_8.c

DEPPATH += --dep-path $(LVGL_DIR)/lvgl/src/lv_font
VPATH += :$(LVGL_DIR)/lvgl/src/lv_font

CFLAGS += "-I$(LVGL_DIR)/lvgl/src/lv_font"
