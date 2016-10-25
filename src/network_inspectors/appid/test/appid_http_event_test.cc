//--------------------------------------------------------------------------
// Copyright (C) 2016-2016 Cisco and/or its affiliates. All rights reserved.
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License Version 2 as published
// by the Free Software Foundation.  You may not use, modify or distribute
// this program under any other version of the GNU General Public License.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//--------------------------------------------------------------------------

// appid_http_event_test.cc author Steve Chew <stechew@cisco.com>
// unit test for the http inspection event handler.

//  Must be included before CppUTest files to avoid compiler error.
#include "network_inspectors/appid/appid_http_event_handler.cc"

#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include <string>

#include "framework/data_bus.h"
#include "protocols/protocol_ids.h"
#include "service_inspectors/http_inspect/http_msg_header.h"

AppIdConfig* pAppidActiveConfig = nullptr;
AppIdApi appid_api;
THREAD_LOCAL ThirdPartyAppIDModule* thirdparty_appid_module = nullptr;

char* snort_strndup(const char* src, size_t dst_size)
{
    return strndup(src, dst_size);
}

char* snort_strdup(const char* src)
{
    return strdup(src);
}

FlowData::FlowData(unsigned, Inspector *)
{
}

FlowData::~FlowData()
{
}

void Flow::set_application_ids(AppId, AppId, AppId, AppId) { }

const char *content_type = nullptr;
const char *host = nullptr;
const char *referer = nullptr;
int32_t response_code = 0;
const char *server = nullptr;
const char *x_working_with = nullptr;
const char *url = nullptr;
const char *useragent = nullptr;
const char *via = nullptr;

void Field::set(int32_t length_, const uint8_t* start_)
{
    start = start_;
    length = length_;
}

Field global_field;

const Field& HttpMsgSection::get_classic_buffer(unsigned id, uint64_t sub_id, uint64_t)
{
    global_field.set(0, nullptr);
    if(id == HttpEnums::HTTP_BUFFER_HEADER)
    {
        if(sub_id == HttpEnums::HEAD_HOST)
        {
            if(host)
                global_field.set(strlen(host), (const uint8_t*)host);
        }

        if(sub_id == HttpEnums::HEAD_USER_AGENT)
        {
            if(useragent)
                global_field.set(strlen(useragent), (const uint8_t*)useragent);
        }

        if(sub_id == HttpEnums::HEAD_REFERER)
        {
            if(referer)
                global_field.set(strlen(referer), (const uint8_t*)referer);
        }

        if(sub_id == HttpEnums::HEAD_SERVER)
        {
            if(server)
                global_field.set(strlen(server), (const uint8_t*)server);
        }

        if(sub_id == HttpEnums::HEAD_X_WORKING_WITH)
        {
            if(x_working_with)
                global_field.set(strlen(x_working_with), (const uint8_t*)x_working_with);
        }

        if(sub_id == HttpEnums::HEAD_VIA)
        {
            if(via)
                global_field.set(strlen(via), (const uint8_t*)via);
        }

        if(sub_id == HttpEnums::HEAD_CONTENT_TYPE)
        {
            if(content_type)
                global_field.set(strlen(content_type), (const uint8_t*)content_type);
        }
    }

    if(id == HttpEnums::HTTP_BUFFER_URI)
    {
        if(sub_id == 0)
        {
            if(url)
                global_field.set(strlen(url), (const uint8_t*)url);
        }
    }

    return global_field;
}

class FakeHttpMsgHeader
{
};

unsigned AppIdSession::flow_id = 0;
AppIdSession *fake_session = nullptr;
FakeHttpMsgHeader *fake_msg_header = nullptr;

AppIdSession::AppIdSession(IpProtocol, const sfip_t*) : FlowData(flow_id, nullptr)
{
}

AppIdSession::~AppIdSession()
{
    if(!hsession)
        return;

    if(hsession->content_type) free(hsession->content_type);
    if(hsession->host) free(hsession->host);
    if(hsession->referer) free(hsession->referer);
    if(hsession->response_code) free(hsession->response_code);
    if(hsession->server) free(hsession->server);
    if(hsession->url) free(hsession->url);
    if(hsession->useragent) free(hsession->useragent);
    if(hsession->via) free(hsession->via);
    if(hsession->x_working_with) free(hsession->x_working_with);
    snort_free(hsession);
}

int AppIdSession::processHTTPPacket(int)
{
    return 0;
}

AppId AppIdSession::pick_service_app_id()
{
    return 0;
}

AppId AppIdSession::pick_client_app_id()
{
    return 0;
}

AppId AppIdSession::pick_payload_app_id()
{
    return 0;
}

AppId AppIdSession::pick_misc_app_id()
{
    return 0;
}

AppIdSession* AppIdApi::get_appid_data(Flow*)
{
    mock().actualCall("get_appid_data");
    return fake_session;
}

const uint8_t* HttpEvent::get_content_type(unsigned &length)
{
    global_field.set(0, nullptr);
    if(content_type)
        global_field.set(strlen(content_type), (const uint8_t*)content_type);
    length = global_field.length;
    return global_field.start;
}

const uint8_t* HttpEvent::get_host(unsigned &length)
{
    global_field.set(0, nullptr);
    if(host)
        global_field.set(strlen(host), (const uint8_t*)host);
    length = global_field.length;
    return global_field.start;
}

const uint8_t* HttpEvent::get_referer(unsigned &length)
{
    global_field.set(0, nullptr);
    if(referer)
        global_field.set(strlen(referer), (const uint8_t*)referer);
    length = global_field.length;
    return global_field.start;
}

int32_t HttpEvent::get_response_code()
{
    return response_code;
}

const uint8_t* HttpEvent::get_server(unsigned &length)
{
    global_field.set(0, nullptr);
    if(server)
        global_field.set(strlen(server), (const uint8_t*)server);
    length = global_field.length;
    return global_field.start;
}

const uint8_t* HttpEvent::get_uri(unsigned &length)
{
    global_field.set(0, nullptr);
    if(url)
        global_field.set(strlen(url), (const uint8_t*)url);
    length = global_field.length;
    return global_field.start;
}

const uint8_t* HttpEvent::get_user_agent(unsigned &length)
{
    global_field.set(0, nullptr);
    if(useragent)
        global_field.set(strlen(useragent), (const uint8_t*)useragent);
    length = global_field.length;
    return global_field.start;
}

const uint8_t* HttpEvent::get_via(unsigned &length)
{
    global_field.set(0, nullptr);
    if(via)
        global_field.set(strlen(via), (const uint8_t*)via);
    length = global_field.length;
    return global_field.start;
}

const uint8_t* HttpEvent::get_x_working_with(unsigned &length)
{
    global_field.set(0, nullptr);
    if(x_working_with)
        global_field.set(strlen(x_working_with), (const uint8_t*)x_working_with);
    length = global_field.length;
    return global_field.start;
}

Flow::Flow() {}
Flow::~Flow() {}

class FakeFlow : public Flow
{
};

#ifdef DEBUG_MSGS
void Debug::print(const char*, int, uint64_t, const char*, ...) { }
#endif

TEST_GROUP(appid_http_event)
{
    void setup()
    {
        appid_stats.http_flows = 0;
    }

    void teardown()
    {
        fake_msg_header = nullptr;
        fake_session = nullptr;
        mock().clear();
    }
};

TEST(appid_http_event, handle_null_appid_data)
{
    FakeFlow flow;
    HttpEvent event(nullptr);
    HttpEventHandler event_handler(HttpEventHandler::REQUEST_EVENT);
    mock().expectOneCall("get_appid_data");
    event_handler.handle(event, &flow);
    mock().checkExpectations();
}

TEST(appid_http_event, handle_null_msg_header)
{
    FakeFlow flow;
    HttpEvent event(nullptr);
    AppIdSession session(IpProtocol::TCP, nullptr);
    HttpEventHandler event_handler(HttpEventHandler::REQUEST_EVENT);
    fake_session = &session;

    mock().strictOrder();
    mock().expectOneCall("get_appid_data");
    event_handler.handle(event, &flow);
    mock().checkExpectations();
}

#define CONTENT_TYPE "html/text"
#define HOST "www.google.com"
#define URL "http://www.google.com/path/to/index.html"
#define USERAGENT "Mozilla/5.0 (Macintosh; Intel Mac OS X)"
#define REFERER "http://www.yahoo.com/search"
#define RESPONSE_CODE 301
#define SERVER "Apache"
#define X_WORKING_WITH "working with string"
#define VIA "via string"

struct TestData
{
    HttpEventHandler::HttpEventType type = HttpEventHandler::REQUEST_EVENT;
    unsigned scan_flags = 0;
    PegCount http_flows = 1;   // Default to 1 since most tests have 1.
    const char *content_type = nullptr;
    const char *host = nullptr;
    const char *referer = nullptr;
    int32_t response_code = 0;
    const char *server = nullptr;
    const char *x_working_with = nullptr;
    const char *url = nullptr;
    const char *useragent = nullptr;
    const char *via = nullptr;
};

void run_event_handler(TestData test_data, TestData *expect_data = nullptr)
{
    FakeFlow flow;
    HttpEvent event(nullptr);
    AppIdSession session(IpProtocol::TCP, nullptr);
    FakeHttpMsgHeader http_msg_header;
    HttpEventHandler event_handler(test_data.type);
    fake_session = &session;
    fake_msg_header = &http_msg_header;

    host = test_data.host;
    referer = test_data.referer;
    server = test_data.server;
    x_working_with = test_data.x_working_with;
    url = test_data.url;
    useragent = test_data.useragent;
    via = test_data.via;
    content_type = test_data.content_type;
    response_code = test_data.response_code;

    if(expect_data == nullptr)
        expect_data = &test_data;

    mock().strictOrder();
    mock().expectOneCall("get_appid_data");
    event_handler.handle(event, &flow);
    LONGS_EQUAL(expect_data->scan_flags, session.scan_flags);
    LONGS_EQUAL(expect_data->http_flows, appid_stats.http_flows);
    STRCMP_EQUAL(expect_data->host, session.hsession->host);
    STRCMP_EQUAL(expect_data->url, session.hsession->url);
    STRCMP_EQUAL(expect_data->content_type, session.hsession->content_type);
    STRCMP_EQUAL(expect_data->referer, session.hsession->referer);
    STRCMP_EQUAL(expect_data->server, session.hsession->server);
    STRCMP_EQUAL(expect_data->x_working_with, session.hsession->x_working_with);
    STRCMP_EQUAL(expect_data->useragent, session.hsession->useragent);
    STRCMP_EQUAL(expect_data->via, session.hsession->via);
    if(nullptr == session.hsession->response_code)
    {
        LONGS_EQUAL(0, expect_data->response_code);
    }
    else
    {
        LONGS_EQUAL(expect_data->response_code, strtol(session.hsession->response_code, nullptr, 10));
    }
    mock().checkExpectations();
}

TEST(appid_http_event, handle_msg_header_no_headers_exist)
{
    TestData test_data;
    test_data.scan_flags = 0;
    
    run_event_handler(test_data);
}


TEST(appid_http_event, handle_msg_header_only_host)
{
    TestData test_data;
    test_data.scan_flags = SCAN_HTTP_HOST_URL_FLAG;
    test_data.host = HOST;
    
    run_event_handler(test_data);
}

TEST(appid_http_event, handle_msg_header_host_and_url)
{
    TestData test_data;
    test_data.scan_flags = SCAN_HTTP_HOST_URL_FLAG;
    test_data.host = HOST;
    test_data.url = URL;
    
    run_event_handler(test_data);
}

TEST(appid_http_event, handle_msg_header_user_agent)
{
    TestData test_data;
    test_data.scan_flags = SCAN_HTTP_USER_AGENT_FLAG;
    test_data.useragent = USERAGENT;
    
    run_event_handler(test_data);
}

TEST(appid_http_event, handle_msg_header_x_working_with)
{
    TestData test_data;
    test_data.scan_flags = 0;
    test_data.x_working_with = X_WORKING_WITH;
    
    run_event_handler(test_data);
}

TEST(appid_http_event, handle_msg_header_referer)
{
    TestData test_data;
    test_data.scan_flags = 0;
    test_data.referer = REFERER;
    
    run_event_handler(test_data);
}

TEST(appid_http_event, handle_msg_header_via)
{
    TestData test_data;
    test_data.scan_flags = SCAN_HTTP_VIA_FLAG;
    test_data.via = VIA;
    
    run_event_handler(test_data);
}

TEST(appid_http_event, handle_msg_header_content_type)
{
    TestData test_data;
    test_data.type = HttpEventHandler::RESPONSE_EVENT;
    test_data.scan_flags = 0;
    test_data.http_flows = 0;   //  Flows are only counted on request header
    test_data.content_type = CONTENT_TYPE;
    
    run_event_handler(test_data);
}

TEST(appid_http_event, handle_msg_header_server)
{
    TestData test_data;
    test_data.type = HttpEventHandler::RESPONSE_EVENT;
    test_data.scan_flags = 0;
    test_data.http_flows = 0;   //  Flows are only counted on request header
    test_data.server = SERVER;
    
    run_event_handler(test_data);
}

TEST(appid_http_event, handle_msg_header_response_code)
{
    TestData test_data;
    test_data.type = HttpEventHandler::RESPONSE_EVENT;
    test_data.scan_flags = 0;
    test_data.http_flows = 0;   //  Flows are only counted on request header
    test_data.response_code = RESPONSE_CODE;
    
    run_event_handler(test_data);
}

TEST(appid_http_event, handle_msg_header_response_code_out_of_range)
{
    TestData test_data;
    test_data.type = HttpEventHandler::RESPONSE_EVENT;
    test_data.scan_flags = 0;
    test_data.http_flows = 0;   //  Flows are only counted on request header
    test_data.response_code = 1000;
    
    TestData expect_data = test_data;
    expect_data.response_code = 0;

    run_event_handler(test_data, &expect_data);
}

TEST(appid_http_event, handle_msg_header_all_response_headers)
{
    TestData test_data;
    test_data.type = HttpEventHandler::RESPONSE_EVENT;
    test_data.scan_flags = 1;
    test_data.http_flows = 0;   //  Flows are only counted on request header
    test_data.response_code = RESPONSE_CODE;
    test_data.content_type = CONTENT_TYPE;
    test_data.via = VIA;

    TestData expect_data = test_data;

    //  Fill in the request data too to demonstrate that it is not copied
    //  during response header handling.
    test_data.url = URL;
    test_data.host = HOST;
    test_data.referer = REFERER;
    test_data.useragent = USERAGENT;
    
    run_event_handler(test_data, &expect_data);
}

TEST(appid_http_event, handle_msg_header_all_request_headers)
{
    TestData test_data;
    test_data.type = HttpEventHandler::REQUEST_EVENT;
    test_data.scan_flags = SCAN_HTTP_VIA_FLAG | SCAN_HTTP_USER_AGENT_FLAG |
        SCAN_HTTP_HOST_URL_FLAG;
    test_data.url = URL;
    test_data.host = HOST;
    test_data.referer = REFERER;
    test_data.useragent = USERAGENT;
    test_data.via = VIA;

    TestData expect_data = test_data;

    //  Fill in the request data too to demonstrate that it is not copied
    //  during response header handling.
    test_data.response_code = RESPONSE_CODE;
    test_data.content_type = CONTENT_TYPE;
    
    run_event_handler(test_data, &expect_data);
}

int main(int argc, char** argv)
{
    int return_value = CommandLineTestRunner::RunAllTests(argc, argv);
    return return_value;
}
