#include <Arduino.h>
#include "epd_driver.h"
#include "esp_adc_cal.h"

#include "opensans8b.h"

enum alignment
{
  LEFT,
  RIGHT,
  CENTER
};
#define Black 0x00

long StartTime = 0;
long SleepTimer = 10;
int vref = 1100;

GFXfont currentFont;
uint8_t *framebuffer;

void BeginSleep();
void InitialiseSystem();
void DisplayWeather();
void edp_update();


void setup()
{
  InitialiseSystem();

  epd_poweron();
  epd_clear();
  DisplayWeather();
  edp_update();
  epd_poweroff_all();

  BeginSleep();
}

void loop() {}

void BeginSleep()
{
  epd_poweroff_all();
  esp_sleep_enable_timer_wakeup(SleepTimer * 1000000LL);
  Serial.println("Awake for : " + String((millis() - StartTime) / 1000.0, 3) + "-secs");
  Serial.println("Entering " + String(SleepTimer) + " (secs) of sleep time");
  Serial.println("Starting deep-sleep period...");
  esp_deep_sleep_start();
}

void InitialiseSystem()
{
  StartTime = millis();
  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println(String(__FILE__) + "\nStarting...");
  epd_init();
  framebuffer = (uint8_t *)ps_calloc(sizeof(uint8_t), EPD_WIDTH * EPD_HEIGHT / 2);
  if (!framebuffer)
    Serial.println("Memory alloc failed!");
  memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
}

void edp_update()
{
  epd_draw_grayscale_image(epd_full_screen(), framebuffer);
}

void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  epd_draw_rect(x, y, w, h, color, framebuffer);
}

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  epd_fill_rect(x, y, w, h, color, framebuffer);
}

void drawString(int x, int y, String text, alignment align)
{
  char *data = const_cast<char *>(text.c_str());
  int x1, y1;
  int w, h;
  int xx = x, yy = y;
  get_text_bounds(&currentFont, data, &xx, &yy, &x1, &y1, &w, &h, NULL);
  if (align == RIGHT)
    x = x - w;
  if (align == CENTER)
    x = x - w / 2;
  int cursor_y = y + h;
  write_string(&currentFont, data, &x, &cursor_y, framebuffer);
}

void setFont(GFXfont const &font)
{
  currentFont = font;
}

void DrawBattery(int x, int y)
{
  uint8_t percentage = 100;
  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
  if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF)
  {
    Serial.printf("eFuse Vref:%u mV", adc_chars.vref);
    vref = adc_chars.vref;
  }
  float voltage = analogRead(36) / 4096.0 * 6.566 * (vref / 1000.0);
  if (voltage > 1)
  {
    Serial.println("\nVoltage = " + String(voltage));
    percentage = 2836.9625 * pow(voltage, 4) - 43987.4889 * pow(voltage, 3) + 255233.8134 * pow(voltage, 2) - 656689.7123 * voltage + 632041.7303;
    if (voltage >= 4.20)
      percentage = 100;
    if (voltage <= 3.20)
      percentage = 0;
    drawRect(x + 25, y - 14, 40, 15, Black);
    fillRect(x + 65, y - 10, 4, 7, Black);
    fillRect(x + 27, y - 12, 36 * percentage / 100.0, 11, Black);
    drawString(x + 85, y - 14, String(percentage) + "%  " + String(voltage, 1) + "v", LEFT);
  }
}

void DisplayWeather()
{
  setFont(OpenSans8B);
  DrawBattery(600, 20);
}
