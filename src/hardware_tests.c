#include "globals.h"
extern uint16_t adc_dma_buffer[NUM_ADCS];

void test_hardware(void) {

    all_lights_off();

    while(PINGBUT) {;}

    while(!PINGBUT) {;}
    update_pwm(1025, PWM_PINGBUT_R);
    while(PINGBUT) {;}
    update_pwm(0, PWM_PINGBUT_R);

    while(!PINGBUT) {;}
    update_pwm(1025, PWM_PINGBUT_B);
    while(PINGBUT) {;}
    update_pwm(0, PWM_PINGBUT_B);

    while(!PINGBUT) {;}
    update_pwm(1025, PWM_PINGBUT_R);
    LED_PING_BUT_G_ON;
    while(PINGBUT) {;}
    update_pwm(0, PWM_PINGBUT_R);
    LED_PING_BUT_G_OFF;

    while(CYCLEBUT) {;}

    while(!CYCLEBUT) {;}
    update_pwm(1025, PWM_CYCLEBUT_R);
    while(CYCLEBUT) {;}
    update_pwm(0, PWM_CYCLEBUT_R);

    while(!CYCLEBUT) {;}
    update_pwm(1025, PWM_CYCLEBUT_G);
    while(CYCLEBUT) {;}
    update_pwm(0, PWM_CYCLEBUT_G);

    while(!CYCLEBUT) {;}
    LED_CYCLE_BUT_B_ON;
    while(CYCLEBUT) {;}
    LED_CYCLE_BUT_B_OFF;

    uint16_t adcval;
    while (!PINGBUT) {
        update_pwm(PINGJACK ? 1024 : 0, PWM_PINGBUT_R);
        update_pwm(CYCLEJACK_READ ? 1024 : 0, PWM_CYCLEBUT_R);
        set_inverted_led(TRIGJACK_READ ? 1024 : 0, PWM_EOF_LED);

        adcval = adc_dma_buffer[CV_SHAPE] / 4;
        update_pwm(adcval, PWM_ENVLED_R);
        update_pwm(1024 - adcval, PWM_ENVLED_B);

        adcval = adc_dma_buffer[CV_DIVMULT] / 4;
        update_pwm(adcval, PWM_5VENVLED_R);
        update_pwm(1024 - adcval, PWM_5VENVLED_B);
    }

    while(PINGBUT) {;}
    all_lights_off();

    while (!PINGBUT) {
        adcval = adc_dma_buffer[POT_SHAPE];
        update_pwm((adcval < 2048) ? 1024:0, PWM_ENVLED_R);
        update_pwm((adcval > 2048) ? 1024:0, PWM_ENVLED_B);

        adcval = adc_dma_buffer[POT_DIVMULT];
        update_pwm((adcval < 2048) ? 1024:0, PWM_PINGBUT_R);
        update_pwm((adcval > 2048) ? 1024:0, PWM_PINGBUT_B);

        adcval = adc_dma_buffer[POT_OFFSET];
        update_pwm((adcval < 2048) ? 1024:0, PWM_5VENVLED_R);
        update_pwm((adcval > 2048) ? 1024:0, PWM_5VENVLED_B);

        adcval = adc_dma_buffer[POT_SCALE];
        update_pwm((adcval < 2048) ? 1024:0, PWM_CYCLEBUT_R);
        update_pwm((adcval > 2048) ? 1024:0, PWM_CYCLEBUT_G);
    }

    while(PINGBUT) {;}

    all_lights_off();

    int16_t i=-1024, i5=-1024;
    uint16_t tri, tri5;
    while (!PINGBUT) {
        if (i++>=1024) i = -1024;
        tri = (i<0) ? -i : i;
        update_pwm(tri, PWM_ENV);
        update_pwm(tri, PWM_ENVLED_B); 

        if (i&1 && i5++>=1024) i5= -1024;
        tri5 = (i5<0) ? -i5 : i5;
        update_pwm(tri5, PWM_5VENV);
        update_pwm(tri5, PWM_5VENVLED_B);

        delay_ticks(2);
    }

    while(PINGBUT) {;}

    all_lights_off();
}
