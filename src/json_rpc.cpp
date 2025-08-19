#include "json_rpc.h"

////////////////////////////////////////////////////////////////////////////////
// 매크로
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// 구조체
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// 상수
////////////////////////////////////////////////////////////////////////////////
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


////////////////////////////////////////////////////////////////////////////////
// 외부 상수
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// 변수
////////////////////////////////////////////////////////////////////////////////

JsonVariant jvNull;
DeserializationError error;

// stores rpc method, parameters
JsonDocument json_rpc_server;

////////////////////////////////////////////////////////////////////////////////
// 외부 변수
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// 함수
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// 외부 함수
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// 함수 정의
////////////////////////////////////////////////////////////////////////////////

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

void json_rpc_make_error(JsonObject jor, int error_code, const char *key, int line)
{
    jor["jsonrpc"] = "2.0";
    jor["error"]["code"] = error_code;
    jor["error"]["message"] = json_rpc_get_error_message(error_code);
    log_i("key %s line %d", key, line);
    jor["id"] = jvNull;
}

void json_rpc_make_error_id(JsonObject jor, int error_code, const char *key, int line, JsonVariant jvId)
{
    jor["jsonrpc"] = "2.0";
    jor["error"]["code"] = error_code;
    jor["error"]["message"] = json_rpc_get_error_message(error_code);
    log_i("key %s line %d", key, line);
    jor["id"] = jvId;
}

bool json_rpc_check_param_exist(const char *n)
{
    if (n == NULL || strlen(n) == 0)
        return false;
    return true;
}

void json_rpc_server_add_method(const char* method, json_rpc_method_t m, 
    const char *p0Name, json_rpc_param_t p0Type, 
    const char *p1Name, json_rpc_param_t p1Type, 
    const char *p2Name, json_rpc_param_t p2Type)
{
    json_rpc_server[method]["func"] = (unsigned int)m;
    
    if (json_rpc_check_param_exist(p0Name) == false &&
        json_rpc_check_param_exist(p1Name) == false &&
        json_rpc_check_param_exist(p2Name) == false)
    {
        json_rpc_server[method]["params"] = jvNull; // no param like 'void'
        return;
    }
    
    if (json_rpc_check_param_exist(p0Name))
        json_rpc_server[method]["params"][p0Name] = p0Type;
    else
        json_rpc_server[method]["params"][p0Name] = jvNull;
    if (json_rpc_check_param_exist(p1Name))
        json_rpc_server[method]["params"][p1Name] = p1Type;
    else
        json_rpc_server[method]["params"][p1Name] = jvNull;
    if (json_rpc_check_param_exist(p2Name))
        json_rpc_server[method]["params"][p2Name] = p2Type;
    else
        json_rpc_server[method]["params"][p2Name] = jvNull;
    
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

// USER CHANGE
void json_rpc_server_send_response(JsonDocument &jd)
{
    String s;
    serializeJson(jd, s);
    Serial1.print(s);    
    Serial.print("<-- ");
    Serial.println(s);
}

// USER CHANGE
bool json_rpc_server_receive_request(JsonDocument &jd)
{
    String s = Serial1.readString();
    Serial.print("--> ");
    Serial.println(s);
    DeserializationError error = deserializeJson(jd, s.c_str());
    if (error != DeserializationError::Ok)
    {
        return false;
    }
    return true;
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
            log_i("v %s", v.as<bool>() ? "true" : "false");
    }
    else if (p == JSON_RPC_PARAM_INT)
    {
        if (v.is<int>() == false)
            return false;
        else
            log_i("v %d", v.as<int>());
    }
    else if (p == JSON_RPC_PARAM_FLOAT)
    {
        if (v.is<double>() == false)
            return false;
        else
            log_i("v %f", v.as<double>());
    }
    else if (p == JSON_RPC_PARAM_STRING)
    {
        if (v.is<const char *>() == false)
            return false;
        else
            log_i("v %s", v.as<const char *>());
    }
    else if (p == JSON_RPC_PARAM_ARRAY)
    {
        if (v.is<JsonArray>() == false)
            return false;
        else
            log_i("v array count %d", v.as<JsonArray>().size());
    }
    else if (p == JSON_RPC_PARAM_OBJECT)
    {
        if (v.is<JsonObject>() == false)
            return false;
        else
            log_i("v object count %d", v.as<JsonObject>().size());
    }

    return true;
}

void json_rpc_process_object(JsonObject jo, JsonObject jor, bool batch)
{
    if (jo["jsonrpc"].is<const char*>() == false)
    {
        json_rpc_make_error(jor, JSON_RPC_ERROR_CODE_INVALID_REQUEST, "jsonrpc", (int)__LINE__);
        return;
    }
    
    if (strcmp(jo["jsonrpc"].as<const char*>(), "2.0") != 0)
    {
        json_rpc_make_error(jor, JSON_RPC_ERROR_CODE_INVALID_REQUEST, "jsonrpc", (int)__LINE__);
        return;
    }

    if (jo["method"].is<const char*>() == false)
    {
        json_rpc_make_error(jor, JSON_RPC_ERROR_CODE_INVALID_REQUEST, "method", __LINE__);
        return;
    }

    if (jo["id"].is<int>() == false && jo["id"].is<const char *>() == false)
    {
        if (batch == false)
        {
            json_rpc_make_error(jor, JSON_RPC_ERROR_CODE_INVALID_REQUEST, "id", __LINE__);
            return;
        }
        else
        {
            // notify hello인데 배치모드이면 무시된다.
            jor["notify"] = true;
            return;
        }
    }

    if (json_rpc_server_contain_method(jo["method"].as<const char*>()) == false)
    {
        json_rpc_make_error_id(jor, JSON_RPC_ERROR_CODE_METHOD_NOT_FOUND, jo["method"].as<const char*>(), __LINE__, jo["id"]);
        return;
    }

    json_rpc_method_t server_method = json_rpc_server_get_method(jo["method"]);
    if (server_method == NULL)
    {
        json_rpc_make_error_id(jor, JSON_RPC_ERROR_CODE_INTERNAL_ERROR, "", __LINE__, jo["id"]);
        return;
    }

    JsonObject server_params = json_rpc_server_get_params(jo["method"]); // 없다면 null, 있다면 세개의 파라미터를 보장한다.
    String s;
    serializeJson(json_rpc_server, s);
    log_i("%s", s.c_str());    
    /*
    // -- no param like 'void'
    if (server_params.isNull())
    {
        json_rpc_make_error_id(jor, JSON_RPC_ERROR_CODE_INTERNAL_ERROR, "", __LINE__, jo["id"]);
        return;
    }
    */
    
    if (jo["params"].is<JsonObject>())
    {
        JsonObject joParams = jo["params"].as<JsonObject>();
        
        // check params
        JsonVariant client_value[3];
        int vi = 0;
        for (JsonPair kv : server_params)
        {
            const char *server_param_key = kv.key().c_str();
            json_rpc_param_t server_param_type = (json_rpc_param_t)kv.value().as<int>();
            log_i("param %s type %d", server_param_key, server_param_type);
            
            if (json_rpc_variant_is(joParams[server_param_key], server_param_type))
            {
                client_value[vi] = joParams[server_param_key];
            }
            vi++;
        }
        
        jor["jsonrpc"] = "2.0";
        jor["result"] = ""; // create a key
        server_method(jor["result"], client_value[0], client_value[1], client_value[2]);
        jor["id"] = jo["id"];
        return;
    }
    
    if (jo["params"].is<JsonArray>())
    {
        JsonArray jaParams = jo["params"].as<JsonArray>();

        // check params
        JsonVariant client_value[3];
        int vi = 0;
        for (JsonPair kv : server_params)
        {
            log_i("param %s type %d", kv.key().c_str(), kv.value().as<int>());
            json_rpc_param_t server_param_type = (json_rpc_param_t)kv.value().as<int>();
            
            if (json_rpc_variant_is(jaParams[vi], server_param_type))
            {
                client_value[vi] = jaParams[vi];
            }
            vi++;
        }

        jor["jsonrpc"] = "2.0";
        jor["result"] = ""; // create a key
        server_method(jor["result"], client_value[0], client_value[1], client_value[2]);
        jor["id"] = jo["id"];
        return;
    }    

    if (jo["params"].isNull()) // no param like 'void'
    {
        JsonVariant client_value[3];

        if (server_params.isNull() == false)
        {
            json_rpc_make_error_id(jor, JSON_RPC_ERROR_CODE_INVALID_PARAMS, "", __LINE__, jo["id"]);
            return;
        }

        jor["jsonrpc"] = "2.0";
        jor["result"] = ""; // create a key
        server_method(jor["result"], client_value[0], client_value[1], client_value[2]);
        jor["id"] = jo["id"];
        return;
    }
    
    json_rpc_make_error_id(jor, JSON_RPC_ERROR_CODE_INTERNAL_ERROR, "", __LINE__, jo["id"]);
}

void json_rpc_loop(void)
{
    JsonDocument jd;
    JsonDocument jr;
    JsonArray ja;
    JsonArray jar;
    JsonObject jo;
    JsonObject jor;
    String s;

    /*
    // Create an object and serialize it
    JsonDocument doc;

    // create an object
    JsonObject object = doc.to<JsonObject>();
    object["hello"] = "world";
    */

    /*
    // deserialize the object
    JsonDocument doc;
    deserializeJson(doc, "{\"hello\":\"world\"}");

    // extract the data
    JsonObject object = doc.as<JsonObject>();
    const char* world = object["hello"];
    */

    if (json_rpc_server_receive_request(jd) == false)
    {
        jor = jr.to<JsonObject>();
        json_rpc_make_error(jor, JSON_RPC_ERROR_CODE_PARSE_ERROR, "jsonrpc", (int)__LINE__);
        json_rpc_server_send_response(jr);
        return;
    }


    if (jd.is<JsonObject>())
    {
        jo = jd.as<JsonObject>();
        jor = jr.to<JsonObject>();
        json_rpc_process_object(jo, jor, false);
        json_rpc_server_send_response(jr);
        return;
    }

    if (jd.is<JsonArray>())
    {
        if (jd.size() == 0)
        {
            jor = jr.to<JsonObject>();
            json_rpc_make_error(jor, JSON_RPC_ERROR_CODE_INVALID_REQUEST, "", __LINE__);
            json_rpc_server_send_response(jr);
            return;
        }

        ja = jd.as<JsonArray>();
        jar = jr.to<JsonArray>();
        for(int i = 0; i < jd.size(); i++)
        {
            jo = ja[i].as<JsonObject>();
            jor = jar.add<JsonObject>();
            json_rpc_process_object(jo, jor, true);
        }
        for(int i = jar.size() - 1; i >= 0; i--)
        {
            jor = jar[i].as<JsonObject>();
            if (jor["notify"].is<bool>()) // notify hello
            {
                jar.remove(i);
                log_i("notify removed %d", jar.size());
            }
        }
        if (jar.size() == 0) // Nothing is returned for all notification batches
            return;
        json_rpc_server_send_response(jr);
        return;
    }

    jor = jr.to<JsonObject>();
    json_rpc_make_error(jor, JSON_RPC_ERROR_CODE_INTERNAL_ERROR, "", __LINE__);
    json_rpc_server_send_response(jr);
}
