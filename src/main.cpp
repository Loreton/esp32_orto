//
// updated by ...: Loreto Notarantonio
// Date .........: 16-02-2026 16.32.37
//




// >>> main.cpp
#include <WiFi.h>
#define __I_AM_MAIN_CPP__
#include "config.h"

// --- CREDENTIALS
#include <lnEsp32Orto_Bot.h>
#include <ssid_casetta.h>
const char* ssid = casettaSSID;
const char* password = casettaPassword;
#define BOTtoken lnEsp32Orto



// const char* ssid = "TUO_WIFI";
// const char* password = "TUA_PASSWORD";

void setup() {

    Serial.begin(115200);
    delay(1000);
    lnLog.init();
    LOG_INFO("Sistema OrtoControl in avvio...");

    // Configurazione Pin
    pinMode(RELAY_PWR, OUTPUT);
    pinMode(RELAY_DIR, OUTPUT);
    pinMode(BUZZER, OUTPUT);
    digitalWrite(RELAY_PWR, LOW);

    // PULSANTI FISICI (con pull-up interna)
    pinMode(BTN_APRI, INPUT_PULLUP);
    pinMode(BTN_CHIU, INPUT_PULLUP);


    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) { delay(500); }
    LOG_INFO("WiFi Connesso. IP: %s", WiFi.localIP().toString().c_str());

    tgClient.setInsecure();
    bot.setUpdateTime(2000);
    bot.setTelegramToken(BOTtoken);

    sensors.begin();

    // WebServer: Gestione rotte
    server.on("/", handleRoot);
    server.on("/favicon.ico", []() { server.send(204); });
    server.onNotFound([]() {
        LOG_WARN("Richiesta 404: %s", server.uri().c_str());
        server.send(404, "text/plain", "Non trovato");
    });
    server.begin();

    LOG_INFO("Setup completato correttamente.");
}

void loop() {
    server.handleClient();
    espalexa.loop();
    checkValvolaTimer();
    updateTempHistory();

    static unsigned long lastCheck = 0;
    if (millis() - lastCheck > 30000) { // Controllo sensore ogni 30s
        lastCheck = millis();
        checkSensorHealth();
    }

    // 1. GESTIONE PULSANTI FISICI
    // Agiscono solo se la valvola non si sta già muovendo
    if (!isMoving) {
        if (digitalRead(BTN_APRI) == LOW) {
            LOG_NOTIFY("Pulsante fisico: APRI premuto");
            delay(50); // Debounce
            startValvola(true, 30); // Apre per 30 min di default
            if (lastAdminChatId != 0) bot.sendTo(lastAdminChatId, "🔘 Pulsante fisico: Apertura avviata.");
        }

        if (digitalRead(BTN_CHIU) == LOW) {
            LOG_NOTIFY("Pulsante fisico: CHIUDI premuto");
            delay(50); // Debounce
            startValvola(false);
            if (lastAdminChatId != 0) bot.sendTo(lastAdminChatId, "🔘 Pulsante fisico: Chiusura avviata.");
        }
    }

    // 2. GESTIONE TELEGRAM
    manageTelegram();

    // 3. AUTO-CHIUSURA
    if (isOpen && !isMoving && (millis() - lastOpenMillis > currentAutoCloseDuration)) {
        LOG_NOTIFY("Timer scaduto. Chiusura automatica.");
        startValvola(false);
        if (lastAdminChatId != 0) bot.sendTo(lastAdminChatId, "🕒 Tempo scaduto: valvola chiusa.");
    }
}





// void loop_() {
//     server.handleClient();
//     checkValvolaTimer();
//     updateTempHistory();

//     static unsigned long lastCheck = 0;
//     if (millis() - lastCheck > 30000) { // Controllo sensore ogni 30s
//         lastCheck = millis();
//         checkSensorHealth();
//     }

//     TBMessage msg;
//     if (bot.getNewMessage(msg)) {
//         lastAdminChatId = msg.chatId;
//         String text = msg.text;

//         if (text == "/reboot") {
//             LOG_WARN("Reboot richiesto da Telegram!");
//             bot.sendMessage(msg, "🔄 Riavvio in corso...");
//             delay(2000);
//             ESP.restart();
//         }

//         if (isMoving) {
//             bot.sendMessage(msg, "⚠️ Operazione in corso... Attendi la fine del movimento.");
//             LOG_DEBUG("Comando ignorato: valvola in movimento.");
//         }
//         else {
//             if (text.startsWith("/apri")) {
//                 int min = 30;
//                 if (text.indexOf(' ') != -1) min = text.substring(text.indexOf(' ') + 1).toInt();
//                 startValvola(true, min);
//                 bot.sendMessage(msg, "✅ Apertura avviata (" + String(min) + " min)");
//             }
//             else if (text == "/chiudi") {
//                 startValvola(false);
//                 bot.sendMessage(msg, "✅ Chiusura avviata.");
//             }
//             else if (text == "/status") {
//                 sensors.requestTemperatures();
//                 String s = "Stato: " + String(isOpen ? "APERTA" : "CHIUSA") + "\n";
//                 s += "Temp: " + String(sensors.getTempCByIndex(0)) + " °C";
//                 bot.sendMessage(msg, s);
//             }
//         }
//     }

//     // Auto-chiusura
//     if (isOpen && !isMoving && (millis() - lastOpenMillis > currentAutoCloseDuration)) {
//         LOG_NOTIFY("Timer scaduto. Chiusura automatica.");
//         startValvola(false);
//         if (lastAdminChatId != 0) bot.sendTo(lastAdminChatId, "🕒 Tempo scaduto: valvola chiusa.");
//     }
// }

void updateTempHistory() {
    if (millis() - lastTempUpdate > 3600000 || lastTempUpdate == 0) {
        lastTempUpdate = millis();
        sensors.requestTemperatures();
        float t = sensors.getTempCByIndex(0);
        if (t != -127.0) {
            for (int i = 0; i < 23; i++) tempHistory[i] = tempHistory[i+1];
            tempHistory[23] = t;
            LOG_DEBUG("Storico temperature aggiornato: %.2f", t);
        }
    }
}

void checkSensorHealth() {
    sensors.requestTemperatures();
    float t = sensors.getTempCByIndex(0);
    if (t == -127.0 || t == 85.0) {
        if (!sensorFault) {
            LOG_ERROR("Errore lettura sensore!");
            sensorFault = true;
            if (lastAdminChatId != 0) bot.sendTo(lastAdminChatId, "🚨 ALLARME: Sensore scollegato!");
        }
    } else {
        if (sensorFault) {
            LOG_INFO("Sensore ripristinato.");
            sensorFault = false;
        }
    }
}