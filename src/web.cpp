//
// updated by ...: Loreto Notarantonio
// Date .........: 16-02-2026 16.32.37
//


// >>> web.cpp
#include <Arduino.h>
#include "config.h"

void handleRoot_prev() {
    sensors.requestTemperatures();
    float t = sensors.getTempCByIndex(0);

    String html = "<html><head><meta charset='utf-8'><title>Orto Control</title>";
    html += "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>";
    html += "<style>body{font-family:sans-serif; text-align:center; padding:20px;} ";
    html += ".btn{padding:15px 30px; font-size:18px; margin:10px; cursor:pointer;} ";
    html += ".chart-container{width:90%; max-width:600px; margin:auto;}</style></head><body>";

    html += "<h1>Controllo Irrigazione</h1>";

    if (sensorFault) {
        html += "<h2 style='color:red;'>ERRORE SENSORE</h2>";
    } else {
        html += "<h2>Temperatura: " + String(t) + " °C</h2>";
    }

    html += "<div class='chart-container'><canvas id='tChart'></canvas></div>";

    html += "<script>const ctx=document.getElementById('tChart').getContext('2d'); ";
    html += "new Chart(ctx,{type:'line',data:{labels:[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24],";
    html += "datasets:[{label:'Temp ultime 24h', data:[";
    for(int i=0; i<24; i++) html += String(tempHistory[i]) + (i==23?"":",");
    html += "],borderColor:'teal',fill:true}]}});</script>";

    if (EV_isMoving) {
        html += "<p style='color:orange;'>⚠️ MOVIMENTO IN CORSO...</p>";
    } else {
        html += "<div><a href='/apri?t=30'><button class='btn'>APRI (30m)</button></a>";
        html += "<a href='/chiudi'><button class='btn'>CHIUDI</button></a></div>";
    }

    html += "</body></html>";
    server.send(200, "text/html", html);
}

void handleRoot() {
    sensors.requestTemperatures();
    float t = sensors.getTempCByIndex(0);

    String html = "<html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<title>Orto Control</title>";
    // ... i tuoi stili ...
    html += "<style>.status-box{padding:20px; border-radius:10px; margin:10px; color:white; font-weight:bold;} ";
    html += ".open{background-color:#2ecc71;} .closed{background-color:#e74c3c;} .moving{background-color:#f1c40f; color:black;}</style>";
    html += "</head><body>";

    html += "<h1>Controllo Irrigazione</h1>";

    // Mostra lo STATO attuale
    if (EV_isMoving) {
        html += "<div class='status-box moving'>MOVIMENTO IN CORSO...</div>";
    } else if (EV_isOpen) {
        html += "<div class='status-box open'>VALVOLA APERTA</div>";
    } else {
        html += "<div class='status-box closed'>VALVOLA CHIUSA</div>";
    }

    if (sensorFault) {
        html += "<h2 style='color:red;'>⚠️ ERRORE SENSORE (Controllare Pull-up 4.7k)</h2>";
    } else {
        html += "<h2>Temperatura: " + String(t, 1) + " °C</h2>";
    }

    // Grafico
    html += "<div class='chart-container'><canvas id='tChart'></canvas></div>";

    html += "<script>const ctx=document.getElementById('tChart').getContext('2d'); ";
    html += "new Chart(ctx,{type:'line',data:{labels:[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24],";
    html += "datasets:[{label:'Temp ultime 24h', data:[";
    for(int i=0; i<24; i++) html += String(tempHistory[i]) + (i==23?"":",");
    html += "],borderColor:'teal',fill:true}]}});</script>";

    if (!EV_isMoving) {
        html += "<div><a href='/apri?t=30'><button class='btn'>APRI (30m)</button></a>";
        html += "<a href='/chiudi'><button class='btn'>CHIUDI</button></a></div>";
    }

    html += "</body></html>";
    server.send(200, "text/html", html);
}