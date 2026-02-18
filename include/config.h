//
// updated by ...: Loreto Notarantonio
// Date .........: 22-11-2025 15.50.06
//

// >>> config.h
#pragma once
#include <Arduino.h>
#include <lnLogger_Class.h> // Tua libreria logger
#include <WiFiClientSecure.h>
#include <Espalexa.h>
#include <AsyncTelegram2.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WebServer.h>

// Livello di log globale per il progetto
#define LOG_MODULE_LEVEL LOG_LEVEL_DEBUG
#include "lnLogger_Class.h"

// Pin Config
// #define RELAY_PWR 18
// #define RELAY_DIR 19
// #define SENSOR_PIN 21

// --- PIN CONFIG ---
#define RELAY_PWR 16
#define RELAY_DIR 17
#define BUZZER    23
#define BTN_APRI  21
#define BTN_CHIU  19
#define SENSOR_PIN 4

#define MOVING_TIME 15000 // 15 secondi

#ifdef __I_AM_MAIN_CPP__

    bool EV_isMoving = false;
    bool EV_isOpen = false;
    bool sensorFault = false;
    bool alarmSent = false;
    unsigned long moveStartTime = 0;
    unsigned long lastOpenMillis = 0;
    unsigned long currentAutoCloseDuration = 1800000;
    int64_t lastAdminChatId = 0;

    float tempHistory[24] = {0};
    unsigned long lastTempUpdate = 0;

    WebServer server(80);
    Espalexa espalexa;
    WiFiClientSecure tgClient;
    AsyncTelegram2 bot(tgClient);
    OneWire oneWire(SENSOR_PIN);
    DallasTemperature sensors(&oneWire);
#else
    extern bool EV_isMoving, EV_isOpen, sensorFault, alarmSent;
    extern unsigned long moveStartTime, lastOpenMillis, currentAutoCloseDuration;
    extern int64_t lastAdminChatId;
    extern float tempHistory[24];
    extern WebServer server;
    extern AsyncTelegram2 bot;
    extern DallasTemperature sensors;
#endif

// Prototipi
void startValvola(bool apri, int minuti = 30);
void checkValvolaTimer(void);
void handleRoot(void);
void updateTempHistory(void);
void checkSensorHealth(void);
void manageTelegram(void);