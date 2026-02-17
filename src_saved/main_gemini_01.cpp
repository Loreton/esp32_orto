//
// updated by ...: Loreto Notarantonio
// Date .........: 16-02-2026 16.32.37
//


#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Espalexa.h>
#include <UniversalTelegramBot.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <lnEsp32Orto_Bot.h>
#include <ssid_casetta.h>

// --- CREDENZIALI ---
const char* ssid = casettaSSID;
const char* password = casettaPassword;

#define BOTtoken lnEsp32Orto // Il tuo Token Telegram

// --- PIN CONFIG (ESP32_Relay_AC_X2) ---
#define RELAY_PWR 16
#define RELAY_DIR 17
#define BUZZER    23
#define BTN_APRI  21
#define BTN_CHIU  19
#define SENSOR_PIN 4 // Pin Shelly DS18B20

// ... dopo le definizioni dei pin

// --- COSTANTI ---
const unsigned long MOVING_TIME = 15000;
const unsigned long AUTO_CLOSE_TIME = 1800000; // 30 min

// --- STATO ---
bool isMoving = false;
bool isOpen = false;
unsigned long lastOpenMillis = 0;
unsigned long lastBotCheck = 0;
const unsigned int botInterval = 2000; // Controlla Telegram ogni 2 sec

Espalexa espalexa;
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
OneWire oneWire(SENSOR_PIN);
DallasTemperature sensors(&oneWire); // Corretto: punta al bus OneWire

// --- FUNZIONI CORE ---

void suonaBuzzer(int beep) {
  for(int i=0; i<beep; i++){
    digitalWrite(BUZZER, HIGH); delay(100);
    digitalWrite(BUZZER, LOW);  delay(100);
  }
}

void gestisciValvola(bool apri) {
  if (isMoving) return; // Protezione attiva

  isMoving = true;
  Serial.println(apri ? "Apertura..." : "Chiusura...");

  // 1. Imposta direzione
  digitalWrite(RELAY_DIR, apri ? HIGH : LOW);
  delay(200); // Sicurezza elettronica

  // 2. Feedback sonoro
  suonaBuzzer(apri ? 2 : 4);

  // 3. Attiva potenza
  digitalWrite(RELAY_PWR, HIGH);

  // Nota: usiamo un delay qui perché durante il movimento
  // la valvola DEVE essere l'unica priorità per sicurezza 220V
  delay(MOVING_TIME);

  digitalWrite(RELAY_PWR, LOW);

  isOpen = apri;
  if(isOpen) lastOpenMillis = millis();
  isMoving = false;
  Serial.println("Movimento completato.");
}

// Callback Alexa
void alexaAction(uint8_t bright) {
  if (bright > 0) gestisciValvola(true);
  else gestisciValvola(false);
}

void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String text = bot.messages[i].text;
    if (text == "/apri") {
      gestisciValvola(true);
      bot.sendMessage(bot.messages[i].chat_id, "Valvola Aperta", "");
    } else if (text == "/chiudi") {
      gestisciValvola(false);
      bot.sendMessage(bot.messages[i].chat_id, "Valvola Chiusa", "");
    } else if (text == "/stato") {
      sensors.requestTemperatures();
      String msg = "Stato: " + String(isOpen ? "APERTA" : "CHIUSA") + "\n";
      msg += "Temp: " + String(sensors.getTempCByIndex(0)) + "C";
      bot.sendMessage(bot.messages[i].chat_id, msg, "");
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PWR, OUTPUT); pinMode(RELAY_DIR, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BTN_APRI, INPUT_PULLUP); pinMode(BTN_CHIU, INPUT_PULLUP);

  digitalWrite(RELAY_PWR, LOW); // Sicurezza iniziale

  WiFi.begin(ssid, password);
  client.setInsecure(); // Per evitare crash certificati SSL con Telegram

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connesso!");

  espalexa.addDevice("Valvola Orto", alexaAction);
  espalexa.begin();
  sensors.begin();
}

void loop() {
  espalexa.loop();

  // Pulsanti Fisici (Bloccati se isMoving è true)
  if (!isMoving) {
    if (digitalRead(BTN_APRI) == LOW) { delay(50); gestisciValvola(true); }
    if (digitalRead(BTN_CHIU) == LOW) { delay(50); gestisciValvola(false); }
  }

  // Auto-chiusura
  if (isOpen && !isMoving && (millis() - lastOpenMillis > AUTO_CLOSE_TIME)) {
    gestisciValvola(false);
  }

  // Telegram - Solo se connesso e non sta muovendo
  if (millis() - lastBotCheck > botInterval && !isMoving) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    handleNewMessages(numNewMessages);
    lastBotCheck = millis();
  }
}