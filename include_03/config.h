//
// updated by ...: Loreto Notarantonio
// Date .........: 22-11-2025 15.50.06
//

// >>> config.h
#pragma once
#include <WiFi.h>
#include <WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Espalexa.h>
#include <AsyncTelegram2.h> // <--- Cambiata!

// PIN
#define RELAY_PWR 16
#define RELAY_DIR 17
#define BUZZER    23
#define BTN_APRI  21
#define BTN_CHIU  19
#define SENSOR_PIN 4

// PARAMETRI
#define MOVING_TIME 15000
#define AUTO_CLOSE_TIME 1800000

#ifdef __I_AM_MAIN_CPP__
    // --- credentials
    #include <lnEsp32Orto_Bot.h>
    #include <ssid_casetta.h>
    const char* wifi_ssid = casettaSSID;
    const char* wifi_password = casettaPassword;
    #define BOTtoken lnEsp32Orto

    bool isMoving = false;
    bool isOpen = false;
    unsigned long moveStartTime = 0;
    unsigned long lastOpenMillis = 0;
    unsigned long currentAutoCloseDuration = 1800000; // Default 30 min

    WebServer server(80);
    Espalexa espalexa;
    WiFiClientSecure tgClient; // Per Telegram
    AsyncTelegram2 bot(tgClient);
    OneWire oneWire(SENSOR_PIN);
    DallasTemperature sensors(&oneWire);
#else
    extern bool isMoving;
    extern bool isOpen;
    extern unsigned long moveStartTime;
    extern unsigned long lastOpenMillis;
    extern unsigned long currentAutoCloseDuration;

    extern WebServer server;
    extern Espalexa espalexa;
    extern AsyncTelegram2 bot;
    extern DallasTemperature sensors;

#endif

// Prototipi
void startValvola(bool apri, int minuti = 30); // Aggiunto parametro minuti
void checkValvolaTimer();
void handleRoot();