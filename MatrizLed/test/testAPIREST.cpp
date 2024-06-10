#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <NeoPixelBus.h>

const char* ssid = "SuGarcias";
const char* password = "sentilo1234";

const uint16_t PixelCount = 64; // Total de LEDs en la matriz 8x8
const uint8_t PixelPin = 2;     // Pin al que está conectado el LED

NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip(PixelCount, PixelPin);

WebServer server(80);

bool ledState = false;
RgbColor currentColor(255, 0, 0); // Color inicial: rojo
uint8_t brightness = 255;         // Brillo inicial máximo



void handleLedStatus() {
    server.send(200, "text/plain", ledState ? "LED is ON" : "LED is OFF");
}

void handleLedControl() {
    if (server.hasArg("state")) {
        String state = server.arg("state");
        if (state == "on") {
            turnOnLed();
            server.send(200, "text/plain", "LED turned ON");
        } else if (state == "off") {
            turnOffLed();
            server.send(200, "text/plain", "LED turned OFF");
        } else {
            server.send(400, "text/plain", "Invalid state parameter");
        }
    } else {
        server.send(400, "text/plain", "Missing state parameter");
    }
}

void handleColorChange() {
    if (server.args() == 3) {
        int r = server.arg("r").toInt();
        int g = server.arg("g").toInt();
        int b = server.arg("b").toInt();
        currentColor = RgbColor(r, g, b);
        applyColor();
        server.send(200, "text/plain", "LED color changed");
    } else {
        server.send(400, "text/plain", "Missing color parameters");
    }
}

void handleBrightnessChange() {
    if (server.hasArg("brightness")) {
        int newBrightness = server.arg("brightness").toInt();
        if (newBrightness >= 0 && newBrightness <= 255) {
            brightness = newBrightness;
            applyColor();
            server.send(200, "text/plain", "Brightness changed");
        } else {
            server.send(400, "text/plain", "Invalid brightness parameter");
        }
    } else {
        server.send(400, "text/plain", "Missing brightness parameter");
    }
}

void turnOnLed() {
    ledState = true;
    applyColor();
}

void turnOffLed() {
    ledState = false;
    applyColor();
}

void applyColor() {
    float brightnessFactor = brightness / 255.0;
    for (int i = 0; i < PixelCount; i++) {
        strip.SetPixelColor(i, AdjustBrightness(currentColor, brightnessFactor));
    }
    strip.Show();
}

RgbColor AdjustBrightness(RgbColor color, float brightnessFactor) {
    return RgbColor(color.R * brightnessFactor, color.G * brightnessFactor, color.B * brightnessFactor);
}


void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    strip.Begin();
    strip.Show();

    // Definición de endpoints para la API REST
    server.on("/led/state", HTTP_GET, handleLedStatus);
    server.on("/led/state", HTTP_POST, handleLedControl);
    server.on("/led/color", HTTP_PUT, handleColorChange);
    server.on("/led/brightness", HTTP_PUT, handleBrightnessChange);

    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();
}
