static int init_component(void);
//void set_digit_value(int value);
static void *emcute_thread(void *arg);
static int publish(char *t, char *message);
void *sampling_temperature(void* arg);
static int sub(void);
static uint8_t get_prefix_len(char *addr);
static int connect_broker(void);
static int add_netif(char *name_if, char *dev_ip_address);
static int setup_mqtt(void);
//void motor_handling(char *message);

