//
// updated by ...: Loreto Notarantonio
// Date .........: 22-11-2025 17.04.08
//




// #include <Arduino.h>    // in testa anche per le definizioni dei type
// #include <stdint.h>    // per gli uintxx_t


#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Espalexa.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Preferences.h>
Preferences prefs;

// #include <OneWire.h>
// #include <DallasTemperature.h>
#include "lnLogger_Class.h"   // classe logger - definisce internamente ESP32Logger lnLog

// #include    "pin_Definitions.h"

// ---------------------------------
// - project headers files
// ---------------------------------
#define __I_AM_MAIN_CPP__
#define  LOG_MODULE_LEVEL LOG_LEVEL_DEBUG
#include "main.h"



// ===== WIFI =====
const char* ssid = "SSID";
const char* password = "PASSWORD";

// ===== TELEGRAM =====
#define BOT_TOKEN "TOKEN"
#define CHAT_ID "CHATID"


// ====== PIN ======
#define EV_RELAY_OPEN_pin   16
#define EV_RELAY_CLOSE_pin  17
#define ACTIVE_BUZZER_pin       23

#define BTN_OPEN     21
#define BTN_CLOSE    19

#define ONE_WIRE_BUS 4


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
  digitalWrite(EV_RELAY_OPEN_pin, LOW);
  digitalWrite(EV_RELAY_CLOSE_pin, LOW);
}

void beep(int n) {
  for(int i=0;i<n;i++) {
    digitalWrite(ACTIVE_BUZZER_pin, HIGH);
    delay(120);
    digitalWrite(ACTIVE_BUZZER_pin, LOW);
    delay(120);
  }
}

void openValve() {
    LOG_INFO("Comando APERTURA valvola");
    if(state==OPENING) return;
    stopRelays();
    digitalWrite(EV_RELAY_OPEN_pin, HIGH);
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
    digitalWrite(EV_RELAY_CLOSE_pin, HIGH);
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
      digitalWrite(ACTIVE_BUZZER_pin, HIGH);
      delay(300);
      digitalWrite(ACTIVE_BUZZER_pin, LOW);
      delay(300);
    }
    delay(1000);
  }

  bot.sendMessage(CHAT_ID,
    "⚠️ ALLARME ANTIALlagamento\nValvola chiusa automaticamente","");
}


void setup() {
    Serial.begin(115200);
    delay(2000);
    lnLog.init();

  LOG_INFO("Sistema irrigazione avviato");

  pinMode(EV_RELAY_OPEN_pin, OUTPUT);
  pinMode(EV_RELAY_CLOSE_pin, OUTPUT);
  pinMode(ACTIVE_BUZZER_pin, OUTPUT);
  pinMode(BTN_OPEN, INPUT_PULLUP);

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
