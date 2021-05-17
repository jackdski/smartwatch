SRC_FILES += lv_theme_alien.c
SRC_FILES += lv_theme.c
SRC_FILES += lv_theme_default.c
SRC_FILES += lv_theme_night.c
SRC_FILES += lv_theme_templ.c
SRC_FILES += lv_theme_zen.c
SRC_FILES += lv_theme_material.c
SRC_FILES += lv_theme_nemo.c
SRC_FILES += lv_theme_mono.c

DEPPATH += --dep-path $(LVGL_DIR)/lvgl/src/lv_themes
VPATH += :$(LVGL_DIR)/lvgl/src/lv_themes

CFLAGS += "-I$(LVGL_DIR)/lvgl/src/lv_themes"
