//
// updated by ...: Loreto Notarantonio
// Date .........: 16-02-2026 16.32.37
//


// >>> elettrovalvola.cpp
#include <Arduino.h>
#include "config.h"

char ev_buf[50];
void startValvola(bool apri, int minuti) {
    if (EV_isMoving) return;

    // Feedback sonoro immediato
    digitalWrite(BUZZER, HIGH); delay(100); digitalWrite(BUZZER, LOW);

    // LOG_INFO("Inizio movimento: %s", apri ? "APERTURA" : "CHIUSURA");
    snprintf(ev_buf, sizeof(ev_buf), "Inizio movimento: %s", apri ? "APERTURA" : "CHIUSURA");
    LOG_INFO(ev_buf);
    bot.sendTo(lastAdminChatId, ev_buf);

    EV_isMoving = true;
    moveStartTime = millis();

    if (apri) {
        currentAutoCloseDuration = (unsigned long)minuti * 60000;
        EV_isOpen = true;
    } else {
        EV_isOpen = false;
    }

    // Questa sequenza è **ottima**: il ritardo di 150ms assicura che il relè della direzione
    // sia già commutato prima di dare potenza, evitando archi elettrici o inversioni brusche.
    digitalWrite(RELAY_DIR, apri ? HIGH : LOW);
    delay(150);
    digitalWrite(RELAY_PWR, HIGH);
}

void checkValvolaTimer() {
    if (EV_isMoving && (millis() - moveStartTime >= MOVING_TIME)) {
        digitalWrite(RELAY_PWR, LOW);
        EV_isMoving = false;
        if (EV_isOpen) lastOpenMillis = millis();

        snprintf(ev_buf, sizeof(ev_buf), "Fine movimento: %s", EV_isOpen ? "APERTURA" : "CHIUSURA");
        LOG_INFO(ev_buf);
        bot.sendTo(lastAdminChatId, ev_buf);
    }
}