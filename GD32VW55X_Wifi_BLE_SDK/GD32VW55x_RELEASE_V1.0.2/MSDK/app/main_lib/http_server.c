#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "http_server.h"
#include "dbg_print.h"
#include "wrapper_os.h"

// For Wi-Fi + MQTT calls
#include "wifi_management.h"
#include "wifi_mqtt.h"
#include "storage.h"

// LWIP includes for sockets, errors
#include "lwip/sockets.h"
#include "lwip/errno.h"

// Hard-coded single user
#define LOGIN_USER "admin"
#define LOGIN_PASS "admin"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Full HTML strings from your original main.c
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static const char login_page[] =
"HTTP/1.0 200 OK\r\n"
"Connection: close\r\n"
"Content-Type: text/html\r\n\r\n"
"<!DOCTYPE html>"
"<html>"
"<head>"
"<meta charset='UTF-8'>"
"<title>MZ-GW Login</title>"
"<style>"
"  body { background: #fafafa; font-family: Arial, sans-serif; padding:20px; }"
"  .container { background: #fff; padding: 20px; border-radius: 6px; max-width:400px; margin:auto; text-align:center; }"
"  .container h2 { margin-top:0; }"
"  .logo { display:block; margin:20px auto; max-width:200px; }"
"  label { font-weight:bold; display:block; text-align:left; margin:10px 0 5px 0; }"
"  input { display:block; margin:0 auto 10px auto; width:90%; padding:8px; }"
"  .btn { background:#3dae2b; color:#fff; border:none; padding:10px 20px; cursor:pointer; display:block; margin:20px auto 0 auto; }"
"  .btn:hover { background:#46b835; }"
"  .error { color:red; margin:10px 0; }"
"</style>"
"</head>"
"<body>"
"<div class='container'>"
"<svg class='logo' id='Layer_1' data-name='Layer 1' xmlns='http://www.w3.org/2000/svg' viewBox='0 0 1496 486'>"
"<title>tr</title>"
"<circle cx='253.73' cy='249' r='208.82' style='fill:transparent'/>"
"<path d='M510.88,186.59H659q32,0,46.28,12.56T719.6,240v92.67H680.78V245.12c0-10.64-1.59-17.71-4.85-21.16s-9.9-5.23-20-5.23H634.23V332.67H594.87V218.73H549.68V332.67h-38.8Z' style='fill:#231f20;fill-rule:evenodd'/>"
"<path d='M785.05,259.7c0,13.76,3.29,24.61,9.72,32.42s15.64,11.67,27.26,11.67,20.57-3.86,27.15-11.67,9.91-18.66,9.91-32.42-3.26-24.6-9.82-32.38S833.63,215.67,822,215.67s-20.58,3.87-27.14,11.65S785.05,245.89,785.05,259.7Zm114.51,0q0,34.41-20.92,54.91c-14,13.64-32.71,20.47-56.39,20.47s-42.41-6.83-56.55-20.56-21.11-32-21.11-54.82,7.07-41.12,21.11-54.93,33-20.71,56.55-20.71c23.4,0,42.07,6.86,56.2,20.62S899.56,236.8,899.56,259.73Z' style='fill:#231f20;fill-rule:evenodd'/>"
"<path d='M961.94,133.64v53H995c14.57,0,26.32,1.72,35.17,5.2A53.8,53.8,0,0,1,1053,209.05a69.89,69.89,0,0,1,11.83,22.16,83.74,83.74,0,0,1,4,26.56q0,35.09-18.75,55t-52.42,19.9H923.36v-199Zm23.54,84.53H961.94v83.19h26.54c14.61,0,25-3.12,30.93-9.3s9-16.64,9-31.4c0-15.16-3.21-26-9.88-32.6S1000.89,218.17,985.48,218.17Z' style='fill:#231f20;fill-rule:evenodd'/>"
"<rect x='1092.52' y='186.59' width='40.46' height='146.09' style='fill:#231f20'/>"
"<rect x='1165.98' y='133.64' width='40.47' height='199.03' style='fill:#231f20'/>"
"<rect x='1239.58' y='186.59' width='40.46' height='146.09' style='fill:#231f20'/>"
"<path d='M1237.58,146.14A21.85,21.85,0,1,1,1259.41,168,21.85,21.85,0,0,1,1237.58,146.14Z' style='fill:#231f20;fill-rule:evenodd'/>"
"<path d='M1090.9,146.14A21.85,21.85,0,1,1,1112.73,168,21.86,21.86,0,0,1,1090.9,146.14Z' style='fill:#231f20;fill-rule:evenodd'/>"
"<polygon points='1313.04 301.39 1399.54 218.17 1313.04 218.17 1313.04 186.59 1450.05 186.59 1450.05 218.11 1363.56 301.36 1450.05 301.36 1450.05 332.67 1313.04 332.67 1313.04 301.39' style='fill:#231f20;fill-rule:evenodd'/>"
"<path d='M301.93,45.78a166.5,166.5,0,0,1-3.78,319.35,4.93,4.93,0,0,1-2.75-9.46A138.83,138.83,0,0,0,243.07,85c-69,4.77-124.62,61-128.72,130a138.88,138.88,0,0,0,94.76,140.23,4.91,4.91,0,0,1,3.4,4.65V360a4.92,4.92,0,0,1-6.23,4.77A166.51,166.51,0,0,1,87.09,190.23C92.77,124,138.1,68.5,198.8,47.49,110.12,71.61,44.91,152.68,44.91,249c0,105.39,78.07,192.53,179.53,206.77V286.61a49,49,0,0,0,57.55,0v169.3C384,442.11,462.55,354.74,462.55,249,462.55,150.27,394,67.55,301.93,45.78ZM210.18,314.47a4.78,4.78,0,0,1,2.33,4.08v.16a4.84,4.84,0,0,1-7.06,4.3,103.7,103.7,0,0,1-55.39-103.74c5.21-47.71,43.59-86.18,91.3-91.47a103.8,103.8,0,0,1,57.55,196.32,4.82,4.82,0,0,1-4.52-8.51,82.5,82.5,0,0,0-50.58-153.46c-38.93,4.3-69.91,36.53-72.86,75.59A82.38,82.38,0,0,0,210.18,314.47Zm43-33.79A28.78,28.78,0,1,1,282,251.9,28.78,28.78,0,0,1,253.22,280.68Z' style='fill:#3dae2b'/>"
"</svg>"
"<h2>MZ-GW Login</h2>"
"<form method='GET' action='/login'>"
"  <label>Username:</label>"
"  <input type='text' name='username' />"
"  <label>Password:</label>"
"  <input type='password' name='password' />"
"  <input class='btn' type='submit' value='Login' />"
"</form>"
"</div>"
"</body></html>\r\n";

static const char login_page_fail[] =
"HTTP/1.0 200 OK\r\n"
"Connection: close\r\n"
"Content-Type: text/html\r\n\r\n"
"<!DOCTYPE html>"
"<html>"
"<head>"
"<meta charset='UTF-8'>"
"<title>MZ-GW Login</title>"
"<style>"
"  body { background: #fafafa; font-family: Arial, sans-serif; padding:20px; }"
"  .container { background: #fff; padding: 20px; border-radius: 6px; max-width:400px; margin:auto; text-align:center; }"
"  .container h2 { margin-top:0; }"
"  .logo { display:block; margin:20px auto; max-width:200px; }"
"  label { font-weight:bold; display:block; text-align:left; margin:10px 0 5px 0; }"
"  input { display:block; margin:0 auto 10px auto; width:90%; padding:8px; }"
"  .btn { background:#3dae2b; color:#fff; border:none; padding:10px 20px; cursor:pointer; display:block; margin:20px auto 0 auto; }"
"  .btn:hover { background:#46b835; }"
"  .error { color:red; margin:10px 0; }"
"</style>"
"</head>"
"<body>"
"<div class='container'>"
"<svg class='logo' id='Layer_1' data-name='Layer 1' xmlns='http://www.w3.org/2000/svg' viewBox='0 0 1496 486'>"
"<title>tr</title>"
"<circle cx='253.73' cy='249' r='208.82' style='fill:transparent'/>"
"<path d='M510.88,186.59H659q32,0,46.28,12.56T719.6,240v92.67H680.78V245.12c0-10.64-1.59-17.71-4.85-21.16s-9.9-5.23-20-5.23H634.23V332.67H594.87V218.73H549.68V332.67h-38.8Z' style='fill:#231f20;fill-rule:evenodd'/>"
"<path d='M785.05,259.7c0,13.76,3.29,24.61,9.72,32.42s15.64,11.67,27.26,11.67,20.57-3.86,27.15-11.67,9.91-18.66,9.91-32.42-3.26-24.6-9.82-32.38S833.63,215.67,822,215.67s-20.58,3.87-27.14,11.65S785.05,245.89,785.05,259.7Zm114.51,0q0,34.41-20.92,54.91c-14,13.64-32.71,20.47-56.39,20.47s-42.41-6.83-56.55-20.56-21.11-32-21.11-54.82,7.07-41.12,21.11-54.93,33-20.71,56.55-20.71c23.4,0,42.07,6.86,56.2,20.62S899.56,236.8,899.56,259.73Z' style='fill:#231f20;fill-rule:evenodd'/>"
"<path d='M961.94,133.64v53H995c14.57,0,26.32,1.72,35.17,5.2A53.8,53.8,0,0,1,1053,209.05a69.89,69.89,0,0,1,11.83,22.16,83.74,83.74,0,0,1,4,26.56q0,35.09-18.75,55t-52.42,19.9H923.36v-199Zm23.54,84.53H961.94v83.19h26.54c14.61,0,25-3.12,30.93-9.3s9-16.64,9-31.4c0-15.16-3.21-26-9.88-32.6S1000.89,218.17,985.48,218.17Z' style='fill:#231f20;fill-rule:evenodd'/>"
"<rect x='1092.52' y='186.59' width='40.46' height='146.09' style='fill:#231f20'/>"
"<rect x='1165.98' y='133.64' width='40.47' height='199.03' style='fill:#231f20'/>"
"<rect x='1239.58' y='186.59' width='40.46' height='146.09' style='fill:#231f20'/>"
"<path d='M1237.58,146.14A21.85,21.85,0,1,1,1259.41,168,21.85,21.85,0,0,1,1237.58,146.14Z' style='fill:#231f20;fill-rule:evenodd'/>"
"<path d='M1090.9,146.14A21.85,21.85,0,1,1,1112.73,168,21.86,21.86,0,0,1,1090.9,146.14Z' style='fill:#231f20;fill-rule:evenodd'/>"
"<polygon points='1313.04 301.39 1399.54 218.17 1313.04 218.17 1313.04 186.59 1450.05 186.59 1450.05 218.11 1363.56 301.36 1450.05 301.36 1450.05 332.67 1313.04 332.67 1313.04 301.39' style='fill:#231f20;fill-rule:evenodd'/>"
"<path d='M301.93,45.78a166.5,166.5,0,0,1-3.78,319.35,4.93,4.93,0,0,1-2.75-9.46A138.83,138.83,0,0,0,243.07,85c-69,4.77-124.62,61-128.72,130a138.88,138.88,0,0,0,94.76,140.23,4.91,4.91,0,0,1,3.4,4.65V360a4.92,4.92,0,0,1-6.23,4.77A166.51,166.51,0,0,1,87.09,190.23C92.77,124,138.1,68.5,198.8,47.49,110.12,71.61,44.91,152.68,44.91,249c0,105.39,78.07,192.53,179.53,206.77V286.61a49,49,0,0,0,57.55,0v169.3C384,442.11,462.55,354.74,462.55,249,462.55,150.27,394,67.55,301.93,45.78ZM210.18,314.47a4.78,4.78,0,0,1,2.33,4.08v.16a4.84,4.84,0,0,1-7.06,4.3,103.7,103.7,0,0,1-55.39-103.74c5.21-47.71,43.59-86.18,91.3-91.47a103.8,103.8,0,0,1,57.55,196.32,4.82,4.82,0,0,1-4.52-8.51,82.5,82.5,0,0,0-50.58-153.46c-38.93,4.3-69.91,36.53-72.86,75.59A82.38,82.38,0,0,0,210.18,314.47Zm43-33.79A28.78,28.78,0,1,1,282,251.9,28.78,28.78,0,0,1,253.22,280.68Z' style='fill:#3dae2b'/>"
"</svg>"
"<h2>MZ-GW Login</h2>"
"<div class='error'>Invalid username or password</div>"
"<form method='GET' action='/login'>"
"  <label>Username:</label>"
"  <input type='text' name='username' />"
"  <label>Password:</label>"
"  <input type='password' name='password' />"
"  <input class='btn' type='submit' value='Login' />"
"</form>"
"</div>"
"</body></html>\r\n";

static const char html_form[] =
"HTTP/1.0 200 OK\r\n"
"Connection: close\r\n"
"Content-Type: text/html\r\n\r\n"
"<!DOCTYPE html>"
"<html>"
"<head>"
"<meta charset='UTF-8'>"
"<title>MZ-GW Wi-Fi Config</title>"
"<style>"
"  body { background: #fafafa; font-family: Arial, sans-serif; padding:20px; }"
"  .container { background: #fff; padding: 20px; border-radius: 6px; max-width:400px; margin:auto; text-align:center; }"
"  .container h2 { margin-top:0; }"
"  .logo { display:block; margin:20px auto; max-width:200px; }"
"  label { font-weight:bold; display:block; text-align:left; margin:10px 0 5px 0; }"
"  input { display:block; margin:0 auto 10px auto; width:90%; padding:8px; }"
"  .btn { background:#3dae2b; color:#fff; border:none; padding:10px 20px; cursor:pointer; display:block; margin:20px auto 0 auto; }"
"  .btn:hover { background:#46b835; }"
"</style>"
"</head>"
"<body>"
"<div class='container'>"
"<svg class='logo' id='Layer_1' data-name='Layer 1' xmlns='http://www.w3.org/2000/svg' viewBox='0 0 1496 486'>"
"<title>tr</title>"
"<circle cx='253.73' cy='249' r='208.82' style='fill:transparent'/>"
"<path d='M510.88,186.59H659q32,0,46.28,12.56T719.6,240v92.67H680.78V245.12c0-10.64-1.59-17.71-4.85-21.16s-9.9-5.23-20-5.23H634.23V332.67H594.87V218.73H549.68V332.67h-38.8Z' style='fill:#231f20;fill-rule:evenodd'/>"
"<path d='M785.05,259.7c0,13.76,3.29,24.61,9.72,32.42s15.64,11.67,27.26,11.67,20.57-3.86,27.15-11.67,9.91-18.66,9.91-32.42-3.26-24.6-9.82-32.38S833.63,215.67,822,215.67s-20.58,3.87-27.14,11.65S785.05,245.89,785.05,259.7Zm114.51,0q0,34.41-20.92,54.91c-14,13.64-32.71,20.47-56.39,20.47s-42.41-6.83-56.55-20.56-21.11-32-21.11-54.82,7.07-41.12,21.11-54.93,33-20.71,56.55-20.71c23.4,0,42.07,6.86,56.2,20.62S899.56,236.8,899.56,259.73Z' style='fill:#231f20;fill-rule:evenodd'/>"
"<path d='M961.94,133.64v53H995c14.57,0,26.32,1.72,35.17,5.2A53.8,53.8,0,0,1,1053,209.05a69.89,69.89,0,0,1,11.83,22.16,83.74,83.74,0,0,1,4,26.56q0,35.09-18.75,55t-52.42,19.9H923.36v-199Zm23.54,84.53H961.94v83.19h26.54c14.61,0,25-3.12,30.93-9.3s9-16.64,9-31.4c0-15.16-3.21-26-9.88-32.6S1000.89,218.17,985.48,218.17Z' style='fill:#231f20;fill-rule:evenodd'/>"
"<rect x='1092.52' y='186.59' width='40.46' height='146.09' style='fill:#231f20'/>"
"<rect x='1165.98' y='133.64' width='40.47' height='199.03' style='fill:#231f20'/>"
"<rect x='1239.58' y='186.59' width='40.46' height='146.09' style='fill:#231f20'/>"
"<path d='M1237.58,146.14A21.85,21.85,0,1,1,1259.41,168,21.85,21.85,0,0,1,1237.58,146.14Z' style='fill:#231f20;fill-rule:evenodd'/>"
"<path d='M1090.9,146.14A21.85,21.85,0,1,1,1112.73,168,21.86,21.86,0,0,1,1090.9,146.14Z' style='fill:#231f20;fill-rule:evenodd'/>"
"<polygon points='1313.04 301.39 1399.54 218.17 1313.04 218.17 1313.04 186.59 1450.05 186.59 1450.05 218.11 1363.56 301.36 1450.05 301.36 1450.05 332.67 1313.04 332.67 1313.04 301.39' style='fill:#231f20;fill-rule:evenodd'/>"
"<path d='M301.93,45.78a166.5,166.5,0,0,1-3.78,319.35,4.93,4.93,0,0,1-2.75-9.46A138.83,138.83,0,0,0,243.07,85c-69,4.77-124.62,61-128.72,130a138.88,138.88,0,0,0,94.76,140.23,4.91,4.91,0,0,1,3.4,4.65V360a4.92,4.92,0,0,1-6.23,4.77A166.51,166.51,0,0,1,87.09,190.23C92.77,124,138.1,68.5,198.8,47.49,110.12,71.61,44.91,152.68,44.91,249c0,105.39,78.07,192.53,179.53,206.77V286.61a49,49,0,0,0,57.55,0v169.3C384,442.11,462.55,354.74,462.55,249,462.55,150.27,394,67.55,301.93,45.78ZM210.18,314.47a4.78,4.78,0,0,1,2.33,4.08v.16a4.84,4.84,0,0,1-7.06,4.3,103.7,103.7,0,0,1-55.39-103.74c5.21-47.71,43.59-86.18,91.3-91.47a103.8,103.8,0,0,1,57.55,196.32,4.82,4.82,0,0,1-4.52-8.51,82.5,82.5,0,0,0-50.58-153.46c-38.93,4.3-69.91,36.53-72.86,75.59A82.38,82.38,0,0,0,210.18,314.47Zm43-33.79A28.78,28.78,0,1,1,282,251.9,28.78,28.78,0,0,1,253.22,280.68Z' style='fill:#3dae2b'/>"
"</svg>"
"<h2>Wi-Fi Config<br>MZ-GW/{{MAC}}</h2>"
"<form method='GET' action='/set'>"
"  <label>Primary SSID:</label>"
"  <input type='text' name='p_ssid'>"
"  <label>Primary PASS:</label>"
"  <input type='text' name='p_pass'>"
"  <label>Backup SSID:</label>"
"  <input type='text' name='b_ssid'>"
"  <label>Backup PASS:</label>"
"  <input type='text' name='b_pass'>"
"  <input class='btn' type='submit' value='Save & Reconnect'/>"
"</form>"
"</div>"
"</body></html>\r\n";

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// URL decode helper
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static int hex_val(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

static void url_decode_in_place(char *s)
{
    char *p = s, *q = s;
    while (*p) {
        if (*p == '+') {
            *q++ = ' ';
            p++;
        } else if (*p == '%' && p[1] && p[2]) {
            int hi = hex_val(p[1]);
            int lo = hex_val(p[2]);
            if (hi >= 0 && lo >= 0) {
                *q++ = (char)((hi << 4) | lo);
                p += 3;
            } else {
                *q++ = *p++;
            }
        } else {
            *q++ = *p++;
        }
    }
    *q = '\0';
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// parse_url_params_login(): parse ?username=...&password=...
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static void parse_url_params_login(const char *query, char *out_user, size_t ulen,
                                   char *out_pass, size_t plen)
{
    memset(out_user, 0, ulen);
    memset(out_pass, 0, plen);

    // Look for "username="
    char *u = strstr((char*)query, "username=");
    if(u){
        u += 9; // skip "username="
        size_t i=0;
        while(*u && *u!='&' && i<(ulen-1)){
            out_user[i++] = *u++;
        }
        out_user[i]=0;
    }

    // Look for "password="
    char *p = strstr((char*)query, "password=");
    if(p){
        p += 9; // skip "password="
        size_t i=0;
        while(*p && *p!='&' && i<(plen-1)){
            out_pass[i++] = *p++;
        }
        out_pass[i]=0;
    }

    // decode
    url_decode_in_place(out_user);
    url_decode_in_place(out_pass);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// parse_url_params(): parse Wi-Fi SSIDs & Passwords
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static void parse_url_params(const char *query,
                             char *p_ssid, size_t p_ssid_len,
                             char *p_pass, size_t p_pass_len,
                             char *b_ssid, size_t b_ssid_len,
                             char *b_pass, size_t b_pass_len)
{
    #define SCAN_PARAM(q, name, dest, maxlen) do { \
        char *p = strstr((q), name "=");           \
        if(p){                                     \
            p += strlen(name) + 1;                 \
            size_t i=0;                            \
            while(*p && *p!='&' && i<(maxlen-1)){  \
                if(*p == '+') dest[i] = ' ';       \
                else dest[i] = *p;                 \
                i++; p++;                          \
            }                                      \
            dest[i] = '\0';                        \
        }                                          \
    } while(0)

    SCAN_PARAM(query, "p_ssid",  p_ssid,  p_ssid_len);
    SCAN_PARAM(query, "p_pass",  p_pass,  p_pass_len);
    SCAN_PARAM(query, "b_ssid",  b_ssid,  b_ssid_len);
    SCAN_PARAM(query, "b_pass",  b_pass,  b_pass_len);

    url_decode_in_place(p_ssid);
    url_decode_in_place(p_pass);
    url_decode_in_place(b_ssid);
    url_decode_in_place(b_pass);

    #undef SCAN_PARAM
}




// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// The main HTTP handler
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static void http_server_handle_request(int fd)
{
    char recv_buf[512];
    int ret = recv(fd, recv_buf, sizeof(recv_buf) - 1, 0);
    if (ret <= 0) {
        close(fd);
        return;
    }
    recv_buf[ret] = '\0';

    // A small success response for /set?
    static const char resp_ok[] =
        "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n"
        "<html><head>"
        "<style>"
        "body { font-family: Arial, sans-serif; text-align: center; margin-top: 20%; }"
        "h3 { font-size: 20px; color: #333; }"
        "</style>"
        "</head><body>"
        "<h3>New Wi-Fi config saved.<br>Attempting reconnection now.</h3>"
        "</body></html>\r\n";


    if (strncmp(recv_buf, "GET /login?", 11) == 0)
    {
        char *qs = &recv_buf[11];
        char *end = strstr(qs, " ");
        if (end) *end = '\0';

        char user[32], pass[32];
        parse_url_params_login(qs, user, sizeof(user), pass, sizeof(pass));

        if ((strcmp(user, LOGIN_USER) == 0) && (strcmp(pass, LOGIN_PASS) == 0))
        {
            char dynamic_html[sizeof(html_form) + 32];
            memset(dynamic_html, 0, sizeof(dynamic_html));
            strncpy(dynamic_html, html_form, sizeof(dynamic_html) - 1);

            // Replace marker {{MAC}} with the gateway MAC address.
            char *marker = strstr(dynamic_html, "{{MAC}}");
            if (marker) {
                char tmp[24];
                snprintf(tmp, sizeof(tmp), "%s", get_gateway_mac());
                size_t mac_len = strlen(tmp);
                memmove(marker + mac_len, marker + 7, strlen(marker + 7) + 1);
                memcpy(marker, tmp, mac_len);
            }

            send(fd, dynamic_html, strlen(dynamic_html), 0);
        }
        else
        {
            send(fd, login_page_fail, strlen(login_page_fail), 0);
        }
    }
    else if (strncmp(recv_buf, "GET /login", 10) == 0)
    {
        send(fd, login_page, strlen(login_page), 0);
    }
    else if (strncmp(recv_buf, "GET /set?", 9) == 0)
    {
        char *qs = &recv_buf[9];
        char *end = strstr(qs, " ");
        if (end) *end = '\0';

        char new_p_ssid[64] = {0};
        char new_p_pass[64] = {0};
        char new_b_ssid[64] = {0};
        char new_b_pass[64] = {0};

        parse_url_params(qs, new_p_ssid, sizeof(new_p_ssid),
                         new_p_pass, sizeof(new_p_pass),
                         new_b_ssid, sizeof(new_b_ssid),
                         new_b_pass, sizeof(new_b_pass));

        if (strlen(new_p_ssid) > 0) {
            strncpy(g_primary_ssid, new_p_ssid, sizeof(g_primary_ssid));
        }
        if (strlen(new_p_pass) > 0) {
            strncpy(g_primary_password, new_p_pass, sizeof(g_primary_password));
        }
        if (strlen(new_b_ssid) > 0) {
            strncpy(g_backup_ssid, new_b_ssid, sizeof(g_backup_ssid));
        }
        if (strlen(new_b_pass) > 0) {
            strncpy(g_backup_password, new_b_pass, sizeof(g_backup_password));
        }

        send(fd, resp_ok, strlen(resp_ok), 0);

        /* Gracefully shutdown the write side to flush the response */
        shutdown(fd, SHUT_WR);
        sys_ms_sleep(300);
        close(fd);

        /* Extra wait for the browser to finalize the connection */
        sys_ms_sleep(2200);

        // Restart Wi-Fi/MQTT procedures.
        my_mqtt_cmd_disconnect();
        wifi_management_disconnect();
        sys_ms_sleep(1000);

        wifi_connect_handler();
        if (wifi_get_status() == WIFI_CONNECTED) {
            printf("[HTTP] Manual connect success => start SNTP, reconnect MQTT.\r\n");
            start_sntp_and_sync();
            my_mqtt_cmd_disconnect();
            sys_ms_sleep(1000);
            my_mqtt_cmd_connect();
            sys_ms_sleep(3000);

            if (mqtt_client_is_connected_ok()) {
                offline_flush_to_mqtt();
            }
            sys_ms_sleep(200);
            start_ota_demo();
        }
        return;
    }
    else
    {
        send(fd, login_page, strlen(login_page), 0);
    }

    /* In all cases, perform a graceful shutdown of the socket */
    shutdown(fd, SHUT_WR);
    sys_ms_sleep(300);
    close(fd);
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// The SoftAP + HTTP server task
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void softap_http_server_task(void *param)
{
    (void)param;

    // Build the AP SSID => "MZ-GW/<MAC>"
    char ap_ssid[32];
    snprintf(ap_ssid, sizeof(ap_ssid), "MZ-GW/%s", get_gateway_mac());
    if (strlen(get_gateway_mac()) == 0) {
        snprintf(ap_ssid, sizeof(ap_ssid), "MZ-GW/");
    }

    char *ap_password = "12345678";
    uint8_t channel   = 6;
    wifi_ap_auth_mode_t auth_mode = AUTH_MODE_WPA2_WPA3;
    uint8_t is_hidden = 0;

    printf("[HTTP] Starting Wi-Fi SoftAP: %s\r\n", ap_ssid);
    int ret = wifi_management_ap_start(ap_ssid, ap_password, channel, auth_mode, is_hidden);
    if(ret != 0){
        printf("[HTTP] wifi_management_ap_start failed, code=%d\r\n", ret);
        goto exit_ap;
    }
    printf("[HTTP] SoftAP started successfully.\r\n");

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_fd < 0){
        printf("[HTTP] Socket create error!\r\n");
        goto exit_ap;
    }
    int reuse=1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    struct sockaddr_in srv_addr;
    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family      = AF_INET;
    srv_addr.sin_port        = htons(80);
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(listen_fd, (struct sockaddr*)&srv_addr, sizeof(srv_addr))<0){
        printf("[HTTP] Bind error!\r\n");
        goto exit_server;
    }
    if(listen(listen_fd, 4)<0){
        printf("[HTTP] Listen error!\r\n");
        goto exit_server;
    }

    printf("[HTTP] Listening on port 80 (SoftAP)...\r\n");

    while(1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int cli_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &addr_len);
        if(cli_fd < 0) {
            if(errno == EAGAIN) {
                continue; // just try again
            }
            printf("[HTTP] accept error %d\r\n", errno);
            break;
        }
        http_server_handle_request(cli_fd);
        // We do a final shutdown/close inside the handler, so nothing more here
    }

exit_server:
    if(listen_fd >= 0){
        shutdown(listen_fd, SHUT_RD);
        close(listen_fd);
    }
exit_ap:
    sys_ms_sleep(500);
    wifi_management_ap_stop();
    printf("[HTTP] SoftAP server ended.\r\n");

    sys_task_delete(NULL);
}
