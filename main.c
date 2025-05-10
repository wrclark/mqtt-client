#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "mqtt.h"
#include "mqtt_net.h"
#include "packet.h"
#include "util.h"
#include "queue.h"

#define SUB_SIZE 512
#define NICK_SIZE 128

char *msg = "hello mqtt!!!";

uint8_t pktbuf[MAX_PACKET_SIZE];
uint8_t subs[SUB_SIZE];    /* sub topics + qos */
uint8_t connect_pl[1024]; /* connect payload */

queue_t tx_queue; /* outgoing pkts */
queue_t rx_queue; /* incoming pkts */

void *net_recv_loop(void *arg);

int main(void) {
    mqtt_conf_t conf = {0};
    mqtt_packet_t pkt = {0};
    mqtt_connect_opt_t conopt = {0};
    mqtt_subscribe_opt_t subopt = {0};
    mqtt_publish_opt_t pubopt = {0};
    pthread_t net_thread; /* does all network IO */

    queue_init(&tx_queue);
    queue_init(&rx_queue);

    /* general config */
    conf.broker = "test.mosquitto.org";
    conf.port = 1883;
    conf.buf = pktbuf;
    conf.size = MAX_PACKET_SIZE;

    /* CONNECT config */
    conopt.flags = MQTT_CONNECT_FLAG_CLEAN | MQTT_CONNECT_FLAG_WILL;
    conopt.keepalive = 60;
    conopt.payload = connect_pl;
    conopt.size = 1024;

    /* SUBSCRIBE config */
    subopt.buf = subs;
    subopt.capacity = SUB_SIZE;


    /* generate payload for CONNECT pkt */
    util_connect_payload(&conopt, "xXxmqttuser1337xXx", "test/topic",
                        "farewell cruel world", NULL, NULL);
    if (mqtt_connect(&conf, &conopt, &pkt, &tx_queue) != 0) {
        fprintf(stderr, "mqtt_connect()\n");
        exit(1);
    }


    /* generate payload for SUBSCRIBE pkt */
    util_subscribe_topics(&subopt,
                          "test", 0,
                          "test/topic", 0,
                          "test/topic123", 0,
                          "#", 0, /* gets all msgs on broker */
                          NULL);
    if (mqtt_subscribe(&conf, &subopt, &pkt, &tx_queue) != 0) {
        fprintf(stderr, "mqtt_subscribe()\n");
        exit(1);
    }


    /* PUBLISH config */
    pubopt.flags = MQTT_PUBLISH_FLAG_QOS_0;
    pubopt.topic = "test/topic";
    pubopt.data = msg;
    pubopt.size = strlen(msg);

    if (mqtt_publish(&conf, &pubopt, &pkt, &tx_queue) != 0) {
        fprintf(stderr, "mqtt_publish()\n");
        exit(1);
    }

    pthread_create(&net_thread, NULL, net_recv_loop, (void *)&conf);


    pthread_join(net_thread, NULL);
    mqtt_net_close(conf.fd);

    return 0;
}

/* receives pkts from broker and puts them inside the rx queue */
/* if any pkts are in the tx queue, it will transmit them */
void *net_recv_loop(void *arg) {
    ssize_t ret;
    mqtt_conf_t *conf = arg;
    mqtt_packet_t pkt;
    pkt_xfer *xfer;
    /* create socket */
    if (mqtt_init(conf) != 0) {
        fprintf(stderr, "mqtt_init()\n");
        return NULL;
    }

    printf("tx_queue size=%d\n", tx_queue.count);
    while (1) {
        ret = mqtt_net_recv_pkt(conf->fd, pktbuf, MAX_PACKET_SIZE);
        if (ret < 0) {
            puts("error: mqtt_net_recv() <= 0");
            break;
        }

        /* nothing to read, check TX queue for pkts to send out */
        else if (ret == 0) {
            if (!queue_empty(&tx_queue)) {
                xfer = (pkt_xfer *)queue_pop(&tx_queue);
                mqtt_net_send(conf->fd, xfer->pkt, xfer->size);
                free(xfer->pkt);
                free(xfer);
                printf("pkt sent!");
            }
            continue;
        } 

        packet_decode(&pkt, (size_t)ret, pktbuf, MAX_PACKET_SIZE);
    }

    return NULL;
}