#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"

#include "xtimer.h"
#include "fmt.h"
#include "dht.h"
#include "dht_params.h"
#include "periph/gpio.h"
//Networking
#include "net/emcute.h"
#include "net/ipv6/addr.h"

// Macro for networks processes.
#define _IPV6_DEFAULT_PREFIX_LEN    (64U)
#define EMCUTE_PRIO         (THREAD_PRIORITY_MAIN - 1)
#define NUMOFSUBS           (16U)
#define TOPIC_MAXLEN        (64U)
#define DEVICE_IP_ADDRESS   ("fec0:affe::99")
#define DEFAULT_INTERFACE   ("4")

#define MQTT_TOPIC          "my_topic"
#define MQTT_QoS            (EMCUTE_QOS_0)

/* GPIO pin for 7Segment Display*/
gpio_t SEGMENT_A = GPIO_PIN(PORT_B,10);   
gpio_t SEGMENT_B = GPIO_PIN(PORT_B,3);
gpio_t SEGMENT_C = GPIO_PIN(PORT_A,7);
gpio_t SEGMENT_D = GPIO_PIN(PORT_B,6);
gpio_t SEGMENT_E = GPIO_PIN(PORT_A,10);
gpio_t SEGMENT_F = GPIO_PIN(PORT_B,5);
gpio_t SEGMENT_G = GPIO_PIN(PORT_B,4);
// Initialize GPIO pin for motor 
gpio_t motor_pin = GPIO_PIN(PORT_C,7);

/* Constant for internal algorithm */
typedef enum{
    AUTO = 0,
    ACTIVE = 1,
    OFF = 2
} state_t;
state_t system_mod = AUTO;
dht_t dev;

// Array di valori per le singole cifre del display 
/*//(only 0,1 but the concept is easy, 0 to turn on led segment, 1 otherwise.)
static const uint8_t digit_values[][7] = {
    {0, 0, 0, 0, 0, 0, 1}, // 0
    {1, 0, 0, 1, 1, 1, 1}, // 1          
};
*/
static char stack[THREAD_STACKSIZE_DEFAULT];
static char temperature_stack[THREAD_STACKSIZE_DEFAULT];
static emcute_sub_t subscriptions[NUMOFSUBS];

// Emcute Thread  -------------------------------
static void *emcute_thread(void *arg){
    (void)arg;
    emcute_run(CONFIG_EMCUTE_DEFAULT_PORT, EMCUTE_ID);
    return NULL;    /* should never be reached */
}

// Message sending management -------------------
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

    //Chiamare motor handling


}
// Function for publish message to a topic
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

// ComponentHandler -----------------------------
// Set value on display
/*
void set_digit_value(int value) {
    const uint8_t* segment_values = digit_values[value];
    gpio_write(SEGMENT_A, segment_values[0]);
    gpio_write(SEGMENT_B, segment_values[1]);
    gpio_write(SEGMENT_C, segment_values[2]);
    gpio_write(SEGMENT_D, segment_values[3]);
    gpio_write(SEGMENT_E, segment_values[4]);
    gpio_write(SEGMENT_F, segment_values[5]);
    gpio_write(SEGMENT_G, segment_values[6]);
}
*/
void *sampling_temperature(void* arg){
    (void)arg;
    bool dataFlag = false;
    bool flag_on = true;
    bool flag_off = true;
    while(1){
        // Retrieve sensor reading 
        int16_t temp, hum;
        if (dht_read(&dev, &temp, &hum) != DHT_OK) {
            printf("sampling_temperature: Error reading values\n");
            dataFlag = true;
        }
        if(!dataFlag){
            // Extract + format temperature from sensor reading 
            char temp_s[10];
            size_t n = fmt_s16_dfp(temp_s, temp, -1);
            temp_s[n] = '\0';

            // Extract + format humidity from sensor reading 
            char hum_s[10];
            n = fmt_s16_dfp(hum_s, hum, -1);
            hum_s[n] = '\0';

            printf("DHT values - temp: %s°C - relative humidity: %s%%\n",temp_s, hum_s);

            //setting up message to send
            char message[15];    
            sprintf(message,"t%dh%d", temp, hum);
            publish(MQTT_TOPIC, message); //publishing message on broker  

            if(system_mod == AUTO ){
                if (flag_on && ((unsigned int)temp >= 255)) {
                    printf("Temperature above threshold - activating motor\n");
                    //set_digit_value(1);
                    //gpio_set(motor_pin);
                    flag_on = false;
                }
                if(flag_off && ((unsigned int)temp <= 254)) {
                    printf("Temperature under threshold - deactivating motor \n");
                    //set_digit_value(0);
                    //gpio_clear(motor_pin);
                    flag_off = false;
                }
            }

            
        }else{
            // Qui invece dobbiamo inviare un messaggio d'errore da gestire attraverso la web app per far capire che c'è stato un errore sul campling dei dati
            // al momento non inviamo niente.
        }

        dataFlag=false;

        // Sampling rate 10 for testing - 3 for video purpose
        xtimer_sleep(2);
    }
}

/*
void motor_handling(char *message){
GESTIONE DI TUTTA LA FASE DI AVVIO FORZATO E OFF FORZATO DEL SERVO.
switch(system_mod){
    case AUTO:  
        system_mod = 
        break;
    case ACTIVE:
        printf("\n Motor activation by external control\n")
        set_digit_value(1);
        gpio_set(motor_pin);
        break;
    case OFF:
        printf("\n Motor deactivation by external control\n")
        set_digit_value(0);
        gpio_clear(motor_pin);
        break;  
    default:
        printf("\n sampling_temperature : error during switch case of system_mod variable\n");
}
*/

// Initialization -------------------------------
static int init_component(void){
    /*// GPIO pin for 7-segment display
    gpio_init(SEGMENT_A, GPIO_OUT);
    gpio_init(SEGMENT_B, GPIO_OUT);
    gpio_init(SEGMENT_C, GPIO_OUT);
    gpio_init(SEGMENT_D, GPIO_OUT);
    gpio_init(SEGMENT_E, GPIO_OUT);
    gpio_init(SEGMENT_F, GPIO_OUT);
    gpio_init(SEGMENT_G, GPIO_OUT);
    */
    // Fix port parameter for digital sensor 
    dht_params_t my_params;
    my_params.pin = GPIO_PIN(PORT_A, 8);
    my_params.in_mode = DHT_PARAM_PULL;

    // Initialize sensor 
    if (dht_init(&dev, &my_params) == DHT_OK) {
        printf("init_component: DHT sensor connected\n");
    }
    else {
        printf("init_component: Failed to connect to DHT sensor\n");
        return 1;
    }
    /*
    if (gpio_init(motor_pin, GPIO_OUT)) {
        printf("init_component: Error to initialize GPIO_PIN(%d %d)\n", PORT_C, 9);
        return -1;
    }
    */
    printf("init_component: Initialization components completed. ");
    return 0;
}
// MQTT Initialization and subscription
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

// Main -----------------------------------------
int main(void){

    printf("RIOT windforme application\n"
            "AirCooler Test Application\n"
            "using RIOT DHT peripheral driver and Motor Mabuchi FC130\n"
            "DHT sensor type %d . Motor Type FC 130RA/SA \n", 22);
  
    // Device initialization 
    
    if(init_component()){
        printf("\n E: Something failed during the init of components ");
        return 1;
    }
    

    //Display 0 : OFF on display 7-segment
    //set_digit_value(0);

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

    //Handle thread creation for sampling data
    thread_create(temperature_stack, sizeof(temperature_stack),EMCUTE_PRIO, 0, sampling_temperature, NULL, "sampling_temperature");
    printf("\n Thread for sending information created ");

    return 0;
}

