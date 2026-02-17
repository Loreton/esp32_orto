//
// updated by ...: Loreto Notarantonio
// Date .........: 16-02-2026 16.32.37
//


// >>> web.cpp
#include "config.h"

void handleRoot() {
    sensors.requestTemperatures();
    float temp = sensors.getTempCByIndex(0);

    String html = "<html><head><title>Orto Control</title><meta name='viewport' content='width=device-width, initial-scale=1' charset='utf-8'>";
    html += "<style>body{font-family:sans-serif; text-align:center;} .status-box{padding:20px; margin:20px; border:2px solid #ccc;}</style></head><body>";
    html += "<h1>Irrigazione Orto</h1>";

    html += "<div class='status-box'>";
    html += "Temp: <b>" + String(temp) + " °C</b><br>";
    if(isMoving) html += "Stato: <b style='color:orange;'>IN MOVIMENTO...</b>";
    else html += "Stato: <b>" + String(isOpen ? "APERTA" : "CHIUSA") + "</b>";
    html += "</div>";

    if(!isMoving) {
        html += "<form action='/apri' method='GET'>";
        html += "Minuti: <input type='number' name='t' value='30' min='1' max='60' style='width:50px;'> ";
        html += "<input type='submit' value='APRI VALVOLA' style='background:green; color:white; padding:10px;'>";
        html += "</form>";

        html += "<br><a href='/chiudi'><button style='background:red; color:white; padding:10px;'>CHIUDI ORA</button></a>";
    }

    html += "</body></html>";
    server.send(200, "text/html", html);
}