#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <robotArm.h>
#include <ESP32Servo.h>
#include <ESPasyncWebServer.h>
#include <WebSocketsServer.h>

AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(1337);
Servo arm;
Servo gripper;
Servo base;
char* in;
char* armValue;
char* gripperValue;
char* baseValue;

void onWebSocketEvent(uint8_t client_num,
                      WStype_t type,
                      uint8_t *payload,
                      size_t length)
{
  // Figure out the type of WebSocket event
  switch (type)
  {

  // Client has disconnected
  case WStype_DISCONNECTED:
    Serial.printf("[%u] Disconnected!\n", client_num);
    break;

  // New client has connected
  case WStype_CONNECTED:
  {
    IPAddress ip = webSocket.remoteIP(client_num);
    Serial.printf("[%u] Connection from ", client_num);
    Serial.println(ip.toString());
  }
  break;

  // Handle text messages from client
  case WStype_TEXT:

    // Print out raw message
    Serial.printf("[%u] Received text: %s\n", client_num, payload);
    in = (char*)payload;
    armValue = strtok(in, "a:");
    baseValue = strtok(NULL, "b:");
    gripperValue = strtok(NULL, "g:");

    Serial.print("Arm Value: ");
    Serial.println(atoi(armValue));
    Serial.print("Gripper Value: ");
    Serial.println(atoi(gripperValue));
    Serial.print("Base Value: ");
    Serial.println(atoi(baseValue));

    arm.write(atoi(armValue));
    gripper.write(atoi(gripperValue));
    base.write(atoi(baseValue));



    break;

  // For everything else: do nothing
  case WStype_BIN:
  case WStype_ERROR:
  case WStype_FRAGMENT_TEXT_START:
  case WStype_FRAGMENT_BIN_START:
  case WStype_FRAGMENT:
  case WStype_FRAGMENT_FIN:
  default:
    break;
  }
}

// Callback: send homepage
void onIndexRequest(AsyncWebServerRequest *request)
{
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/index.html", "text/html");
}

// Callback: send style sheet
void onCSSRequest(AsyncWebServerRequest *request)
{
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/bootstrap.min.css", "text/css");
}

// Callback: send 404 if requested file does not exist
void onPageNotFound(AsyncWebServerRequest *request)
{
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(404, "text/plain", "Not found");
}

void setup()
{
  // put your setup code here, to run once:

  // Start Serial port
  Serial.begin(115200);
  // Make sure we can read the file system
  if (!SPIFFS.begin())
  {
    Serial.println("Error mounting SPIFFS");
    while (1)
      ;
  }
  arm.attach(armPin);
  gripper.attach(gripperPin);
  base.attach(basePin);

  // Start access point
  WiFi.softAP(ssid, password);

  // Print our IP address
  Serial.println();
  Serial.println("AP running");
  Serial.print("My IP address: ");
  Serial.println(WiFi.softAPIP());

  // On HTTP request for root, provide index.html file
  server.on("/", HTTP_GET, onIndexRequest);

  // On HTTP request for style sheet, provide style.css
  server.on("/bootstrap.min.css", HTTP_GET, onCSSRequest);

  server.on("/bootstrap.bundle.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/bootstrap.bundle.min.js", "text/javascript"); });

  server.on("/jquery-3.3.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/jquery-3.3.1.min.js", "text/javascript"); });
  // Handle requests for pages that do not exist
  server.onNotFound(onPageNotFound);

  // Start web server
  server.begin();

  // Start WebSocket server and assign callback
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
}

void loop()
{
  // put your main code here, to run repeatedly:
  yield();
  webSocket.loop();
}