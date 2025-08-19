#include <Arduino.h>
#include <ArduinoJson.h>

const int JSON_RPC_ERROR_CODE_PARSE_ERROR = -32700; // Invalid JSON was received by the server.An error occurred on the server while parsing the JSON text.
const int JSON_RPC_ERROR_CODE_INVALID_REQUEST = -32600; // The JSON sent is not a valid Request object.
const int JSON_RPC_ERROR_CODE_METHOD_NOT_FOUND = -32601; // The method does not exist / is not available.
const int JSON_RPC_ERROR_CODE_INVALID_PARAMS = -32602; // Invalid method parameter(s).
const int JSON_RPC_ERROR_CODE_INTERNAL_ERROR = -32603; // Internal JSON - RPC error.

const char json_rpc_error_message[][20] = {
    "Parse error",
    "Invalid Request",
    "Method not found",
    "Invalid params",
    "Internal error",
    "Server error"
};
const int json_rpc_error_message_count = sizeof(json_rpc_error_message) / (sizeof(char) * 20);

const char* json_rpc_get_error_message(int json_rpc_error_code)
{
    const char* p = NULL;
    switch(json_rpc_error_code)
    {
        case JSON_RPC_ERROR_CODE_PARSE_ERROR: p = json_rpc_error_message[0]; break;
        case JSON_RPC_ERROR_CODE_INVALID_REQUEST: p = json_rpc_error_message[1]; break;
        case JSON_RPC_ERROR_CODE_METHOD_NOT_FOUND: p = json_rpc_error_message[2]; break;
        case JSON_RPC_ERROR_CODE_INVALID_PARAMS: p = json_rpc_error_message[3]; break;
        case JSON_RPC_ERROR_CODE_INTERNAL_ERROR: p = json_rpc_error_message[4]; break;
        default : p = json_rpc_error_message[5]; break;
    }
    return p;
}

JsonVariant jvNull;
DeserializationError error;

// method
//typedef void (*json_rpc_method_t)(JsonDocument &jd, JsonDocument &jr);
typedef void (*json_rpc_method_t)(JsonVariant r, JsonVariant p0, JsonVariant p1, JsonVariant p2);

// param
enum json_rpc_param_t {
    JSON_RPC_PARAM_NONE,
    JSON_RPC_PARAM_BOOL,
    JSON_RPC_PARAM_INT,
    JSON_RPC_PARAM_FLOAT,
    JSON_RPC_PARAM_STRING,
    JSON_RPC_PARAM_ARRAY,
    JSON_RPC_PARAM_OBJECT
};

// json_rpc_method_t json_rpc_method_array[10];
JsonDocument json_rpc_server;

void json_rpc_make_error(JsonDocument &jd, int error_code, const char *key, int line)
{
    jd["jsonrpc"] = "2.0";
    jd["error"]["code"] = error_code;
    jd["error"]["message"] = json_rpc_get_error_message(error_code);
    //jd["error"]["key"] = key;
    //jd["error"]["data"] = line;
    jd["id"] = jvNull;
}

void json_rpc_make_error_id(JsonDocument &jd, int error_code, const char *key, int line, int id)
{
    jd["jsonrpc"] = "2.0";
    jd["error"]["code"] = error_code;
    jd["error"]["message"] = json_rpc_get_error_message(error_code);
    //jd["error"]["key"] = key;
    //jd["error"]["data"] = line;
    jd["id"] = id;
}

void json_rpc_server_add_method(const char* method, json_rpc_method_t m, 
    const char *p0Name, json_rpc_param_t p0Type, 
    const char *p1Name, json_rpc_param_t p1Type, 
    const char *p2Name, json_rpc_param_t p2Type)
{
    json_rpc_server[method]["func"] = (unsigned int)m;
    if (p0Name != NULL && strlen(p0Name) != 0)
        json_rpc_server[method]["params"][p0Name] = p0Type;
    if (p1Name != NULL && strlen(p1Name) != 0)
        json_rpc_server[method]["params"][p1Name] = p1Type;
    if (p2Name != NULL && strlen(p2Name) != 0)
        json_rpc_server[method]["params"][p2Name] = p2Type;
}

bool json_rpc_server_contain_method(const char* method)
{
    if (json_rpc_server[method]["func"].is<unsigned int>() == false)
        return false;

    return true;
}

json_rpc_method_t json_rpc_server_get_method(const char* method)
{
    if (json_rpc_server[method]["func"].is<unsigned int>() == false)
        return NULL;

    return (json_rpc_method_t)json_rpc_server[method]["func"].as<unsigned int>();
}

JsonObject json_rpc_server_get_params(const char* method)
{
    JsonObject joNull;
    if (json_rpc_server[method]["params"].is<JsonObject>() == false)
        return joNull;

    return json_rpc_server[method]["params"].as<JsonObject>();
}

bool json_rpc_server_receive_request(JsonDocument &jd)
{
    String s = Serial1.readString();
    Serial.print("--> ");
    Serial.println(s);
    error = deserializeJson(jd, s.c_str());
    if (error != DeserializationError::Ok)
    {
        return false;
    }
    return true;
}

void json_rpc_server_send_response(JsonDocument &jd)
{
    String s;
    serializeJson(jd, s);
    Serial1.print(s);    
    Serial.print("<-- ");
    Serial.println(s);
}

void json_rpc_client_send_request(String s)
{
    Serial1.print(s);
}

String json_rpc_client_receive_response(void)
{
    return Serial1.readString();
}

bool json_rpc_variant_is(JsonVariant v, json_rpc_param_t p)
{
    if (p == JSON_RPC_PARAM_NONE)
        return false;
    else if (p == JSON_RPC_PARAM_BOOL)
    {
        if (v.is<bool>() == false)
            return false;
        else
            Serial.printf("v %s\n", v.as<bool>() ? "true" : "false");
    }
    else if (p == JSON_RPC_PARAM_INT)
    {
        if (v.is<int>() == false)
            return false;
        else
            Serial.printf("v %d\n", v.as<int>());
    }
    else if (p == JSON_RPC_PARAM_FLOAT)
    {
        if (v.is<double>() == false)
            return false;
        else
            Serial.printf("v %f\n", v.as<double>());
    }
    else if (p == JSON_RPC_PARAM_STRING)
    {
        if (v.is<const char *>() == false)
            return false;
        else
            Serial.printf("v %s\n", v.as<const char *>());
    }
    else if (p == JSON_RPC_PARAM_ARRAY)
    {
        if (v.is<JsonArray>() == false)
            return false;
        else
            Serial.printf("v array count %d\n", v.as<JsonArray>().size());
    }
    else if (p == JSON_RPC_PARAM_OBJECT)
    {
        if (v.is<JsonObject>() == false)
            return false;
        else
            Serial.printf("v object count %d\n", v.as<JsonObject>().size());
    }

    return true;
}

void json_rpc_loop(void)
{
    JsonDocument jd;
    JsonDocument jr;
    //JsonObject jo;
    String s;
    // s = Serial1.readString();
    // if (s == "Test Data")
    // {
    //     Serial1.print("Test Data1");
    //     return;
    // }

    if (json_rpc_server_receive_request(jd) == false)
    {
        json_rpc_make_error(jr, JSON_RPC_ERROR_CODE_PARSE_ERROR, "jsonrpc", (int)__LINE__);
        json_rpc_server_send_response(jr);
        return;
    }

    //jo = jd.as<JsonObject>(); // as<>()함수는 jd를 초기화 하지 않는다. // JsonObject는 가리키기만 한다.

    if (jd["jsonrpc"].is<const char*>() == false)
    {
        json_rpc_make_error(jr, JSON_RPC_ERROR_CODE_INVALID_REQUEST, "jsonrpc", (int)__LINE__);
        json_rpc_server_send_response(jr);
        return;
    }
    
    if (strcmp(jd["jsonrpc"].as<const char*>(), "2.0") != 0)
    {
        json_rpc_make_error(jr, JSON_RPC_ERROR_CODE_INVALID_REQUEST, "jsonrpc", (int)__LINE__);
        json_rpc_server_send_response(jr);
        return;
    }

    if (jd["method"].is<const char*>() == false)
    {
        json_rpc_make_error(jr, JSON_RPC_ERROR_CODE_INVALID_REQUEST, "method", __LINE__);
        json_rpc_server_send_response(jr);
        return;
    }

    if (jd["id"].is<int>() == false && jd["id"].is<const char *>() == false)
    {
        json_rpc_make_error(jr, JSON_RPC_ERROR_CODE_INVALID_REQUEST, "id", __LINE__);
        json_rpc_server_send_response(jr);
        return;
    }
    
    if (json_rpc_server_contain_method(jd["method"].as<const char*>()) == false)
    {
        json_rpc_make_error_id(jr, JSON_RPC_ERROR_CODE_METHOD_NOT_FOUND, jd["method"].as<const char*>(), __LINE__, jd["id"].as<int>());
        json_rpc_server_send_response(jr);
        return;
    }

    json_rpc_method_t server_method = json_rpc_server_get_method(jd["method"]);
    if (server_method == NULL)
    {
        json_rpc_make_error_id(jr, JSON_RPC_ERROR_CODE_INTERNAL_ERROR, "", __LINE__, jd["id"].as<int>());
        json_rpc_server_send_response(jr);
        return;
    }

    JsonObject server_params = json_rpc_server_get_params(jd["method"]);
    //serializeJson(json_rpc_server, s);
    //Serial.print(s);    
    if (server_params.isNull())
    {
        json_rpc_make_error_id(jr, JSON_RPC_ERROR_CODE_INTERNAL_ERROR, "", __LINE__, jd["id"].as<int>());
        json_rpc_server_send_response(jr);
        return;
    }
    
    if (jd["params"].is<JsonObject>())
    {
        JsonObject joParams = jd["params"].as<JsonObject>();
        
        // check params
        JsonVariant client_value[3];
        int vi = 0;
        for (JsonPair kv : server_params)
        {
            const char *server_param_key = kv.key().c_str();
            json_rpc_param_t server_param_type = (json_rpc_param_t)kv.value().as<int>();
            Serial.printf("param %s type %d\n", server_param_key, server_param_type);
            
            if (json_rpc_variant_is(joParams[server_param_key], server_param_type))
            {
                client_value[vi] = joParams[server_param_key];
                //Serial.printf("v[%d] %d\n", vi, client_value[vi].as<int>());
            }
            vi++;
        }
        
        jr["jsonrpc"] = "2.0";
        jr["result"] = ""; // create a key
        server_method(jr["result"], client_value[0], client_value[1], client_value[2]);
        jr["id"] = jd["id"].as<int>();
        json_rpc_server_send_response(jr);
        return;
    }
    
    if (jd["params"].is<JsonArray>())
    {
        JsonArray jaParams = jd["params"].as<JsonArray>();

        // check params
        JsonVariant client_value[3];
        int vi = 0;
        for (JsonPair kv : server_params)
        {
            Serial.printf("param %s type %d\n", kv.key().c_str(), kv.value().as<int>());
            json_rpc_param_t server_param_type = (json_rpc_param_t)kv.value().as<int>();
            
            if (json_rpc_variant_is(jaParams[vi], server_param_type))
            {
                client_value[vi] = jaParams[vi];
                //Serial.printf("v[%d] %d\n", vi, client_value[vi].as<int>());
            }
            vi++;
        }

        jr["jsonrpc"] = "2.0";
        jr["result"] = ""; // create a key
        server_method(jr["result"], client_value[0], client_value[1], client_value[2]);
        jr["id"] = jd["id"].as<int>();
        json_rpc_server_send_response(jr);
        return;
    }    
    
    json_rpc_make_error_id(jr, JSON_RPC_ERROR_CODE_INTERNAL_ERROR, "", __LINE__, jd["id"].as<int>());
    json_rpc_server_send_response(jr);
}

void json_rpc_method_subtract2(JsonVariant r, JsonVariant p0, JsonVariant p1, JsonVariant p2)
{
    Serial.printf("json_rpc_method_subtract2\n");
    // Serial.printf("p0 %d\n", p0.as<int>());
    // Serial.printf("p1 %d\n", p1.as<int>());
    // Serial.printf("p2 %d\n", p2.as<int>());
    r.set<int>(p0.as<int>() - p1.as<int>());
    // Serial.printf("r %d\n", r.as<int>());
}


void json_rpc_method_choice(JsonVariant r, JsonVariant p0, JsonVariant p1, JsonVariant p2)
{
    Serial.printf("json_rpc_method_choice\n");
    
    Serial.printf("p0 %s\n", p0.as<bool>() ? "true" : "false");
    Serial.printf("p1 %s\n", p1.as<const char *>());
    Serial.printf("p2 %s\n", p2.as<const char *>());
    if (p0.as<bool>())
        r.set<const char *>(p1.as<const char *>());
    else
        r.set<const char *>(p2.as<const char *>());
    Serial.printf("r %f\n", r.as<double>());
    
}

void json_rpc_method_add2(JsonVariant r, JsonVariant p0, JsonVariant p1, JsonVariant p2)
{
    Serial.printf("json_rpc_method_add2\n");
    Serial.printf("p0 %f\n", p0.as<double>());
    Serial.printf("p1 %f\n", p1.as<double>());
    Serial.printf("p2 %f\n", p2.as<double>());
    r.set<double>(p0.as<double>() + p2.as<double>());
    Serial.printf("r %f\n", r.as<double>());
}

void json_rpc_method_get_obj_name(JsonVariant r, JsonVariant p0, JsonVariant p1, JsonVariant p2)
{
    Serial.printf("json_rpc_method_get_obj_name\n");
    if (p0.is<JsonObject>() == false)
        return;    

    JsonObject jo = p0.as<JsonObject>();
    Serial.printf("p0[\"name\"] %s\n", jo["name"].as<const char*>());
    Serial.printf("p1 %f\n", p1.as<double>());
    Serial.printf("p2 %f\n", p2.as<double>());
    r.set<const char*>(jo["name"].as<const char*>());
    Serial.printf("r %f\n", r.as<const char *>());
}

void json_rpc_method_sum_array(JsonVariant r, JsonVariant p0, JsonVariant p1, JsonVariant p2)
{
    Serial.printf("json_rpc_method_sum_array\n");
    json_rpc_variant_is(p0, JSON_RPC_PARAM_ARRAY);

    if (p0.is<JsonArray>() == false)
    {
        Serial.printf("p0 is not array\n");
        r.set<int>(0);
        return;    
    }
    int sum = 0;
    JsonArray ja = p0.as<JsonArray>();
    for(int i = 0; i < ja.size(); i++)
    {
        if (ja[i].is<int>() == false)
        {
            Serial.printf("ja[%d] is not int\n", i);
            r.set<int>(0);
            return;    
        }
        sum += ja[i].as<int>();
    }
    
    r.set<int>(sum);
    Serial.printf("r %d\n", r.as<int>());    
}

void json_rpc_method_sum_array_object(JsonVariant r, JsonVariant p0, JsonVariant p1, JsonVariant p2)
{
    Serial.printf("json_rpc_method_sum_array_object\n");
    json_rpc_variant_is(p0, JSON_RPC_PARAM_ARRAY);

    if (p0.is<JsonArray>() == false)
    {
        Serial.printf("p0 is not array\n");
        r.set<int>(0);
        return;    
    }
    int sum = 0;
    JsonArray ja = p0.as<JsonArray>();
    for(int i = 0; i < ja.size(); i++)
    {
        if (ja[i].is<JsonObject>() == false)
        {
            Serial.printf("ja[%d] is not int\n", i);
            r.set<int>(0);
            return;    
        }
        JsonObject jo = ja[i].as<JsonObject>();
        if (jo["age"].is<int>() == false)
        {
            Serial.printf("jo[\"age\"] is not int\n", i);
            r.set<int>(0);
            return;    
        }
        sum += jo["age"].as<int>();
    }
    
    r.set<int>(sum);
    Serial.printf("r %d\n", r.as<int>());    
}

void json_rpc_method_sum_all_object(JsonVariant r, JsonVariant p0, JsonVariant p1, JsonVariant p2)
{
    Serial.printf("json_rpc_method_sum_all_object\n");
    if (p0.is<JsonObject>() == false)
    {
        Serial.printf("p0 is not object\n");
        r.set<int>(0);
        return;    
    }
    if (p1.is<JsonObject>() == false)
    {
        Serial.printf("p1 is not object\n");
        r.set<int>(0);
        return;    
    }
    if (p2.is<JsonObject>() == false)
    {
        Serial.printf("p2 is not object\n");
        r.set<int>(0);
        return;    
    }
    int ip0, ip1, ip2;
    JsonObject jo;
    jo = p0.as<JsonObject>();
    if (jo["age"].is<int>() == false)
        {
        Serial.printf("p2 is not int\n");
        r.set<int>(0);
        return;    
    }
    ip0 = jo["age"].as<int>();

    jo = p1.as<JsonObject>();
    if (jo["age"].is<int>() == false)
        {
        Serial.printf("p2 is not int\n");
        r.set<int>(0);
        return;    
    }
    ip1 = jo["age"].as<int>();
    
    jo = p2.as<JsonObject>();
    if (jo["age"].is<int>() == false)
        {
        Serial.printf("p2 is not int\n");
        r.set<int>(0);
        return;    
    }
    ip2 = jo["age"].as<int>();

    r.set<int>(ip0 + ip1 + ip2);
    Serial.printf("r %f\n", r.as<int>());
}

void setup(void)
{
    printf("arduino-json-rpc-server-example\n");
    Serial.begin(115200);
    Serial1.begin(115200, SERIAL_8N1, 18, 17);

    json_rpc_server_add_method("subtract", json_rpc_method_subtract2, "minuend", JSON_RPC_PARAM_INT, "subtrahend", JSON_RPC_PARAM_INT, "", JSON_RPC_PARAM_NONE);
    json_rpc_server_add_method("choice", json_rpc_method_choice, "select", JSON_RPC_PARAM_BOOL, "text1", JSON_RPC_PARAM_STRING, "text2", JSON_RPC_PARAM_STRING);
    json_rpc_server_add_method("add", json_rpc_method_add2, "a", JSON_RPC_PARAM_FLOAT, "", JSON_RPC_PARAM_NONE, "c", JSON_RPC_PARAM_FLOAT);
    json_rpc_server_add_method("get_obj_name", json_rpc_method_get_obj_name, "obj", JSON_RPC_PARAM_OBJECT, "", JSON_RPC_PARAM_NONE, "", JSON_RPC_PARAM_NONE);
    json_rpc_server_add_method("sum_array", json_rpc_method_sum_array, "obj", JSON_RPC_PARAM_ARRAY, "", JSON_RPC_PARAM_NONE, "", JSON_RPC_PARAM_NONE);
    json_rpc_server_add_method("sum_array_object", json_rpc_method_sum_array_object, "obj", JSON_RPC_PARAM_ARRAY, "", JSON_RPC_PARAM_NONE, "", JSON_RPC_PARAM_NONE);
    json_rpc_server_add_method("sum_all_object", json_rpc_method_sum_all_object, "a", JSON_RPC_PARAM_OBJECT, "b", JSON_RPC_PARAM_OBJECT, "c", JSON_RPC_PARAM_OBJECT);
}

void loop(void)
{

    String s;
    /*
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
    */
    
    // CUSTOM TEST CASE

    /*
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
    */

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
    delay(1000);
}
