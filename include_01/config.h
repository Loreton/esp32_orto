//
// updated by ...: Loreto Notarantonio
// Date .........: 22-11-2025 15.50.06
//

// >>> config.h
#pragma once
#include <WiFiClientSecure.h>
#include <Espalexa.h>
#include <UniversalTelegramBot.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WebServer.h>

// --- PIN CONFIG ---
#define RELAY_PWR 16
#define RELAY_DIR 17
#define BUZZER    23
#define BTN_APRI  21
#define BTN_CHIU  19
#define SENSOR_PIN 4

// --- PARAMETRI (Sostituzione testo, visibili ovunque) ---
#define MOVING_TIME 15000
#define AUTO_CLOSE_TIME 1800000
#define BOT_INTERVAL 2000

#ifdef __I_AM_MAIN_CPP__
    #include <lnEsp32Orto_Bot.h>
    #include <ssid_casetta.h>
    const char* ssid = casettaSSID;
    const char* password = casettaPassword;
    #define BOTtoken lnEsp32Orto

    bool isMoving = false;
    bool isOpen = false;
    unsigned long lastOpenMillis = 0;
    unsigned long lastBotCheck = 0;
    unsigned long moveStartTime = 0; // Nuova!

    Espalexa espalexa;
    WiFiClientSecure client;
    UniversalTelegramBot bot(BOTtoken, client);
    OneWire oneWire(SENSOR_PIN);
    DallasTemperature sensors(&oneWire);
    WebServer server(80);
#else
    extern bool isMoving;
    extern bool isOpen;
    extern unsigned long lastOpenMillis;
    extern unsigned long moveStartTime;
    extern Espalexa espalexa;
    extern UniversalTelegramBot bot;
    extern DallasTemperature sensors;
    extern WebServer server;
#endif

void startValvola(bool apri); // Rinominata per chiarezza
void checkValvolaTimer();      // Nuova funzione da mettere nel loop
void handleRoot(void);