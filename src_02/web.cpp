//
// updated by ...: Loreto Notarantonio
// Date .........: 16-02-2026 16.32.37
//


// >>> web.cpp
#include "config.h"

void handleRoot() {
    sensors.requestTemperatures();
    float temp = sensors.getTempCByIndex(0);

    String html = "<html><head><title>Orto</title><meta charset='utf-8'></head><body>";
    html += "<h1>Irrigazione</h1>";
    html += "<p>Stato: " + String(isOpen ? "APERTA" : "CHIUSA") + "</p>";
    html += "<p>Temperatura: " + String(temp) + " C</p>";

    // Se la valvola si muove, nascondi i tasti o disabilitali
    if(isMoving) {
        html += "<p style='color:orange;'><b>MOVIMENTO IN CORSO...</b></p>";
    } else {
        html += "<a href='/apri'><button>APRI</button></a> ";
        html += "<a href='/chiudi'><button>CHIUDI</button></a>";
    }
    html += "</body></html>";
    server.send(200, "text/html", html);
}