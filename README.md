# mqtt-client

MQTT 3.1.1 client

> QoS 0,1,2 support

```
[william@m409da mqtt-client]$ ./mqtt
[enc] type=0x10 (CONNECT)
[enc] type=0x80 (SUBSCRIBE)
[enc] type=0x30 (PUBLISH)
[Tx] pop'd message (0/64)
[Tx] pop'd message (0/64)
[Tx] pop'd message (0/64)
[Rx] pushed message (1/64)
[Rx] pop'd message (1/64)
[dec] type=0x20 (CONNACK)
[Rx] pushed message (1/64)
[Rx] pushed message (2/64)
[Rx] pop'd message (2/64)
[dec] type=0x90 (SUBACK)
flags: 0x00
rem: 5
packet_id=1
QoS=0x00
QoS=0x00
QoS=0x00
[Rx] pushed message (2/64)
[Rx] pushed message (3/64)
[Rx] pop'd message (3/64)
[dec] type=0x31 (PUBLISH)
flags=1 remain=23 topic: test
payload (size=17): A further message
[Rx] pop'd message (2/64)
[dec] type=0x30 (PUBLISH)
flags=0 remain=25 topic: test/topic
payload (size=13): hello mqtt!!!
[Rx] pop'd message (1/64)
[dec] type=0x50 (PUBREC)
qos pkt id=1337
update_qos>>old=0
update_qos>>new=3
got pubrec, should send pubrel for id=1337
[Tx] pushed message (1/64)
[Tx] pop'd message (0/64)
[Rx] pushed message (1/64)
[Rx] pop'd message (1/64)
[dec] type=0x70 (PUBCOMP)
qos pkt id=1337
update_qos>>old=3
update_qos>>new=5
pubcomp for id=1337 received
[Rx] pushed message (1/64)
[Rx] pop'd message (1/64)
[dec] type=0x30 (PUBLISH)
flags=0 remain=25 topic: test/topic
payload (size=13): aaaaaaaaaaaaa
[Rx] pushed message (1/64)
[Rx] pop'd message (1/64)
[dec] type=0x30 (PUBLISH)
flags=0 remain=254 topic: test/topic
payload (size=242): testsfsdfs testsfsdfs testsfsdfs testsfsdfs testsfsdfs testsfsdfs testsfsdfs testsfsdfs testsfsdfs testsfsdfs testsfsdfs testsfsdfs testsfsdfs testsfsdfs testsfsdfs testsfsdfs testsfsdfs testsfsdfs testsfsdfs testsfsdfs testsfsdfs testsfsdfs 
[Rx] pushed message (1/64)
[Rx] pop'd message (1/64)
[dec] type=0x30 (PUBLISH)
flags=0 remain=399 topic: test/topic123
payload (size=384): asdasfsagdadhashasdasfsagdadhashasdasfsagdadhashasdasfsagdadhashasdasfsagdadhashasdasfsagdadhashasdasfsagdadhashasdasfsagdadhashasdasfsagdadhashasdasfsagdadhashasdasfsagdadhashasdasfsagdadhashasdasfsagdadhashasdasfsagdadhashasdasfsagdadhashasdasfsagdadhashasdasfsagdadhashasdasfsagdadhashasdasfsagdadhashasdasfsagdadhashasdasfsagdadhashasdasfsagdadhashasdasfsagdadhashasdasfsagdadhash
PING
[Tx] pop'd message (0/64)
[Rx] pushed message (1/64)
[Rx] pop'd message (1/64)
[dec] type=0xD0 (PINGRESP)
[Rx] pushed message (1/64)
[Rx] pop'd message (1/64)
[dec] type=0x30 (PUBLISH)
flags=0 remain=886 topic: test
payload (size=880): zxczxczxczxczxczxczxczxczxczxczxczxc
zxczxczxczxczxczxczxczxczxczxczxczxc
zxczxczxczxczxczxczxczxczxczxczxczxc
zxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxc
zxczxczxczxczxczxczxczxczxczxczxczxc
zxczxczxczxczxczxczxczxczxczxczxczxc
zxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxc
zxczxczxczxczxczxczxczxczxczxczxczxc
zxczxczxczxczxczxczxczxczxczxczxczxc
zxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxczxc
zxczxczxczxczxczxczxczxczxczxczxczxc
zxczxczxczxczxczxczxczxczxczxczxczxc
zxczxczxczxczxczxczxczxczxczxczxczxzxczxczxczxczxczxczxczxczxczxczxczxc
zxczxczxczxczxczxczxczxczxczxczxczxc
zxczxczxczxczxczxczxczxczxczxczxczxc
zxczxczxczxczxczxczxczxczxczxczxczxzxczxczxczxczxczxczxczxczxczxczxczxc
zxczxczxczxczxczxczxczxczxczxczxczxc
zxczxczxczxczxczxczxczxczxczxczxczxc
zxczxczxczxczxczxczxczxczxczxczxczxc
[Rx] pushed message (1/64)
[Rx] pop'd message (1/64)
[dec] type=0x30 (PUBLISH)
flags=0 remain=11 topic: test
payload (size=5): hello
[Rx] pushed message (1/64)
[Rx] pop'd message (1/64)
[dec] type=0x30 (PUBLISH)
flags=0 remain=11 topic: test
payload (size=5): hello
[Rx] pushed message (1/64)
[Rx] pop'd message (1/64)
[dec] type=0x30 (PUBLISH)
flags=0 remain=11 topic: test
payload (size=5): hello
[Rx] pushed message (1/64)
[Rx] pop'd message (1/64)
[dec] type=0x30 (PUBLISH)
flags=0 remain=11 topic: test
payload (size=5): hello
[Rx] pushed message (1/64)
[Rx] pop'd message (1/64)
[dec] type=0x30 (PUBLISH)
flags=0 remain=11 topic: test
payload (size=5): hello
^CExiting..
Bye
[william@m409da mqtt-client]$ 
```