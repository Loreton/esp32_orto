//
// updated by ...: Loreto Notarantonio
// Date .........: 16-02-2026 16.32.37
//




// >>> main.cpp
#include <WiFi.h>
// #include <ArduinoJson.h>

#include <WiFi.h>
#define __I_AM_MAIN_CPP__
#include "config.h"

void setup() {
    Serial.begin(115200);
    pinMode(RELAY_PWR, OUTPUT); pinMode(RELAY_DIR, OUTPUT);
    pinMode(BUZZER, OUTPUT);
    pinMode(BTN_APRI, INPUT_PULLUP); pinMode(BTN_CHIU, INPUT_PULLUP);

    WiFi.begin(wifi_ssid, wifi_password);
    tgClient.setInsecure(); // Per AsyncTelegram

    while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
    Serial.println("\nIP: " + WiFi.localIP().toString());

    // TELEGRAM
    bot.setUpdateTime(2000);
    bot.setTelegramToken(BOTtoken);

    // WEB SERVER
    server.on("/", handleRoot);
    server.on("/apri", []() { if(!isMoving) startValvola(true); server.sendHeader("Location", "/"); server.send(303); });
    server.on("/chiudi", []() { if(!isMoving) startValvola(false); server.sendHeader("Location", "/"); server.send(303); });
    server.on("/favicon.ico", []() { server.send(204); }); // Fix Errore Console
    server.begin();

    // ALEXA
    espalexa.addDevice("Valvola Orto", [](uint8_t b){ if(!isMoving) startValvola(b > 0); });
    espalexa.begin();

    sensors.begin();
}

void loop() {
    server.handleClient();
    espalexa.loop();
    checkValvolaTimer(); // Spegne il relè dopo 15s

    // Pulsanti Fisici
    if (!isMoving) {
        if (digitalRead(BTN_APRI) == LOW) startValvola(true);
        if (digitalRead(BTN_CHIU) == LOW) startValvola(false);
    }

    // Auto-chiusura 30min
    if (isOpen && !isMoving && (millis() - lastOpenMillis > AUTO_CLOSE_TIME)) {
        startValvola(false);
    }

    // Telegram Asincrono (non blocca!)
    TBMessage msg;
    if (bot.getNewMessage(msg)) {
        if (msg.text == "/apri") startValvola(true);
        else if (msg.text == "/chiudi") startValvola(false);
    }
}