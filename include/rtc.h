#ifndef RTC_H
#define RTC_H

void rtc_get_time(
    unsigned char *second,
    unsigned char *minute,
    unsigned char *hour,
    unsigned char *day,
    unsigned char *month,
    unsigned short *year
);

#endif
