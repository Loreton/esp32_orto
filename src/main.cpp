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
#define BOTtoken lnEsp32Orto_token
#define BOTchatid lnEsp32Orto_chatid




void setup() {

    Serial.begin(115200);
    delay(1000);
    lnLog.init(22);
    LOG_INFO("Sistema OrtoControl in avvio...");

    if (lastAdminChatId == 0) {
        lastAdminChatId = BOTchatid;
    }
    // Configurazione Pin
    pinMode(RELAY_PWR, OUTPUT);
    pinMode(RELAY_DIR, OUTPUT);
    pinMode(BUZZER, OUTPUT);
    digitalWrite(RELAY_PWR, LOW);

    // PULSANTI FISICI (con pull-up interna)
    pinMode(BTN_APRI, INPUT_PULLUP);
    pinMode(BTN_CHIU, INPUT_PULLUP);


    WiFi.begin(ssid, password);
    int8_t timeout=5;
    while (WiFi.status() != WL_CONNECTED && timeout>0) {
        --timeout;
        delay(1000);
    }

    if (WiFi.status() == WL_CONNECTED) {
        LOG_INFO("WiFi Connesso. IP: %s", WiFi.localIP().toString().c_str());
    } else {
        LOG_ERROR("ERRORE Connessione WiFi...");
    }


    // WiFi.begin(ssid, password);
    // while (WiFi.status() != WL_CONNECTED) { delay(500); }

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




    // Gestione APRI dal Web
    server.on("/apri", []() {
        int minuti = 30;
        if (server.hasArg("t")) minuti = server.arg("t").toInt();

        LOG_INFO("Web: Richiesta apertura per %d min", minuti);
        startValvola(true, minuti);

        // Redirect alla home per non vedere "Not Found"
        server.sendHeader("Location", "/");
        server.send(303);
    });

    // Gestione CHIUDI dal Web
    server.on("/chiudi", []() {
        LOG_INFO("Web: Richiesta chiusura");
        startValvola(false);

        server.sendHeader("Location", "/");
        server.send(303);
    });

    server.onNotFound([]() {
        String uri = server.uri();
        // Silenziamo le richieste di Alexa/Hue per non sporcare il log
        if (uri.indexOf("api") == -1) {
            LOG_WARN("404: %s", uri.c_str());
        }
        server.send(404, "text/plain", "Not Found");
    });

    server.on("/favicon.ico", []() { server.send(204); });

    server.begin();

    LOG_INFO("Setup completato correttamente.");
}

void loop() {
    server.handleClient();
    espalexa.loop();
    checkValvolaTimer();
    updateTempHistory();
    handleStatusLED();

    static unsigned long lastCheck = 0;
    if (millis() - lastCheck > 30000) { // Controllo sensore ogni 30s
        lastCheck = millis();
        checkSensorHealth();
    }

    // 1. GESTIONE PULSANTI FISICI
    // Agiscono solo se la valvola non si sta già muovendo
    if (!EV_isMoving) {
        if (digitalRead(BTN_APRI) == LOW) {
            LOG_NOTIFY("Pulsante fisico: APRI");
            delay(50); // Debounce
            startValvola(true, 30);

            // if (lastAdminChatId == 0) {
                // lastAdminChatId = lnEsp32Orto_chatid;
            bot.sendTo(lastAdminChatId, "🔘 Pulsante fisico: Apertura avviata.");
            // } else {
                // LOG_WARN("Impossibile notificare Telegram: lastAdminChatId è zero!");
            // }
            delay(500); // Evita letture multiple
        }

        if (digitalRead(BTN_CHIU) == LOW) {
            LOG_NOTIFY("Pulsante fisico: CHIUDI");
            delay(50); // Debounce
            startValvola(false);

            if (lastAdminChatId != 0) {
                bot.sendTo(lastAdminChatId, "🔘 Pulsante fisico: Chiusura avviata.");
            }
            delay(500);
        }
    }




    // 2. GESTIONE TELEGRAM
    manageTelegram();

    // 3. AUTO-CHIUSURA
    if (EV_isOpen && !EV_isMoving && (millis() - lastOpenMillis > currentAutoCloseDuration)) {
        LOG_NOTIFY("Timer scaduto. Chiusura automatica.");
        startValvola(false);
        if (lastAdminChatId != 0) bot.sendTo(lastAdminChatId, "🕒 Tempo scaduto: valvola chiusa.");
    }
}





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


void handleStatusLED() {
    static unsigned long lastBlink = 0;
    static bool ledState = false;

    if (EV_isMoving) {
        // Lampeggio rapido durante il movimento
        if (millis() - lastBlink > 200) {
            lastBlink = millis();
            ledState = !ledState;
            digitalWrite(STATUS_LED, ledState);
        }
    } else {
        // Stato fisso quando ferma
        digitalWrite(STATUS_LED, EV_isOpen ? HIGH : LOW);
    }
}