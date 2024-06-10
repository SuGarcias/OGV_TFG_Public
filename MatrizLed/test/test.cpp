#include <Arduino.h>

#include <NeoPixelBus.h>

const uint16_t PixelCount = 64; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = 2;  // make sure to set this to the correct pin, ignored for Esp8266

#define colorSaturation 128

// three element pixels, in different order and speeds
NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip(PixelCount, PixelPin);


RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

HslColor hslRed(red);
HslColor hslGreen(green);
HslColor hslBlue(blue);
HslColor hslWhite(white);
HslColor hslBlack(black);


void setup()
{
    Serial.begin(9600);
    while (!Serial); // wait for serial attach

    Serial.println();
    Serial.println("Initializing...");
    Serial.flush();

    // this resets all the neopixels to an off state
    strip.Begin();
    strip.Show();


    Serial.println();
    Serial.println("Running...");
}


void loop()
{
    delay(5000);

    Serial.println("Colors R, G, B, W...");


    strip.SetPixelColor(0, red);
    strip.SetPixelColor(1, green);
    strip.SetPixelColor(2, blue);
    strip.SetPixelColor(3, white);
    strip.SetPixelColor(4, red);
    strip.SetPixelColor(5, green);
    strip.SetPixelColor(6, blue);
    strip.SetPixelColor(7, white);

    strip.Show();


    delay(5000);

    Serial.println("Off ...");

    // turn off the pixels
    strip.SetPixelColor(0, black);
    strip.SetPixelColor(1, black);
    strip.SetPixelColor(2, black);
    strip.SetPixelColor(3, black);
    strip.SetPixelColor(4, black);
    strip.SetPixelColor(5, black);
    strip.SetPixelColor(6, black);
    strip.SetPixelColor(7, black);
    strip.Show();

    delay(5000);

    Serial.println("HSL Colors R, G, B, W...");

    // set the colors, 
    // if they don't match in order, you may need to use NeoGrbFeature feature
    strip.SetPixelColor(0, hslRed);
    strip.SetPixelColor(1, hslGreen);
    strip.SetPixelColor(2, hslBlue);
    strip.SetPixelColor(3, hslWhite);
    strip.Show();


    delay(5000);

    Serial.println("Off again...");

    // turn off the pixels
    strip.SetPixelColor(0, hslBlack);
    strip.SetPixelColor(1, hslBlack);
    strip.SetPixelColor(2, hslBlack);
    strip.SetPixelColor(3, hslBlack);
    strip.Show();

}

