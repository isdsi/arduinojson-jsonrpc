#include <Arduino.h>
#include "json_rpc.h"

// USER CHANGE
void json_rpc_client_send_request(String s)
{
    Serial1.print(s);
}

// USER CHANGE
String json_rpc_client_receive_response(void)
{
    return Serial1.readString();
}

// USER CHANGE
void json_rpc_method_subtract(JsonVariant r, JsonVariant p0, JsonVariant p1, JsonVariant p2)
{
    log_i("p0 %d", p0.as<int>());
    log_i("p1 %d", p1.as<int>());
    log_i("p2 %d", p2.as<int>());
    r.set<int>(p0.as<int>() - p1.as<int>());
    log_i("r %d", r.as<int>());
}

// USER CHANGE
void json_rpc_method_choice(JsonVariant r, JsonVariant p0, JsonVariant p1, JsonVariant p2)
{
    log_i("p0 %s", p0.as<bool>() ? "true" : "false");
    log_i("p1 %s", p1.as<const char *>());
    log_i("p2 %s", p2.as<const char *>());
    if (p0.as<bool>())
        r.set<const char *>(p1.as<const char *>());
    else
        r.set<const char *>(p2.as<const char *>());
    log_i("r %f", r.as<double>());
    
}

// USER CHANGE
void json_rpc_method_add(JsonVariant r, JsonVariant p0, JsonVariant p1, JsonVariant p2)
{
    log_i("p0 %f", p0.as<double>());
    log_i("p1 %f", p1.as<double>());
    log_i("p2 %f", p2.as<double>());
    r.set<double>(p0.as<double>() + p2.as<double>());
    log_i("r %f", r.as<double>());
}

// USER CHANGE
void json_rpc_method_get_obj_name(JsonVariant r, JsonVariant p0, JsonVariant p1, JsonVariant p2)
{
    if (p0.is<JsonObject>() == false)
        return;    

    JsonObject jo = p0.as<JsonObject>();
    log_i("p0[\"name\"] %s", jo["name"].as<const char*>());
    log_i("p1 %f", p1.as<double>());
    log_i("p2 %f", p2.as<double>());
    r.set<const char*>(jo["name"].as<const char*>());
    log_i("r %f", r.as<const char *>());
}

// USER CHANGE
void json_rpc_method_sum_array(JsonVariant r, JsonVariant p0, JsonVariant p1, JsonVariant p2)
{
    if (p0.is<JsonArray>() == false)
    {
        log_i("p0 is not array");
        r.set<int>(0);
        return;    
    }
    int sum = 0;
    JsonArray ja = p0.as<JsonArray>();
    for(int i = 0; i < ja.size(); i++)
    {
        if (ja[i].is<int>() == false)
        {
            log_i("ja[%d] is not int", i);
            r.set<int>(0);
            return;    
        }
        sum += ja[i].as<int>();
    }
    
    r.set<int>(sum);
    log_i("r %d", r.as<int>());    
}

// USER CHANGE
void json_rpc_method_sum_array_object(JsonVariant r, JsonVariant p0, JsonVariant p1, JsonVariant p2)
{
    if (p0.is<JsonArray>() == false)
    {
        log_i("p0 is not array");
        r.set<int>(0);
        return;    
    }
    int sum = 0;
    JsonArray ja = p0.as<JsonArray>();
    for(int i = 0; i < ja.size(); i++)
    {
        if (ja[i].is<JsonObject>() == false)
        {
            log_i("ja[%d] is not int", i);
            r.set<int>(0);
            return;    
        }
        JsonObject jo = ja[i].as<JsonObject>();
        if (jo["age"].is<int>() == false)
        {
            log_i("jo[\"age\"] is not int", i);
            r.set<int>(0);
            return;    
        }
        sum += jo["age"].as<int>();
    }
    
    r.set<int>(sum);
    log_i("r %d", r.as<int>());    
}

// USER CHANGE
void json_rpc_method_sum_all_object(JsonVariant r, JsonVariant p0, JsonVariant p1, JsonVariant p2)
{
    if (p0.is<JsonObject>() == false)
    {
        log_i("p0 is not object");
        r.set<int>(0);
        return;    
    }
    if (p1.is<JsonObject>() == false)
    {
        log_i("p1 is not object");
        r.set<int>(0);
        return;    
    }
    if (p2.is<JsonObject>() == false)
    {
        log_i("p2 is not object");
        r.set<int>(0);
        return;    
    }
    int ip0, ip1, ip2;
    JsonObject jo;
    jo = p0.as<JsonObject>();
    if (jo["age"].is<int>() == false)
        {
        log_i("p2 is not int");
        r.set<int>(0);
        return;    
    }
    ip0 = jo["age"].as<int>();

    jo = p1.as<JsonObject>();
    if (jo["age"].is<int>() == false)
        {
        log_i("p2 is not int");
        r.set<int>(0);
        return;    
    }
    ip1 = jo["age"].as<int>();

    jo = p2.as<JsonObject>();
    if (jo["age"].is<int>() == false)
        {
        log_i("p2 is not int");
        r.set<int>(0);
        return;    
    }
    ip2 = jo["age"].as<int>();

    r.set<int>(ip0 + ip1 + ip2);
    log_i("r %f", r.as<int>());
}

// USER CHANGE
void json_rpc_method_say_hello(JsonVariant r, JsonVariant p0, JsonVariant p1, JsonVariant p2)
{
    r.set<const char *>("hello");
    log_i("r %s", r.as<const char *>());
}


void setup(void)
{
    Serial.begin(115200);
    Serial1.begin(115200, SERIAL_8N1, 18, 17);
    Serial.printf("arduino-json-rpc-server-example\n");
    
    log_i("info");
    json_rpc_server_add_method("subtract", json_rpc_method_subtract, "minuend", JSON_RPC_PARAM_INT, "subtrahend", JSON_RPC_PARAM_INT, "", JSON_RPC_PARAM_NONE);
    json_rpc_server_add_method("choice", json_rpc_method_choice, "select", JSON_RPC_PARAM_BOOL, "text1", JSON_RPC_PARAM_STRING, "text2", JSON_RPC_PARAM_STRING);
    json_rpc_server_add_method("add", json_rpc_method_add, "a", JSON_RPC_PARAM_FLOAT, "", JSON_RPC_PARAM_NONE, "c", JSON_RPC_PARAM_FLOAT);
    json_rpc_server_add_method("get_obj_name", json_rpc_method_get_obj_name, "obj", JSON_RPC_PARAM_OBJECT, "", JSON_RPC_PARAM_NONE, "", JSON_RPC_PARAM_NONE);
    json_rpc_server_add_method("sum_array", json_rpc_method_sum_array, "obj", JSON_RPC_PARAM_ARRAY, "", JSON_RPC_PARAM_NONE, "", JSON_RPC_PARAM_NONE);
    json_rpc_server_add_method("sum_array_object", json_rpc_method_sum_array_object, "obj", JSON_RPC_PARAM_ARRAY, "", JSON_RPC_PARAM_NONE, "", JSON_RPC_PARAM_NONE);
    json_rpc_server_add_method("sum_all_object", json_rpc_method_sum_all_object, "a", JSON_RPC_PARAM_OBJECT, "b", JSON_RPC_PARAM_OBJECT, "c", JSON_RPC_PARAM_OBJECT);
    json_rpc_server_add_method("say_hello", json_rpc_method_say_hello, "", JSON_RPC_PARAM_NONE, "", JSON_RPC_PARAM_NONE, "", JSON_RPC_PARAM_NONE);
}

void loop(void)
{

    String s;
    
    // rpc call with positional parameters
    json_rpc_client_send_request("{\"jsonrpc\":\"2.0\",\"method\":\"subtract\",\"params\":[42,23],\"id\":1}");
    json_rpc_loop();
    s = json_rpc_client_receive_response();
    if (strcmp("{\"jsonrpc\":\"2.0\",\"result\":19,\"id\":1}", s.c_str()) != 0)
        Serial.printf("error\n");
    else
        Serial.printf("ok\n");
      
    json_rpc_client_send_request("{\"jsonrpc\":\"2.0\",\"method\":\"subtract\",\"params\":[23,42],\"id\":1}");
    json_rpc_loop();
    s = json_rpc_client_receive_response();
    if (strcmp("{\"jsonrpc\":\"2.0\",\"result\":-19,\"id\":1}", s.c_str()) != 0)
        Serial.printf("error\n");
    else
        Serial.printf("ok\n");
    
    // rpc call with named parameters:
    json_rpc_client_send_request("{\"jsonrpc\":\"2.0\",\"method\":\"subtract\",\"params\":{\"subtrahend\":23,\"minuend\":42},\"id\":1}");
    json_rpc_loop();
    s = json_rpc_client_receive_response();
    if (strcmp("{\"jsonrpc\":\"2.0\",\"result\":19,\"id\":1}", s.c_str()) != 0)
        Serial.printf("error\n");
    else
        Serial.printf("ok\n");
    
    json_rpc_client_send_request("{\"jsonrpc\":\"2.0\",\"method\":\"subtract\",\"params\":{\"minuend\":42,\"subtrahend\":23},\"id\":1}");
    json_rpc_loop();
    s = json_rpc_client_receive_response();
    if (strcmp("{\"jsonrpc\":\"2.0\",\"result\":19,\"id\":1}", s.c_str()) != 0)
        Serial.printf("error\n");
    else
        Serial.printf("ok\n");
    
    // rpc call of non-existent method:
    json_rpc_client_send_request("{\"jsonrpc\":\"2.0\",\"method\":\"foobar\",\"id\":\"1\"}");
    json_rpc_loop();
    s = json_rpc_client_receive_response();
    if (strcmp("{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32601,\"message\":\"Method not found\"},\"id\":1}", s.c_str()) != 0)
        Serial.printf("error\n");
    else
        Serial.printf("ok\n");

    // rpc call with invalid JSON:
    json_rpc_client_send_request("{\"jsonrpc\": \"2.0\", \"method\": \"foobar, \"params\": \"bar\", \"baz]");
    json_rpc_loop();
    s = json_rpc_client_receive_response();
    if (strcmp("{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32700,\"message\":\"Parse error\"},\"id\":null}", s.c_str()) != 0)
        Serial.printf("error\n");
    else
        Serial.printf("ok\n");
        
    // rpc call with invalid Request object:
    json_rpc_client_send_request("{\"jsonrpc\":\"2.0\",\"method\":1,\"params\":\"bar\"}");
    json_rpc_loop();
    s = json_rpc_client_receive_response();
    if (strcmp("{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32600,\"message\":\"Invalid Request\"},\"id\":null}", s.c_str()) != 0)
        Serial.printf("error\n");
    else
        Serial.printf("ok\n");

    // rpc call Batch, invalid JSON:
    json_rpc_client_send_request("[{\"jsonrpc\": \"2.0\", \"method\": \"sum\", \"params\": [1,2,4], \"id\": \"1\"},{\"jsonrpc\": \"2.0\", \"method\"]");
    json_rpc_loop();
    s = json_rpc_client_receive_response();
    if (strcmp("{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32700,\"message\":\"Parse error\"},\"id\":null}", s.c_str()) != 0)
        Serial.printf("error\n");
    else
        Serial.printf("ok\n");

    
    // CUSTOM TEST CASE

    // param type is bool, string
    json_rpc_client_send_request("{\"jsonrpc\":\"2.0\",\"method\":\"choice\",\"params\":{\"select\":true,\"text1\":\"seoul\",\"text2\":\"tokyo\"},\"id\":1}");
    json_rpc_loop();
    s = json_rpc_client_receive_response();
    if (strcmp("{\"jsonrpc\":\"2.0\",\"result\":\"seoul\",\"id\":1}", s.c_str()) != 0)
        Serial.printf("error\n");
    else
        Serial.printf("ok\n");

    json_rpc_client_send_request("{\"jsonrpc\":\"2.0\",\"method\":\"choice\",\"params\":{\"select\":false,\"text1\":\"seoul\",\"text2\":\"tokyo\"},\"id\":1}");
    json_rpc_loop();
    s = json_rpc_client_receive_response();
    if (strcmp("{\"jsonrpc\":\"2.0\",\"result\":\"tokyo\",\"id\":1}", s.c_str()) != 0)
        Serial.printf("error\n");
    else
        Serial.printf("ok\n");

    json_rpc_client_send_request("{\"jsonrpc\":\"2.0\",\"method\":\"add\",\"params\":[23.1, 0, 42.555],\"id\":1}");
    json_rpc_loop();
    s = json_rpc_client_receive_response();
    if (strcmp("{\"jsonrpc\":\"2.0\",\"result\":65.65500069,\"id\":1}", s.c_str()) != 0)
        Serial.printf("error\n");
    else
        Serial.printf("ok\n");
    
    // param type is float, param a,c use, param b is no use
    json_rpc_client_send_request("{\"jsonrpc\":\"2.0\",\"method\":\"add\",\"params\":{\"a\":23.1,\"c\":42.555},\"id\":111}");
    json_rpc_loop();
    s = json_rpc_client_receive_response();
    if (strcmp("{\"jsonrpc\":\"2.0\",\"result\":65.65500069,\"id\":111}", s.c_str()) != 0)
        Serial.printf("error\n");
    else
        Serial.printf("ok\n");

    json_rpc_client_send_request("{\"jsonrpc\":\"2.0\",\"method\":\"add\",\"params\":[23.1, 0, 42.555],\"id\":111}");
    json_rpc_loop();
    s = json_rpc_client_receive_response();
    if (strcmp("{\"jsonrpc\":\"2.0\",\"result\":65.65500069,\"id\":111}", s.c_str()) != 0)
        Serial.printf("error\n");
    else
        Serial.printf("ok\n");
    
    // param type is object, Parameter type checking is performed only up to depth level 1.
    json_rpc_client_send_request("{\"jsonrpc\":\"2.0\",\"method\":\"get_obj_name\",\"params\": { \"obj\" : {\"name\":\"myself\"} },\"id\":1}");
    json_rpc_loop();
    s = json_rpc_client_receive_response();
    if (strcmp("{\"jsonrpc\":\"2.0\",\"result\":\"myself\",\"id\":1}", s.c_str()) != 0)
        Serial.printf("error\n");
    else
        Serial.printf("ok\n");        

    // param 1 type is array, Parameter type checking is performed only up to depth level 1.
    json_rpc_client_send_request("{\"jsonrpc\":\"2.0\",\"method\":\"sum_array\",\"params\": { \"obj\" : [ 1,2,3,4,5 ]},\"id\":1}");
    json_rpc_loop();
    s = json_rpc_client_receive_response();
    if (strcmp("{\"jsonrpc\":\"2.0\",\"result\":15,\"id\":1}", s.c_str()) != 0)
        Serial.printf("error\n");
    else
        Serial.printf("ok\n");    

    // param 1 type is array of object
    json_rpc_client_send_request("{\"jsonrpc\":\"2.0\",\"method\":\"sum_array_object\",\"params\": { \"obj\" : [ {\"age\": 1 }, {\"age\": 2 }, {\"age\": 3 } ]},\"id\":1}");
    json_rpc_loop();
    s = json_rpc_client_receive_response();
    if (strcmp("{\"jsonrpc\":\"2.0\",\"result\":6,\"id\":1}", s.c_str()) != 0)
        Serial.printf("error\n");
    else
        Serial.printf("ok\n");   

    // param 1, param 2, param 3 are all object
    json_rpc_client_send_request("{\"jsonrpc\":\"2.0\",\"method\":\"sum_all_object\",\"params\": [ {\"age\": 1 }, {\"age\": 2 }, {\"age\": 3 } ],\"id\":1}");
    json_rpc_loop();
    s = json_rpc_client_receive_response();
    if (strcmp("{\"jsonrpc\":\"2.0\",\"result\":6,\"id\":1}", s.c_str()) != 0)
        Serial.printf("error\n");
    else
        Serial.printf("ok\n");   

    // there is no param
    json_rpc_client_send_request("{\"jsonrpc\":\"2.0\",\"method\":\"say_hello\",\"params\": null,\"id\":1}");
    json_rpc_loop();
    s = json_rpc_client_receive_response();
    if (strcmp("{\"jsonrpc\":\"2.0\",\"result\":\"hello\",\"id\":1}", s.c_str()) != 0)
        Serial.printf("error\n");
    else
        Serial.printf("ok\n");   

    // there is no param with array
    json_rpc_client_send_request("{\"jsonrpc\":\"2.0\",\"method\":\"say_hello\",\"params\": [],\"id\":1}");
    json_rpc_loop();
    s = json_rpc_client_receive_response();
    if (strcmp("{\"jsonrpc\":\"2.0\",\"result\":\"hello\",\"id\":1}", s.c_str()) != 0)
        Serial.printf("error\n");
    else
        Serial.printf("ok\n");   
    
    delay(1000);
}
