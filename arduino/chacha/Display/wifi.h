#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

void start_wifi_client(const char *ssid, const char *pass) {
  WiFiMulti WiFiMulti;
  WiFiMulti.addAP(ssid, pass);

  M5.Lcd.print("Connecting");

  while(WiFiMulti.run() != WL_CONNECTED) { // ②
    M5.Lcd.print(".");
    delay(1000);
  }
  M5.Lcd.println("");
  M5.Lcd.println("Connected to");
  M5.Lcd.println(WiFi.localIP()); // ③
  delay(500);
}
