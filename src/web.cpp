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

void handleRoot_2() {
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



void handleRoot() {
    String html = "<html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<title>Orto Control</title>";
    html += "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>";
    html += "<style>";
    html += "body{font-family:sans-serif; text-align:center; background:#f4f4f4; padding:10px;}";
    html += ".status-box{padding:20px; border-radius:10px; margin:10px auto; width:90%; max-width:400px; color:white; font-weight:bold;}";
    html += ".open{background-color:#2ecc71;} .closed{background-color:#e74c3c;} .moving{background-color:#f1c40f; color:black;}";
    html += ".btn{padding:15px; font-size:16px; margin:5px; cursor:pointer; border:none; border-radius:5px; background:#3498db; color:white; text-decoration:none; display:inline-block;}";
    html += ".chart-container{position:relative; margin:10px auto; height:250px; width:100%; max-width:600px; background:white; padding:5px; border-radius:10px;}";
    html += "</style></head><body>";

    html += "<h1>Orto Control</h1>";

    if (EV_isMoving) html += "<div class='status-box moving'>IN MOVIMENTO...</div>";
    else html += EV_isOpen ? "<div class='status-box open'>APERTA</div>" : "<div class='status-box closed'>CHIUSA</div>";

    if (sensorFault) html += "<h2 style='color:red;'>⚠️ ERRORE SENSORE</h2>";
    else html += "<h2>" + String(currentTemperature, 1) + " °C</h2>";

    html += "<div class='chart-container'><canvas id='tChart'></canvas></div>";
    html += "<script>const ctx=document.getElementById('tChart').getContext('2d');";
    html += "new Chart(ctx,{type:'line',data:{labels:['24h','20h','16h','12h','8h','4h','Ora'],";
    html += "datasets:[{label:'Temp', data:[";
    for(int i=0; i<24; i++) html += String(tempHistory[i]) + (i==23?"":",");
    html += "],borderColor:'#1abc9c',fill:true,tension:0.3}]";
    html += "},options:{maintainAspectRatio:false}});</script>";

    if (!EV_isMoving) {
        html += "<div><a href='/apri?t=30' class='btn'>APRI 30m</a>";
        html += "<a href='/chiudi' class='btn'>CHIUDI</a></div>";
    }

    html += "</body></html>";
    server.send(200, "text/html", html);
}