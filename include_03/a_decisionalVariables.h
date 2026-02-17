//
// updated by ...: Loreto Notarantonio
// Date .........: 22-11-2025 15.47.45
//
#pragma once


    // definiti in piorun.sh
    //      ln_RELEASE_TYPE
    //      ln_DEVEL
    //      ln_PRODUCTION

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
