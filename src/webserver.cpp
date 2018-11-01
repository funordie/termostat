/*
  To upload through terminal you can use: curl -F "image=@firmware.bin" esp8266-webupdate.local/update
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <time.h>

#include <common.hpp>

//String weblog;

const char* host = "esp8266-webupdate";

ESP8266WebServer server(80);
String webpage = "";

const char * javascriptCode = ""
"<p>Click the button to get a message from the ESP8266:</p> "
"<button onclick=\"buttonFunction()\">Message</button> "
"<script>"
"function buttonFunction() {"
" alert(\"Hello from the ESP8266!\");"
"}"
"</script>";

const char * jslogger = ""
        "<script>"
        "function(e){\"use strict\";var n={};n.VERSION=\"1.5.0\";var t,o={},r=function(e,n){return function(){return n.apply(e,arguments)}},i=function(){var e,n,t=arguments,o=t[0];for(n=1;n<t.length;n++)for(e in t[n])e in o||!t[n].hasOwnProperty(e)||(o[e]=t[n][e]);return o},l=function(e,n){return{value:e,name:n}};n.TRACE=l(1,\"TRACE\"),n.DEBUG=l(2,\"DEBUG\"),n.INFO=l(3,\"INFO\"),n.TIME=l(4,\"TIME\"),n.WARN=l(5,\"WARN\"),n.ERROR=l(8,\"ERROR\"),n.OFF=l(99,\"OFF\");var u=function(e){this.context=e,this.setLevel(e.filterLevel),this.log=this.info};u.prototype={setLevel:function(e){e&&\"value\"in e&&(this.context.filterLevel=e)},getLevel:function(){return this.context.filterLevel},enabledFor:function(e){var n=this.context.filterLevel;return e.value>=n.value},trace:function(){this.invoke(n.TRACE,arguments)},debug:function(){this.invoke(n.DEBUG,arguments)},info:function(){this.invoke(n.INFO,arguments)},warn:function(){this.invoke(n.WARN,arguments)},error:function(){this.invoke(n.ERROR,arguments)},time:function(e){\"string\"==typeof e&&e.length>0&&this.invoke(n.TIME,[e,\"start\"])},timeEnd:function(e){\"string\"==typeof e&&e.length>0&&this.invoke(n.TIME,[e,\"end\"])},invoke:function(e,n){t&&this.enabledFor(e)&&t(n,i({level:e},this.context))}};var a=new u({filterLevel:n.OFF});!function(){var e=n;e.enabledFor=r(a,a.enabledFor),e.trace=r(a,a.trace),e.debug=r(a,a.debug),e.time=r(a,a.time),e.timeEnd=r(a,a.timeEnd),e.info=r(a,a.info),e.warn=r(a,a.warn),e.error=r(a,a.error),e.log=e.info}(),n.setHandler=function(e){t=e},n.setLevel=function(e){a.setLevel(e);for(var n in o)o.hasOwnProperty(n)&&o[n].setLevel(e)},n.getLevel=function(){return a.getLevel()},n.get=function(e){return o[e]||(o[e]=new u(i({name:e},a.context)))},n.createDefaultHandler=function(e){e=e||{},e.formatter=e.formatter||function(e,n){n.name&&e.unshift(\"[\"+n.name+\"]\")};var t={},o=function(e,n){Function.prototype.apply.call(e,console,n)};return\"undefined\"==typeof console?function(){}:function(r,i){r=Array.prototype.slice.call(r);var l,u=console.log;i.level===n.TIME?(l=(i.name?\"[\"+i.name+\"] \":\"\")+r[0],\"start\"===r[1]?console.time?console.time(l):t[l]=(new Date).getTime():console.timeEnd?console.timeEnd(l):o(u,[l+\": \"+((new Date).getTime()-t[l])+\"ms\"])):(i.level===n.WARN&&console.warn?u=console.warn:i.level===n.ERROR&&console.error?u=console.error:i.level===n.INFO&&console.info?u=console.info:i.level===n.DEBUG&&console.debug?u=console.debug:i.level===n.TRACE&&console.trace&&(u=console.trace),e.formatter(r,i),o(u,r))}},n.useDefaults=function(e){n.setLevel(e&&e.defaultLevel||n.DEBUG),n.setHandler(n.createDefaultHandler(e))},\"function\"==typeof define&&define.amd?define(n):\"undefined\"!=typeof module&&module.exports?module.exports=n:(n._prevLogger=e.Logger,n.noConflict=function(){return e.Logger=n._prevLogger,n},e.Logger=n)}(this)"
        "</script>";

void append_webpage_header() {
    // webpage is a global variable
    webpage = ""; // Clear the variable
    webpage += "<!DOCTYPE html><html lang=\"en\"><head><title>esp web server</title>";
    webpage += "</head><body>";
}

// A short methoid of adding the same web page header to some text
void append_webpage_footer() {
    webpage += "</body></html>";
}

void handle_root() {
    append_webpage_header();
    webpage += ""
            "<form method='POST' action='/update' enctype='multipart/form-data'>"
                "<input type='file' name='update'>"
                "<input type='submit' value='Update'>"
            "</form>";
    webpage += "<br>";
    webpage += "<a href=/log>go to log page</a>";

    append_webpage_footer();
    server.send(200, "text/html", webpage);
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
        addToLog(LOG_LEVEL_ERROR, "Update: %s", upload.filename.c_str());
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
            addToLog(LOG_LEVEL_ERROR, "Update Success: %u\nRebooting...", upload.totalSize);
        } else {
            Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
    }
    yield();
}

// send the state of the switch to the web browser
void GetSwitchState()
{
    if (digitalRead(3)) {
        server.send(200, "text/html", "Switch state: ON");
    }
    else {
        server.send(200, "text/html", "Switch state: OFF");
    }
}

void handle_ajax_log() {
    addToLog(LOG_LEVEL_ERROR, "%s: enter", __FUNCTION__);
#if 0
    // read switch state and send appropriate paragraph text
    GetSwitchState();
#else
    String str;
    getLogAll(str);
    server.send(200, "text/html", str);
#endif
}

void handle_log() {

    webpage = "";
    webpage.concat("<!DOCTYPE html><html lang=\"en\"><head><title>esp web server</title>");
    //javascript
    webpage.concat("<script>");
    webpage.concat("function GetSwitchState() {");
#if 0
    webpage.concat(      "nocache = \"&nocache=\" + Math.random() * 1000000;");
#endif
    webpage.concat(      "var request = new XMLHttpRequest();");
    webpage.concat(      "request.onreadystatechange = function() {");
    webpage.concat(          "if (this.readyState == 4 && this.status == 200 && this.responseText != null) {");
    webpage.concat(          "document.getElementById(\"switch_txt\").innerHTML = this.responseText;");
    webpage.concat(          "}");
    webpage.concat(      "}");
#if 0
    webpage.concat(      "request.open(\"GET\", \"ajax_switch\" + nocache, true);");
#else
    webpage.concat(      "request.open(\"GET\", \"ajax_switch\", true);");
#endif
    webpage.concat(      "request.send(null);");
    webpage.concat(  "}");
    webpage.concat(  "</script>");

    webpage.concat( "</head><body>");
    //html code
    webpage.concat(  "<h1>Arduino AJAX Switch Status</h1>");
    webpage.concat(  "<button type=\"button\" onclick=\"GetSwitchState()\">Get Switch State</button>");
    webpage.concat(  "<p id=\"switch_txt\"> No Log for now...</p>");

    append_webpage_footer();
    server.send(200, "text/html", webpage);
}

void web_setup(void) {
    addToLog(LOG_LEVEL_DEBUG_MORE, "%s: enter", __FUNCTION__);
    while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
        delay(250);
        addToLogEx(LOG_LEVEL_ERROR, '.');
    }

    MDNS.begin(host);
    server.on("/", HTTP_GET, handle_root);
    server.on("/update", HTTP_POST, handle_update_fn,  handle_update_ufn);
    server.on("/log", HTTP_GET, handle_log);
    server.on("/ajax_switch", HTTP_GET, handle_ajax_log);

    server.begin();
    MDNS.addService("http", "tcp", 80);

    addToLog(LOG_LEVEL_ERROR, "Ready! Open http://%s.local in your browser", host);
}

void web_loop(void) {
    server.handleClient();
    addToLog(LOG_LEVEL_DEBUG_MORE, "%s: enter", __FUNCTION__);
}
