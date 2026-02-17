//
// updated by ...: Loreto Notarantonio
// Date .........: 22-11-2025 15.47.45
//
#pragma once

    #ifdef __I_AM_MAIN_CPP__
        #if ln_RELEASE_TYPE == ln_PRODUCTION
            #pragma message "siamo in PRODUCTION"
        #else
            #pragma message "siamo in TEST"
        #endif

        #if ln_ESP32_BOARD_TYPE == ln_ESP32_WROOM_32E_MODULE
            #pragma message "siamo con la board ln_ESP32_WROOM_32E_MODULE"
        #else
            #pragma message "siamo con la board ln_ESP32_WROOM_32E_MODULE_2RELAY"
        #endif

    #endif

    #if ln_ESP32_BOARD_TYPE == ln_ESP32_WROOM_32E_MODULE
        #define RELE_ON  HIGH
        #define RELE_OFF LOW

        #define EV_OPEN   HIGH // riguarda il RELAY_DIR
        #define EV_CLOSE  LOW  // riguarda il RELAY_DIR
        #define EV_220    HIGH // riguarda il RELAY_PWR
    #else
        #define RELE_ON  LOW
        #define RELE_OFF HIGH

        #define EV_OPEN   LOW // riguarda il RELAY_DIR
        #define EV_CLOSE  HIGH  // riguarda il RELAY_DIR
        #define EV_220    LOW // riguarda il RELAY_PWR
    #endif


