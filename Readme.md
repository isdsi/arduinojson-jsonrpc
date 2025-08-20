# Overview
This implementation implements json-prc 2.0 using ArduinoJson.
Up to three parameters are supported. Parameter type checking is performed only once, 
and nested JsonObjects or JsonArrays must be handled manually.

# License
MIT License

# Modules Used
ArduinoJson 7 (https://arduinojson.org/)

# Reference
https://www.jsonrpc.org/specification

# Note: 
Handling of various exceptions has not been considered. 
Only the white cases outlined in the specification have been implemented, 
and we are not responsible for any issues arising from the use of this code.

# Build


# Test
Tested on the esp32-s3-devkitc-1 board. Pins 17 and 18 must be shorted during execution.
See the log.
```
ESP-ROM:esp32s3-20210327
Build:Mar 27 2021
rst:0x1 (POWERON),boot:0x8 (SPI_FAST_FLASH_BOOT)
SPIWP:0xee
mode:DIO, clock div:1
load:0x3fce3808,len:0x44c
load:0x403c9700,len:0xbd8
load:0x403cc700,len:0x2a80
entry 0x403c98d0
arduino-json-rpc-server-example
--> {"jsonrpc":"2.0","method":"subtract","params":[42,23],"id":1}
<-- {"jsonrpc":"2.0","result":19,"id":1}
ok
--> {"jsonrpc":"2.0","method":"subtract","params":[23,42],"id":1}
<-- {"jsonrpc":"2.0","result":-19,"id":1}
ok
--> {"jsonrpc":"2.0","method":"subtract","params":{"subtrahend":23,"minuend":42},"id":1}
<-- {"jsonrpc":"2.0","result":19,"id":1}
ok
--> {"jsonrpc":"2.0","method":"subtract","params":{"minuend":42,"subtrahend":23},"id":1}
<-- {"jsonrpc":"2.0","result":19,"id":1}
ok
--> {"jsonrpc":"2.0","method":"foobar","id":"1"}
<-- {"jsonrpc":"2.0","error":{"code":-32601,"message":"Method not found"},"id":"1"}
ok
--> {"jsonrpc": "2.0", "method": "foobar, "params": "bar", "baz]
<-- {"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null}
ok
--> {"jsonrpc":"2.0","method":1,"params":"bar"}
<-- {"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null}
ok
--> [{"jsonrpc": "2.0", "method": "sum", "params": [1,2,4], "id": "1"},{"jsonrpc": "2.0", "method"]
<-- {"jsonrpc":"2.0","error":{"code":-32700,"message":"Parse error"},"id":null}
ok
--> [1,2,3]
<-- [{"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null},{"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null},{"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null}]
ok
--> []
<-- {"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null}
ok
--> {"jsonrpc":"2.0","method":"sum","params":[1,2,4],"id":"1"}
<-- {"jsonrpc":"2.0","result":7,"id":"1"}
ok
--> {"jsonrpc":"2.0","method":"notify_hello","params":[7]}
<-- {"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null}
ok
--> {"jsonrpc":"2.0","method":"subtract","params":[42,23],"id":"2"}
<-- {"jsonrpc":"2.0","result":19,"id":"2"}
ok
--> {"foo":"boo"}
<-- {"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null}
ok
--> {"jsonrpc":"2.0","method":"foo.get","params":{"name":"myself"},"id":"5"}
<-- {"jsonrpc":"2.0","error":{"code":-32601,"message":"Method not found"},"id":"5"}
ok
--> {"jsonrpc":"2.0","method":"get_data","id":"9"}
<-- {"jsonrpc":"2.0","result":["hello",5],"id":"9"}
ok
--> [{"jsonrpc":"2.0","method":"sum","params":[1,2,4],"id":"1"},{"jsonrpc":"2.0","method":"notify_hello","params":[7]},{"jsonrpc":"2.0","method":"subtract","params":[42,23],"id":"2"},{"foo":"boo"},{"jsonrpc":"2.0","method":"foo.get","params":{"name":"myself"},"id":"5"},{"jsonrpc":"2.0","method":"get_data","id":"9"}]
<-- [{"jsonrpc":"2.0","result":7,"id":"1"},{"jsonrpc":"2.0","result":19,"id":"2"},{"jsonrpc":"2.0","error":{"code":-32600,"message":"Invalid Request"},"id":null},{"jsonrpc":"2.0","error":{"code":-32601,"message":"Method not found"},"id":"5"},{"jsonrpc":"2.0","result":["hello",5],"id":"9"}]
ok
--> [{"jsonrpc":"2.0","method":"notify_sum","params":[1,2,4]},{"jsonrpc":"2.0","method":"notify_hello","params":[7]}]
ok
--> {"jsonrpc":"2.0","method":"choice","params":{"select":true,"text1":"seoul","text2":"tokyo"},"id":1}
<-- {"jsonrpc":"2.0","result":"seoul","id":1}
ok
--> {"jsonrpc":"2.0","method":"choice","params":{"select":false,"text1":"seoul","text2":"tokyo"},"id":1}
<-- {"jsonrpc":"2.0","result":"tokyo","id":1}
ok
--> {"jsonrpc":"2.0","method":"add","params":[23.1, 0, 42.555],"id":1}
<-- {"jsonrpc":"2.0","result":65.65500069,"id":1}
ok
--> {"jsonrpc":"2.0","method":"add","params":{"a":23.1,"c":42.555},"id":111}
<-- {"jsonrpc":"2.0","result":65.65500069,"id":111}
ok
--> {"jsonrpc":"2.0","method":"add","params":[23.1, 0, 42.555],"id":111}
<-- {"jsonrpc":"2.0","result":65.65500069,"id":111}
ok
--> {"jsonrpc":"2.0","method":"get_obj_name","params": { "obj" : {"name":"myself"} },"id":1}
<-- {"jsonrpc":"2.0","result":"myself","id":1}
ok
--> {"jsonrpc":"2.0","method":"sum_array","params": { "obj" : [ 1,2,3,4,5 ]},"id":1}
<-- {"jsonrpc":"2.0","result":15,"id":1}
ok
--> {"jsonrpc":"2.0","method":"sum_array_object","params": { "obj" : [ {"age": 1 }, {"age": 2 }, {"age": 3 } ]},"id":1}
<-- {"jsonrpc":"2.0","result":6,"id":1}
ok
--> {"jsonrpc":"2.0","method":"sum_all_object","params": [ {"age": 1 }, {"age": 2 }, {"age": 3 } ],"id":1}
<-- {"jsonrpc":"2.0","result":6,"id":1}
ok
--> {"jsonrpc":"2.0","method":"say_hello","params": null,"id":1}
<-- {"jsonrpc":"2.0","result":"hello","id":1}
ok
--> {"jsonrpc":"2.0","method":"say_hello","params": [],"id":1}
<-- {"jsonrpc":"2.0","result":"hello","id":1}
ok
```
