SRC_FILES += lv_draw_basic.c
SRC_FILES += lv_draw.c
SRC_FILES += lv_draw_rect.c
SRC_FILES += lv_draw_label.c
SRC_FILES += lv_draw_line.c
SRC_FILES += lv_draw_img.c
SRC_FILES += lv_draw_arc.c
SRC_FILES += lv_draw_triangle.c
SRC_FILES += lv_img_decoder.c
SRC_FILES += lv_img_cache.c

DEPPATH += --dep-path $(LVGL_DIR)/lvgl/src/lv_draw
VPATH += :$(LVGL_DIR)/lvgl/src/lv_draw

CFLAGS += "-I$(LVGL_DIR)lvgl/src/lv_draw"
