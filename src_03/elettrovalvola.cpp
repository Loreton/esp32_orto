//
// updated by ...: Loreto Notarantonio
// Date .........: 16-02-2026 16.32.37
//


// >>> elettrovalvola.cpp
#include <Arduino.h>
#include "config.h"

void startValvola(bool apri, int minuti) {
    if (isMoving) return;

    isMoving = true;
    moveStartTime = millis();

    // Calcola il tempo di auto-chiusura in base ai minuti ricevuti
    if(apri) {
        currentAutoCloseDuration = (unsigned long)minuti * 60000;
        Serial.printf(">>> Apertura per %d minuti\n", minuti);
    }

    digitalWrite(RELAY_DIR, apri ? HIGH : LOW);
    delay(100);

    // Buzzer
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