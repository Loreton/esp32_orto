//
// updated by ...: Loreto Notarantonio
// Date .........: 16-02-2026 16.32.37
//




// >>> telegram.cpp
#include "config.h"



void manageTelegram() {
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

        if (isMoving) {
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
                String s = "Stato: " + String(isOpen ? "APERTA" : "CHIUSA") + "\n";
                s += "Temp: " + String(sensors.getTempCByIndex(0)) + " °C";
                bot.sendMessage(msg, s);
            }
        }
    }
}
