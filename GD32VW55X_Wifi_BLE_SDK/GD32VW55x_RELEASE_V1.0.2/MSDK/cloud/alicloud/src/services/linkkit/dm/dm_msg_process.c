/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#include "iotx_dm_internal.h"
#ifdef AIOT_AUTHORIZE_ENABLE
#include "aiot_authorize_api.h"
#endif

const char DM_URI_SYS_PREFIX[]                        DM_READ_ONLY = "/sys/%s/%s/";
const char DM_URI_EXT_SESSION_PREFIX[]                DM_READ_ONLY = "/ext/session/%s/%s/";
const char DM_URI_EXT_NTP_PREFIX[]                    DM_READ_ONLY = "/ext/ntp/%s/%s/";
const char DM_URI_EXT_ERROR_PREFIX[]                  DM_READ_ONLY = "/ext/error/%s/%s";
const char DM_URI_REPLY_SUFFIX[]                      DM_READ_ONLY = "_reply";

/* From Cloud To Local Request And Response*/
const char DM_URI_THING_MODEL_DOWN_RAW[]              DM_READ_ONLY = "thing/model/down_raw";
const char DM_URI_THING_MODEL_DOWN_RAW_REPLY[]        DM_READ_ONLY = "thing/model/down_raw_reply";

/* From Local To Cloud Request And Response*/
const char DM_URI_THING_MODEL_UP_RAW[]                DM_READ_ONLY = "thing/model/up_raw";
const char DM_URI_THING_MODEL_UP_RAW_REPLY[]          DM_READ_ONLY = "thing/model/up_raw_reply";

const char DM_URI_RRPC_REQUEST_WILDCARD[]             DM_READ_ONLY = "rrpc/request/+";
#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
#ifdef LINK_VISUAL_ENABLE
    const char DM_URI_LINK_VISUAL_WILDCARD_DOWNSTREAM[]   DM_READ_ONLY = "vision/+/+/+/downstream";
    const char DM_URI_LINK_VISUAL_WILDCARD_INVOKE[]       DM_READ_ONLY = "vision/+/+/+/invoke";
    const char DM_URI_LINK_VISUAL_WILDCARD_DOWNSTREAM_2[]   DM_READ_ONLY = "vision/+/+/downstream";
    const char DM_URI_LINK_VISUAL_WILDCARD_INVOKE_2[]       DM_READ_ONLY = "vision/+/+/invoke";
#endif

/* From Cloud To Local Request And Response*/
    const char DM_URI_THING_SERVICE_PROPERTY_SET[]        DM_READ_ONLY = "thing/service/property/set";
    const char DM_URI_THING_SERVICE_PROPERTY_SET_REPLY[]  DM_READ_ONLY = "thing/service/property/set_reply";
    const char DM_URI_THING_SERVICE_PROPERTY_GET[]        DM_READ_ONLY = "thing/service/property/get";
    const char DM_URI_THING_SERVICE_PROPERTY_GET_REPLY[]  DM_READ_ONLY = "thing/service/property/get_reply";
    const char DM_URI_THING_SERVICE_REQUEST_WILDCARD[]    DM_READ_ONLY = "thing/service/+";
    const char DM_URI_THING_SERVICE_REQUEST_WILDCARD2[]   DM_READ_ONLY = "thing/service/#";
    const char DM_URI_THING_SERVICE_REQUEST[]             DM_READ_ONLY = "thing/service/%s";
    const char DM_URI_THING_SERVICE_RESPONSE[]            DM_READ_ONLY = "thing/service/%.*s_reply";

    /* From Local To Cloud Request And Response*/
    const char DM_URI_THING_EVENT_PROPERTY_POST[]         DM_READ_ONLY = "thing/event/property/post";
    const char DM_URI_THING_EVENT_PROPERTY_POST_REPLY[]   DM_READ_ONLY = "thing/event/property/post_reply";
    const char DM_URI_THING_EVENT_POST[]                  DM_READ_ONLY = "thing/event/%.*s/post";
    const char DM_URI_THING_EVENT_POST_REPLY[]            DM_READ_ONLY = "thing/event/%s/post_reply";
    const char DM_URI_THING_EVENT_POST_REPLY_WILDCARD[]   DM_READ_ONLY = "thing/event/+/post_reply";
    #ifdef LOG_REPORT_TO_CLOUD
        const char DM_URI_THING_LOG_POST[]                    DM_READ_ONLY = "thing/log/post";
    #endif
    const char DM_URI_THING_DEVICEINFO_UPDATE[]           DM_READ_ONLY = "thing/deviceinfo/update";
    const char DM_URI_THING_DEVICEINFO_UPDATE_REPLY[]     DM_READ_ONLY = "thing/deviceinfo/update_reply";
    const char DM_URI_THING_DEVICEINFO_DELETE[]           DM_READ_ONLY = "thing/deviceinfo/delete";
    const char DM_URI_THING_DEVICEINFO_DELETE_REPLY[]     DM_READ_ONLY = "thing/deviceinfo/delete_reply";
#endif

const char DM_URI_THING_EVENT_NOTIFY[]                DM_READ_ONLY = "_thing/event/notify";
const char DM_URI_THING_EVENT_NOTIFY_REPLY[]          DM_READ_ONLY = "_thing/event/notify_reply";
const char DM_URI_NTP_REQUEST[]                       DM_READ_ONLY = "request";
const char DM_URI_NTP_RESPONSE[]                      DM_READ_ONLY = "response";

#ifdef DM_UNIFIED_SERVICE_POST
const char DM_URI_UNIFIED_SERVICE_POST[]              DM_READ_ONLY = "_thing/service/post";
const char DM_URI_UNIFIED_SERVICE_POST_REPLY[]        DM_READ_ONLY = "_thing/service/post_reply";
#endif

const char DM_URI_DEV_CORE_SERVICE_DEV[]              DM_READ_ONLY = "/dev/core/service/dev";

#ifdef DEVICE_MODEL_GATEWAY
    /* From Cloud To Local Request And Response*/
    const char DM_URI_THING_TOPO_ADD_NOTIFY[]             DM_READ_ONLY = "thing/topo/add/notify";
    const char DM_URI_THING_TOPO_ADD_NOTIFY_REPLY[]       DM_READ_ONLY = "thing/topo/add/notify_reply";
    const char DM_URI_THING_DELETE[]                      DM_READ_ONLY = "thing/delete";
    const char DM_URI_THING_DELETE_REPLY[]                DM_READ_ONLY = "thing/delete_reply";
    const char DM_URI_THING_DISABLE[]                     DM_READ_ONLY = "thing/disable";
    const char DM_URI_THING_DISABLE_REPLY[]               DM_READ_ONLY = "thing/disable_reply";
    const char DM_URI_THING_ENABLE[]                      DM_READ_ONLY = "thing/enable";
    const char DM_URI_THING_ENABLE_REPLY[]                DM_READ_ONLY = "thing/enable_reply";
    const char DM_URI_THING_GATEWAY_PERMIT[]              DM_READ_ONLY = "thing/gateway/permit";
    const char DM_URI_THING_GATEWAY_PERMIT_REPLY[]        DM_READ_ONLY = "thing/gateway/permit_reply";

    /* From Local To Cloud Request And Response*/
    const char DM_URI_THING_SUB_REGISTER[]                DM_READ_ONLY = "thing/sub/register";
    const char DM_URI_THING_SUB_REGISTER_REPLY[]          DM_READ_ONLY = "thing/sub/register_reply";
    const char DM_URI_THING_SUB_UNREGISTER[]              DM_READ_ONLY = "thing/sub/unregister";
    const char DM_URI_THING_SUB_UNREGISTER_REPLY[]        DM_READ_ONLY = "thing/sub/unregister_reply";
    const char DM_URI_THING_TOPO_ADD[]                    DM_READ_ONLY = "thing/topo/add";
    const char DM_URI_THING_TOPO_ADD_REPLY[]              DM_READ_ONLY = "thing/topo/add_reply";
    const char DM_URI_THING_TOPO_DELETE[]                 DM_READ_ONLY = "thing/topo/delete";
    const char DM_URI_THING_TOPO_DELETE_REPLY[]           DM_READ_ONLY = "thing/topo/delete_reply";
    const char DM_URI_THING_SUB_RESET[]                   DM_READ_ONLY = "thing/reset";
    const char DM_URI_THING_SUB_RESET_REPLY[]             DM_READ_ONLY = "thing/reset_reply";
    const char DM_URI_THING_TOPO_GET[]                    DM_READ_ONLY = "thing/topo/get";
    const char DM_URI_THING_TOPO_GET_REPLY[]              DM_READ_ONLY = "thing/topo/get_reply";
    const char DM_URI_THING_TOPO_CHANGE[]                 DM_READ_ONLY = "thing/topo/change";
    const char DM_URI_THING_LIST_FOUND[]                  DM_READ_ONLY = "thing/list/found";
    const char DM_URI_THING_LIST_FOUND_REPLY[]            DM_READ_ONLY = "thing/list/found_reply";

    const char DM_URI_COMBINE_BATCH_LOGIN[]               DM_READ_ONLY = "combine/batch_login";
    const char DM_URI_COMBINE_BATCH_LOGIN_REPLY[]         DM_READ_ONLY = "combine/batch_login_reply";
    const char DM_URI_COMBINE_BATCH_LOGOUT[]              DM_READ_ONLY = "combine/batch_logout";
    const char DM_URI_COMBINE_BATCH_LOGOUT_REPLY[]        DM_READ_ONLY = "combine/batch_logout_reply";
#endif

int dm_msg_proc_thing_model_down_raw(_IN_ dm_msg_source_t *source)
{
    int res = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};

    /* Parse Product Key And Device Name */
    res = dm_msg_uri_parse_pkdn((char *)source->uri, strlen(source->uri), 2 + DM_URI_OFFSET, 4 + DM_URI_OFFSET, product_key,
                                device_name);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return dm_msg_thing_model_down_raw(product_key, device_name, (char *)source->payload, source->payload_len);
}

#ifdef LINK_VISUAL_ENABLE
int dm_msg_proc_thing_model_link_visual(_IN_ dm_msg_source_t *source)
{
    int res = 0, topic_pos = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};

    res = dm_utils_memtok((char *)source->uri, strlen(source->uri), DM_URI_SERVICE_DELIMITER, 4, &topic_pos);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Parse Product Key And Device Name */
    res = dm_msg_uri_parse_pkdn((char *)source->uri, strlen(source->uri), 2 + DM_URI_OFFSET, 4 + DM_URI_OFFSET, product_key,
                                device_name);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return dm_msg_thing_model_link_visual(product_key, device_name, (char *)source->uri + topic_pos,
        strlen(source->uri) - topic_pos, (char *)source->payload, source->payload_len);
}
#endif
int dm_msg_proc_thing_model_up_raw_reply(_IN_ dm_msg_source_t *source)
{
    int res = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};

    dm_log_info(DM_URI_THING_MODEL_UP_RAW_REPLY);

    res = dm_msg_uri_parse_pkdn((char *)source->uri, strlen(source->uri), 2 + DM_URI_OFFSET, 4 + DM_URI_OFFSET, product_key,
                                device_name);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Operation */
    res = dm_msg_thing_model_up_raw_reply(product_key, device_name, (char *)source->payload, source->payload_len);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    return SUCCESS_RETURN;
}

#if !defined(DEVICE_MODEL_RAWDATA_SOLO)
int dm_msg_proc_thing_service_property_set(_IN_ dm_msg_source_t *source, _IN_ dm_msg_dest_t *dest,
        _OU_ dm_msg_request_payload_t *request, _OU_ dm_msg_response_t *response)
{
    int res = 0, devid = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};

    dm_log_info(DM_URI_THING_SERVICE_PROPERTY_SET);

    /* Request */
    res = dm_msg_uri_parse_pkdn((char *)source->uri, strlen(source->uri), 2 + DM_URI_OFFSET, 4 + DM_URI_OFFSET, product_key,
                                device_name);
    if (res < SUCCESS_RETURN) {
        return res;
    }
#ifdef ALCS_GROUP_COMM_ENABLE
    if (strcmp(product_key,"service") == 0 && strcmp(device_name,"property") == 0){
        res = HAL_GetProductKey(product_key);
        res = HAL_GetDeviceName(device_name);
    }
#endif
    res = dm_mgr_search_device_by_pkdn(product_key, device_name, &devid);
    if (res < SUCCESS_RETURN) {
        return res;
    }

    res = dm_msg_request_parse((char *)source->payload, source->payload_len, request);
    if (res < SUCCESS_RETURN) {
        return res ;
    }

    /* Operation */
    res = dm_msg_property_set(devid, request);

    /* Response */
    response->service_prefix = DM_URI_SYS_PREFIX;
    response->service_name = dest->uri_name;
    memcpy(response->product_key, product_key, strlen(product_key));
    memcpy(response->device_name, device_name, strlen(device_name));
    response->code = (res == SUCCESS_RETURN) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);

    return SUCCESS_RETURN;
}

int dm_msg_proc_thing_service_property_get(_IN_ dm_msg_source_t *source, _IN_ dm_msg_dest_t *dest,
        _OU_ dm_msg_request_payload_t *request, _OU_ dm_msg_response_t *response,
        _OU_ unsigned char **data, int *data_len)
{
    int res = 0, devid = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};

    dm_log_info(DM_URI_THING_SERVICE_PROPERTY_GET);

    /* Request */
    res = dm_msg_uri_parse_pkdn((char *)source->uri, strlen(source->uri), 2 + DM_URI_OFFSET, 4 + DM_URI_OFFSET, product_key,
                                device_name);
    if (res < SUCCESS_RETURN) {
        return res;
    }

    res = dm_mgr_search_device_by_pkdn(product_key, device_name, &devid);
    if (res < SUCCESS_RETURN) {
        return res;
    }

    res = dm_msg_request_parse((char *)source->payload, source->payload_len, request);
    if (res < SUCCESS_RETURN) {
        return res ;
    }

    /* Operation */
    res = dm_msg_property_get(devid, request, source->context);

    if (res != SUCCESS_RETURN) {
        dm_log_err("DM Property Get Failed");
    }

    return res;
}

int dm_msg_proc_thing_service_property_post(_IN_ dm_msg_source_t *source, _IN_ dm_msg_dest_t *dest,
        _OU_ dm_msg_request_payload_t *request, _OU_ dm_msg_response_t *response)
{
    int res = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};

    dm_log_info(DM_URI_THING_EVENT_PROPERTY_POST);

    /* Request */
    res = dm_msg_uri_parse_pkdn((char *)source->uri, strlen(source->uri), 2 + DM_URI_OFFSET, 4 + DM_URI_OFFSET, product_key,
                                device_name);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_msg_request_parse((char *)source->payload, source->payload_len, request);

    /* Response */
    response->service_prefix = DM_URI_SYS_PREFIX;
    response->service_name = dest->uri_name;
    memcpy(response->product_key, product_key, strlen(product_key));
    memcpy(response->device_name, device_name, strlen(device_name));
    response->code = (res == SUCCESS_RETURN) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);

    return SUCCESS_RETURN;
}

int dm_msg_proc_thing_service_request(_IN_ dm_msg_source_t *source)
{
    int res = 0, serviceid_pos = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};
    dm_msg_request_payload_t request;

    memset(&request, 0, sizeof(dm_msg_request_payload_t));

#ifdef ALCS_GROUP_COMM_ENABLE
    char *group = "/thing/service/", url[128] = {0};
    if (strncmp(source->uri, group, strlen(group)) == 0){
        res = HAL_GetProductKey(product_key);
        res = HAL_GetDeviceName(device_name);
        sprintf(url,"/sys/%s/%s%s", product_key, device_name, source->uri);
        source->uri = url;
    }
#endif
    res = dm_utils_memtok((char *)source->uri, strlen(source->uri), DM_URI_SERVICE_DELIMITER, 6, &serviceid_pos);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    dm_log_info("Service Identifier: %.*s", strlen(source->uri) - serviceid_pos - 1, source->uri + serviceid_pos + 1);

    /* Parse Product Key And Device Name */
    res = dm_msg_uri_parse_pkdn((char *)source->uri, strlen(source->uri), 2 + DM_URI_OFFSET, 4 + DM_URI_OFFSET, product_key,
                                device_name);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Request */
    res = dm_msg_request_parse((char *)source->payload, source->payload_len, &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Operation */
    return dm_msg_thing_service_request(product_key, device_name, (char *)source->uri + serviceid_pos + 1,
                                        strlen(source->uri) - serviceid_pos - 1, &request, source->context);
}

int dm_msg_proc_thing_event_post_reply(_IN_ dm_msg_source_t *source)
{
    int res = 0, eventid_start_pos = 0, eventid_end_pos = 0;
    dm_msg_response_payload_t response;

    res = dm_utils_memtok((char *)source->uri, strlen(source->uri), DM_URI_SERVICE_DELIMITER, 6 + DM_URI_OFFSET,
                          &eventid_start_pos);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_utils_memtok((char *)source->uri, strlen(source->uri), DM_URI_SERVICE_DELIMITER, 7 + DM_URI_OFFSET,
                          &eventid_end_pos);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    dm_log_debug("Event Id: %.*s", eventid_end_pos - eventid_start_pos - 1, source->uri + eventid_start_pos + 1);

    /* Response */
    res = dm_msg_response_parse((char *)source->payload, source->payload_len, &response);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Operation */
    if ((strlen("property") == eventid_end_pos - eventid_start_pos - 1) &&
        (memcmp("property", source->uri + eventid_start_pos + 1, eventid_end_pos - eventid_start_pos - 1) == 0)) {
        dm_msg_thing_event_property_post_reply(&response);
    } else {
        dm_msg_thing_event_post_reply((char *)source->uri + eventid_start_pos + 1, eventid_end_pos - eventid_start_pos - 1,
                                      &response);
    }

    /* Remove Message From Cache */
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    char int_id[DM_UTILS_UINT32_STRLEN+1] = {0};
    if (response.id.value_length > DM_UTILS_UINT32_STRLEN) {
        return STATE_DEV_MODEL_WRONG_JSON_FORMAT;
    }
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
#endif
    return SUCCESS_RETURN;
}

#ifdef DM_UNIFIED_SERVICE_POST
int dm_msg_proc_unified_service_post_reply(_IN_ dm_msg_source_t *source)
{
    int res = 0;
    dm_msg_response_payload_t response;
#ifdef AIOT_AUTHORIZE_ENABLE
    if(strstr((char *)source->payload, "ServerCodeAuth.reply") != NULL){
        aiot_authorize_setopt((void *)&(source->payload_len), AUTHORIZEOPT_AUTHORIZE_PARSE, (char *)source->payload);
        return SUCCESS_RETURN;
    }
#endif
    memset(&response, 0, sizeof(dm_msg_response_payload_t));

    /* Response */
    res = dm_msg_response_parse((char *)source->payload, source->payload_len, &response);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Operation */
    dm_msg_unified_service_post_reply(&response);

    /* Remove Message From Cache */
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    char int_id[DM_UTILS_UINT32_STRLEN+1] = {0};
    if (response.id.value_length > DM_UTILS_UINT32_STRLEN) {
        return STATE_DEV_MODEL_WRONG_JSON_FORMAT;
    }
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
#endif

    return SUCCESS_RETURN;
}
#endif

int dm_msg_proc_thing_deviceinfo_update_reply(_IN_ dm_msg_source_t *source)
{
    int res = 0;
    dm_msg_response_payload_t response;

    dm_log_info(DM_URI_THING_DEVICEINFO_UPDATE_REPLY);

    /* Response */
    res = dm_msg_response_parse((char *)source->payload, source->payload_len, &response);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Operation */
    dm_msg_thing_deviceinfo_update_reply(&response);

    /* Remove Message From Cache */
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    char int_id[DM_UTILS_UINT32_STRLEN+1] = {0};
    if (response.id.value_length > DM_UTILS_UINT32_STRLEN) {
        return STATE_DEV_MODEL_WRONG_JSON_FORMAT;
    }
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
#endif
    return SUCCESS_RETURN;
}

int dm_msg_proc_thing_deviceinfo_delete_reply(_IN_ dm_msg_source_t *source)
{
    int res = 0;
    dm_msg_response_payload_t response;

    dm_log_info(DM_URI_THING_DEVICEINFO_DELETE_REPLY);

    /* Response */
    res = dm_msg_response_parse((char *)source->payload, source->payload_len, &response);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Operation */
    dm_msg_thing_deviceinfo_delete_reply(&response);

    /* Remove Message From Cache */
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    char int_id[DM_UTILS_UINT32_STRLEN+1] = {0};
    if (response.id.value_length > DM_UTILS_UINT32_STRLEN) {
        return STATE_DEV_MODEL_WRONG_JSON_FORMAT;
    }
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
#endif
    return SUCCESS_RETURN;
}
#endif /*end of DEVICE_MODEL_RAWDATA_SOLO*/

int dm_msg_proc_thing_event_notify_reply(_IN_ dm_msg_source_t *source)
{
    int res = 0, eventid_start_pos = 0, eventid_end_pos = 0;
    dm_msg_response_payload_t response;

    res = dm_utils_memtok((char *)source->uri, strlen(source->uri), DM_URI_SERVICE_DELIMITER, 6 + DM_URI_OFFSET,
                          &eventid_start_pos);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    res = dm_utils_memtok((char *)source->uri, strlen(source->uri), DM_URI_SERVICE_DELIMITER, 7 + DM_URI_OFFSET,
                          &eventid_end_pos);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    dm_log_info("Event Id: %.*s", eventid_end_pos - eventid_start_pos - 1, source->uri + eventid_start_pos + 1);

    /* Response */
    res = dm_msg_response_parse((char *)source->payload, source->payload_len, &response);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Operation */
    // if ((strlen("property") == eventid_end_pos - eventid_start_pos - 1) &&
    //     (memcmp("property", source->uri + eventid_start_pos + 1, eventid_end_pos - eventid_start_pos - 1) == 0)) {
        dm_msg_thing_event_notify_reply(&response);
    // } else {
    //     dm_msg_thing_event_post_reply((char *)source->uri + eventid_start_pos + 1, eventid_end_pos - eventid_start_pos - 1,
    //                                   &response);
    // }

    /* Remove Message From Cache */
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    char int_id[DM_UTILS_UINT32_STRLEN+1] = {0};
    if (response.id.value_length > DM_UTILS_UINT32_STRLEN) {
        return STATE_DEV_MODEL_WRONG_JSON_FORMAT;
    }
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
#endif
    return SUCCESS_RETURN;
}

int dm_msg_proc_thing_event_notify(_IN_ dm_msg_source_t *source, _IN_ dm_msg_dest_t *dest,
        _OU_ dm_msg_request_payload_t *request, _OU_ dm_msg_response_t *response)
{
    int res = 0, devid = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};

    dm_log_info(DM_URI_THING_EVENT_NOTIFY);

    /* Request */
    res = dm_msg_uri_parse_pkdn((char *)source->uri, strlen(source->uri), 2 + DM_URI_OFFSET, 4 + DM_URI_OFFSET, product_key,
                                device_name);
    if (res < SUCCESS_RETURN) {
        return res;
    }

    res = dm_mgr_search_device_by_pkdn(product_key, device_name, &devid);
    if (res < SUCCESS_RETURN) {
        return res;
    }

    res = dm_msg_request_parse((char *)source->payload, source->payload_len, request);
    if (res < SUCCESS_RETURN) {
        return res ;
    }

    /* Operation */
    res = dm_msg_thing_event_notify(devid, request);

    /* Response */
    response->service_prefix = DM_URI_SYS_PREFIX;
    response->service_name = dest->uri_name;
    memcpy(response->product_key, product_key, strlen(product_key));
    memcpy(response->device_name, device_name, strlen(device_name));
    response->code = (res == SUCCESS_RETURN) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);

    return SUCCESS_RETURN;
}

int dm_msg_proc_rrpc_request(_IN_ dm_msg_source_t *source)
{
    int res = 0, rrpcid_pos = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};
    dm_msg_request_payload_t request;

    memset(&request, 0, sizeof(dm_msg_request_payload_t));

    res = dm_utils_memtok((char *)source->uri, strlen(source->uri), DM_URI_SERVICE_DELIMITER, 6, &rrpcid_pos);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }
    dm_log_info("Rrpc Id: %.*s", strlen(source->uri) - rrpcid_pos - 1, source->uri + rrpcid_pos + 1);

    /* Parse Product Key And Device Name */
    res = dm_msg_uri_parse_pkdn((char *)source->uri, strlen(source->uri), 2 + DM_URI_OFFSET, 4 + DM_URI_OFFSET, product_key,
                                device_name);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Request */
    res = dm_msg_request_parse((char *)source->payload, source->payload_len, &request);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Operation */
    return dm_msg_rrpc_request(product_key, device_name, (char *)source->uri + rrpcid_pos + 1,
                               strlen(source->uri) - rrpcid_pos - 1, &request);
}

int dm_disp_ntp_response(_IN_ dm_msg_source_t *source)
{
    dm_log_info(DM_URI_NTP_RESPONSE);

    /* Operation */
    return dm_msg_ntp_response((char *)source->payload, source->payload_len);
}

int dm_disp_ext_error_response(_IN_ dm_msg_source_t *source)
{
    return dm_msg_ext_error_response((char *)source->payload, source->payload_len);
}

#ifdef DEVICE_MODEL_GATEWAY
int dm_msg_proc_thing_topo_add_notify(_IN_ dm_msg_source_t *source, _IN_ dm_msg_dest_t *dest,
                                      _OU_ dm_msg_request_payload_t *request, _OU_ dm_msg_response_t *response)
{
    int res = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};

    dm_log_info(DM_URI_THING_TOPO_ADD_NOTIFY);

    /* Request */
    res = dm_msg_uri_parse_pkdn((char *)source->uri, strlen(source->uri), 2 + DM_URI_OFFSET, 4 + DM_URI_OFFSET, product_key,
                                device_name);
    if (res < SUCCESS_RETURN) {
        return res;
    }

    res = dm_msg_request_parse((char *)source->payload, source->payload_len, request);
    if (res < SUCCESS_RETURN) {
        return res ;
    }

    /* Operation */
    res = dm_msg_topo_add_notify(request->params.value, request->params.value_length);
    if (res < SUCCESS_RETURN) {
        return res ;
    }

    /* Response */
    response->service_prefix = DM_URI_SYS_PREFIX;
    response->service_name = dest->uri_name;
    memcpy(response->product_key, product_key, strlen(product_key));
    memcpy(response->device_name, device_name, strlen(device_name));
    response->code = (res == SUCCESS_RETURN) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);

    return SUCCESS_RETURN;
}

int dm_msg_proc_thing_disable(_IN_ dm_msg_source_t *source, _IN_ dm_msg_dest_t *dest,
                              _OU_ dm_msg_request_payload_t *request, _OU_ dm_msg_response_t *response)
{
    int res = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};

    dm_log_info(DM_URI_THING_DISABLE);

    /* Request */
    res = dm_msg_uri_parse_pkdn((char *)source->uri, strlen(source->uri), 2 + DM_URI_OFFSET, 4 + DM_URI_OFFSET, product_key,
                                device_name);
    if (res < SUCCESS_RETURN) {
        return res;
    }

    res = dm_msg_request_parse((char *)source->payload, source->payload_len, request);
    if (res != SUCCESS_RETURN) {
        return res;
    }

    /* Operation */
    res = dm_msg_thing_disable(product_key, device_name);

    /* Response */
    response->service_prefix = DM_URI_SYS_PREFIX;
    response->service_name = dest->uri_name;
    memcpy(response->product_key, product_key, strlen(product_key));
    memcpy(response->device_name, device_name, strlen(device_name));
    response->code = (res == SUCCESS_RETURN) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);

    return SUCCESS_RETURN;
}

int dm_msg_proc_thing_enable(_IN_ dm_msg_source_t *source, _IN_ dm_msg_dest_t *dest,
                             _OU_ dm_msg_request_payload_t *request, _OU_ dm_msg_response_t *response)
{
    int res = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};

    dm_log_info(DM_URI_THING_DISABLE);

    /* Request */
    res = dm_msg_uri_parse_pkdn((char *)source->uri, strlen(source->uri), 2 + DM_URI_OFFSET, 4 + DM_URI_OFFSET, product_key,
                                device_name);
    if (res < SUCCESS_RETURN) {
        return res;
    }

    res = dm_msg_request_parse((char *)source->payload, source->payload_len, request);
    if (res != SUCCESS_RETURN) {
        return res;
    }

    /* Operation */
    res = dm_msg_thing_enable(product_key, device_name);

    /* Response */
    response->service_prefix = DM_URI_SYS_PREFIX;
    response->service_name = dest->uri_name;
    memcpy(response->product_key, product_key, strlen(product_key));
    memcpy(response->device_name, device_name, strlen(device_name));
    response->code = (res == SUCCESS_RETURN) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);

    return SUCCESS_RETURN;
}

int dm_msg_proc_thing_delete(_IN_ dm_msg_source_t *source, _IN_ dm_msg_dest_t *dest,
                             _OU_ dm_msg_request_payload_t *request, _OU_ dm_msg_response_t *response)
{
    int res = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};

    dm_log_info(DM_URI_THING_DELETE);

    /* Request */
    res = dm_msg_uri_parse_pkdn((char *)source->uri, strlen(source->uri), 2 + DM_URI_OFFSET, 4 + DM_URI_OFFSET, product_key,
                                device_name);
    if (res < SUCCESS_RETURN) {
        return res;
    }

    res = dm_msg_request_parse((char *)source->payload, source->payload_len, request);
    if (res != SUCCESS_RETURN) {
        return res;
    }

    /* Operation */
    res = dm_msg_thing_delete(product_key, device_name);

    /* Response */
    response->service_prefix = DM_URI_SYS_PREFIX;
    response->service_name = dest->uri_name;
    memcpy(response->product_key, product_key, strlen(product_key));
    memcpy(response->device_name, device_name, strlen(device_name));
    response->code = (res == SUCCESS_RETURN) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);

    return SUCCESS_RETURN;
}

int dm_msg_proc_thing_gateway_permit(_IN_ dm_msg_source_t *source, _IN_ dm_msg_dest_t *dest,
                                     _OU_ dm_msg_request_payload_t *request, _OU_ dm_msg_response_t *response)
{
    int res = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};

    dm_log_info(DM_URI_THING_DELETE);

    /* Request */
    res = dm_msg_uri_parse_pkdn((char *)source->uri, strlen(source->uri), 2 + DM_URI_OFFSET, 4 + DM_URI_OFFSET, product_key,
                                device_name);
    if (res < SUCCESS_RETURN) {
        return res;
    }

    res = dm_msg_request_parse((char *)source->payload, source->payload_len, request);
    if (res != SUCCESS_RETURN) {
        return res;
    }

    /* Operation */
    res = dm_msg_thing_gateway_permit(request->params.value, request->params.value_length);

    /* Response */
    response->service_prefix = DM_URI_SYS_PREFIX;
    response->service_name = dest->uri_name;
    memcpy(response->product_key, product_key, strlen(product_key));
    memcpy(response->device_name, device_name, strlen(device_name));
    response->code = (res == SUCCESS_RETURN) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);

    return SUCCESS_RETURN;
}

int dm_msg_proc_thing_sub_register_reply(_IN_ dm_msg_source_t *source)
{
    int res = 0;
    dm_msg_response_payload_t response;

    dm_log_info(DM_URI_THING_SUB_REGISTER_REPLY);

    memset(&response, 0, sizeof(dm_msg_response_payload_t));

    /* Response */
    res = dm_msg_response_parse((char *)source->payload, source->payload_len, &response);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Operation */
    dm_msg_thing_sub_register_reply(&response);

    /* Remove Message From Cache */
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    char int_id[DM_UTILS_UINT32_STRLEN+1] = {0};
    if (response.id.value_length > DM_UTILS_UINT32_STRLEN) {
        return STATE_DEV_MODEL_WRONG_JSON_FORMAT;
    }
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
#endif

    return SUCCESS_RETURN;
}

int dm_msg_proc_thing_sub_unregister_reply(_IN_ dm_msg_source_t *source)
{
    int res = 0;
    dm_msg_response_payload_t response;

    dm_log_info(DM_URI_THING_SUB_UNREGISTER_REPLY);

    memset(&response, 0, sizeof(dm_msg_response_payload_t));

    /* Response */
    res = dm_msg_response_parse((char *)source->payload, source->payload_len, &response);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Operation */
    dm_msg_thing_sub_unregister_reply(&response);

    /* Remove Message From Cache */
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    char int_id[DM_UTILS_UINT32_STRLEN+1] = {0};
    if (response.id.value_length > DM_UTILS_UINT32_STRLEN) {
        return STATE_DEV_MODEL_WRONG_JSON_FORMAT;
    }
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
#endif
    return SUCCESS_RETURN;
}

int dm_msg_proc_thing_topo_add_reply(_IN_ dm_msg_source_t *source)
{
    int res = 0;
    dm_msg_response_payload_t response;

    dm_log_info(DM_URI_THING_TOPO_ADD_REPLY);

    memset(&response, 0, sizeof(dm_msg_response_payload_t));

    /* Response */
    res = dm_msg_response_parse((char *)source->payload, source->payload_len, &response);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Operation */
    dm_msg_thing_topo_add_reply(&response);

    /* Remove Message From Cache */
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    char int_id[DM_UTILS_UINT32_STRLEN+1] = {0};
    if (response.id.value_length > DM_UTILS_UINT32_STRLEN) {
        return STATE_DEV_MODEL_WRONG_JSON_FORMAT;
    }
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
#endif
    return SUCCESS_RETURN;
}

int dm_msg_proc_thing_topo_delete_reply(_IN_ dm_msg_source_t *source)
{
    int res = 0;
    dm_msg_response_payload_t response;

    dm_log_info(DM_URI_THING_TOPO_DELETE_REPLY);

    memset(&response, 0, sizeof(dm_msg_response_payload_t));

    /* Response */
    res = dm_msg_response_parse((char *)source->payload, source->payload_len, &response);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Operation */
    dm_msg_thing_topo_delete_reply(&response);

    /* Remove Message From Cache */
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    char int_id[DM_UTILS_UINT32_STRLEN+1] = {0};
    if (response.id.value_length > DM_UTILS_UINT32_STRLEN) {
        return STATE_DEV_MODEL_WRONG_JSON_FORMAT;
    }
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
#endif
    return SUCCESS_RETURN;
}

int dm_msg_proc_thing_sub_reset_reply(_IN_ dm_msg_source_t *source)
{
    int res = 0;
    dm_msg_response_payload_t response;

    dm_log_info(DM_URI_THING_SUB_RESET_REPLY);

    memset(&response, 0, sizeof(dm_msg_response_payload_t));

    /* Response */
    res = dm_msg_response_parse((char *)source->payload, source->payload_len, &response);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Operation */
    dm_msg_thing_subdev_reset_reply(&response);

    /* Remove Message From Cache */
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    char int_id[DM_UTILS_UINT32_STRLEN+1] = {0};
    if (response.id.value_length > DM_UTILS_UINT32_STRLEN) {
        return STATE_DEV_MODEL_WRONG_JSON_FORMAT;
    }
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
#endif
    return SUCCESS_RETURN;
}

int dm_msg_proc_thing_topo_get_reply(_IN_ dm_msg_source_t *source)
{
    int res = 0;
    dm_msg_response_payload_t response;

    dm_log_info(DM_URI_THING_TOPO_GET_REPLY);

    memset(&response, 0, sizeof(dm_msg_response_payload_t));

    /* Response */
    res = dm_msg_response_parse((char *)source->payload, source->payload_len, &response);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Operation */
    dm_msg_topo_get_reply(&response);

    /* Remove Message From Cache */
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    char int_id[DM_UTILS_UINT32_STRLEN+1] = {0};
    if (response.id.value_length > DM_UTILS_UINT32_STRLEN) {
        return STATE_DEV_MODEL_WRONG_JSON_FORMAT;
    }
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
#endif
    return SUCCESS_RETURN;
}

int dm_msg_proc_thing_topo_change(_IN_ dm_msg_source_t *source, _IN_ dm_msg_dest_t *dest,
                                     _OU_ dm_msg_request_payload_t *request, _OU_ dm_msg_response_t *response)
{
    int res = 0;
    char product_key[PRODUCT_KEY_MAXLEN] = {0};
    char device_name[DEVICE_NAME_MAXLEN] = {0};

    dm_log_info(DM_URI_THING_TOPO_CHANGE);

    /* Request */
    res = dm_msg_uri_parse_pkdn((char *)source->uri, strlen(source->uri), 2 + DM_URI_OFFSET, 4 + DM_URI_OFFSET, product_key,
                                device_name);
    if (res < SUCCESS_RETURN) {
        return res;
    }

    res = dm_msg_request_parse((char *)source->payload, source->payload_len, request);
    if (res != SUCCESS_RETURN) {
        return res;
    }

    /* Operation */
    res = dm_msg_thing_topo_change(request->params.value, request->params.value_length);

    /* Response */
    response->service_prefix = DM_URI_SYS_PREFIX;
    response->service_name = dest->uri_name;
    memcpy(response->product_key, product_key, strlen(product_key));
    memcpy(response->device_name, device_name, strlen(device_name));
    response->code = (res == SUCCESS_RETURN) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);

    return SUCCESS_RETURN;
}

int dm_msg_proc_thing_list_found_reply(_IN_ dm_msg_source_t *source)
{
    int res = 0;
    dm_msg_response_payload_t response;

    dm_log_info(DM_URI_THING_LIST_FOUND_REPLY);

    memset(&response, 0, sizeof(dm_msg_response_payload_t));

    /* Response */
    res = dm_msg_response_parse((char *)source->payload, source->payload_len, &response);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Operation */
    dm_msg_thing_list_found_reply(&response);

    /* Remove Message From Cache */
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    char int_id[DM_UTILS_UINT32_STRLEN+1] = {0};
    if (response.id.value_length > DM_UTILS_UINT32_STRLEN) {
        return STATE_DEV_MODEL_WRONG_JSON_FORMAT;
    }
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
#endif
    return SUCCESS_RETURN;
}

int dm_msg_proc_combine_batch_login_reply(_IN_ dm_msg_source_t *source)
{
    int res = 0;
    dm_msg_response_payload_t response;

    dm_log_info(DM_URI_COMBINE_BATCH_LOGIN_REPLY);

    memset(&response, 0, sizeof(dm_msg_response_payload_t));

    /* Response */
    res = dm_msg_response_parse((char *)source->payload, source->payload_len, &response);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Operation */
    dm_msg_combine_batch_login_reply(&response);

    /* Remove Message From Cache */
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    char int_id[DM_UTILS_UINT32_STRLEN+1] = {0};
    if (response.id.value_length > DM_UTILS_UINT32_STRLEN) {
        return STATE_DEV_MODEL_WRONG_JSON_FORMAT;
    }
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
#endif
    return SUCCESS_RETURN;
}

int dm_msg_proc_combine_batch_logout_reply(_IN_ dm_msg_source_t *source)
{
    int res = 0;
    dm_msg_response_payload_t response;

    dm_log_info(DM_URI_COMBINE_BATCH_LOGOUT_REPLY);

    memset(&response, 0, sizeof(dm_msg_response_payload_t));

    /* Response */
    res = dm_msg_response_parse((char *)source->payload, source->payload_len, &response);
    if (res != SUCCESS_RETURN) {
        return FAIL_RETURN;
    }

    /* Operation */
    dm_msg_combine_batch_logout_reply(&response);

    /* Remove Message From Cache */
#if !defined(DM_MESSAGE_CACHE_DISABLED)
    char int_id[DM_UTILS_UINT32_STRLEN+1] = {0};
    if (response.id.value_length > DM_UTILS_UINT32_STRLEN) {
        return STATE_DEV_MODEL_WRONG_JSON_FORMAT;
    }
    memcpy(int_id, response.id.value, response.id.value_length);
    dm_msg_cache_remove(atoi(int_id));
#endif
    return SUCCESS_RETURN;
}
#endif

#ifdef ALCS_ENABLED
int dm_msg_proc_thing_dev_core_service_dev(_IN_ dm_msg_source_t *source, _IN_ dm_msg_dest_t *dest,
        _OU_ dm_msg_request_payload_t *request, _OU_ dm_msg_response_t *response,
        _OU_ unsigned char **data, int *data_len)
{
    int res = 0;

    dm_log_info(DM_URI_DEV_CORE_SERVICE_DEV);

    /* Request */
    res = dm_msg_request_parse((char *)source->payload, source->payload_len, request);
    if (res < SUCCESS_RETURN) {
        return res ;
    }

    /* Operation */
    res = dm_msg_dev_core_service_dev((char **)data, data_len);
    if (res < SUCCESS_RETURN) {
        return res;
    }

    /* Response */
    response->service_prefix = NULL;
    response->service_name = dest->uri_name;
    response->code = (res == SUCCESS_RETURN) ? (IOTX_DM_ERR_CODE_SUCCESS) : (IOTX_DM_ERR_CODE_REQUEST_ERROR);

    return SUCCESS_RETURN;
}
#endif
