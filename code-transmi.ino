#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SD.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Pines del NRF24L01
#define CE_PIN 9
#define CSN_PIN 10

// Configuración del NRF24L01
RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00001";

// Pines del Servo
#define SERVO_PIN 6

// Pines de la tarjeta SD
#define SD_CS_PIN 4

// Configuración de la tarjeta SD
File dataFile;

// Configuración del Servo
Servo myServo;

// Configuración del GPS (NEO6MV2)
SoftwareSerial ss(7, 8); // RX, TX
TinyGPSPlus gps;

// Estructura para recibir datos
struct DataPacket {
  float temp;        // Temperatura (°C)
  float press;       // Presión (hPa)
  float hum;         // Humedad (%)
  int16_t ax, ay, az; // Aceleración en x, y, z
  int16_t gx, gy, gz; // Giroscopio en x, y, z
  double lat;        // Latitud
  double lon;        // Longitud
  float speed;       // Velocidad (m/s)
};

DataPacket receivedData;

void setup() {
  Serial.begin(9600);

  // Inicialización del NRF24L01
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening(); // Configurar en modo receptor

  // Inicialización del Servo
  myServo.attach(SERVO_PIN);

  // Inicialización de la tarjeta SD
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Fallo al iniciar la tarjeta SD.");
    return;
  }
  Serial.println("Tarjeta SD iniciada.");

  // Crear o abrir el archivo para guardar los datos
  dataFile = SD.open("data.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.println("Temperatura, Presion, Humedad, Aceleracion (X,Y,Z), Giroscopio (X,Y,Z), Latitud, Longitud, Velocidad");
    dataFile.close();
  }

  // Inicialización del GPS
  ss.begin(9600);
  Serial.println("Receptor iniciado y esperando datos...");
}

void loop() {
  // Verificar si hay datos disponibles para leer
  if (radio.available()) {
    // Leer los datos en la estructura
    radio.read(&receivedData, sizeof(receivedData));

    // Mostrar los datos en el Monitor Serial
    Serial.println("Datos recibidos:");
    Serial.print("Temperatura: "); Serial.print(receivedData.temp); Serial.println(" °C");
    Serial.print("Presión: "); Serial.print(receivedData.press); Serial.println(" hPa");
    Serial.print("Humedad: "); Serial.print(receivedData.hum); Serial.println(" %");
    Serial.print("Aceleración (X, Y, Z): ");
    Serial.print(receivedData.ax); Serial.print(", ");
    Serial.print(receivedData.ay); Serial.print(", ");
    Serial.println(receivedData.az);
    Serial.print("Giroscopio (X, Y, Z): ");
    Serial.print(receivedData.gx); Serial.print(", ");
    Serial.print(receivedData.gy); Serial.print(", ");
    Serial.println(receivedData.gz);
    Serial.print("Latitud: "); Serial.print(receivedData.lat, 6); Serial.println("°");
    Serial.print("Longitud: "); Serial.print(receivedData.lon, 6); Serial.println("°");
    Serial.print("Velocidad: "); Serial.print(receivedData.speed); Serial.println(" m/s");
    Serial.println("------------------------------");

    // Verificar si la velocidad es mayor a 1 m/s
    if (receivedData.speed > 1.0) {
      // Mover el servo a 136° si la condición es verdadera
      myServo.write(136);
      Serial.println("Servo movido a 136°");
    }

    // Guardar los datos en la tarjeta SD
    dataFile = SD.open("data.txt", FILE_WRITE);
    if (dataFile) {
      dataFile.print(receivedData.temp); dataFile.print(", ");
      dataFile.print(receivedData.press); dataFile.print(", ");
      dataFile.print(receivedData.hum); dataFile.print(", ");
      dataFile.print(receivedData.ax); dataFile.print(", ");
      dataFile.print(receivedData.ay); dataFile.print(", ");
      dataFile.print(receivedData.az); dataFile.print(", ");
      dataFile.print(receivedData.gx); dataFile.print(", ");
      dataFile.print(receivedData.gy); dataFile.print(", ");
      dataFile.print(receivedData.gz); dataFile.print(", ");
      dataFile.print(receivedData.lat, 6); dataFile.print(", ");
      dataFile.print(receivedData.lon, 6); dataFile.print(", ");
      dataFile.println(receivedData.speed);
      dataFile.close();
      Serial.println("Datos guardados en la tarjeta SD.");
    } else {
      Serial.println("Error al abrir el archivo para guardar datos.");
    }
  }

  // Leer y procesar datos del GPS
  while (ss.available() > 0) {
    gps.encode(ss.read());
    if (gps.location.isUpdated()) {
      Serial.print("Latitud: "); Serial.print(gps.location.lat(), 6); Serial.print("° ");
      Serial.print("Longitud: "); Serial.print(gps.location.lng(), 6); Serial.println("°");
    }
  }
}
