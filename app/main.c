#include <stdio.h>

#include "xtimer.h"
#include "fmt.h"
#include "dht.h"
#include "dht_params.h"
#include "periph/gpio.h"

// For MQTT
#include "net/emcute.h"


int main(void){
    
    printf("RIOT windforme application\n"
           "AirCooler Test Application\n"
           "using RIOT DHT peripheral driver and Motor Mabuchi FC130\n"
           "DHT sensor type %d . Motor Type FC 130RA/SA \n", 22);

    //Initialize display pin
    //init_display();

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
        printf("Error to initialize GPIO_PIN(%d %d)\n", PORT_A, 9);
        return -1;
    }
    else {
        printf("Pin for Activation motor initialization\n");
    }
    

    int i = 0;
    bool flag = false;
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
        if (((unsigned int)temp >= 241) && (!flag) && (!controlflag)) {
            printf("Temperature above threshold - activating motor\n");
            gpio_set(motor_pin);
            flag = true;
        }
        if(((unsigned int)temp <= 240) && flag && (!controlflag)){
            xtimer_sleep(2);
            printf("Temperature under threshold - deactivating motor \n");
            gpio_clear(motor_pin);
            flag = false;
            xtimer_sleep(5);
        }
        //only for testing - personal debug
        if(controlflag){
            printf("Reset, error on reading values from DHT sensor. ");
            //force deactivation motor. 
            gpio_clear(motor_pin);
        }
        
        //Clock Time for picking samples
        xtimer_sleep(3);
    }
    
    return 0;
}