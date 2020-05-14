//
// Created by jack on 5/14/20.
//

#include "display.h"

static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(DISPLAY_TWI_INSTANCE_ID);
static volatile bool m_xfer_done = false;

/*A static or global variable to store the buffers*/
static lv_disp_buf_t disp_buf;

/*Static or global buffer(s). The second buffer is optional*/
static lv_color_t buf_1[DISP_BUF_SIZE];
//static lv_color_t buf_2[LV_HOR_RES_MAX * 10];


// Display Task
void display_task(void * arg) {
    display_init();

    lv_disp_drv_t disp_drv;                 /*A variable to hold the drivers. Can be local variable*/
    lv_disp_drv_init(&disp_drv);            /*Basic initialization*/
    disp_drv.buffer = &disp_buf;            /*Set an initialized buffer*/
    disp_drv.flush_cb = my_flush_cb;        /*Set a flush callback to draw to the display*/
    lv_disp_t * disp;
    disp = lv_disp_drv_register(&disp_drv); /*Register the driver and save the created display objects*/

    /*Initialize `disp_buf` with the buffer(s) */
    lv_disp_buf_init(&disp_buf, buf_1, NULL, DISP_BUF_SIZE);

    lv_theme_mono_init(0 /* hue */,NULL /* use LV_FONT_DEFAULT */);
    lv_theme_set_current( lv_theme_get_mono() );

    while(1) {
        lv_task_handler();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}


void display_flush(lv_disp_t * disp, const lv_area_t * area, lv_color_t * color_p) {
    uint8_t row1 = area->y1 >> 3;
    uint8_t row2 = area->y2 >> 3;
    uint8_t *buf = (uint8_t*)color_p;
    uint8_t *send_buf[1024];
    uint32_t buf_index = 0;

    uint8_t row;
    for(row = row1; row <= row2; row++) {
        display_start_send(row, area->x1 );
        for(uint16_t x = area->x1; x <= area->x2; x++) {
            send_buf[buf_index](*buf);
            buf++;
            buf_index++;
        }
        oledEndSend();
    }

    lv_disp_flush_ready(disp);         /* Indicate you are ready with the flushing*/
}

void display_init(void) {
    ret_code_t err_code;

    // init TWI/I2C
    const nrf_drv_twi_config_t twi_display_config = {
            .scl                = NRF_GPIO_PIN_MAP(0, 30),
            .sda                = NRF_GPIO_PIN_MAP(0, 31),
            .frequency          = NRF_DRV_TWI_FREQ_400K,
            .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
            .clear_bus_init     = true
    };

    err_code = nrf_drv_twi_init(&m_twi, &twi_display_config, display_twi_handler, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);
    
    // init display
    display_turn_off();
    display_twi_write_cmd(0x00);    //Set Memory Addressing Mode
    display_twi_write_cmd(0x10);    //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    display_twi_write_cmd(0x40);    //Set Page Start Address for Page Addressing Mode,0-7
    display_twi_write_cmd(0xB0);    //Set COM Output Scan Direction
    display_twi_write_cmd(0x81);    //---set low column address
    display_twi_write_cmd(0xCF);    //---set high column address
    display_twi_write_cmd(0xA1);    //--set start line address
    display_twi_write_cmd(0xA6);    //--set contrast control register
    display_twi_write_cmd(0xA8);
    display_twi_write_cmd(0x3F);    //--set segment re-map 0 to 127
    display_twi_write_cmd(0xC8);    //--set normal display
    display_twi_write_cmd(0xD3);    //--set multiplex ratio(1 to 64)
    display_twi_write_cmd(0x00);    //
    display_twi_write_cmd(0xD5);    //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    display_twi_write_cmd(0x80);    //-set display offset
    display_twi_write_cmd(0xD9);    //-not offset
    display_twi_write_cmd(0xF1);    //--set display clock divide ratio/oscillator frequency
    display_twi_write_cmd(0xDA);   //--set divide ratio
    display_twi_write_cmd(0x12);    //--set pre-charge period
    display_twi_write_cmd(0xDB);    //
    display_twi_write_cmd(0x40);    //--set com pins hardware configuration
    display_twi_write_cmd(0x8D);    //--set vcomh
    display_twi_write_cmd(0x14);    //0x20,0.77xVcc
    display_twi_write_cmd(0xAF);    //--set DC-DC enable
    display_turn_on();
}

void display_twi_write_cmd(uint8_t cmd) {
    ret_code_t  err_code;
    uint8_t cmd_array[2] = {SSD1306_CMD_REG, cmd};
    m_xfer_done = false;
    err_code = nrf_drv_twi_tx(&m_twi, SSD1306_ADDR, cmd_array, sizeof(cmd_array), false);
    APP_ERROR_CHECK(err_code);
    while(m_xfer_done == false);
}

void display_turn_on(void) {
    ret_code_t  err_code;
    uint8_t cmd_array[2] = {SSD1306_CMD_REG, SSD1306_CMD_DISPLAY_ON};
    m_xfer_done = false;
    err_code = nrf_drv_twi_tx(&m_twi, SSD1306_ADDR, cmd_array, sizeof(cmd_array), false);
    APP_ERROR_CHECK(err_code);
    while(m_xfer_done == false);
}

void display_turn_off(void) {
    ret_code_t  err_code;
    uint8_t cmd_array[2] = {SSD1306_CMD_REG, SSD1306_CMD_DISPLAY_OFF};
    m_xfer_done = false;
    err_code = nrf_drv_twi_tx(&m_twi, SSD1306_ADDR, cmd_array, sizeof(cmd_array), false);
    APP_ERROR_CHECK(err_code);
    while(m_xfer_done == false);
}

void display_start_send(uint8_t row, uint8_t col) {
    display_twi_write_cmd(0xB0 | row);
    // for SH1106 - remove for SSD1306
    col += 2;

    display_twi_write_cmd(0x00 | (col & 0xF));          // col LS 4 bits
    display_twi_write_cmd(0x10 | ((col >> 4) & 0xF));   // col LS 4 bits
}

void display_twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context) {
    switch (p_event->type) {
        case NRF_DRV_TWI_EVT_DONE: {
            m_xfer_done = true;
            break;
        }
        default:
            break;
    }
}