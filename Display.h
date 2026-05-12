#ifndef DISPLAY_H
#define DISPLAY_H

#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>

// =====================================================
// VARIÁVEIS GLOBAIS
// =====================================================

extern int latest_bpm;
extern float latest_spo2;
extern float latest_glucose;

// =====================================================
// TOUCH / DISPLAY
// =====================================================

#define XPT2046_IRQ   36
#define XPT2046_MOSI  32
#define XPT2046_MISO  39
#define XPT2046_CLK   25
#define XPT2046_CS    33

#define SCREEN_WIDTH   320
#define SCREEN_HEIGHT  240

static SPIClass touchscreenSPI = SPIClass(VSPI);
static XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);
static TFT_eSPI tft = TFT_eSPI();

// =====================================================
// BUFFER LVGL
// =====================================================

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[SCREEN_WIDTH * 20];

// =====================================================
// OBJETOS
// =====================================================

static lv_obj_t* label_bpm;
static lv_obj_t* label_spo2;
static lv_obj_t* label_glucose;

static lv_obj_t* scr_splash;
static lv_obj_t* scr_main;

// =====================================================
// SPLASH
// =====================================================

static unsigned long splash_start_ms = 0;

// 10 segundos
static const unsigned long SPLASH_MS = 10000;

// =====================================================
// DISPLAY FLUSH
// =====================================================

void my_disp_flush(
    lv_disp_drv_t *disp,
    const lv_area_t *area,
    lv_color_t *color_p
)
{
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;

    tft.startWrite();

    tft.setAddrWindow(
        area->x1,
        area->y1,
        w,
        h
    );

    tft.pushColors(
        (uint16_t *)&color_p->full,
        w * h,
        true
    );

    tft.endWrite();

    lv_disp_flush_ready(disp);
}

// =====================================================
// TOUCH
// =====================================================

void touchscreen_read(
    lv_indev_drv_t * indev_driver,
    lv_indev_data_t * data
)
{
    if (touchscreen.touched())
    {
        TS_Point p = touchscreen.getPoint();

        int x = map(
            p.x,
            200,
            3700,
            0,
            SCREEN_WIDTH
        );

        int y = map(
            p.y,
            240,
            3800,
            0,
            SCREEN_HEIGHT
        );

        data->point.x = x;
        data->point.y = y;

        data->state = LV_INDEV_STATE_PRESSED;
    }
    else
    {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

// =====================================================
// CARD
// =====================================================

lv_obj_t* create_card(
    lv_obj_t* parent,
    const char* icon_text,
    const char* title,
    const char* unit,
    int y
)
{
    lv_obj_t* card = lv_obj_create(parent);

    lv_obj_set_size(card, 290, 52);

    lv_obj_align(card, LV_ALIGN_TOP_MID, 0, y);

    lv_obj_set_style_radius(card, 16, 0);

    lv_obj_set_style_bg_color(
        card,
        lv_color_hex(0xFFFFFF),
        0
    );

    lv_obj_set_style_border_width(card, 0, 0);

    lv_obj_set_style_shadow_width(card, 12, 0);

    lv_obj_set_style_shadow_color(
        card,
        lv_color_hex(0xD6D6D6),
        0
    );

    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

    // =================================================
    // FUNDO ÍCONE
    // =================================================

    lv_obj_t* icon_bg = lv_obj_create(card);

    lv_obj_set_size(icon_bg, 36, 36);

    lv_obj_align(icon_bg, LV_ALIGN_LEFT_MID, 8, 0);

    lv_obj_set_style_radius(icon_bg, 10, 0);

    lv_obj_set_style_bg_color(
        icon_bg,
        lv_color_hex(0xE3C82B),
        0
    );

    lv_obj_set_style_border_width(icon_bg, 0, 0);

    lv_obj_clear_flag(icon_bg, LV_OBJ_FLAG_SCROLLABLE);

    // =================================================
    // ÍCONE
    // =================================================

    lv_obj_t* icon = lv_label_create(icon_bg);

    lv_label_set_text(icon, icon_text);

    lv_obj_set_style_text_font(
        icon,
        &lv_font_montserrat_18,
        0
    );

    lv_obj_set_style_text_color(
        icon,
        lv_color_hex(0xFFFFFF),
        0
    );

    lv_obj_center(icon);

    // =================================================
    // TÍTULO
    // =================================================

    lv_obj_t* lbl_title = lv_label_create(card);

    lv_label_set_text(lbl_title, title);

    lv_obj_set_style_text_font(
        lbl_title,
        &lv_font_montserrat_16,
        0
    );

    lv_obj_set_style_text_color(
        lbl_title,
        lv_color_hex(0x444444),
        0
    );

    lv_obj_align(lbl_title, LV_ALIGN_LEFT_MID, 58, -2);

    // =================================================
    // VALOR
    // =================================================

    lv_obj_t* lbl_value = lv_label_create(card);

    lv_label_set_text(lbl_value, "--");

    lv_obj_set_style_text_font(
        lbl_value,
        &lv_font_montserrat_22,
        0
    );

    lv_obj_set_style_text_color(
        lbl_value,
        lv_color_hex(0xE3C82B),
        0
    );

    lv_obj_align(lbl_value, LV_ALIGN_RIGHT_MID, -16, -8);

    // =================================================
    // UNIDADE
    // =================================================

    lv_obj_t* lbl_unit = lv_label_create(card);

    lv_label_set_text(lbl_unit, unit);

    lv_obj_set_style_text_font(
        lbl_unit,
        &lv_font_montserrat_12,
        0
    );

    lv_obj_set_style_text_color(
        lbl_unit,
        lv_color_hex(0x777777),
        0
    );

    lv_obj_align(lbl_unit, LV_ALIGN_RIGHT_MID, -16, 12);

    return lbl_value;
}

// =====================================================
// SPLASH SCREEN
// =====================================================

void lv_create_splash()
{
    scr_splash = lv_obj_create(NULL);

    lv_obj_set_style_bg_color(
        scr_splash,
        lv_color_hex(0xFFFFFF),
        0
    );

    lv_obj_clear_flag(
        scr_splash,
        LV_OBJ_FLAG_SCROLLABLE
    );

    // =================================================
    // SPINNER
    // =================================================

    lv_obj_t* spinner = lv_spinner_create(
        scr_splash,
        1000,
        60
    );

    lv_obj_set_size(spinner, 100, 100);

    lv_obj_align(
        spinner,
        LV_ALIGN_CENTER,
        0,
        -40
    );

    lv_obj_set_style_arc_width(
        spinner,
        8,
        LV_PART_INDICATOR
    );

    lv_obj_set_style_arc_color(
        spinner,
        lv_color_hex(0xE3C82B),
        LV_PART_INDICATOR
    );

    // =================================================
    // TEXTO
    // =================================================

    lv_obj_t* title = lv_label_create(scr_splash);

    lv_label_set_text(
        title,
        "CALIBRANDO..."
    );

    lv_obj_set_style_text_font(
        title,
        &lv_font_montserrat_26,
        0
    );

    lv_obj_set_style_text_color(
        title,
        lv_color_hex(0xE3C82B),
        0
    );

    lv_obj_align(
        title,
        LV_ALIGN_CENTER,
        0,
        45
    );

    lv_obj_t* subtitle = lv_label_create(scr_splash);

    lv_label_set_text(
        subtitle,
        "Aguarde enquanto o dispositivo\nrealiza a calibracao"
    );

    lv_obj_set_style_text_font(
        subtitle,
        &lv_font_montserrat_14,
        0
    );

    lv_obj_set_style_text_align(
        subtitle,
        LV_TEXT_ALIGN_CENTER,
        0
    );

    lv_obj_set_style_text_color(
        subtitle,
        lv_color_hex(0x666666),
        0
    );

    lv_obj_align(
        subtitle,
        LV_ALIGN_CENTER,
        0,
        82
    );
}

// =====================================================
// MAIN SCREEN
// =====================================================

void lv_create_main()
{
    scr_main = lv_obj_create(NULL);

    lv_obj_set_style_bg_color(
        scr_main,
        lv_color_hex(0xF7F7F7),
        0
    );

    lv_obj_clear_flag(
        scr_main,
        LV_OBJ_FLAG_SCROLLABLE
    );

    // =================================================
    // HEADER
    // =================================================

    lv_obj_t* header = lv_label_create(scr_main);

    lv_label_set_text(
        header,
        "MONITORAMENTO"
    );

    lv_obj_set_style_text_font(
        header,
        &lv_font_montserrat_24,
        0
    );

    lv_obj_set_style_text_color(
        header,
        lv_color_hex(0x333333),
        0
    );

    lv_obj_align(
        header,
        LV_ALIGN_TOP_LEFT,
        16,
        12
    );

    // =================================================
    // LINHA
    // =================================================

    lv_obj_t* line = lv_obj_create(scr_main);

    lv_obj_set_size(line, 180, 3);

    lv_obj_align(
        line,
        LV_ALIGN_TOP_LEFT,
        16,
        48
    );

    lv_obj_set_style_bg_color(
        line,
        lv_color_hex(0xE3C82B),
        0
    );

    lv_obj_set_style_border_width(
        line,
        0,
        0
    );

    // =================================================
    // CARD BATIMENTOS
    // =================================================

    label_bpm = create_card(
        scr_main,
            LV_SYMBOL_BELL,

        "Batimentos",
        "bpm",
        65
    );

    // =================================================
    // CARD OXIMETRIA
    // =================================================

    label_spo2 = create_card(
        scr_main,
        "O2",
        "Oximetria",
        "%",
        128
    );

    // =================================================
    // CARD GLICEMIA
    // =================================================

    label_glucose = create_card(
        scr_main,
        "\xEF\x81\x83",
        "Glicemia",
        "mg/dL",
        191
    );
}

// =====================================================
// UPDATE BPM / OXIMETRIA
// =====================================================

void update_sensor_values(
    int bpm,
    float spo2
)
{
    char buf[16];

    sprintf(buf, "%d", bpm);

    lv_label_set_text(
        label_bpm,
        buf
    );

    sprintf(buf, "%.0f", spo2);

    lv_label_set_text(
        label_spo2,
        buf
    );

    latest_bpm = bpm;
    latest_spo2 = spo2;
}

// =====================================================
// UPDATE GLICEMIA
// =====================================================

void add_glucose_measurement(float glucose)
{
    char buf[16];

    sprintf(buf, "%.0f", glucose);

    lv_label_set_text(
        label_glucose,
        buf
    );

    latest_glucose = glucose;
}

// =====================================================
// SETUP
// =====================================================

void lvgl_setup()
{
    Serial.println("Inicializando display");

    tft.begin();

    // HORIZONTAL
    tft.setRotation(0);

    lv_init();

    touchscreenSPI.begin(
        XPT2046_CLK,
        XPT2046_MISO,
        XPT2046_MOSI,
        XPT2046_CS
    );

    touchscreen.begin(
        touchscreenSPI
    );

    touchscreen.setRotation(0);

    lv_disp_draw_buf_init(
        &draw_buf,
        buf1,
        NULL,
        SCREEN_WIDTH * 20
    );

    static lv_disp_drv_t disp_drv;

    lv_disp_drv_init(&disp_drv);

    disp_drv.hor_res = SCREEN_WIDTH;
    disp_drv.ver_res = SCREEN_HEIGHT;

    disp_drv.flush_cb = my_disp_flush;

    disp_drv.draw_buf = &draw_buf;

    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;

    lv_indev_drv_init(&indev_drv);

    indev_drv.type = LV_INDEV_TYPE_POINTER;

    indev_drv.read_cb = touchscreen_read;

    lv_indev_drv_register(&indev_drv);

    // CRIA TELAS
    lv_create_splash();

    lv_create_main();

    // CARREGA SPLASH
    lv_scr_load(scr_splash);

    splash_start_ms = millis();
}

// =====================================================
// LOOP
// =====================================================

void display_loop()
{
    static bool changed = false;

    if (!changed)
    {
        if (
            millis() - splash_start_ms
            >= SPLASH_MS
        )
        {
            lv_scr_load_anim(
                scr_main,
                LV_SCR_LOAD_ANIM_FADE_ON,
                500,
                0,
                false
            );

            changed = true;
        }
    }

    lv_timer_handler();

    delay(5);
}

#endif.  troque apenas o icone de coracao que nao esta carregando