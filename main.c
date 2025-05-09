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

#define SUB_SIZE 512
#define NICK_SIZE 128

char *msg = "hello mqtt!!!";

uint8_t pktbuf[MAX_PACKET_SIZE];
uint8_t subs[SUB_SIZE];    /* sub topics + qos */
uint8_t connect_pl[1024]; /* connect payload */

struct pinger_opts {
    mqtt_conf_t *conf;
    uint16_t keepalive;
};

void publish_callback(mqtt_packet_t *pkt);
void *pinger(void *conf);
void *publisher(void *conf);

int main(void) {

    mqtt_conf_t conf;
    mqtt_packet_t pkt;
    mqtt_connect_opt_t conopt;
    mqtt_subscribe_opt_t subopt;
    mqtt_publish_opt_t pubopt;
    ssize_t ret;
    int fd;
    pthread_t pingthread;
    pthread_t pubthread;
    struct pinger_opts popts;

    memset(&conf, 0, sizeof(conf));
    memset(&pkt, 0, sizeof(pkt));
    memset(&conopt, 0, sizeof(conopt));
    memset(&subopt, 0, sizeof(subopt));
    memset(&pubopt, 0, sizeof(pubopt));

    /* general config */
    conf.broker = "test.mosquitto.org";
    conf.port = 1883;
    conf.buf = pktbuf;
    conf.size = MAX_PACKET_SIZE;
    conf.publish = publish_callback;

    /* CONNECT config */
    conopt.flags = MQTT_CONNECT_FLAG_CLEAN | MQTT_CONNECT_FLAG_WILL;
    conopt.keepalive = 60;
    conopt.payload = connect_pl;
    conopt.size = 1024;

    /* SUBSCRIBE config */
    subopt.buf = subs;
    subopt.capacity = SUB_SIZE;

    /* create socket */
    if (mqtt_init(&conf) != 0) {
        fprintf(stderr, "mqtt_init()\n");
        exit(1);
    }

    fd = conf.fd; /* temp */

    /* generate payload for CONNECT pkt */
    util_connect_payload(&conopt, "xXxmqttuser1337xXx", "test/topic",
                        "farewell cruel world", NULL, NULL);
    if (mqtt_connect(&conf, &conopt, &pkt) != 0) {
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
    if (mqtt_subscribe(&conf, &subopt, &pkt) != 0) {
        fprintf(stderr, "mqtt_subscribe()\n");
        exit(1);
    }


    /* PUBLISH config */
    pubopt.flags = MQTT_PUBLISH_FLAG_QOS_0;
    pubopt.topic = "test/topic";
    pubopt.data = msg;
    pubopt.size = strlen(msg);

    if (mqtt_publish(&conf, &pubopt, &pkt) != 0) {
        fprintf(stderr, "mqtt_publish()\n");
        exit(1);
    }

    popts.conf = &conf;
    popts.keepalive = conopt.keepalive;
    pthread_create(&pingthread, NULL, pinger, (void *)&popts);
    pthread_create(&pubthread, NULL, publisher, (void *)&conf);

    while (1) {
        ret = mqtt_net_recv(fd, pktbuf, MAX_PACKET_SIZE);
        if (ret <= 0) {
            puts("error: mqtt_net_recv() <= 0");
            break;
        }

        packet_decode(&pkt, (size_t)ret, pktbuf, MAX_PACKET_SIZE);
        memset(pktbuf, 0, pkt.real_size);
        memset(&pkt, 0, sizeof(pkt));
    }

    pthread_join(pingthread, NULL);
    pthread_join(pubthread, NULL);
    mqtt_net_close(fd);

    return 0;
}

void publish_callback(mqtt_packet_t *pkt) {
    printf("PUBLISH: %lu\n", pkt->real_size);
}

void *pinger(void *conf) {
    int ret;
    while(1) {
        puts("--> PING -->");
        ret = mqtt_ping(((struct pinger_opts *)conf)->conf);
        if (ret > 0) break;
        sleep(((struct pinger_opts *)conf)->keepalive);
    }
    
    return NULL;
}

void *publisher(void *conf) {
    int ret;
    mqtt_packet_t pkt = {0};
    mqtt_publish_opt_t opt = {0};
    opt.data = "hello from publisher thread";
    opt.size = strlen(opt.data);
    opt.topic = "test/topic";
    opt.flags = 0;
    while(1) {
        puts("--> PUBLISH -->");
        ret = mqtt_publish((mqtt_conf_t *)conf, &opt, &pkt);
        if (ret > 0) break;
        sleep(10);
        printf("SENT\n\n\n\n\n");
    }
    
    return NULL;
}