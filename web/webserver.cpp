/*
  To upload through terminal you can use: curl -F "image=@firmware.bin" esp8266-webupdate.local/update
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* host = "esp8266-webupdate";

ESP8266WebServer server(80);
String webpage = "";

void handle_root() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>");
}

void handle_update_fn() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
}

void handle_update_ufn() {

    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
        Serial.setDebugOutput(true);
        WiFiUDP::stopAll();
        Serial.printf("Update: %s\n", upload.filename.c_str());
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if (!Update.begin(maxSketchSpace)) { //start with max available size
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) { //true to set the size to the current progress
            Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
            Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
    }
    yield();
}

// A short methoid of adding the same web page header to some text
void append_webpage_header() {
    // webpage is a global variable
    webpage = ""; // Clear the variable
    webpage += "<!DOCTYPE html><html lang=\"en\"><head><title>DHT Web Server</title>";
    webpage += "<style>";
    webpage += "#header  {background-color:blue;      font-family:Tahoma,Verdana,Serif,sans-serif; width:1024px; padding:5px; color:white; text-align:center; }";
    webpage += "#section {background-color:#C2DEFF;   font-family:Tahoma,Verdana,Serif,sans-serif; width:1024px; padding:5px; color:blue;  font-size:20px; text-align:center;}";
    webpage += "#footer  {background-color:steelblue; font-family:Tahoma,Verdana,Serif,sans-serif; width:1024px; padding:5px; color:white; font-size:12px; clear:both; text-align:center;}";
    webpage += "</style></head><body>";
}

void append_webpage_footer() {
    webpage += "</body></html>";
}

void handle_log() {
    append_webpage_header();

    append_webpage_footer();
    server.send(200, "text/html", webpage);
}

void web_setup(void) {
    Serial.println("WIFI Connecting ...");
    while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
        delay(250);
        Serial.print('.');
    }

    MDNS.begin(host);
    server.on("/", HTTP_GET, handle_root);
    server.on("/update", HTTP_POST, handle_update_fn,  handle_update_ufn);
    server.on("/log", HTTP_POST, handle_log);
    server.begin();
    MDNS.addService("http", "tcp", 80);

    Serial.printf("Ready! Open http://%s.local in your browser\n", host);
}

void web_loop(void) {
    server.handleClient();
    delay(1);
}
