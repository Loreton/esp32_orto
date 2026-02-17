//
// updated by ...: Loreto Notarantonio
// Date .........: 16-02-2026 16.32.37
//


// >>> elettrovalvola.cpp
#include <Arduino.h>
#include "config.h"


const unsigned long MOVING_TIME = 15000;      // 15 secondi
void suonaBuzzer(int beep) {
    for(int i=0; i<beep; i++){
        digitalWrite(BUZZER, HIGH); delay(100);
        digitalWrite(BUZZER, LOW);  delay(100);
    }
}

void gestisciValvola(bool apri) {
    if (isMoving) return;

    isMoving = true;
    Serial.println(apri ? ">>> Comando: APERTURA" : ">>> Comando: CHIUSURA");

    digitalWrite(RELAY_DIR, apri ? HIGH : LOW);
    delay(200);

    suonaBuzzer(apri ? 2 : 4);

    digitalWrite(RELAY_PWR, HIGH);
    delay(MOVING_TIME);
    digitalWrite(RELAY_PWR, LOW);

    isOpen = apri;
    if(isOpen) lastOpenMillis = millis();
    isMoving = false;
    Serial.println(">>> Movimento terminato.");
}