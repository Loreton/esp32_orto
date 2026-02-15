//
// updated by ...: Loreto Notarantonio
// Date .........: 22-11-2025 17.04.08
//




// #include <Arduino.h>    // in testa anche per le definizioni dei type
// #include <stdint.h>    // per gli uintxx_t


// #include <WiFi.h>
// #include <WebServer.h>
// #include <Espalexa.h>
// #include <WiFiClientSecure.h>
// #include <UniversalTelegramBot.h>
// #include <OneWire.h>
// #include <DallasTemperature.h>
// #include <Preferences.h>
// Preferences prefs;


// ===== WIFI =====
const char* ssid = "SSID";
const char* password = "PASSWORD";

// ===== TELEGRAM =====
#define BOT_TOKEN "TOKEN"
#define CHAT_ID "CHATID"

// ===== PIN =====
#define RELAY_OPEN   16
#define RELAY_CLOSE  17
#define activeBuzzer_pin       23
// #define BTN_OPEN     0
#define startButton_pin             21  // INPUT
#define ONE_WIRE_BUS 4


#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "lnLogger_Class.h"   // la tua classe logger

ESP32Logger logger;

// ====== PIN ======
#define RELAY_OPEN   16
#define RELAY_CLOSE  17
#define BUZZER       23

#define BTN_OPEN     21
#define BTN_CLOSE    19

#define ONE_WIRE_BUS 4

// ====== COSTANTI ======
const unsigned long VALVE_TIME = 15000;       // 15 s
const unsigned long AUTO_CLOSE_TIME = 1800000; // 30 min
const unsigned long ANTI_FLOOD_TIME = 7200000; // 2 ore sicurezza

// ====== TEMPERATURA ======
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// ====== STATO ======
bool valveOpen = false;
unsigned long openTime = 0;

// ====== BUZZER ======
void beep(int count)
{
    LOG_DEBUG("Buzzer beep x%d", count);

    for (int i = 0; i < count; i++) {
        digitalWrite(BUZZER, HIGH);
        delay(150);
        digitalWrite(BUZZER, LOW);
        delay(200);
    }
}

// ====== APERTURA ======
void openValve()
{
    if (valveOpen) {
        LOG_WARN("Apertura richiesta ma valvola già aperta");
        return;
    }

    LOG_INFO("APERTURA valvola");

    digitalWrite(RELAY_OPEN, HIGH);
    delay(VALVE_TIME);
    digitalWrite(RELAY_OPEN, LOW);

    valveOpen = true;
    openTime = millis();

    beep(2);

    LOG_INFO("Valvola aperta (alimentazione rimossa)");
}

// ====== CHIUSURA ======
void closeValve()
{
    if (!valveOpen) {
        LOG_WARN("Chiusura richiesta ma valvola già chiusa");
        return;
    }

    LOG_INFO("CHIUSURA valvola");

    digitalWrite(RELAY_CLOSE, HIGH);
    delay(VALVE_TIME);
    digitalWrite(RELAY_CLOSE, LOW);

    valveOpen = false;

    beep(4);

    LOG_INFO("Valvola chiusa");
}

// ====== SETUP ======
void setup()
{
    Serial.begin(115200);

    LOG_INFO("Sistema irrigazione avviato");

    pinMode(RELAY_OPEN, OUTPUT);
    pinMode(RELAY_CLOSE, OUTPUT);
    pinMode(BUZZER, OUTPUT);

    pinMode(BTN_OPEN, INPUT_PULLUP);
    pinMode(BTN_CLOSE, INPUT_PULLUP);

    digitalWrite(RELAY_OPEN, LOW);
    digitalWrite(RELAY_CLOSE, LOW);

    sensors.begin();

    LOG_INFO("Sensore temperatura inizializzato");
}

// ====== LOOP ======
void loop() {
    // ---- Pulsanti ----
    if (digitalRead(BTN_OPEN) == LOW) {
        LOG_INFO("Pulsante apertura premuto");
        openValve();
        delay(500);
    }

    if (digitalRead(BTN_CLOSE) == LOW) {
        LOG_INFO("Pulsante chiusura premuto");
        closeValve();
        delay(500);
    }

    // ---- Auto-chiusura ----
    if (valveOpen && millis() - openTime > AUTO_CLOSE_TIME) {
        LOG_WARN("Auto-chiusura per timeout 30 min");
        closeValve();
    }

    // ---- Anti-allagamento ----
    if (valveOpen && millis() - openTime > ANTI_FLOOD_TIME) {
        LOG_ERROR("ANTI-ALLAGAMENTO ATTIVATO!");
        closeValve();
    }

    // ---- Temperatura ogni 30 s ----
    static unsigned long lastTemp = 0;
    if (millis() - lastTemp > 30000) {
        sensors.requestTemperatures();
        float t = sensors.getTempCByIndex(0);

        LOG_INFO("Temperatura esterna: %.1f C", t);

        lastTemp = millis();
    }

    delay(50);
}


#if 0
// ===== TEMPI =====
const unsigned long MOVE_TIME = 15000;
const unsigned long AUTO_CLOSE_TIME = 1800000;
const unsigned long MAX_IRRIGATION_TIME = 2400000; // 40 min
unsigned long irrigationStart = 0;


// ===== DS18B20 =====
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// ===== TELEGRAM =====
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// ===== SERVER & ALEXA =====
WebServer server(80);
Espalexa espalexa;

enum State {IDLE, OPENING, CLOSING, OPENED};
State state = IDLE;

unsigned long moveStart;
unsigned long autoCloseStart;

// ===== FUNZIONI =====

void stopRelays() {
  digitalWrite(RELAY_OPEN, LOW);
  digitalWrite(RELAY_CLOSE, LOW);
}

void beep(int n) {
  for(int i=0;i<n;i++) {
    digitalWrite(activeBuzzer_pin, HIGH);
    delay(120);
    digitalWrite(activeBuzzer_pin, LOW);
    delay(120);
  }
}

void openValve() {
    LOG_INFO("Comando APERTURA valvola");
    if(state==OPENING) return;
    stopRelays();
    digitalWrite(RELAY_OPEN, HIGH);
    state = OPENING;
    moveStart = millis();
    bot.sendMessage(CHAT_ID,"Valvola APERTURA","");
    prefs.putBool("opened", true);
    LOG_INFO("Valvola aperta (alimentazione rimossa)");
}


void closeValve() {
    LOG_INFO("Comando CHIUSURA valvola");
    if(state==CLOSING) return;
    stopRelays();
    digitalWrite(RELAY_CLOSE, HIGH);
    state = CLOSING;
    moveStart = millis();
    bot.sendMessage(CHAT_ID,"Valvola CHIUSURA","");
    prefs.putBool("opened", false);
    LOG_INFO("Valvola chiusa (alimentazione rimossa)");
}


// ===== ALEXA =====
void alexaCallback(uint8_t brightness) {
  if(brightness) openValve();
  else closeValve();
}

// ===== WEB =====
void handleRoot() {
    sensors.requestTemperatures();
    float t = sensors.getTempCByIndex(0);
    LOG_INFO("Temperatura esterna: %.1f C", t);

    String page =
      "<h1>Irrigazione</h1>"
      "Temp: " + String(t) + " C<br>"
      "<a href='/open'>APRI</a><br>"
      "<a href='/close'>CHIUDI</a>";

    server.send(200,"text/html",page);
}

// ===== TELEGRAM COMMANDS =====
void checkTelegram() {
  int num = bot.getUpdates(bot.last_message_received + 1);

  for(int i=0;i<num;i++) {
    String text = bot.messages[i].text;

    if(text=="/apri") openValve();
    if(text=="/chiudi") closeValve();

    if(text=="/temp") {
      sensors.requestTemperatures();
      float t = sensors.getTempCByIndex(0);
      bot.sendMessage(CHAT_ID,"Temperatura: "+String(t)+" C","");
    }

    if(text=="/stato") {
      bot.sendMessage(CHAT_ID,"Stato valvola: "+String(state),"");
    }
  }
}

void alarmFlood() {
  for(int r=0;r<3;r++) {
    for(int i=0;i<5;i++) {
      digitalWrite(activeBuzzer_pin, HIGH);
      delay(300);
      digitalWrite(activeBuzzer_pin, LOW);
      delay(300);
    }
    delay(1000);
  }

  bot.sendMessage(CHAT_ID,
    "⚠️ ALLARME ANTIALlagamento\nValvola chiusa automaticamente","");
}


void setup() {
  LOG_INFO("Sistema irrigazione avviato");

  pinMode(RELAY_OPEN, OUTPUT);
  pinMode(RELAY_CLOSE, OUTPUT);
  pinMode(activeBuzzer_pin, OUTPUT);
  pinMode(startButton_pin, INPUT_PULLUP);

  stopRelays();

  prefs.begin("valve", false);
  bool wasOpen = prefs.getBool("opened", false);
  if(wasOpen) {
    // se era aperta prima del blackout → chiudi subito
    closeValve();
  }

  WiFi.begin(ssid,password);
  while(WiFi.status()!=WL_CONNECTED) delay(500);

  client.setInsecure();

  sensors.begin();

  server.on("/",handleRoot);
  server.on("/open",[]() {openValve(); server.send(200,"text/plain","Opening");});
  server.on("/close",[]() {closeValve(); server.send(200,"text/plain","Closing");});
  server.begin();

  espalexa.addDevice("Irrigazione", alexaCallback);
  espalexa.begin();



}

void loop() {
    server.handleClient();
    espalexa.loop();
    checkTelegram();

    if (digitalRead(BTN_OPEN) == LOW) {
        LOG_INFO("Pulsante apertura premuto");
        openValve();
    }
    if (digitalRead(BTN_CLOSE) == LOW) {
        LOG_INFO("Pulsante chiusura premuto");
        closeValve();
    }

  // movimento
    if (state==OPENING && millis()-moveStart>=MOVE_TIME) {
        stopRelays();
        beep(2);
        state = OPENED;
        autoCloseStart = millis();
        if(state==OPENED) {
            irrigationStart = millis();
        }

    }

  if(state==CLOSING && millis()-moveStart>=MOVE_TIME) {
    stopRelays();
    beep(4);
    state = IDLE;
  }

  // auto chiusura
  if(state==OPENED && millis()-autoCloseStart>=AUTO_CLOSE_TIME) {
    LOG_WARN("Auto-chiusura per timeout");
    closeValve();
  }
  // ===== ANTIALlagamento =====
  if(state==OPENED && millis()-irrigationStart >= MAX_IRRIGATION_TIME) {
    LOG_WARN("Auto-chiusura per timeout");
    closeValve();
    alarmFlood();
  }

}
#endif