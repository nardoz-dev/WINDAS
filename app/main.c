#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"

#include "xtimer.h"
#include "net/emcute.h"
#include "net/ipv6/addr.h"

#define _IPV6_DEFAULT_PREFIX_LEN    (64U)

#define EMCUTE_PRIO         (THREAD_PRIORITY_MAIN - 1)
#define NUMOFSUBS           (1U)
#define TOPIC_MAXLEN        (64U)
#define DEVICE_IP_ADDRESS   ("fec0:affe::99")
#define DEFAULT_INTERFACE   ("4")


#define MQTT_TOPIC          "my_topic"
#define MQTT_QoS            (EMCUTE_QOS_0)

static char stack[THREAD_STACKSIZE_DEFAULT];
static char temperature_stack[THREAD_STACKSIZE_DEFAULT];


static emcute_sub_t subscriptions[NUMOFSUBS];
//static char topics[NUMOFSUBS][TOPIC_MAXLEN];

//Function called when a message is published on the topic the board is subscribed
static void on_pub(const emcute_topic_t *topic, void *data, size_t len){
    (void)topic;
    (void)len;
    char *in = (char *)data;

    printf(" Contenuto di in : %s", in);

    char msg[len+1];
    strncpy(msg, in, len);
    msg[len] = '\0';
    printf(" Contenuto di msg : %s", msg);

    printf("### got publication for topic '%s' [%i] ###\n",
           topic->name, (int)topic->id);
    for (size_t i = 0; i < len; i++) {
        printf("%c", in[i]);
    }
    puts("");

}


static void *emcute_thread(void *arg){
    (void)arg;
    emcute_run(CONFIG_EMCUTE_DEFAULT_PORT, EMCUTE_ID);
    return NULL;    /* should never be reached */
}
// Function for sending data
static int publish(char *t, char *message){
    emcute_topic_t topic;
    topic.name = t;
    
    //getting ID from the topic
    if(emcute_reg(&topic) != EMCUTE_OK){ 
        printf("publish : cannot find topic:%s", topic.name);
        return 1;
    }

    //publishing on the broker
    if(emcute_pub(&topic, message, strlen(message), MQTT_QoS) != EMCUTE_OK){ 
        puts("publish : cannot publish data\n");
        return 1;
    }
    //printf("published message: %s (%i) on topic %s (id:%i)",
    //    message, (int)strlen(message), topic.name, topic.id);
    return 0;
}
void *send_data(void* arg){
    (void)arg;
    while(1){
        char message[9];
        sprintf(message,"36");
        publish(MQTT_TOPIC,message); 
        xtimer_sleep(20); 
    }
}
// Function Setup
static int sub(void){
    //Setup subscription
    subscriptions[0].cb = on_pub;
    subscriptions[0].topic.name = MQTT_TOPIC;

    //Subscribing to topic
    if (emcute_sub(&subscriptions[0], MQTT_QoS) != EMCUTE_OK) {
        printf("connect_broker: unable to subscribe to %s\n", subscriptions[0].topic.name);
        return 1;
    }

    printf("Now subscribed to %s\n", subscriptions[0].topic.name);
    return 0;
}
static int connect_broker(void){

    printf("Connecting to MQTT-SN broker %s port %d.\n",SERVER_ADDR, SERVER_PORT);

    // Socket creation 
    sock_udp_ep_t gw = { 
        .family = AF_INET6, 
        .port = SERVER_PORT 
    }; 

    char *topic = NULL;
    char *message = NULL;
    size_t len = 0;
    
    //Parsing IPv6 Address from String
    if (ipv6_addr_from_str((ipv6_addr_t *)&gw.addr.ipv6, SERVER_ADDR) == NULL) {
        puts("connect_broker: error parsing IPv6 address\n");
        return 1;
    }

    //Connecting to broker
    if (emcute_con(&gw, true, topic, message, len, 0) != EMCUTE_OK) {
        printf("connect_broker: unable to connect to %s:%i\n", SERVER_ADDR, (int)gw.port);
        return 1;
    }

    printf("Successfully connected to gateway at [%s]:%i\n",SERVER_ADDR, (int)gw.port);

    return 0;
}
static uint8_t get_prefix_len(char *addr) {
    int prefix_len = ipv6_addr_split_int(addr, '/', _IPV6_DEFAULT_PREFIX_LEN);

    if(prefix_len < 1)
        prefix_len = _IPV6_DEFAULT_PREFIX_LEN;

    return prefix_len;
}
static int add_netif(char *name_if, char *dev_ip_address){

    netif_t *iface = netif_get_by_name(name_if); //getting the interface where to add the address
    if(!iface){
        puts("add_netif: No valid Interface");
        return 1;
    }

    ipv6_addr_t ip_addr;
    uint16_t flag = GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID; 
    uint8_t prefix_len;
    
    prefix_len = get_prefix_len(dev_ip_address); 

    //Parsing IPv6 Address from String 
    if(ipv6_addr_from_str(&ip_addr, dev_ip_address) == NULL){
        puts("add_netif: Error in parsing ipv6 address");
        return 1;
    }

    flag |= (prefix_len << 8U);

    //Set Interface Options 
    if(netif_set_opt(iface, NETOPT_IPV6_ADDR, flag, &ip_addr, sizeof(ip_addr)) < 0){
            puts("add_netif: Error in Adding ipv6 address");
            return 1;
        }

    printf("Added %s with prefix %d to interface %s\n", dev_ip_address, prefix_len, name_if);
    return 0;

}
static int setup_mqtt(void){

    //Setting memory for the subscription list
    memset(subscriptions, 0, (NUMOFSUBS * sizeof(emcute_sub_t)));

    //Starting Emcute Thread
    thread_create(stack, sizeof(stack), EMCUTE_PRIO, 0,emcute_thread, NULL, "emcute");
    printf("Emcute Thread Started\n");

    if(add_netif(DEFAULT_INTERFACE,DEVICE_IP_ADDRESS)){
        puts("\n setup_mqtt: Faile to add network interface\n");
        return 1;
    } 

    return 0;
}

int main(void){

    xtimer_sleep(2);
    printf("RIOT mqtt test Application \n");

    printf("\n Setting up MQTT-SN . \n");
    if(setup_mqtt()){
        printf("\n E: Something failed during the setup of MQTT");
        return 1;
    }
    
    printf("\n Broker Connection . \n");
    if(connect_broker()){
        printf("\n E: Something failed during the broker connection");
        return 1;
    }

    printf("\n Starting subscription . \n");
    if(sub()){
        printf("\n E: Something went wrong while subscribing to the topic ");
        
        return 1;
    }

    /* Inizializzazione sensori - Anche prima. */
    /*-----------------------------------------*/
    /*-----------------------------------------*/
    /*-----------------------------------------*/

    //Handle thread creation
    thread_create(temperature_stack, sizeof(temperature_stack),EMCUTE_PRIO, 0,send_data, NULL, "send_data");
    printf("\n Thread for sending information created ");

    return 0;
}