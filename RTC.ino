#include <M5EPD.h>
#include <WiFi.h>

#include "WifiConst.h"

M5EPD_Canvas canvas(&M5.EPD);

void setup() {
    M5.begin();
    M5.EPD.SetRotation(90);
    M5.EPD.Clear(true);

    canvas.createCanvas(540, 960);
    canvas.setTextSize(4);

    showTime();

    canvas.pushCanvas(0,0,UPDATE_MODE_GC16);
}

bool wifiStart = false;
void startWifi() {
    if (wifiStart) return;

    wifiStart = true;
    WiFi.begin(WifiSSID, WifiPass);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
}

void setupRtc() {
    startWifi();
    configTime(9*3600L,0,"ntp.nict.jp","time.google.com","ntp.jst.mfeed.ad.jp");

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    rtc_date_t rdt;
    rtc_time_t rtm;

    rdt.year = tm->tm_year + 1900;
    rdt.mon = tm->tm_mon + 1;
    rdt.day = tm->tm_mday;
    M5.RTC.setDate(&rdt);

    rtm.hour = tm->tm_hour;
    rtm.min = tm->tm_min;
    rtm.sec = tm->tm_sec;
    M5.RTC.setTime(&rtm);
}


void showTime() {
    rtc_date_t dt;
    rtc_time_t tm;
    M5.RTC.getDate(&dt);
    M5.RTC.getTime(&tm);

    char dateText[256];
    snprintf(dateText,256,"%04d-%02d-%02d", dt.year, dt.mon, dt.day);

    char timeText[256];
    snprintf(timeText,256,"%02d:%02d:%02d", tm.hour, tm.min, tm.sec);

    canvas.setTextDatum(TL_DATUM);

    canvas.fillRect(0,0, 540, 64, 0);

    canvas.drawString(String("DATE:") + dateText, 0, 0);
    canvas.drawString(String("TIME:") + timeText, 0, 32);
}

void updateFooter(const char *msg) {
    canvas.fillRect(0,900, 540, 60, 0);
    canvas.drawString(msg, 0, 900);

}

void loop() {
    if (M5.BtnL.wasPressed()) {
        updateFooter("CORRECT TIME FROM NTP...");
        setupRtc();
        showTime();
        canvas.pushCanvas(0,0,UPDATE_MODE_GC16);
        canvas.fillRect(0, 900, 540, 60, 0);
        canvas.pushCanvas(0,0,UPDATE_MODE_DU4);
    }

    if (M5.BtnR.wasPressed()) {
        updateFooter("UPDATE...");
        showTime();
        canvas.pushCanvas(0,0,UPDATE_MODE_GC16);
        canvas.fillRect(0, 900, 540, 60, 0);
        canvas.pushCanvas(0,0,UPDATE_MODE_DU4);
    }

    if(M5.BtnP.wasPressed()){
        updateFooter("SHUTDOWN...");
        canvas.fillRect(0,0, 540, 32, 0);
        canvas.drawString("SHUTDOWN...",0, 900);
        canvas.pushCanvas(0,0,UPDATE_MODE_DU4);
        delay(2000);
        M5.shutdown();
    }
    M5.update();
    delay(100);
}
