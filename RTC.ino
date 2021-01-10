#include <M5EPD.h>
#include <WiFi.h>

#include "rtc.h"
#include "WifiConst.h"

M5EPD_Canvas canvas(&M5.EPD);
Rtc rtc;

void setup() {
    M5.begin();
    M5.EPD.SetRotation(90);
    M5.EPD.Clear(true);

    canvas.createCanvas(540, 960);
    canvas.setTextSize(4);

    showInfo();
}

bool wifiStart = false;
void startWifi() {
    if (wifiStart) return;
    wifiStart = true;

    WiFi.begin(WifiSSID, WifiPass);

    while (WiFi.status() != WL_CONNECTED) { delay(500); }
}

void showTime() {
    rtc_date_t dt;
    rtc_time_t tm;
    M5.RTC.getDate(&dt);
    M5.RTC.getTime(&tm);

    canvas.setTextDatum(TL_DATUM);

    char text[256];
    snprintf(text,256,"DATE: %04d-%02d-%02d", dt.year, dt.mon, dt.day);
    canvas.drawString(text, 0, 32);

    snprintf(text,256,"TIME: %02d:%02d:%02d", tm.hour, tm.min, tm.sec);
    canvas.drawString(text, 0, 64);
}

void updateFooter(const char *msg) {
    canvas.fillRect(0,900, 540, 60, 0);
    canvas.drawString(msg, 0, 900);
    canvas.pushCanvas(0,0,UPDATE_MODE_DU4);
}

void showInfo() {
    canvas.fillRect(0,0, 540, 32*3, 0);
    canvas.drawString("RTC SETUP 20210111", 0, 0);

    showTime();
    canvas.pushCanvas(0,0,UPDATE_MODE_GC16);
    // Fill 60px at the bottom of the screen
    canvas.fillRect(0, 900, 540, 60, 0);
    canvas.pushCanvas(0,0,UPDATE_MODE_DU);
}

void loop() {
    if (M5.BtnL.wasPressed()) {
        updateFooter("SYNC DATETIME FROM NTP...");
        startWifi();
        rtc.Setup();
        showInfo();
    }

    if (M5.BtnR.wasPressed()) {
        updateFooter("UPDATE...");
        showInfo();
    }

    if(M5.BtnP.wasPressed()){
        updateFooter("SHUTDOWN...");
        canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);
        // shutdown after 2second to wait update display
        delay(2000);
        M5.shutdown();
    }
    M5.update();
    delay(100);
}
