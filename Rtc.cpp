#include <M5EPD.h>

#include "rtc.h"

void Rtc::Setup() {
    ConfigTime();
    SetDateTime();
}

void Rtc::ConfigTime() {
    configTime(9*3600L, 0, "ntp.nict.jp", "time.google.com", "ntp.jst.mfeed.ad.jp");

    struct tm timeinfo;
    while (!getLocalTime(&timeinfo)) {
        delay(1000);
    }
}

void Rtc::SetDateTime() {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    rtc_date_t rdt;
    rtc_time_t rtm;

    M5.RTC.getDate(&rdt);
    M5.RTC.getTime(&rtm);

    rdt.year = tm->tm_year + 1900;
    rdt.mon = tm->tm_mon + 1;
    rdt.day = tm->tm_mday;
    M5.RTC.setDate(&rdt);

    rtm.hour = tm->tm_hour;
    rtm.min = tm->tm_min;
    rtm.sec = tm->tm_sec;
    M5.RTC.setTime(&rtm);
}


