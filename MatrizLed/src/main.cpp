#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <NeoPixelBus.h>

// Configuración de WiFi
const char *ssid = "WIFI_SSID";
const char *password = "WIFI_PASSWORD";

// Configuración de la matriz de LEDs
const uint16_t PixelCount = 64;
const uint8_t PixelPin = 2;

NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip(PixelCount, PixelPin);
WebServer server(80);

RgbColor currentColor(255, 0, 0); // Color por defecto: rojo
bool isOn = false;
uint8_t brightness = 255;
bool animationActive = false;
unsigned long previousMillis = 0;
const long interval = 100; // Intervalo para animaciones

class LedMatrix {
public:
    LedMatrix(NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod>& strip) : strip(strip) {}

    void setColor(RgbColor color, uint8_t brightness) {
        Serial.println("Setting color");
        currentColor = color;
        this->brightness = brightness;
        if (isOn) {
            updateMatrix(color, brightness);
        }
    }

    void turnOff() {
        Serial.println("Turning off LEDs");
        clearMatrix();
        strip.Show();
        isOn = false;
        animationActive = false;
    }

    void setBrightness(uint8_t newBrightness) {
        Serial.println("Setting brightness");
        brightness = newBrightness;
        if (isOn) {
            updateBrightness();
        }
    }

    void increaseBrightness() {
        Serial.println("Increasing brightness");
        brightness = min(255, brightness + 51); // Aumenta la luminosidad en incrementos de 51
        updateBrightness();
    }

    void decreaseBrightness() {
        Serial.println("Decreasing brightness");
        brightness = max(0, brightness - 51); // Disminuye la luminosidad en decrementos de 51
        updateBrightness();
    }

    void showPattern(int pattern) {
        if (!isOn) return;
        Serial.println("Showing pattern");
        stopCurrentAnimation();
        clearMatrix();
        switch(pattern) {
            case 0: // Heart pattern
                displayPattern(heartPattern, RgbColor(255, 0, 0));
                break;
            case 1: // Smile pattern
                displayPattern(smilePattern, RgbColor(255, 255, 0));
                break;
            case 2: // Checkered pattern
                displayPattern(checkeredPattern, RgbColor(233, 207, 171), RgbColor(187, 71, 41));
                break;
            case 3: // Diamond pattern
                displayPattern(diamondPattern, RgbColor(255, 0, 255));
                break;
            case 4: // Arrow pattern
                displayPattern(arrowPattern, RgbColor(0, 255, 255));
                break;
            case 5: // Spiral pattern
                displayPattern(spiralPattern, RgbColor(255, 165, 0));
                break;
        }
        strip.Show();
    }

    void animate(int animation) {
        if (!isOn) return;
        Serial.println("Starting animation");
        stopCurrentAnimation();
        clearMatrix();
        animationActive = true;
        currentAnimation = animation;
        previousMillis = millis(); // Inicializa el temporizador para animaciones
    }

    void stopCurrentAnimation() {
        Serial.println("Stopping current animation");
        animationActive = false;
    }

    void update() {
        if (!animationActive) return;

        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;

            switch(currentAnimation) {
                case 0:
                    animateRainbow();
                    break;
                case 1:
                    animateBlink();
                    break;
                case 2:
                    animateWave();
                    break;
                case 3:
                    animateSpiral();
                    break;
                case 4:
                    animateBreathing();
                    break;
                case 5:
                    animateFireworks();
                    break;
            }
        }
    }

private:
    NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod>& strip;
    uint8_t brightness;
    RgbColor currentColor;
    int currentAnimation;

    const uint8_t heartPattern[64] = {
        0, 1, 1, 0, 0, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        0, 1, 1, 1, 1, 1, 1, 0,
        0, 0, 1, 1, 1, 1, 0, 0,
        0, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    };

    const uint8_t smilePattern[64] = {
        0, 0, 1, 1, 1, 1, 0, 0,
        0, 1, 0, 0, 0, 0, 1, 0,
        1, 0, 1, 0, 0, 1, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 1, 0, 0, 1, 0, 1,
        1, 0, 0, 1, 1, 0, 0, 1,
        0, 1, 0, 0, 0, 0, 1, 0,
        0, 0, 1, 1, 1, 1, 0, 0
    };

    const uint8_t checkeredPattern[64] = {
        1, 0, 1, 0, 1, 0, 1, 0,
        0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0,
        0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0,
        0, 1, 0, 1, 0, 1, 0, 1,
        1, 0, 1, 0, 1, 0, 1, 0,
        0, 1, 0, 1, 0, 1, 0, 1
    };

    const uint8_t diamondPattern[64] = {
        0, 0, 0, 1, 1, 0, 0, 0,
        0, 0, 1, 0, 0, 1, 0, 0,
        0, 1, 0, 0, 0, 0, 1, 0,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        0, 1, 0, 0, 0, 0, 1, 0,
        0, 0, 1, 0, 0, 1, 0, 0,
        0, 0, 0, 1, 1, 0, 0, 0
    };

    const uint8_t arrowPattern[64] = {
        0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 1, 1, 1, 0, 0, 0,
        0, 1, 0, 1, 0, 1, 0, 0,
        1, 0, 0, 1, 0, 0, 1, 0,
        0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 0
    };

    const uint8_t spiralPattern[64] = {
        1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 0, 1,
        1, 0, 0, 0, 0, 1, 0, 1,
        1, 0, 1, 1, 0, 1, 0, 1,
        1, 0, 1, 0, 0, 1, 0, 1,
        1, 0, 1, 1, 1, 1, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1
    };

    void displayPattern(const uint8_t pattern[64], RgbColor color) {
        for (int i = 0; i < PixelCount; i++) {
            if (pattern[i]) {
                strip.SetPixelColor(i, adjustBrightness(color, brightness));
            }
        }
        strip.Show();
    }

    void displayPattern(const uint8_t pattern[64], RgbColor color1, RgbColor color2) {
        for (int i = 0; i < PixelCount; i++) {
            if (pattern[i]) {
                strip.SetPixelColor(i, adjustBrightness(color1, brightness));
            } else {
                strip.SetPixelColor(i, adjustBrightness(color2, brightness));
            }
        }
        strip.Show();
    }

    RgbColor adjustBrightness(RgbColor color, uint8_t brightness) {
        return RgbColor(color.R * brightness / 255, color.G * brightness / 255, color.B * brightness / 255);
    }

    void updateBrightness() {
        for (int i = 0; i < PixelCount; i++) {
            RgbColor color = strip.GetPixelColor(i);
            strip.SetPixelColor(i, adjustBrightness(color, brightness));
        }
        strip.Show();
    }

    void updateMatrix(RgbColor color, uint8_t brightness) {
        for (int i = 0; i < PixelCount; i++) {
            strip.SetPixelColor(i, adjustBrightness(color, brightness));
        }
        strip.Show();
    }

    void clearMatrix() {
        for (int i = 0; i < PixelCount; i++) {
            strip.SetPixelColor(i, RgbColor(0, 0, 0));
        }
        strip.Show();
    }

    void animateRainbow() {
        static int hueOffset = 0;
        for (int i = 0; i < PixelCount; i++) {
            float hue = ((float)i / PixelCount) + hueOffset / 255.0;
            if (hue > 1.0) hue -= 1.0;
            strip.SetPixelColor(i, adjustBrightness(HslColor(hue, 1.0, 0.5), brightness));
        }
        strip.Show();
        hueOffset = (hueOffset + 1) % 256;
    }

    void animateBlink() {
        static bool isOn = false;
        RgbColor color = isOn ? adjustBrightness(RgbColor(random(0, 255), random(0, 255), random(0, 255)), brightness) : RgbColor(0, 0, 0);
        for (int i = 0; i < PixelCount; i++) {
            strip.SetPixelColor(i, color);
        }
        strip.Show();
        isOn = !isOn;
    }

    void animateWave() {
        static int wavePosition = 0;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                int position = (wavePosition + i) % 8;
                strip.SetPixelColor(i + j * 8, (position == j) ? adjustBrightness(RgbColor(0, 0, 255), brightness) : RgbColor(0, 0, 0));
            }
        }
        strip.Show();
        wavePosition = (wavePosition + 1) % 8;
    }

    void animateSpiral() {
        static int spiralIndex = 0;
        static int radius = 0;
        int x = spiralIndex % 8;
        int y = spiralIndex / 8;
        if (x < radius || x >= 8 - radius || y < radius || y >= 8 - radius) {
            strip.SetPixelColor(spiralIndex, adjustBrightness(RgbColor(255, 0, 0), brightness));
        } else {
            strip.SetPixelColor(spiralIndex, RgbColor(0, 0, 0));
        }
        strip.Show();
        spiralIndex = (spiralIndex + 1) % PixelCount;
        if (spiralIndex == 0) {
            radius = (radius + 1) % 4;
        }
    }

    void animateBreathing() {
        static bool increasing = true;
        static int currentBrightness = 0;
        static float hue = 0.0;
        if (increasing) {
            currentBrightness += 5;
            if (currentBrightness >= brightness) {
                currentBrightness = brightness;
                increasing = false;
                hue += 0.02;
                if (hue > 1.0) hue -= 1.0;
            }
        } else {
            currentBrightness -= 5;
            if (currentBrightness <= 0) {
                currentBrightness = 0;
                increasing = true;
            }
        }
        for (int i = 0; i < PixelCount; i++) {
            strip.SetPixelColor(i, adjustBrightness(HslColor(hue, 1.0, 0.5), currentBrightness));
        }
        strip.Show();
    }

    void animateFireworks() {
        static int frame = 0;
        static int direction = 0;
        static bool inward = true;
        RgbColor color = adjustBrightness(RgbColor(255, 0, 0), brightness); // Color de la explosión

        // Coordenadas centrales
        int centerX = 3;
        int centerY = 3;

        // Borrar la matriz en cada fotograma
        clearMatrix();

        if (inward) {
            // Movimiento hacia el centro
            for (int i = 0; i <= frame; i++) {
                // Dirección 1: Diagonal superior izquierda
                if (centerX - i >= 0 && centerY - i >= 0) {
                    strip.SetPixelColor((centerX - i) + (centerY - i) * 8, color);
                }

                // Dirección 2: Diagonal superior derecha
                if (centerX + i < 8 && centerY - i >= 0) {
                    strip.SetPixelColor((centerX + i) + (centerY - i) * 8, color);
                }

                // Dirección 3: Diagonal inferior izquierda
                if (centerX - i >= 0 && centerY + i < 8) {
                    strip.SetPixelColor((centerX - i) + (centerY + i) * 8, color);
                }

                // Dirección 4: Diagonal inferior derecha
                if (centerX + i < 8 && centerY + i < 8) {
                    strip.SetPixelColor((centerX + i) + (centerY + i) * 8, color);
                }

                // Dirección 5: Horizontal derecha
                if (centerX + i < 8) {
                    strip.SetPixelColor((centerX + i) + centerY * 8, color);
                }

                // Dirección 6: Horizontal izquierda
                if (centerX - i >= 0) {
                    strip.SetPixelColor((centerX - i) + centerY * 8, color);
                }

                // Dirección 7: Vertical arriba
                if (centerY - i >= 0) {
                    strip.SetPixelColor(centerX + (centerY - i) * 8, color);
                }

                // Dirección 8: Vertical abajo
                if (centerY + i < 8) {
                    strip.SetPixelColor(centerX + (centerY + i) * 8, color);
                }
            }

            frame++;
            if (frame > 3) {
                inward = false;
                frame = 0;
                color = adjustBrightness(RgbColor(random(0, 255), random(0, 255), random(0, 255)), brightness); // Cambiar el color de la explosión
            }
        } else {
            // Movimiento hacia afuera (explosión)
            for (int i = 0; i <= 3; i++) {
                if (frame <= i) {
                    // Dirección 1: Diagonal superior izquierda
                    if (centerX - (3 - i) >= 0 && centerY - (3 - i) >= 0) {
                        strip.SetPixelColor((centerX - (3 - i)) + (centerY - (3 - i)) * 8, color);
                    }

                    // Dirección 2: Diagonal superior derecha
                    if (centerX + (3 - i) < 8 && centerY - (3 - i) >= 0) {
                        strip.SetPixelColor((centerX + (3 - i)) + (centerY - (3 - i)) * 8, color);
                    }

                    // Dirección 3: Diagonal inferior izquierda
                    if (centerX - (3 - i) >= 0 && centerY + (3 - i) < 8) {
                        strip.SetPixelColor((centerX - (3 - i)) + (centerY + (3 - i)) * 8, color);
                    }

                    // Dirección 4: Diagonal inferior derecha
                    if (centerX + (3 - i) < 8 && centerY + (3 - i) < 8) {
                        strip.SetPixelColor((centerX + (3 - i)) + (centerY + (3 - i)) * 8, color);
                    }

                    // Dirección 5: Horizontal derecha
                    if (centerX + (3 - i) < 8) {
                        strip.SetPixelColor((centerX + (3 - i)) + centerY * 8, color);
                    }

                    // Dirección 6: Horizontal izquierda
                    if (centerX - (3 - i) >= 0) {
                        strip.SetPixelColor((centerX - (3 - i)) + centerY * 8, color);
                    }

                    // Dirección 7: Vertical arriba
                    if (centerY - (3 - i) >= 0) {
                        strip.SetPixelColor(centerX + (centerY - (3 - i)) * 8, color);
                    }

                    // Dirección 8: Vertical abajo
                    if (centerY + (3 - i) < 8) {
                        strip.SetPixelColor(centerX + (centerY + (3 - i)) * 8, color);
                    }
                }
            }

            frame++;
            if (frame > 3) {
                inward = true;
                frame = 0;
                color = adjustBrightness(RgbColor(255, 0, 0), brightness); 
            }
        }

        strip.Show();
    }
};

LedMatrix ledMatrix(strip);

void connectToWiFi() {
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    int retryCount = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        retryCount++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected.");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nFailed to connect to WiFi.");
    }
}

void handleOn() {
    Serial.println("Handle On Request");
    isOn = true;
    ledMatrix.setColor(currentColor, brightness);
    server.send(200, "text/plain", "LEDs are ON");
}

void handleOff() {
    Serial.println("Handle Off Request");
    ledMatrix.turnOff();
    server.send(200, "text/plain", "LEDs are OFF");
}

void handlePattern() {
    Serial.println("Handle Pattern Request");
    static int currentPattern = 0;
    ledMatrix.stopCurrentAnimation();
    delay(100); // Ensure animation is stopped
    ledMatrix.showPattern(currentPattern);
    currentPattern = (currentPattern + 1) % 6; // Update to 6 patterns
    server.send(200, "text/plain", "Pattern displayed");
}

void handleAnimate() {
    Serial.println("Handle Animate Request");
    static int currentAnimation = 0;
    ledMatrix.stopCurrentAnimation();
    delay(100); // Ensure animation is stopped
    ledMatrix.animate(currentAnimation);
    currentAnimation = (currentAnimation + 1) % 6; // Update to 6 animations
    server.send(200, "text/plain", "Animation played");
}

void handleSetColor() {
    Serial.println("Handle SetColor Request");
    if (server.hasArg("r") && server.hasArg("g") && server.hasArg("b")) {
        int r = server.arg("r").toInt();
        int g = server.arg("g").toInt();
        int b = server.arg("b").toInt();
        currentColor = RgbColor(r, g, b);
        if (isOn) {
            ledMatrix.setColor(currentColor, brightness);
        }
        server.send(200, "text/plain", "Color set");
    } else {
        server.send(400, "text/plain", "Missing color parameters");
    }
}

void handleSetBrightness() {
    Serial.println("Handle SetBrightness Request");
    if (server.hasArg("brightness")) {
        brightness = server.arg("brightness").toInt();
        ledMatrix.setBrightness(brightness);
        server.send(200, "text/plain", "Brightness set");
    } else {
        server.send(400, "text/plain", "Missing brightness parameter");
    }
}

void handleIncreaseBrightness() {
    Serial.println("Handle IncreaseBrightness Request");
    ledMatrix.increaseBrightness();
    server.send(200, "text/plain", "Brightness increased");
}

void handleDecreaseBrightness() {
    Serial.println("Handle DecreaseBrightness Request");
    ledMatrix.decreaseBrightness();
    server.send(200, "text/plain", "Brightness decreased");
}

void handleStatus() {
    Serial.println("Handle Status Request");
    String status = (WiFi.status() == WL_CONNECTED) ? "Connected" : "Disconnected";
    status += ", LEDs are " + String(isOn ? "ON" : "OFF");
    status += ", Animation " + String(animationActive ? "active" : "inactive");
    server.send(200, "text/plain", status);
}

void setup() {
    Serial.begin(115200);
    connectToWiFi();

    strip.Begin();
    strip.Show();

    server.on("/on", HTTP_GET, handleOn);
    server.on("/off", HTTP_GET, handleOff);
    server.on("/pattern", HTTP_GET, handlePattern);
    server.on("/animate", HTTP_GET, handleAnimate);
    server.on("/setcolor", HTTP_GET, handleSetColor);
    server.on("/setbrightness", HTTP_GET, handleSetBrightness);
    server.on("/increasebrightness", HTTP_GET, handleIncreaseBrightness);
    server.on("/decreasebrightness", HTTP_GET, handleDecreaseBrightness);
    server.on("/status", HTTP_GET, handleStatus);

    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    server.handleClient();
    ledMatrix.update();
}
