//
// updated by ...: Loreto Notarantonio
// Date .........: 16-02-2026 16.32.37
//




// >>> main.cpp
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

    // --- WEB SERVER ---
    server.on("/", handleRoot);

    server.on("/apri", []() {
        int m = 30; // Default
        if (server.hasArg("t")) m = server.arg("t").toInt();
        if (!isMoving) startValvola(true, m);
        server.sendHeader("Location", "/"); server.send(303);
    });

    server.on("/chiudi", []() {
        if (!isMoving) startValvola(false);
        server.sendHeader("Location", "/"); server.send(303);
    });

    // SOLUZIONE ERRORI CONSOLE: Gestore per risorse non trovate
    server.onNotFound([]() {
        server.send(204, "text/plain", ""); // Risponde "No Content" e sta zitto
    });

    server.begin();
    // ... (Alexa e sensori uguale) ...

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
    // --- TELEGRAM ASYNC ---
    TBMessage msg;
    if (bot.getNewMessage(msg)) {
        String message = msg.text;

        if (message.startsWith("/apri")) {
            int minuti = 30;
            // Estrae il numero se presente (es: /apri 15)
            int spaceIdx = message.indexOf(' ');
            if (spaceIdx != -1) {
                minuti = message.substring(spaceIdx + 1).toInt();
                if (minuti < 1) minuti = 1;
                if (minuti > 60) minuti = 60;
            }
            startValvola(true, minuti);
            bot.sendMessage(msg, "Apertura avviata per " + String(minuti) + " min");
        }
        else if (message == "/chiudi") {
            startValvola(false);
            bot.sendMessage(msg, "Chiusura avviata");
        }
        else if (message == "/status") {
            sensors.requestTemperatures();
            String s = "Stato: " + String(isOpen ? "APERTA" : "CHIUSA") + "\n";
            if(isOpen) s += "Tempo residuo: " + String((currentAutoCloseDuration - (millis() - lastOpenMillis))/60000) + " min\n";
            s += "Temp: " + String(sensors.getTempCByIndex(0)) + " C";
            bot.sendMessage(msg, s);
        }
    }
}
