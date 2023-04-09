#include <stdio.h>

#include "xtimer.h"
#include "fmt.h"
#include "dht.h"
#include "dht_params.h"

#define MOTOR_PIN GPIO_PIN(PORT_A,9)

#define SEGMENT_A GPIO_PIN(PORT_A,3)
#define SEGMENT_B GPIO_PIN(PORT_A,2)
#define SEGMENT_C GPIO_PIN(PORT_B,3)
#define SEGMENT_D GPIO_PIN(PORT_B,5)
#define SEGMENT_E GPIO_PIN(PORT_B,4)
#define SEGMENT_F GPIO_PIN(PORT_B,10)
#define SEGMENT_G GPIO_PIN(PORT_A,8)
#define DIGIT_1 GPIO_PIN(PORT_C,7)
#define DIGIT_2 GPIO_PIN(PORT_A,7)
#define DIGIT_3 GPIO_PIN(PORT_A,6)
#define DIGIT_4 GPIO_PIN(PORT_A,5)
#define DP GPIO_PIN(PORT_B,6)



// Array di valori per le singole cifre del display
// Array di valori per le singole cifre del display
static const uint8_t digit_values[][7] = {
    {0, 0, 0, 0, 0, 0, 1}, // 0
    {0, 1, 1, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1}, // 2
    {1, 1, 1, 1, 0, 0, 1}, // 3
    {0, 1, 1, 0, 0, 1, 1}, // 4
    {1, 0, 1, 1, 0, 1, 1}, // 5
    {1, 0, 1, 1, 1, 1, 1}, // 6
    {1, 1, 1, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 1, 0, 1, 1}, // 9
};


// Inizializza il display
void init_display(void) {
    gpio_init(SEGMENT_A, GPIO_OUT);
    gpio_init(SEGMENT_B, GPIO_OUT);
    gpio_init(SEGMENT_C, GPIO_OUT);
    gpio_init(SEGMENT_D, GPIO_OUT);
    gpio_init(SEGMENT_E, GPIO_OUT);
    gpio_init(SEGMENT_F, GPIO_OUT);
    gpio_init(SEGMENT_G, GPIO_OUT);
    gpio_init(DIGIT_1, GPIO_OUT);
    gpio_init(DIGIT_2, GPIO_OUT);
    gpio_init(DIGIT_3, GPIO_OUT);
    gpio_init(DIGIT_4, GPIO_OUT);
}


// Imposta il valore di una singola cifra del display
void set_digit_value(int digit, int value) {
    gpio_clear(DIGIT_1);
    gpio_clear(DIGIT_2);
    gpio_clear(DIGIT_3);
    gpio_clear(DIGIT_4);

    const uint8_t* segment_values = digit_values[value];
    gpio_write(SEGMENT_A, segment_values[0]);
    gpio_write(SEGMENT_B, segment_values[1]);
    gpio_write(SEGMENT_C, segment_values[2]);
    gpio_write(SEGMENT_D, segment_values[3]);
    gpio_write(SEGMENT_E, segment_values[4]);
    gpio_write(SEGMENT_F, segment_values[5]);
    gpio_write(SEGMENT_G, segment_values[6]);

    switch (digit) {
        case 1:
            gpio_set(DIGIT_1);
            break;
        case 2:
            gpio_set(DIGIT_2);
            break;
        case 3:
            gpio_set(DIGIT_3);
            break;
        case 4:
            gpio_set(DIGIT_4);
            break;
        default:
            break;
    }
}


int main(void){
    init_display();
    
    set_digit_value(1,0);

    /*
    printf("RIOT temperature_humidity application\n"
           "DHT temperature and humidity sensor test application\n"
           "using RIOT DHT peripheral driver\n"
           "DHT sensor type %d\n", 22);

    // Fix port parameter for digital sensor 
    dht_params_t my_params;
    my_params.pin = GPIO_PIN(PORT_A, 10);
    my_params.in_mode = DHT_PARAM_PULL;

    // Initialize digital sensor 
    dht_t dev;
    if (dht_init(&dev, &my_params) == DHT_OK) {
        printf("DHT sensor connected\n");
    }
    else {
        printf("Failed to connect to DHT sensor\n");
        return 1;
    }

    // Initialize GPIO pin for motor 
    gpio_init(MOTOR_PIN,GPIO_OUT);
    

    int i = 0;
    while(i == 0){

        // Retrieve sensor reading 
        int16_t temp, hum;
        if (dht_read(&dev, &temp, &hum) != DHT_OK) {
            printf("Error reading values\n");
        }

        // Extract + format temperature from sensor reading 
        char temp_s[10];
        size_t n = fmt_s16_dfp(temp_s, temp, -1);
        temp_s[n] = '\0';

        // Extract + format humidity from sensor reading 
        char hum_s[10];
        n = fmt_s16_dfp(hum_s, hum, -1);
        hum_s[n] = '\0';

        printf("DHT values - temp: %s°C - relative humidity: %s%%\n",temp_s, hum_s);

        printf("%u\n", (unsigned int)temp);
         // Check temperature and activate motor if necessary 
        if ((unsigned int)temp >= 200) {
            printf("Temperature above threshold - activating motor\n");
            gpio_set(MOTOR_PIN);
            xtimer_usleep(25000000);
            gpio_clear(MOTOR_PIN);
        }
        
        //struct timespec delay = { .tv_sec = 5, .tv_nsec = 0 };
        //nanosleep(&delay, NULL);
         xtimer_usleep(3000000);


    }
    
    */

    return 0;
}