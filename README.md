# mqtt-client

MQTT 3.1.1 client

```
[william@m409da mqtt-client]$ ./mqtt 
[enc] type=0x10 (CONNECT)
[enc] type=0x80 (SUBSCRIBE)
[enc] type=0x30 (PUBLISH)
[tx] pop'd message (0/512)
[tx] pop'd message (0/512)
[tx] pop'd message (0/512)
[rx] pushed message (1/512)
[rx] pop'd message (1/512)
[dec] type=0x20 (CONNACK)
[rx] pushed message (1/512)
[rx] pushed message (2/512)
[rx] pop'd message (2/512)
[dec] type=0x90 (SUBACK)
flags: 0x00
rem: 5
packet_id=1
QoS=0x00
QoS=0x00
QoS=0x00
[rx] pushed message (2/512)
[rx] pushed message (3/512)
[rx] pop'd message (3/512)
[dec] type=0x31 (PUBLISH)
flags=1 remain=23 topic: test
payload (size=17): A further message
[rx] pop'd message (2/512)
[dec] type=0x30 (PUBLISH)
flags=0 remain=25 topic: test/topic
payload (size=13): hello mqtt!!!
[rx] pop'd message (1/512)
[dec] type=0x50 (PUBREC)
flags=80
rem=80
update_qos>>old=0
update_qos>>new=3
got pubrec, should send pubrel for id=1
[tx] pushed message (1/512)
[tx] pop'd message (0/512)
[rx] pushed message (1/512)
[rx] pop'd message (1/512)
[dec] type=0x70 (PUBCOMP)
flags=112
rem=112
update_qos>>old=0
update_qos>>new=5
pubcomp for id=1 received
[rx] pushed message (1/512)
[rx] pop'd message (1/512)
[dec] type=0x30 (PUBLISH)
flags=0 remain=8 topic: test
payload (size=2): 11
[rx] pushed message (1/512)
[rx] pop'd message (1/512)
[dec] type=0x30 (PUBLISH)
flags=0 remain=8 topic: test
payload (size=2): 50
[rx] pushed message (1/512)
[rx] pop'd message (1/512)
[dec] type=0x30 (PUBLISH)
flags=0 remain=8 topic: test
payload (size=2): 68
[rx] pushed message (1/512)
[rx] pop'd message (1/512)
[dec] type=0x30 (PUBLISH)
flags=0 remain=7 topic: test
payload (size=1): 1
[rx] pushed message (1/512)
[rx] pop'd message (1/512)
[dec] type=0x30 (PUBLISH)
flags=0 remain=8 topic: test
payload (size=2): 27
[rx] pushed message (1/512)
[rx] pop'd message (1/512)
[dec] type=0x30 (PUBLISH)
flags=0 remain=8 topic: test
payload (size=2): 87
[rx] pushed message (1/512)
[rx] pop'd message (1/512)
[dec] type=0x30 (PUBLISH)
flags=0 remain=8 topic: test
payload (size=2): 62
[rx] pushed message (1/512)
[rx] pop'd message (1/512)
[dec] type=0x30 (PUBLISH)
flags=0 remain=8 topic: test
payload (size=2): 82
[rx] pushed message (1/512)
[rx] pop'd message (1/512)
[dec] type=0x30 (PUBLISH)
flags=0 remain=8 topic: test
payload (size=2): 40
[rx] pushed message (1/512)
[rx] pop'd message (1/512)
[dec] type=0x30 (PUBLISH)
flags=0 remain=7 topic: test
payload (size=1): 8
^CExiting..
Bye
[william@m409da mqtt-client]$ 
```