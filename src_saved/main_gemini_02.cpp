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


// --- PIN CONFIG ---
#define RELAY_PWR 16
#define RELAY_DIR 17
#define BUZZER    23
#define BTN_APRI  21
#define BTN_CHIU  19
#define SENSOR_PIN 4

// --- VARIABILI DI STATO ---
bool isMoving = false;
bool isOpen = false;
float lastTemp = 0.0;
unsigned long lastOpenMillis = 0;
unsigned long lastBotCheck = 0;

WebServer server(80);
Espalexa espalexa;
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
OneWire oneWire(SENSOR_PIN);
DallasTemperature sensors(&oneWire);

// --- LOGICA VALVOLA ---
void gestisciValvola(bool apri) {
  if (isMoving) return;
  isMoving = true;

  digitalWrite(RELAY_DIR, apri ? HIGH : LOW);
  delay(200);

  // Feedback Buzzer
  for(int i=0; i<(apri?2:4); i++){
    digitalWrite(BUZZER, HIGH); delay(100);
    digitalWrite(BUZZER, LOW);  delay(100);
  }

  digitalWrite(RELAY_PWR, HIGH);
  delay(15000); // 15 secondi di movimento
  digitalWrite(RELAY_PWR, LOW);

  isOpen = apri;
  if(isOpen) lastOpenMillis = millis();
  isMoving = false;
}

// --- PAGINA WEB ---
void handleRoot() {
  sensors.requestTemperatures();
  lastTemp = sensors.getTempCByIndex(0);

  String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1' charset='utf-8'>";
  html += "<style>body{font-family:sans-serif; text-align:center; background:#f4f4f4;} ";
  html += ".btn{display:inline-block; padding:15px 30px; margin:10px; font-size:20px; color:white; text-decoration:none; border-radius:5px;} ";
  html += ".open{background:green;} .close{background:red;} .status{font-weight:bold; color:#333;}</style>";
  html += "<title>Controllo Orto</title></head><body>";
  html += "<h1>Irrigazione Orto</h1>";
  html += "<p class='status'>Stato Valvola: " + String(isOpen ? "APERTA" : "CHIUSA") + "</p>";
  html += "<p class='status'>Temperatura: " + String(lastTemp) + " &deg;C</p>";
  html += "<a href='/apri' class='btn open'>APRI</a>";
  html += "<a href='/chiudi' class='btn close'>CHIUDI</a>";
  html += "<script>setTimeout(function(){location.reload();}, 10000);</script>"; // Auto-refresh ogni 10s
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PWR, OUTPUT); pinMode(RELAY_DIR, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BTN_APRI, INPUT_PULLUP); pinMode(BTN_CHIU, INPUT_PULLUP);

  WiFi.begin(ssid, password);
  client.setInsecure();
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }

  // Rotte Web
  server.on("/", handleRoot);
  server.on("/apri", []() { gestisciValvola(true); server.sendHeader("Location", "/"); server.send(303); });
  server.on("/chiudi", []() { gestisciValvola(false); server.sendHeader("Location", "/"); server.send(303); });
  server.begin();

  espalexa.addDevice("Valvola Orto", [](uint8_t b){ if(b>0) gestisciValvola(true); else gestisciValvola(false); });
  espalexa.begin();
  sensors.begin();
}

void loop() {
  server.handleClient();
  espalexa.loop();

  // Pulsanti Fisici
  if (!isMoving) {
    if (digitalRead(BTN_APRI) == LOW) gestisciValvola(true);
    if (digitalRead(BTN_CHIU) == LOW) gestisciValvola(false);
  }

  // Auto-chiusura (30 min)
  if (isOpen && !isMoving && (millis() - lastOpenMillis > 1800000)) gestisciValvola(false);

  // Telegram
  if (millis() - lastBotCheck > 2000 && !isMoving) {
    int num = bot.getUpdates(bot.last_message_received + 1);
    for (int i=0; i<num; i++) {
      String t = bot.messages[i].text;
      if (t == "/apri") gestisciValvola(true);
      else if (t == "/chiudi") gestisciValvola(false);
      else if (t == "/stato") {
         sensors.requestTemperatures();
         bot.sendMessage(bot.messages[i].chat_id, "Stato: " + String(isOpen?"Open":"Closed") + "\nTemp: " + String(sensors.getTempCByIndex(0)) + "C", "");
      }
    }
    lastBotCheck = millis();
  }
}