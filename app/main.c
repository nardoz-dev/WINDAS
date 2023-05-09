#include <stdio.h>

#include "xtimer.h"
#include "fmt.h"
#include "dht.h"
#include "dht_params.h"
#include "periph/gpio.h"


//Pin for the display 7 segment.
/* OLD CONFIGURATION 
gpio_t segment_a = GPIO_PIN(PORT_A,9);   
gpio_t segment_b = GPIO_PIN(PORT_B,8);
gpio_t segment_c = GPIO_PIN(PORT_A,5);
gpio_t segment_d = GPIO_PIN(PORT_A,2);
gpio_t segment_e = GPIO_PIN(PORT_A,10);
gpio_t segment_f = GPIO_PIN(PORT_B,3);
gpio_t segment_g = GPIO_PIN(PORT_B,9);
gpio_t segment_h = GPIO_PIN(PORT_A,3);
gpio_t digit = GPIO_PIN(PORT_C,7);
*/

/*
New configuration
A -> D6
B -> D3
C -> D11
D -> D10
E -> D2
F -> D4
G -> D5
*/
gpio_t SEGMENT_A = GPIO_PIN(PORT_B,10);   
gpio_t SEGMENT_B = GPIO_PIN(PORT_B,3);
gpio_t SEGMENT_C = GPIO_PIN(PORT_A,7);
gpio_t SEGMENT_D = GPIO_PIN(PORT_B,6);
gpio_t SEGMENT_E = GPIO_PIN(PORT_A,10);
gpio_t SEGMENT_F = GPIO_PIN(PORT_B,5);
gpio_t SEGMENT_G = GPIO_PIN(PORT_B,4);

// Array di valori per le singole cifre del display
// Array di valori per le singole cifre del display
static const uint8_t digit_values[][7] = {
    {0, 0, 0, 0, 0, 0, 1}, // 0
    {1, 0, 0, 1, 1, 1, 1}, // 1
    {1, 1, 0, 1, 1, 0, 1}, // 2
    {1, 1, 1, 1, 0, 0, 1}, // 3
    {0, 1, 1, 0, 0, 1, 1}, // 4
    {1, 0, 1, 1, 0, 1, 1}, // 5
    {1, 0, 1, 1, 1, 1, 1}, // 6
    {1, 1, 1, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 1, 0, 1, 1}, // 9
};


//Inizializza il display
void init_display(void) {
    gpio_init(SEGMENT_A, GPIO_OUT);
    gpio_init(SEGMENT_B, GPIO_OUT);
    gpio_init(SEGMENT_C, GPIO_OUT);
    gpio_init(SEGMENT_D, GPIO_OUT);
    gpio_init(SEGMENT_E, GPIO_OUT);
    gpio_init(SEGMENT_F, GPIO_OUT);
    gpio_init(SEGMENT_G, GPIO_OUT);
}


// Imposta il valore di una singola cifra del display
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



int main(void){
    
    /*
    xtimer_sleep(2);
    gpio_t motor_pin = GPIO_PIN(PORT_C,7);
    if (gpio_init(motor_pin, GPIO_OUT)) {
        printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_C, 7);
        return -1;
    }
    else {
        printf("Good initialization\n");
    }

    gpio_t led_pin = GPIO_PIN(PORT_A,9);
    if (gpio_init(led_pin, GPIO_OUT)) {
        printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_A, 9);
        return -1;
    }
    else {
        printf("Good initialization\n");
    }
    */
    /*
    while(1){
        xtimer_sleep(5);
        printf("Set motor On\n");
        gpio_set(motor_pin);
        //gpio_set(led_pin);
        xtimer_sleep(5);

        printf("Set motor Off\n");
        gpio_clear(motor_pin);
        //gpio_clear(led_pin);

    }
    */
    
    
    printf("RIOT windforme application\n"
           "AirCooler Test Application\n"
           "using RIOT DHT peripheral driver and Motor Mabuchi FC130\n"
           "DHT sensor type %d . Motor Type FC 130RA/SA \n", 22);

    //Initialize display pin
    init_display();
    set_digit_value(1);
    /*

    // Fix port parameter for digital sensor 
    dht_params_t my_params;
    my_params.pin = GPIO_PIN(PORT_A, 8);
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
    gpio_t motor_pin = GPIO_PIN(PORT_C,7);
    if (gpio_init(motor_pin, GPIO_OUT)) {
        printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_C, 9);
        return -1;
    }
    else {
        printf("Pin for Activation motor initialization\n");
    }
    

    int i = 0;
    bool flag = false;
    //flag to avoid conflit in case DHT sensor failed to reading values
    bool controlflag = false;
    while(i == 0){

        // Retrieve sensor reading 
        int16_t temp, hum;
        if (dht_read(&dev, &temp, &hum) != DHT_OK) {
            printf("Error reading values\n");
            controlflag = true;
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
        if (((unsigned int)temp >= 255) && (!flag) && (!controlflag)) {
            printf("Temperature above threshold - activating motor\n");
            gpio_set(motor_pin);
            flag = true;
        }
        if(((unsigned int)temp <= 254) && flag && (!controlflag)){
            xtimer_sleep(2);
            printf("Temperature under threshold - deactivating motor \n");
            gpio_clear(motor_pin);
            flag = false;
            xtimer_sleep(5);
        }
        //only for testing - personal debug
        if(controlflag){
            printf("Reset, error on reading values from DHT sensor. Waiting ... ");
            xtimer_sleep(3);
            // force deactivation motor. 
            // gpio_clear(motor_pin);
            controlflag = false;
            printf("\n Reset Completed \n");
        }
        
        //Clock Time for picking samples
        xtimer_sleep(3);
    }
    */
    return 0;
}