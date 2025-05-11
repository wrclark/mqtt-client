#define _GNU_SOURCE /* for usleep() */
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

const char *msg = "hello mqtt!!!";

uint8_t pktbuf[MAX_PACKET_SIZE]; /* for generating pkts */
uint8_t subs[SUB_SIZE];    /* sub topics + qos */
uint8_t connect_pl[1024]; /* connect payload */

queue_t tx_queue; /* outgoing pkts */
queue_t rx_queue; /* incoming pkts */

void *net_recv_loop(void *arg);
void *pinger(void *arg);

static volatile int should_run = 1;
static uint16_t keepalive = 60;

int main(void) {
    mqtt_conf_t conf = {0};
    mqtt_packet_t pkt = {0};
    mqtt_connect_opt_t conopt = {0};
    mqtt_subscribe_opt_t subopt = {0};
    mqtt_publish_opt_t pubopt = {0};
    pkt_xfer *xfer;
    pthread_t net_thread; /* does all network IO */
    pthread_t ping_thread; /* sends hello occasionally */

    queue_init(&tx_queue);
    queue_init(&rx_queue);

    /* general config */
    conf.broker = "broker.hivemq.com"; /* mosquitto boots for no reason */
    conf.port = 1883;
    conf.buf = pktbuf;
    conf.size = MAX_PACKET_SIZE;

    /* CONNECT config */
    conopt.flags = MQTT_CONNECT_FLAG_CLEAN | MQTT_CONNECT_FLAG_WILL;
    conopt.keepalive = keepalive;
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
    pthread_create(&ping_thread, NULL, pinger, (void *)&conf);

    usleep(100000); 

    while (should_run) {
        if (!queue_empty(&rx_queue)) {
            xfer = (pkt_xfer *)queue_pop(&rx_queue);
            packet_decode(&pkt, xfer->size, xfer->pkt, xfer->size);
            free(xfer->pkt);
            free(xfer);
        }
        usleep(20000);
    }

    pthread_join(net_thread, NULL);
    mqtt_net_close(conf.fd);

    return 0;
}

/* sends a ping to broker every so often */
void *pinger(void *arg) {
    mqtt_conf_t *conf = arg;

    while(should_run) {
        sleep(keepalive);
        mqtt_ping(conf, &tx_queue);
        puts("PING");
    }

    return NULL;
}

/* receives pkts from broker and puts them inside the rx queue */
/* if any pkts are in the tx queue, it will transmit them */
void *net_recv_loop(void *arg) {
    mqtt_recv_state_t rxstate;
    mqtt_conf_t *conf = arg;
    pkt_xfer *xfer;
    ssize_t ret;
    memset(&rxstate, 0, sizeof(rxstate));

    rxstate.buf = malloc(MAX_PACKET_SIZE);
    if (!rxstate.buf) {
        fprintf(stderr, "unable to malloc pkt buf (size=%lu)\n", MAX_PACKET_SIZE);
        should_run = 0;
        return NULL;
    }

    if (mqtt_init(conf) != 0) {
        fprintf(stderr, "mqtt_init()\n");
        return NULL;
    }

    while (should_run) {
        ret = mqtt_net_recv_pkt_stateful(conf->fd, &rxstate);
        if (ret < 0) {
            puts("error: mqtt_net_recv_pkt_stateful() < 0");
            should_run = 0;
            return NULL;

        } else if (ret == 0) {
            /* no full packet, check TX queue */
            if (!queue_empty(&tx_queue)) {
                xfer = (pkt_xfer *)queue_pop(&tx_queue);
                mqtt_net_send(conf->fd, xfer->pkt, xfer->size);
                free(xfer->pkt);
                free(xfer);
                printf("pkt sent!\n");
            }
            usleep(5000);
            continue;
        }

        /* full packet received */
        if (!queue_full(&rx_queue)) {
            xfer = malloc(sizeof(pkt_xfer));
            xfer->pkt = malloc((size_t) ret);
            xfer->size = (size_t) ret;
            memcpy(xfer->pkt, rxstate.buf, (size_t) ret);
            queue_push(&rx_queue, (void *)xfer);
            printf("pushed message (%d/%d)\n", rx_queue.count, QUEUE_SIZE);
        } else {
            puts("QUEUE FULL !!!");
        }

        usleep(5000);
    }

    return NULL;
}
