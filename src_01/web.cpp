//
// updated by ...: Loreto Notarantonio
// Date .........: 16-02-2026 16.32.37
//

// >>> web.cpp

#include <Arduino.h>
#include <WebServer.h>
#include "config.h"



// --- HANDLER WEB ---
void handleRoot() {
    sensors.requestTemperatures();
    float temp = sensors.getTempCByIndex(0);

    String html = "<html><head><title>Orto Control</title><meta charset='utf-8'></head><body>";
    html += "<h1>Sistema Irrigazione</h1>";
    html += "<p>Stato: <b>" + String(isOpen ? "APERTA" : "CHIUSA") + "</b></p>";
    html += "<p>Temperatura: <b>" + String(temp) + " &deg;C</b></p>";
    html += "<hr><a href='/apri'><button>APRI</button></a>";
    html += " <a href='/chiudi'><button>CHIUDI</button></a>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}
