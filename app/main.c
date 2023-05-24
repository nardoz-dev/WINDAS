#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "random.h"

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

#define MQTT_TOPIC_INT      "topic_board"
#define MQTT_TOPIC_EXT      "topic_data"
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
    ON = 1,
    OFF = 2
} state_t;
state_t system_mod = AUTO;
dht_t dev;

// Array di valori per le singole cifre del display 
//(only 0,1 but the concept is easy, 0 to turn on led segment, 1 otherwise.)
static const uint8_t digit_values[][7] = {
    {0, 0, 0, 0, 0, 0, 1}, // 0 = OFF
    {1, 0, 0, 1, 1, 1, 1}, // 1 = ON
    {0, 0, 1, 0, 0, 1, 0}, // 2 = CUSTOM MODE      
};

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

    char msg[len+1];
    strncpy(msg, in, len);
    msg[len] = '\0';
    printf("\nContenuto di msg : %s", msg);

    if (strcmp(msg, "AUTO") == 0) {
        system_mod = AUTO;
    } else if (strcmp(msg, "ON") == 0) {
        system_mod = ON;
    } else if (strcmp(msg, "OFF") == 0) {
        system_mod = OFF;
    } else {
        printf("Valore di msg non valido ");
    }
    printf("\n\nsystem_mod: %d\n\n", system_mod);
    motor_handling();
    /*
    printf("### got publication for topic '%s' [%i] ###\n",
           topic->name, (int)topic->id);
    for (size_t i = 0; i < len; i++) {
        printf("%c", in[i]);
    }
    puts("");
    */

    //Setting memory for the message buffer
    memset(in, 0, sizeof(char));
}
// Function for publish message to a topic
static int publish(char *t, char *message){
    emcute_topic_t topic;
    topic.name = t;
    
    //getting ID from the topic
    if(emcute_reg(&topic) != EMCUTE_OK){ 
        printf("\npublish : cannot find topic:%s ", topic.name);
        return 1;
    }

    //publishing on the broker
    if(emcute_pub(&topic, message, strlen(message), MQTT_QoS) != EMCUTE_OK){ 
        puts("\npublish : cannot publish data ");
        return 1;
    }
    //printf("published message: %s (%i) on topic %s (id:%i)",
    //    message, (int)strlen(message), topic.name, topic.id);
    return 0;
}

// ComponentHandler -----------------------------
// Set value on display
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
void *sampling_temperature(void* arg){
    (void)arg;
    bool dataFlag = false;
    bool flag_on = true;
    bool flag_off = true;
    while(1){
        // Retrieve sensor reading 
        int16_t temp, hum;
        if (dht_read(&dev, &temp, &hum) != DHT_OK) {
            printf("\nsampling_temperature: Error reading values ");
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

            printf("\nDHT values - temp: %s°C - relative humidity: %s%% ",temp_s, hum_s);

            //setting up message to send
            char message[15];    
            sprintf(message,"t%dh%d", temp, hum);
            publish(MQTT_TOPIC_EXT, message); //publishing message on broker  
            
            if(system_mod == AUTO ){
                if (flag_on && ((unsigned int)temp >= 255)) {
                    printf("\nTemperature above threshold - activating motor ");
                    set_digit_value(1);
                    gpio_set(motor_pin);
                    flag_on = false;
                }
                if(flag_off && ((unsigned int)temp <= 254)) {
                    printf("\nTemperature under threshold - deactivating motor  ");
                    set_digit_value(0);
                    gpio_clear(motor_pin);
                    flag_off = false;
                }
            }

            
        }else{
            uint32_t random_number;
            int16_t temp,hum;
            random_number = random_uint32();

            temp = (int16_t)(random_number % (350 - 330 +1) +330);
            hum = (int16_t)(random_number % (640 - 620 +1) +620);

            char temp_s[10];
            char hum_s[10];
            snprintf(temp_s, sizeof(temp_s),"%d",temp);
            snprintf(hum_s,sizeof(hum_s),"%d",hum);
            // Default Message for testing purpose* (DHT11 SENSOR BROKEN)
            printf("\nDefault random value sending for TESTING purpose.");
            printf("\nDHT values - temp: %s°C - relative humidity: %s%% ",temp_s, hum_s);

            //setting up message to send
            char message[25];    
            sprintf(message,"t%sh%s", temp_s, hum_s);
            publish(MQTT_TOPIC_EXT, message); //publishing message on broker 
        }

        dataFlag=false;

        // Sampling rate 10 for testing - 3 for video purpose
        xtimer_sleep(10);
    }
}


void motor_handling(void){
    //GESTIONE DI TUTTA LA FASE DI AVVIO FORZATO E OFF FORZATO DEL SERVO.
    switch(system_mod){
        case AUTO:  
            printf("\nAUTO ");
            break;
        case ON:
            printf("\nMotor activation by external control ");
            set_digit_value(2);
            gpio_set(motor_pin);
            break;
        case OFF:
            printf("\nMotor deactivation by external control ");
            set_digit_value(2);
            gpio_clear(motor_pin);
            break;  
        default:
            printf("\nmotor_handling : command not found ");
    }
}


// Initialization -------------------------------
static int init_component(void){
    // GPIO pin for 7-segment display
    gpio_init(SEGMENT_A, GPIO_OUT);
    gpio_init(SEGMENT_B, GPIO_OUT);
    gpio_init(SEGMENT_C, GPIO_OUT);
    gpio_init(SEGMENT_D, GPIO_OUT);
    gpio_init(SEGMENT_E, GPIO_OUT);
    gpio_init(SEGMENT_F, GPIO_OUT);
    gpio_init(SEGMENT_G, GPIO_OUT);
    
    // Fix port parameter for digital sensor 
    dht_params_t my_params;
    my_params.pin = GPIO_PIN(PORT_A, 8);
    my_params.in_mode = DHT_PARAM_PULL;

    // Initialize sensor 
    if (!(dht_init(&dev, &my_params) == DHT_OK)) {
        printf("\ninit_component: Failed to connect to DHT sensor ");
        return 1;
    }
    
    if (gpio_init(motor_pin, GPIO_OUT)) {
        printf("\ninit_component: Error to initialize GPIO_PIN(%d %d) ", PORT_C, 9);
        return 1;
    }
    
    printf("\ninit_component: Initialization components completed. ");
    return 0;
}
// MQTT Initialization and subscription
static int sub(void){
    //Setup subscription
    subscriptions[0].cb = on_pub;
    subscriptions[0].topic.name = MQTT_TOPIC_INT;

    //Subscribing to topic
    if (emcute_sub(&subscriptions[0], MQTT_QoS) != EMCUTE_OK) {
        printf("\nconnect_broker: unable to subscribe to %s ", subscriptions[0].topic.name);
        return 1;
    }

    printf("\nsub: Now subscribed to %s ", subscriptions[0].topic.name);
    return 0;
}
static int connect_broker(void){

    printf("\nConnecting to MQTT-SN broker %s port %d. ",SERVER_ADDR, SERVER_PORT);

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
        puts("\nconnect_broker: error parsing IPv6 address ");
        return 1;
    }

    //Connecting to broker
    if (emcute_con(&gw, true, topic, message, len, 0) != EMCUTE_OK) {
        printf("\nconnect_broker: unable to connect to %s:%i ", SERVER_ADDR, (int)gw.port);
        return 1;
    }

    printf("\nSuccessfully connected to gateway at [%s]:%i ",SERVER_ADDR, (int)gw.port);

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
        puts("\nadd_netif: No valid Interface");
        return 1;
    }

    ipv6_addr_t ip_addr;
    uint16_t flag = GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID; 
    uint8_t prefix_len;
    
    prefix_len = get_prefix_len(dev_ip_address); 

    //Parsing IPv6 Address from String 
    if(ipv6_addr_from_str(&ip_addr, dev_ip_address) == NULL){
        puts("\nadd_netif: Error in parsing ipv6 address");
        return 1;
    }

    flag |= (prefix_len << 8U);

    //Set Interface Options 
    if(netif_set_opt(iface, NETOPT_IPV6_ADDR, flag, &ip_addr, sizeof(ip_addr)) < 0){
            puts("\nadd_netif: Error in Adding ipv6 address");
            return 1;
        }

    printf("\nadd_netif : Added %s with prefix %d to interface %s ", dev_ip_address, prefix_len, name_if);
    return 0;

}
static int setup_mqtt(void){

    //Setting memory for the subscription list
    memset(subscriptions, 0, (NUMOFSUBS * sizeof(emcute_sub_t)));

    //Starting Emcute Thread
    thread_create(stack, sizeof(stack), EMCUTE_PRIO, 0,emcute_thread, NULL, "emcute");
    printf("\nEmcute Thread Started ");

    if(add_netif(DEFAULT_INTERFACE,DEVICE_IP_ADDRESS)){
        puts("\nsetup_mqtt: Faile to add network interface ");
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

    printf("\nSetting up MQTT-SN .  ");
    if(setup_mqtt()){
        printf("\nE: Something failed during the setup of MQTT");
        return 1;
    }
    
    printf("\nBroker Connection .  ");
    if(connect_broker()){
        printf("\nE: Something failed during the broker connection");
        return 1;
    }

    printf("\nStarting subscription .  ");
    if(sub()){
        printf("\nE: Something went wrong while subscribing to the topic ");
        
        return 1;
    }

    //Handle thread creation for sampling data
    thread_create(temperature_stack, sizeof(temperature_stack),EMCUTE_PRIO, 0, sampling_temperature, NULL, "sampling_temperature");
    printf("\nThread for campling data created ");

    return 0;
}

