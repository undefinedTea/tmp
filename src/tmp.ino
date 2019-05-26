#include "Adafruit_BME680.h"

#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#define BME_SCK 7
#define BME_MISO 5
#define BME_MOSI 3
#define BME_CS 13

#define SEALEVELPRESSURE_HPA (1013.25)

#define OLED 11

Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK);

double temperature = 0;
double relativeHumidity = 0;
double pressureHpa = 0;
double gasResistance = 0;
double approxAltitude = 0;

Adafruit_SSD1306 display(OLED);

void setup() {
  if (!bme.begin()) {
    Particle.publish("log", "sensor not found");
  } else {
    Particle.publish("log", "connected");

    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme.setGasHeater(320, 150);

    Particle.variable("temperature", &temperature, DOUBLE);
    Particle.variable("humidity", &relativeHumidity, DOUBLE);
    Particle.variable("pressure", &pressureHpa, DOUBLE);
    Particle.variable("gas", &gasResistance, DOUBLE);
    Particle.variable("altitude", &approxAltitude, DOUBLE);
  }

  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  
  display.display();
  delay(1000);
  display.clearDisplay();
}

void loop() {
  if (! bme.performReading()) {
    Particle.publish("error", "no data available");
  } else {
    temperature = bme.temperature;
    relativeHumidity = bme.humidity;
    pressureHpa = bme.pressure / 100.0;
    gasResistance = bme.gas_resistance / 1000.0;
    approxAltitude = bme.readAltitude(SEALEVELPRESSURE_HPA);

    String data = String::format(
      "{"
        "\"temperature\":%.2f,"
        "\"humidityPercentage\":%.2f,"
        "\"pressureHpa\":%.2f,"
        "\"gasResistance\":%.2f"
        "\"approxAltitude\":%.2f"
      "}",
      temperature,
      relativeHumidity,
      pressureHpa,
      gasResistance,
      approxAltitude);

    Particle.publish("c137/sensor", data, PRIVATE);

    display.clearDisplay();

    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("tmp");
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.printlnf("%.2f", temperature);
    display.display();
  }
  delay(180 * 1000);
}
