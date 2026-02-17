//
// updated by ...: Loreto Notarantonio
// Date .........: 16-02-2026 16.32.37
//


// >>> elettrovalvola.cpp
#include <Arduino.h>
#include "config.h"

void startValvola(bool apri, int minuti) {
    if (isMoving) return;

    // Feedback sonoro immediato
    digitalWrite(BUZZER, HIGH); delay(100); digitalWrite(BUZZER, LOW);

    LOG_INFO("Inizio movimento: %s", apri ? "APERTURA" : "CHIUSURA");

    isMoving = true;
    moveStartTime = millis();

    if (apri) {
        currentAutoCloseDuration = (unsigned long)minuti * 60000;
        isOpen = true;
    } else {
        isOpen = false;
    }

    digitalWrite(RELAY_DIR, apri ? HIGH : LOW);
    delay(150);
    digitalWrite(RELAY_PWR, HIGH);
}

void checkValvolaTimer() {
    if (isMoving && (millis() - moveStartTime >= MOVING_TIME)) {
        digitalWrite(RELAY_PWR, LOW);
        isMoving = false;
        if (isOpen) lastOpenMillis = millis();
        LOG_DEBUG("Fine movimento valvola.");
    }
}