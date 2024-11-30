#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Configuración del módulo RF24
RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

// Variables para almacenar los datos recibidos
float data1[4];       // GPS: latitud, longitud, altitud, velocidad
char data2[64];       // GPS: fecha y hora
float data3[3];       // BME280: humedad, temperatura, presión
float data4[6];       // MPU6050: aceleraciones y giroscopio

void setup() {
  Serial.begin(9600);  // Inicializa el monitor serial
  radio.begin();       // Inicializa el módulo RF24
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.startListening(); // Cambia a modo de recepción

  Serial.println("Receptor listo");
}

void loop() {
  // Verifica si hay datos disponibles
  if (radio.available()) {
    // Recibe los datos en orden
    radio.read(&data1, sizeof(data1)); // GPS: latitud, longitud, altitud, velocidad
    Serial.println("Datos GPS:");
    Serial.print("Latitud: "); Serial.println(data1[0], 6);
    Serial.print("Longitud: "); Serial.println(data1[1], 6);
    Serial.print("Altitud (m): "); Serial.println(data1[2], 2);
    Serial.print("Velocidad (km/h): "); Serial.println(data1[3], 2);

    radio.read(&data2, sizeof(data2)); // GPS: fecha y hora
    Serial.print("Fecha y hora: ");
    Serial.println(data2);

    radio.read(&data3, sizeof(data3)); // BME280: humedad, temperatura, presión
    Serial.println("Datos BME280:");
    Serial.print("Humedad (%): "); Serial.println(data3[0], 2);
    Serial.print("Temperatura (°C): "); Serial.println(data3[1], 2);
    Serial.print("Presión (hPa): "); Serial.println(data3[2], 2);

    radio.read(&data4, sizeof(data4)); // MPU6050: aceleraciones y giroscopio
    Serial.println("Datos MPU6050:");
    Serial.print("Aceleración X (m/s²): "); Serial.println(data4[0], 2);
    Serial.print("Aceleración Y (m/s²): "); Serial.println(data4[1], 2);
    Serial.print("Aceleración Z (m/s²): "); Serial.println(data4[2], 2);
    Serial.print("Giroscopio X (rad/s): "); Serial.println(data4[3], 2);
    Serial.print("Giroscopio Y (rad/s): "); Serial.println(data4[4], 2);
    Serial.print("Giroscopio Z (rad/s): "); Serial.println(data4[5], 2);

    Serial.println("------------------------------");
  }

  delay(500); // Breve pausa para la lectura
}

