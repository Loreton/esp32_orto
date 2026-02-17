//
// updated by ...: Loreto Notarantonio
// Date .........: 16-02-2026 16.32.37
//


// >>> elettrovalvola.cpp
#include <Arduino.h>
#include "config.h"


void suonaBuzzer(int beep) {
    for(int i=0; i<beep; i++){
        digitalWrite(BUZZER, HIGH); delay(100);
        digitalWrite(BUZZER, LOW);  delay(100);
    }
}

void startValvola(bool apri) {
    if (isMoving) return; // Se sta già girando, ignora tutto

    isMoving = true;
    moveStartTime = millis();

    digitalWrite(RELAY_DIR, apri ? HIGH : LOW);
    delay(100);

    // Buzzer (breve, non disturba)
    for(int i=0; i<(apri?2:4); i++){
        digitalWrite(BUZZER, HIGH); delay(80);
        digitalWrite(BUZZER, LOW);  delay(80);
    }

    digitalWrite(RELAY_PWR, HIGH);
    isOpen = apri;
}

void checkValvolaTimer() {
    if (isMoving && (millis() - moveStartTime >= MOVING_TIME)) {
        digitalWrite(RELAY_PWR, LOW);
        isMoving = false;
        if(isOpen) lastOpenMillis = millis();
        Serial.println(">>> Valvola ferma, alimentazione rimossa.");
    }
}