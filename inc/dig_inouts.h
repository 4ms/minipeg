/*
 * dig_inouts.h
 */

#ifndef INOUTS_H_
#define INOUTS_H_

#include <stm32f0xx.h>


#define PIN_IS_HIGH(x,y) (x->IDR & y)
#define PIN_IS_LOW(x,y) (!(x->IDR & y))

#define PIN_HIGH(x,y) x->BSRR = y
#define PIN_LOW(x,y) x->BRR = y

#define ALL_GPIO_RCC (RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOF)

#define TRIGJACK_pin GPIO_Pin_15
#define TRIGJACK_GPIO GPIOA
#define TRIGJACK PIN_IS_HIGH(TRIGJACK_GPIO, TRIGJACK_pin)

#define PINGJACK_pin GPIO_Pin_5
#define PINGJACK_GPIO GPIOB
#define PINGJACK PIN_IS_HIGH(PINGJACK_GPIO, PINGJACK_pin)

#define PINGBUT_pin GPIO_Pin_3
#define PINGBUT_GPIO GPIOB
#define PINGBUT PIN_IS_LOW(PINGBUT_GPIO, PINGBUT_pin)

#define CYCLEJACK_pin GPIO_Pin_6
#define CYCLEJACK_GPIO GPIOB
#define CYCLEJACK PIN_IS_HIGH(CYCLEJACK_GPIO, CYCLEJACK_pin)

#define CYCLEBUT_pin GPIO_Pin_1
#define CYCLEBUT_GPIO GPIOF
#define CYCLEBUT PIN_IS_LOW(CYCLEBUT_GPIO, CYCLEBUT_pin)

#define EOF_pin GPIO_Pin_12
#define EOF_GPIO GPIOA
#define EOF_ON PIN_HIGH(EOF_GPIO, EOF_pin)
#define EOF_OFF PIN_LOW(EOF_GPIO, EOF_pin)


/*PEG v2

#define PINGLED_pin PB1
#define PINGLED_init DDRB |= (1<<PINGLED_pin)
#define PINGLEDFLIP PORTB ^= (1<<PINGLED_pin)
#define PINGLEDHIGH PORTB |= (1<<PINGLED_pin)
#define PINGLEDLOW PORTB &= ~(1<<PINGLED_pin)

#define PING_pin PD2
#define PING_init DDRD &= ~(1<<PING_pin); PORTD &= ~(1<<PING_pin)
#define PING (PIND & (1<<PING_pin))

#define TRIGA_pin PD4
#define TRIGA_init DDRD &= ~(1<<TRIGA_pin); PORTD &= ~(1<<TRIGA_pin)
#define TRIGA (PIND & (1<<TRIGA_pin))

#define TRIGQ_pin PD3
#define TRIGQ_init DDRD &= ~(1<<TRIGQ_pin); PORTD &= ~(1<<TRIGQ_pin)
#define TRIGQ (PIND & (1<<TRIGQ_pin))

#define CYCLE_BUT_pin PC3
#define CYCLE_BUT_init DDRC &= ~(1<<CYCLE_BUT_pin); PORTC |= (1<<CYCLE_BUT_pin)
#define CYCLE_BUT_RAW (!(PINC & (1<<CYCLE_BUT_pin)))

#define CYCLE_LED_pin PC4
#define CYCLE_LED_init DDRC |= (1<<CYCLE_LED_pin)
#define CYCLE_LED_ON PORTC |= (1<<CYCLE_LED_pin)
#define CYCLE_LED_OFF PORTC &= ~(1<<CYCLE_LED_pin)

#define CYCLE_INVERT_pin PC5
#define CYCLE_INVERT_init DDRC &= ~(1<<CYCLE_INVERT_pin); PORTC &= ~(1<<CYCLE_INVERT_pin)
#define CYCLE_INVERT (PINC & (1<<CYCLE_INVERT_pin))

#define CYCLE_BUT ((CYCLE_BUT_RAW && !CYCLE_INVERT) || (!CYCLE_BUT_RAW && CYCLE_INVERT))

#define EO1_pin PD6
#define EO2_pin PD5

#define EO1_OFF PORTD |= (1<<EO1_pin)
#define EO1_ON PORTD &= ~(1<<EO1_pin)

#define EO2_OFF PORTD |= (1<<EO2_pin)
#define EO2_ON PORTD &= ~(1<<EO2_pin)


#define EO_init DDRD |= (1<<EO1_pin) | (1<<EO2_pin)


#define CHANNEL_ID_OUT_pin PC0
#define CHANNEL_ID_OUT_init DDRC |= (1 << CHANNEL_ID_OUT_pin)
#define CHANNEL_ID_OUT_ON PORTC |= (1 << CHANNEL_ID_OUT_pin)
#define CHANNEL_ID_OUT_OFF PORTC &= ~(1 << CHANNEL_ID_OUT_pin)

#define TAPIN_pin PD7
#define TAPIN_init DDRD &= ~(1<<TAPIN_pin); \
		PORTD |=(1<<TAPIN_pin)
#define TAPIN (!(PIND & (1<<TAPIN_pin)))


#define ADC_DDR DDRC
#define ADC_PORT PORTC
#define ADC_mask 0b00000111
#define NUM_ADC 3


#define CURVE_adc 6
#define CLKDIV_adc PC2
#define SKEW_adc PC1


#define SPI_PIN PINB
#define SPI_PORT PORTB
#define SPI_DDR DDRB
#define SPI_MOSI PB3
#define SPI_MISO PB4
#define SPI_SCLK PB5

#define BLUE_DETECT_PIN PB0
#define READ_BLUE_DETECT (PINB & (1<<BLUE_DETECT_PIN))
#define BLUE_DETECT_init DDRD &= ~(1<<BLUE_DETECT_PIN)
char BLUE_DETECT=0;

*/

/*QPLFO*/
/*
#define PING_RCC RCC_AHBPeriph_GPIOB
#define PING_GPIO GPIOB
#define PING1_pin GPIO_Pin_0
#define PING1 ((PING_GPIO->IDR & PING1_pin))
#define PING2_pin GPIO_Pin_1
#define PING2 ((PING_GPIO->IDR & PING2_pin))
#define PING3_pin GPIO_Pin_3
#define PING3 ((PING_GPIO->IDR & PING3_pin))
#define PING4_pin GPIO_Pin_4
#define PING4 ((PING_GPIO->IDR & PING4_pin))


#define RESET_RCC (RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOF)

#define RESET123_GPIO GPIOB
#define RESET1_pin GPIO_Pin_5
#define RESET1 ((RESET123_GPIO->IDR & RESET1_pin))
#define RESET2_pin GPIO_Pin_6
#define RESET2 ((RESET123_GPIO->IDR & RESET2_pin))
#define RESET3_pin GPIO_Pin_7
#define RESET3 ((RESET123_GPIO->IDR & RESET3_pin))

#define RESET4_GPIO GPIOF
#define RESET4_pin GPIO_Pin_1
#define RESET4 ((RESET4_GPIO->IDR & RESET4_pin))
*/

/*
#define EXTI_CLOCK_GPIO EXTI_PortSourceGPIOB
#define EXTI_CLOCK_pin EXTI_PinSource12
#define EXTI_CLOCK_line EXTI_Line12
#define EXTI_CLOCK_IRQ EXTI15_10_IRQn
*/


//OUTPUTS
/*
#define TAPLED_RCC (RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOF)
#define TAPLED1_GPIO GPIOA
#define TAPLED2_GPIO GPIOA
#define TAPLED3_GPIO GPIOF
#define TAPLED4_GPIO GPIOA

#define TAPLED1_pin GPIO_Pin_12
#define TAPLED2_pin GPIO_Pin_13
#define TAPLED3_pin GPIO_Pin_0
#define TAPLED4_pin GPIO_Pin_15

#define TAPLED1_ON TAPLED1_GPIO->BSRRL = TAPLED1_pin
#define TAPLED1_OFF TAPLED1_GPIO->BSRRH = TAPLED1_pin

#define TAPLED2_ON TAPLED2_GPIO->BSRRL = TAPLED2_pin
#define TAPLED2_OFF TAPLED2_GPIO->BSRRH = TAPLED2_pin

#define TAPLED3_ON TAPLED3_GPIO->BSRRL = TAPLED3_pin
#define TAPLED3_OFF TAPLED3_GPIO->BSRRH = TAPLED3_pin

#define TAPLED4_ON TAPLED4_GPIO->BSRRL = TAPLED4_pin
#define TAPLED4_OFF TAPLED4_GPIO->BSRRH = TAPLED4_pin


#define LFOPWM_GPIO_RCC RCC_AHBPeriph_GPIOA
#define LFOPWM_GPIO GPIOA
#define LFOPWM1_pin GPIO_Pin_8
#define LFOPWM2_pin GPIO_Pin_9
#define LFOPWM3_pin GPIO_Pin_10
#define LFOPWM4_pin GPIO_Pin_11

#define LFOPWM1_source GPIO_PinSource8
#define LFOPWM2_source GPIO_PinSource9
#define LFOPWM3_source GPIO_PinSource10
#define LFOPWM4_source GPIO_PinSource11
*/




void init_dig_inouts(void);
// void init_EXTI_inputs(void);
// void init_inputread_timer(void);

#endif /* INOUTS_H_ */
