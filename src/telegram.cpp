//
// updated by ...: Loreto Notarantonio
// Date .........: 16-02-2026 16.32.37
//




// >>> telegram.cpp
#include "config.h"

// Buffer generico per comporre i messaggi (abbastanza grande per i testi previsti)
char tg_buf[256];
size_t BUFFER_LEN=sizeof(tg_buf);

void manageTelegram() {
    TBMessage msg;
    if (bot.getNewMessage(msg)) {

        // 1. Visualizza sempre il ChatID nel LOG
        LOG_INFO("Messaggio ricevuto da ChatID: %lld", (long long)msg.chatId);

        // 2. Controllo se il ChatID è diverso dall'ultimo salvato
        if (msg.chatId != lastAdminChatId) {
            LOG_NOTIFY("Nuovo utente rilevato! Precedente: %lld, Attuale: %lld", (long long)lastAdminChatId, (long long)msg.chatId);

            // Notifica l'utente del suo ID tramite snprintf
            snprintf(tg_buf, BUFFER_LEN,
                     "Ciao! Il tuo ChatID è: %lld\nOra sono sincronizzato con te per le notifiche automatiche.",
                     (long long)msg.chatId);

            bot.sendMessage(msg, tg_buf);

            // Aggiorna la variabile globale
            lastAdminChatId = msg.chatId;
        }

        String text = msg.text; // Il comando in entrata rimane String per comodità di confronto

        // Gestione Reboot
        if (text == "/reboot") {
            LOG_WARN("Reboot richiesto da Telegram!");
            bot.sendMessage(msg, "🔄 Riavvio in corso...");
            delay(2000);
            ESP.restart();
        }

        // Controllo stato movimento
        if (EV_isMoving) {
            bot.sendMessage(msg, "⚠️ Operazione in corso... Attendi la fine del movimento.");
            LOG_DEBUG("Comando ignorato: valvola in movimento.");
        } else {
            if (text.startsWith("/apri")) {
                int min = 30;
                if (text.indexOf(' ') != -1) {
                    min = text.substring(text.indexOf(' ') + 1).toInt();
                }
                startValvola(true, min);

                snprintf(tg_buf, BUFFER_LEN, "✅ Apertura avviata (%d min)", min);
                bot.sendMessage(msg, tg_buf);

            } else if (text == "/chiudi") {
                startValvola(false);
                bot.sendMessage(msg, "✅ Chiusura avviata.");

            } else if (text == "/status") {
                sensors.requestTemperatures();
                float temp = sensors.getTempCByIndex(0);

                // Composizione messaggio di stato con snprintf
                // %s per stringhe, %.1f per float con 1 decimale, %lld per long long
                snprintf(tg_buf, BUFFER_LEN,
                         "Stato: %s\nTemp: %.1f °C\nIl tuo ID: %lld",
                         EV_isOpen ? "APERTA" : "CHIUSA",
                         temp,
                         (long long)msg.chatId);

                bot.sendMessage(msg, tg_buf);
            }
        }
    }
}

#if 0
void manageTelegram__() {
    TBMessage msg;
    if (bot.getNewMessage(msg)) {
        // 1. Visualizza sempre il ChatID nel LOG (usiamo %lld per int64_t)
        LOG_INFO("Messaggio ricevuto da ChatID: %lld", msg.chatId);

        // 2. Controllo se il ChatID è diverso dall'ultimo salvato
        if (msg.chatId != lastAdminChatId) {
            LOG_NOTIFY("Nuovo utente rilevato! Precedente: %lld, Attuale: %lld", lastAdminChatId, msg.chatId);

            // Notifica l'utente del suo ID
            String welcomeMsg = "Ciao! Il tuo ChatID è: " + String((int64_t)msg.chatId);
            welcomeMsg += "\nOra sono sincronizzato con te per le notifiche automatiche.";
            bot.sendMessage(msg, welcomeMsg);

            // Aggiorna la variabile globale
            lastAdminChatId = msg.chatId;
        }

        String text = msg.text;

        // Gestione Reboot
        if (text == "/reboot") {
            LOG_WARN("Reboot richiesto da Telegram!");
            bot.sendMessage(msg, "🔄 Riavvio in corso...");
            delay(2000);
            ESP.restart();
        }

        // Controllo stato movimento
        if (EV_isMoving) {
            bot.sendMessage(msg, "⚠️ Operazione in corso... Attendi la fine del movimento.");
            LOG_DEBUG("Comando ignorato: valvola in movimento.");
        }
        else {
            if (text.startsWith("/apri")) {
                int min = 30;
                if (text.indexOf(' ') != -1) min = text.substring(text.indexOf(' ') + 1).toInt();
                startValvola(true, min);
                bot.sendMessage(msg, "✅ Apertura avviata (" + String(min) + " min)");
            }
            else if (text == "/chiudi") {
                startValvola(false);
                bot.sendMessage(msg, "✅ Chiusura avviata.");
            }
            else if (text == "/status") {
                sensors.requestTemperatures();
                String s = "Stato: " + String(EV_isOpen ? "APERTA" : "CHIUSA") + "\n";
                s += "Temp: " + String(sensors.getTempCByIndex(0)) + " °C\n";
                s += "Il tuo ID: " + String((int64_t)msg.chatId);
                bot.sendMessage(msg, s);
            }
        }
    }
}




void manageTelegram_() {
    // 2. GESTIONE TELEGRAM
    TBMessage msg;
    if (bot.getNewMessage(msg)) {
        lastAdminChatId = msg.chatId;
        String text = msg.text;

        if (text == "/reboot") {
            LOG_WARN("Reboot richiesto da Telegram!");
            bot.sendMessage(msg, "🔄 Riavvio in corso...");
            delay(2000);
            ESP.restart();
        }

        if (EV_isMoving) {
            bot.sendMessage(msg, "⚠️ Operazione in corso... Attendi la fine del movimento.");
            LOG_DEBUG("Comando ignorato: valvola in movimento.");
        }
        else {
            if (text.startsWith("/apri")) {
                int min = 30;
                if (text.indexOf(' ') != -1) min = text.substring(text.indexOf(' ') + 1).toInt();
                startValvola(true, min);
                bot.sendMessage(msg, "✅ Apertura avviata (" + String(min) + " min)");
            }
            else if (text == "/chiudi") {
                startValvola(false);
                bot.sendMessage(msg, "✅ Chiusura avviata.");
            }
            else if (text == "/status") {
                sensors.requestTemperatures();
                String s = "Stato: " + String(EV_isOpen ? "APERTA" : "CHIUSA") + "\n";
                s += "Temp: " + String(sensors.getTempCByIndex(0)) + " °C";
                bot.sendMessage(msg, s);
            }
        }
    }
}
#endif