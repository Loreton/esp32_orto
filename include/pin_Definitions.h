//
// updated by ...: Loreto Notarantonio
// Date .........: 16-09-2025 12.36.43
//

#pragma once
#include <Arduino.h>

// ;  //this line ALSO solves everything!


    #include "a_decisionalVariables.h"



/*
    ESP32 Pinout Description https://www.teachmemicro.com/esp32-pinout-diagram-wroom-32/
    The ESP32 pins are categorized into digital pins, analog pins, and power pins.
    Refer to the table below for details on pins with secondary functions.
    Moreover, these secondary pins often serve communication purposes, such as I2C and SPI or as ADC channels.
*/



//           GPIO                      description                                   Input       Output   Notes
    #define GPIO_00        0        // avoid - IO0   ADC2_CH1 TOUCH_CH1 Boot         pulled up   OK       outputs PWM signal at boot, must be LOW to enter flashing mode
    #define GPIO_01        1        // avoid - U0TXD -                               --          OK       TX pin debug output at boot
    #define GPIO_02        2        // avoid - IO2   ADC2_CH2 TOUCH_CH2              OK          OK       GPIO1, TXD connected to on-board LED, must be left floating or LOW to enter flashing mode
    #define GPIO_03        3        // avoid - RX    U0RXD                           OK          --       GPIO3, RXD HIGH at boot
    #define GPIO_04        4        // good  - IO4   ADC2_CH0 TOUCH_CH0              OK          OK       ADC2_CH0 TOUCH_CH0
    #define GPIO_05        5        // avoid - IO5                                   OK          OK       outputs PWM signal at boot, strapping pin

    #define GPIO_06        6        // avoid - CLK   CLK 2                           connected to the integrated SPI flash
    #define GPIO_07        7        // avoid - D0    D0 2                            connected to the integrated SPI flash
    #define GPIO_08        8        // avoid - D1    D1 2                            connected to the integrated SPI flash
    #define GPIO_09        9        // avoid - D2    D2 2                            connected to the integrated SPI flash
    #define GPIO_10       10        // avoid - D3    D3 2                            connected to the integrated SPI flash
    #define GPIO_11       11        // avoid - CMD   CMD 2                           connected to the integrated SPI flash

    #define GPIO_12       12        // good  - IO12  ADC2_CH5  TOUCH_CH5 M-TDI        OK          OK       boot fails if pulled high, strapping pin
    #define GPIO_13       13        // good  - IO13  ADC2_CH4  TOUCH_CH4 M-TCK        OK          OK       ADC2_CH4  TOUCH_CH4 MTCK
    #define GPIO_14       14        // good  - IO14  ADC2_CH6  TOUCH_CH6 M-TMS        OK          OK       outputs PWM signal at boot
    #define GPIO_15       15        // good  - IO15  ADC2_CH3  TOUCH_CH3 M-TDO         OK          OK       outputs PWM signal at boot, strapping pin
    #define GPIO_16       16        // good  - IO16  3                               OK          OK       RELAY1
    #define GPIO_17       17        // good  - IO17  3                               OK          OK       RELAY2
    #define GPIO_18       18        // good  - IO18                                  OK          OK
    #define GPIO_19       19        // good  - IO19                                  OK          OK
    #define GPIO_21       21        // good  - IO21                                  OK          OK
    #define GPIO_22       22        // good  - IO22                                  OK          OK
    #define GPIO_23       23        // good  - IO23                                  OK          OK       LED_ON_BOARD
    #define GPIO_25       25        // good  - IO25 ADC1_CH8  DAC_1                  OK          OK
    #define GPIO_26       26        // good  - IO26 ADC2_CH9  DAC_2                  OK          OK
    #define GPIO_27       27        // good  - IO27 ADC2_CH7  TOUCH_CH7OK            OK
    #define GPIO_32       32        // good  - IO32 ADC1_CH4  TOUCH_CH9 XTAL_32K_P   OK          OK
    #define GPIO_33       33        // good  - IO33 ADC1_CH5  TOUCH_CH8 XTAL_32K_N   OK          OK
    #define GPIO_34       34        // good  - IO34 ADC1_CH6  VDET_1                 OK          --   input only (PULL_DOWN)
    #define GPIO_35       35        // good  - IO35 ADC1_CH7  VDET_2                 OK          --   input only (PULL_DOWN)

    #define GPIO_36       36        // avoid - VP   ADC1_CH0  S_VP                   OK          --   input only (NOT found)
    #define GPIO_39       39        // avoid - VN   ADC1_CH3  S_VN TX                OK          --   input only (NOT found)




    // #define PROTOTYPE

    #if ln_ESP32_BOARD_TYPE == ln_ESP32_WROOM_32E_MODULE_2RELAY
        #define SPECIAL_ON_OFF_LEVEL        HIGH  // cambia in base alla board utilizzata
        #define passiveBuzzer_pin           GPIO_22  // OUTPUT
        #define activeBuzzer_pin            GPIO_23  // OUTPUT
        #define pressControlLED_pin         GPIO_25  // OUTPUT
        #define pumpLED_pin                 GPIO_26  // OUTPUT

        #define JTAG_TDI_reserved           GPIO_12  // TDI
        #define JTAG_TCK_reserved           GPIO_13  // TCK
        #define JTAG_TMS_reserved           GPIO_14  // TMS
        #define JTAG_TDO_reserved           GPIO_15  // TDO


        #define pressControlRelay_pin       GPIO_16  // OUTPUT
        #define magnetoTermicoRelay_pin     GPIO_17  // OUTPUT

        #define pressControl_pin            GPIO_18  // INPUT
        #define pumpState_pin               GPIO_19  // INPUT
        #define startButton_pin             GPIO_21  // INPUT

        #ifdef _I_AM_PIN_INITIALIZATION_CPP_
            #pragma message("using board:\n- ESP32_WROOM_32E_MODULE_2RELAY\n- ESP32_WROOM_32E_MODULE_2RELAY\n- ESP32_WROOM_32E_MODULE_2RELAY")
        #endif

    #elif ln_ESP32_BOARD_TYPE == ln_ESP32_WROOM_32E_MODULE
        #define SPECIAL_ON_OFF_LEVEL         LOW
        #define passiveBuzzer_pin           GPIO_22  // OUTPUT
        #define activeBuzzer_pin            GPIO_23  // OUTPUT
        #define pressControlLED_pin         GPIO_25  // OUTPUT
        #define pumpLED_pin                 GPIO_26  // OUTPUT

        #define JTAG_TDI_reserved           GPIO_12  // TDI
        #define JTAG_TCK_reserved           GPIO_13  // TCK
        #define JTAG_TMS_reserved           GPIO_14  // TMS
        #define JTAG_TDO_reserved           GPIO_15  // TDO


        #define pressControlRelay_pin       GPIO_16  // OUTPUT
        #define magnetoTermicoRelay_pin     GPIO_17  // OUTPUT

        #define pressControl_pin            GPIO_18  // INPUT
        #define pumpState_pin               GPIO_19  // INPUT
        #define startButton_pin             GPIO_21  // INPUT

        #ifdef _I_AM_PIN_INITIALIZATION_CPP_
            #pragma message("using board:\n- ln_ESP32_WROOM_32E_MODULE\n- ln_ESP32_WROOM_32E_MODULE\n- ln_ESP32_WROOM_32E_MODULE")
        #endif

    #else
        #error "BOARD non specificata"
    #endif


