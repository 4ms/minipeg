#include "globals.h"
extern uint16_t adc_cv_dma_buffer[NUM_CV_ADCS];
extern uint16_t adc_pot_dma_buffer[NUM_POT_ADCS];

const uint16_t max_pwm = 4095;
const uint16_t min_pwm = 0;

void test_hardware(void) {

    all_lights_off();

    while(PINGBUT) {;}

    while(!PINGBUT) {;}
    update_pwm(max_pwm, PWM_PINGBUT_R);
    while(PINGBUT) {;}
    update_pwm(min_pwm, PWM_PINGBUT_R);

    while(!PINGBUT) {;}
    update_pwm(max_pwm, PWM_PINGBUT_B);
    while(PINGBUT) {;}
    update_pwm(min_pwm, PWM_PINGBUT_B);

    while(!PINGBUT) {;}
    update_pwm(max_pwm, PWM_PINGBUT_R);
    LED_PING_BUT_G_ON;
    while(PINGBUT) {;}
    update_pwm(min_pwm, PWM_PINGBUT_R);
    LED_PING_BUT_G_OFF;

    while(CYCLEBUT) {;}

    while(!CYCLEBUT) {;}
    update_pwm(max_pwm, PWM_CYCLEBUT_R);
    while(CYCLEBUT) {;}
    update_pwm(min_pwm, PWM_CYCLEBUT_R);

    while(!CYCLEBUT) {;}
    update_pwm(max_pwm, PWM_CYCLEBUT_G);
    while(CYCLEBUT) {;}
    update_pwm(min_pwm, PWM_CYCLEBUT_G);

    while(!CYCLEBUT) {;}
    LED_CYCLE_BUT_B_ON;
    while(CYCLEBUT) {;}
    LED_CYCLE_BUT_B_OFF;

    uint16_t adcval;
    while (!PINGBUT) {
        update_pwm(PING_JACK_READ ? max_pwm : min_pwm, PWM_PINGBUT_R);
        update_pwm(CYCLE_JACK_READ ? max_pwm : min_pwm, PWM_CYCLEBUT_R);
        set_inverted_led(TRIG_JACK_READ ? max_pwm : min_pwm, PWM_EOF_LED);

        adcval = adc_dma_buffer[CV_SHAPE] / (4095/max_pwm);
        update_pwm(adcval, PWM_ENVLED_R);
        update_pwm(max_pwm - adcval, PWM_ENVLED_B);

        adcval = adc_dma_buffer[CV_DIVMULT] / (4095/max_pwm);
        update_pwm(adcval, PWM_5VENVLED_R);
        update_pwm(max_pwm - adcval, PWM_5VENVLED_B);
    }

    while(PINGBUT) {;}
    all_lights_off();

    while (!PINGBUT) {
        adcval = adc_dma_buffer[POT_SHAPE];
        update_pwm((adcval < 2048) ? max_pwm : min_pwm, PWM_ENVLED_R);
        update_pwm((adcval > 2048) ? max_pwm : min_pwm, PWM_ENVLED_B);

        adcval = adc_dma_buffer[POT_DIVMULT];
        update_pwm((adcval < 2048) ? max_pwm : min_pwm, PWM_PINGBUT_R);
        update_pwm((adcval > 2048) ? max_pwm : min_pwm, PWM_PINGBUT_B);

        adcval = adc_dma_buffer[POT_OFFSET];
        update_pwm((adcval < 2048) ? max_pwm : min_pwm, PWM_5VENVLED_R);
        update_pwm((adcval > 2048) ? max_pwm : min_pwm, PWM_5VENVLED_B);

        adcval = adc_dma_buffer[POT_SCALE];
        update_pwm((adcval < 2048) ? max_pwm : min_pwm, PWM_CYCLEBUT_R);
        update_pwm((adcval > 2048) ? max_pwm : min_pwm, PWM_CYCLEBUT_G);
    }

    while(PINGBUT) {;}

    all_lights_off();

    int16_t i=-max_pwm, i5=-max_pwm;
    uint16_t tri, tri5;
    while (!PINGBUT) {
        if (i++>=max_pwm) i = -max_pwm;
        tri = (i<0) ? -i : i;
        update_pwm(tri, PWM_ENV);
        update_pwm(tri, PWM_ENVLED_B); 

        if (i&1 && i5++>=max_pwm) i5= -max_pwm;
        tri5 = (i5<0) ? -i5 : i5;
        update_pwm(tri5, PWM_5VENV);
        update_pwm(tri5, PWM_5VENVLED_B);

        delay_ticks(2);
    }

    while(PINGBUT) {;}

    all_lights_off();
}
