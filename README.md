# mqtt-client

MQTT 3.1.1 client


```
$ ./mqtt 
type=0x10
mqtt_net_connect OK
mqtt_net_send OK
mqtt_net_recv OK
recv size: 4
20 CONNACK
CONACK:
20 02 00 00 
??test???test/topic2???test123???test/topic???test/topic5?
00 04 74 65 73 74 00 00 
0b 74 65 73 74 2f 74 6f 
70 69 63 32 00 00 07 74 
65 73 74 31 32 33 00 00 
0a 74 65 73 74 2f 74 6f 
70 69 63 00 00 0b 74 65 
73 74 2f 74 6f 70 69 63 
35 00 
type=0x80
ret=62
SUBSCRIBE:
?<????test???test/topic2???test123???test/topic???test/topic5?
82 3c 00 01 00 04 74 65 
73 74 00 00 0b 74 65 73 
74 2f 74 6f 70 69 63 32 
00 00 07 74 65 73 74 31 
32 33 00 00 0a 74 65 73 
74 2f 74 6f 70 69 63 00 
00 0b 74 65 73 74 2f 74 
6f 70 69 63 35 00 
mqtt_net_send SUBSCRIBE OK
mqtt_net_recv SUBACK OK
recv size: 67
90 SUBACK
flags: 0x00
rem: 7
packet_id=1
0x00 - Success - Max QoS 0
0x00 - Success - Max QoS 0
0x00 - Success - Max QoS 0
0x00 - Success - Max QoS 0
0x00 - Success - Max QoS 0
SUBACK:
90 07 00 01 00 00 00 00 
00 
size=18
30 PUBLISH
flags=0 remain=16 topic: test/topic
payload: 6559
size=18
30 PUBLISH
flags=0 remain=16 topic: test/topic
payload: 6559
size=18
30 PUBLISH
flags=0 remain=16 topic: test/topic
payload: 6559
size=18
30 PUBLISH
flags=0 remain=16 topic: test/topic
payload: 6559
size=530
30 PUBLISH
flags=0 remain=527 topic: test
payload: {"command":1,"timestamp":1736014549,"node_id":15,"node_name":"ตู้ ห้อง Excellent","place_name":"Chonburi.Hospital","gps":{"lat":13.6340239,"lon":100.4924908},"user_id":99,"temp":5.37,"humid":0,"status_door":0,"status_door_alarm":0,"status_ac":0,"setting_temp_high":38,"setting_temp_on":35,"setting_temp_off":34,"setting_temp_overlow":32,"slave_cnt":1,"slaves":[{"temp_cnt":5,"temp_avr":5.37,"compressor_current":0.95,"temps":[3.24,6.61,5.93,5.7,-88.88],"status":{"slave":1,"temp":1,"compressor":1,"ac":0}}]}
size=18
30 PUBLISH
flags=0 remain=16 topic: test/topic
payload: 6559
size=18
30 PUBLISH
flags=0 remain=16 topic: test/topic
payload: 6559
size=18
30 PUBLISH
flags=0 remain=16 topic: test/topic
payload: 6559
size=18
30 PUBLISH
flags=0 remain=16 topic: test/topic
payload: 6559
size=18
30 PUBLISH
flags=0 remain=16 topic: test/topic
payload: 6559
size=18
30 PUBLISH
flags=0 remain=16 topic: test/topic
payload: 6559
size=18
30 PUBLISH
flags=0 remain=16 topic: test/topic
payload: 6559
^C
```