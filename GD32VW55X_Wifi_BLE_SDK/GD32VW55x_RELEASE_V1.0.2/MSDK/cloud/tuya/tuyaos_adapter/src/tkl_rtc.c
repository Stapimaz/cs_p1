/**
 * @file tkl_rtc.c
 * @brief this file was auto-generated by tuyaos v&v tools, developer can add implements between BEGIN and END
 *
 * @warning: changes between user 'BEGIN' and 'END' will be keeped when run tuyaos v&v tools
 *           changes in other place will be overwrited and lost
 *
 * @copyright Copyright 2020-2021 Tuya Inc. All Rights Reserved.
 *
 */

// --- BEGIN: user defines and implements ---
#include "gd32vw55x_rtc.h"
#include "tkl_rtc.h"
#include "tuya_error_code.h"
// --- END: user defines and implements ---

#ifdef TUYAOS_SUPPORT

extern __IO UINT32_T prescaler_a, prescaler_s;
extern void rtc_pre_config(void);

typedef struct localtimeinformation
{
    int year;
    int month;
    int day;
    int week;
    int hour;
    int minute;
    int second;
} localtimeinfo_t;

static void seconds_to_gmt(UINT32_T gmt_time, localtimeinfo_t *time)
{
    INT32_T time_zone = 8; // Beijing time zone
    UINT32_T gmttime = gmt_time + time_zone * 3600;
    INT32_T leap_years = 0, i = 0, days = 0;
    INT32_T month_days[] = {31,28,31,30,31,30,31,31,30,31,30,31};

    // get second
    time->second = (INT32_T)(gmttime % 60);
    gmttime = gmttime / 60;
    // get minute
    time->minute = (INT32_T)(gmttime % 60);
    gmttime = gmttime / 60;
    // get hour
    time->hour = (INT32_T)(gmttime % 24);
    gmttime = gmttime / 24;
    // get week
    time->week = ((gmttime % 7 + 3) % 7) + 1; // monday(0x1) is the first day

    gmttime += 719162; // add days from 0 to 1970

    // get year
    time->year = (INT32_T)(gmttime / 365);
    days = (INT32_T)(gmttime % 365);
    while(1) {
        leap_years = ((time->year) / 4) - ((time->year) / 100);
        leap_years += ((time->year) / 400);
        if (days < leap_years) {
            days += 365;
            (time->year)--;
        } else {
            break;
        }
    }

    days -= leap_years;
    (time->year)++;
    days++;

    if (((((time->year) % 4) == 0) && (((time->year) % 100) != 0)) || (((time->year) % 400) == 0)) {
        month_days[1] = 29;
    }

    time->month = 1;

    for(i = 0; i < 12; i++) {
        if (days <= month_days[i]) {
            break;
        } else {
            days -= month_days[i];
            (time->month)++;
        }
    }

    time->day = days;

    return;
}

static UINT32_T mktime_second(localtimeinfo_t local_time)
{
    INT_T leap_years = 0, i = 0, time_zone = 8;
    UINT32_T days = 0, resultValue = 0;
    INT_T year = local_time.year - 1;
    const INT_T month_days[] = {31,28,31,30,31,30,31,31,30,31,30,31};

    leap_years = (year / 4) - (year / 100);
    leap_years += (year / 400);
    days = year * 365 + leap_years;

    for (i = 0; i < local_time.month - 1; i++) {
        days += month_days[i];
    }

    if (local_time.month > 2) {
        if (((((local_time.year) % 4) == 0) && (((local_time.year) % 100) != 0)) || (((local_time.year) % 400) == 0))
        days ++;
    }

    days = days + local_time.day - 1;

    resultValue = (days - 719162) * 24 * 3600;
    resultValue += ((local_time.hour) * 3600 + (local_time.minute) * 60 + local_time.second);
    resultValue -= ((UINT32_T)time_zone) * 3600;

    return resultValue;
}

static int int_2_bcd(int bcd)
{
    return (bcd + (bcd/10) * 6);
}

static int bcd_2_int(int decimal)
{
    return (decimal - (decimal>>4) * 6);
}

/**
 * @brief rtc init
 *
 * @param[in] none
 *
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_rtc_init(VOID_T)
{
    // --- BEGIN: user implements ---
    if (RCU_RTCSRC_NONE == (RCU_BDCTL & RCU_BDCTL_RTCSRC)) {
        rtc_parameter_struct rtc_initpara = {0};
        /* Set rtc clock, use 32k clock */
        rtc_pre_config();

        /* RTC clock source is not configured */
        rtc_initpara.factor_asyn = prescaler_a;
        rtc_initpara.factor_syn = prescaler_s;
        rtc_initpara.year = 0x23; // bcd code
        rtc_initpara.day_of_week = RTC_SUNDAY;
        rtc_initpara.month = RTC_JAN;
        rtc_initpara.date = 0x1;
        rtc_initpara.display_format = RTC_24HOUR;
        rtc_initpara.am_pm = RTC_AM;
        rtc_initpara.hour = 0;
        rtc_initpara.minute = 0;
        rtc_initpara.second = 0;

        /* RTC current time configuration */
        if (ERROR == rtc_init(&rtc_initpara)) {
            return OPRT_COM_ERROR;
        }
    } else {
        // printf("---------------rtc has been init\r\n");
    }

    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief rtc deinit
 * @param[in] none
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_rtc_deinit(VOID_T)
{
    // --- BEGIN: user implements ---
    rtc_deinit();
    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief rtc time set
 *
 * @param[in] time_sec: rtc time seconds
 *
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_rtc_time_set(TIME_T time_sec)
{
    rtc_parameter_struct rtc_para = {0};
    localtimeinfo_t time = {0};

    seconds_to_gmt(time_sec, &time);

    rtc_current_time_get(&rtc_para);

    rtc_para.second      = int_2_bcd(time.second);
    rtc_para.minute      = int_2_bcd(time.minute);
    rtc_para.hour        = int_2_bcd(time.hour);
    rtc_para.day_of_week = time.week;
    rtc_para.date        = int_2_bcd(time.day);
    rtc_para.month       = int_2_bcd(time.month);
    rtc_para.year        = int_2_bcd(time.year - 2000);// start from 2000
    rtc_para.am_pm = RTC_AM;
    rtc_para.display_format = RTC_24HOUR;

    // printf("set rtc time,time:%d:d:%x-%x-%x,w:%d,t:%x:%x:%x\r\n",time_sec,
    //         rtc_para.year, rtc_para.month, rtc_para.date, rtc_para.day_of_week,
    //         rtc_para.hour, rtc_para.minute, rtc_para.second);

    if (ERROR == rtc_init(&rtc_para)) {
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
}

/**
 * @brief rtc time get
 *
 * @param[in] time_sec:rtc time seconds
 *
* @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_rtc_time_get(TIME_T *time_sec)
{
    rtc_parameter_struct rtc_para;
    localtimeinfo_t time = {0};

    rtc_register_sync_wait();
    rtc_current_time_get(&rtc_para);

    time.second = bcd_2_int(rtc_para.second);
    time.minute = bcd_2_int(rtc_para.minute);
    time.hour   = bcd_2_int(rtc_para.hour);
    time.week   = rtc_para.day_of_week;
    time.day    = bcd_2_int(rtc_para.date);
    time.month  = bcd_2_int(rtc_para.month);
    time.year   = bcd_2_int(rtc_para.year) + 2000;
    *time_sec   = mktime_second(time);

    // printf("get rtc time,time:%d:d:%d-%d-%d,w:%d,t:%d:%d:%d\r\n",*time_sec,
    //         time.year, time.month, time.day, time.week, time.hour, time.minute, time.second);

    return OPRT_OK;
}

#endif /* TUYAOS_SUPPORT */
