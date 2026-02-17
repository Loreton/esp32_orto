//
// updated by ...: Loreto Notarantonio
// Date .........: 16-02-2026 16.32.37
//




// >>> main.cpp
#include <WiFi.h>
#include <ArduinoJson.h>


#define __I_AM_MAIN_CPP__
#include "config.h"



void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(RELAY_PWR, OUTPUT);
    pinMode(RELAY_DIR, OUTPUT);
    pinMode(BUZZER, OUTPUT);
    pinMode(BTN_APRI, INPUT_PULLUP);
    pinMode(BTN_CHIU, INPUT_PULLUP);

    digitalWrite(RELAY_PWR, LOW);

    // Connessione WiFi
    Serial.print("Connessione a "); Serial.println(ssid);
    WiFi.begin(ssid, password);
    client.setInsecure();

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    // --- DISPLAY IP ---
    Serial.println("");
    Serial.println("WiFi Connesso!");
    Serial.print("Indirizzo IP: ");
    Serial.println(WiFi.localIP()); // <--- Ecco l'IP per le tue prove
    Serial.println("Digita questo IP nel browser per la pagina web.");

    // Configurazione Web Server
    server.on("/", handleRoot);
    server.on("/apri", []() { gestisciValvola(true); server.sendHeader("Location", "/"); server.send(303); });
    server.on("/chiudi", []() { gestisciValvola(false); server.sendHeader("Location", "/"); server.send(303); });
    server.begin();

    // Alexa
    espalexa.addDevice("Valvola Orto", [](uint8_t b){ if(b>0) gestisciValvola(true); else gestisciValvola(false); });
    espalexa.begin();

    sensors.begin();
}

void loop() {
    server.handleClient();
    espalexa.loop();

    // Pulsanti Fisici (Interbloccati)
    if (!isMoving) {
        if (digitalRead(BTN_APRI) == LOW) { delay(50); gestisciValvola(true); }
        if (digitalRead(BTN_CHIU) == LOW) { delay(50); gestisciValvola(false); }
    }

    // Timer Auto-Chiusura
    if (isOpen && !isMoving && (millis() - lastOpenMillis > AUTO_CLOSE_TIME)) {
        Serial.println("Timer 30min scaduto: chiusura automatica.");
        gestisciValvola(false);
    }

    // Gestione Telegram
    if (millis() - lastBotCheck > BOT_INTERVAL && !isMoving) {
        int num = bot.getUpdates(bot.last_message_received + 1);
        for (int i=0; i<num; i++) {
            String t = bot.messages[i].text;
            if (t == "/apri") gestisciValvola(true);
            else if (t == "/chiudi") gestisciValvola(false);
            else if (t == "/stato") {
                sensors.requestTemperatures();
                bot.sendMessage(bot.messages[i].chat_id, "Stato: " + String(isOpen?"APERTA":"CHIUSA") + "\nTemp: " + String(sensors.getTempCByIndex(0)) + "C", "");
            }
        }
        lastBotCheck = millis();
    }
}