#define DEBUG 1
#define EPD 1

#include <Arduino.h>
#if EPD
#include "epd_driver.h"
#include "esp_adc_cal.h"
#endif
#include <WiFi.h>
#include <ArduinoJson.h>

#include "config.h"

#include "fonts/opensans8b.h"
#include "fonts/opensans12b.h"
#include "client.h"

enum alignment
{
  LEFT,
  RIGHT,
  CENTER
};
#define White 0xFF
#define LightGrey 0xBB
#define Grey 0x88
#define DarkGrey 0x44
#define Black 0x00

#define SCREEN_WIDTH EPD_WIDTH
#define SCREEN_HEIGHT EPD_HEIGHT

long StartTime = 0;
long DrawingTime = 0;
long SleepTimer = SLEEP_TIMER; //sec
int vref = 1100;

GFXfont currentFont;
uint8_t *framebuffer;

void BeginSleep();
void InitialiseSystem();
void DisplayStatus();
void DisplayCalendar(String *calendarData);
uint8_t StartWiFi();
void StopWiFi();
String ObtainCalendarData();
void edp_update();

void setup()
{
  esp_log_level_set("", ESP_LOG_VERBOSE);
  InitialiseSystem();
  if (StartWiFi() == WL_CONNECTED)
  {
    String response = ObtainCalendarData();

    StopWiFi();
#if EPD
    DrawingTime = millis();
    epd_poweron();
    epd_clear();
    DisplayStatus();
    DisplayCalendar(&response);
    edp_update();
    epd_poweroff_all();
#endif
  }
  BeginSleep();
}

void loop() {}

String ObtainCalendarData()
{
  String response = httpsGet();
  Serial.printf("Response: %s\n\n", response.c_str());

  return response;
}

uint8_t StartWiFi()
{
  Serial.println("\r\nConnecting to: " + String(WIFI_SSID));
  IPAddress dns(8, 8, 8, 8);
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.printf("STA: Failed!\n");
    WiFi.disconnect(false);
    delay(500);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("WiFi connected at: " + WiFi.localIP().toString());
  }
  else
    Serial.println("WiFi connection *** FAILED ***");
  return WiFi.status();
}

void StopWiFi()
{
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  Serial.println("WiFi switched Off");
}

void BeginSleep()
{
  epd_poweroff_all();
  esp_sleep_enable_timer_wakeup(SleepTimer * 1000000LL);
  Serial.println("Awake for : " + String((millis() - StartTime) / 1000.0, 3) + "-secs. Drawing time: " + String((millis() - DrawingTime) / 1000.0, 3) + "-secs.");
  Serial.println("Entering " + String(SleepTimer) + " (secs) of sleep time");
  Serial.println("Starting deep-sleep...");
  esp_deep_sleep_start();
}

void InitialiseSystem()
{
  StartTime = millis();
  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println(String(__FILE__) + "\nStarting...");
#if EPD
  epd_init();
  framebuffer = (uint8_t *)ps_calloc(sizeof(uint8_t), EPD_WIDTH * EPD_HEIGHT / 2);
  if (!framebuffer)
    Serial.println("Memory alloc failed!");
  memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
#endif
}

#if EPD
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

void drawFastVLine(int16_t x0, int16_t y0, int length, uint16_t color)
{
  epd_draw_vline(x0, y0, length, color, framebuffer);
}

void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
  epd_write_line(x0, y0, x1, y1, color, framebuffer);
}

void fillCircle(int x, int y, int r, uint8_t color)
{
  epd_fill_circle(x, y, r, color, framebuffer);
}

void drawCircle(int x0, int y0, int r, uint8_t color)
{
  epd_draw_circle(x0, y0, r, color, framebuffer);
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

void DisplayStatus()
{
  setFont(OpenSans8B);
  DrawBattery(770, 18);
  drawLine(0, 20, SCREEN_WIDTH, 20, Black);
}

int getColor(String color)
{
  if (color == "")
    return Grey;

#define COLOR_LAVENDER (1)
#define COLOR_SAGE (2)
#define COLOR_GRAPE (3)
#define COLOR_FLAMINGO (4)
#define COLOR_BANANA (5)
#define COLOR_TANGERINE (6)
#define COLOR_PEACOCK (7)
#define COLOR_GRAPHITE (8)
#define COLOR_BLUEBERRY (9)
#define COLOR_BASIL (10)
#define COLOR_TOMATO (11)

  switch (color.toInt())
  {
  case COLOR_TOMATO:
  case COLOR_TANGERINE:
  case COLOR_SAGE:
    return LightGrey;

  case COLOR_PEACOCK:
  case COLOR_LAVENDER:
  case COLOR_GRAPHITE:
    return Grey;

  case COLOR_FLAMINGO:
  case COLOR_BANANA:
  case COLOR_BASIL:
    return DarkGrey;

  case COLOR_BLUEBERRY:
  case COLOR_GRAPE:
    return Black;

  default:
    return Grey;
  }
}

String createEventRow(String startTime, int daysToEvent){
  String eventRow = "[";
  if(daysToEvent < 10) eventRow+=" ";
  eventRow += String(daysToEvent);
  eventRow += "]  " + startTime;

  return eventRow;
}

void DisplayCalendarRow(int row, String color, String title, String startTime, int daysToEvent)
{
  int row_h_offset = 30;
  int row_y_pos = 65;
  int circle_r = 10;
  int c = getColor(color);
  String eventRow = createEventRow(startTime, daysToEvent);

  //first row
  fillCircle(10, (row * row_y_pos) + row_h_offset + (circle_r / 2) + 5, circle_r, c);
  drawString(30, (row * row_y_pos) + row_h_offset - 5, eventRow, LEFT);

  //second row
  drawString(20, (row * row_y_pos + 25) + row_h_offset, title, LEFT);
}

void DrawNumberMissingEntries(int item_cnt)
{
  setFont(OpenSans8B);
  drawString(10, 6, "-" + String(item_cnt), LEFT);
}

void DrawCurrentDate(String curr_date)
{
  setFont(OpenSans8B);
  drawString(40, 4, curr_date, LEFT);
}

void DisplayCalendar(String *calendarData)
{
  DynamicJsonDocument doc(6 * 1024);
  DeserializationError error = deserializeJson(doc, *calendarData);

  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  JsonObject root = doc.as<JsonObject>();
  String status = root["status"].as<char *>();
  Serial.println("status: " + String(status));

  if (status != "success")
  {
    Serial.print(F("\"status\" is not success"));
    return;
  }

  int item_cnt = root["data"]["itemsCnt"].as<int>();

  int items_to_display = item_cnt > MAX_CALENDAR_ROW ? MAX_CALENDAR_ROW : item_cnt;

  setFont(OpenSans12B);
  for (int i = 0; i < items_to_display; i++, item_cnt--)
  {
    Serial.println("Try to get element: " + String(i));
    String title = root["data"]["items"][i]["title"].as<char *>();
    String color = root["data"]["items"][i]["color"].as<char *>();
    String startTime = root["data"]["items"][i]["startTime"].as<char *>();
    int daysToEvent = root["data"]["items"][i]["daysToEvent"].as<int>();

    Serial.println(" " + String(i) + " color:" + color + ", title: " + title + ", startTime: " + startTime + ", daysToEvent: " + String(daysToEvent));
    DisplayCalendarRow(i, color, title, startTime, daysToEvent);
  }

  if (item_cnt > 0)
    DrawNumberMissingEntries(item_cnt);

  DrawCurrentDate(root["date"].as<char *>());
}
#endif