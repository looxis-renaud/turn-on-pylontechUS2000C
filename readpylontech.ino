#include <HardwareSerial.h>

// Pins für die UART-Verbindung zum Pylontech-Akku
#define RX_PIN 16 // Beispiel: RX-Pin
#define TX_PIN 17 // Beispiel: TX-Pin

// Anfrage an den Pylontech-Akku
byte pylonask[] = {0x7E, 0x32, 0x30, 0x30, 0x31, 0x34, 0x36, 0x34, 0x32, 0x45, 0x30, 0x30, 0x32, 0x30, 0x31, 0x46, 0x44, 0x33, 0x35, 0x0D};

// Variablen zur Speicherung der empfangenen Daten
char msg_c[130];
long msg_i[130];
long I_Batt = 0;  // Batterie-Strom (1/10 Ampere)
long U_Batt = 0;  // Batterie-Spannung (Millivolt)
long C_Batt = 0;  // Ladezustand (mAh)
long C_Ges = 0;   // Gesamtkapazität (mAh)
long SoC = 0;     // Ladezustand in %

int U_Zell[15];   // Zellenspannungen (Millivolt)
int U_Zell_min = 0;
long U_Zell_mid = 0;
int U_Zell_max = 0;

const long interval = 10000; // Abfrageintervall (10 Sekunden)
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

void setup() {
  Serial.begin(9600); // Serielle Ausgabe für den Serial Monitor
  Serial1.begin(1200, SERIAL_8N1, RX_PIN, TX_PIN); // UART1 für Pylontech

  // Initialisierung
  for (int j = 0; j < 130; j++) {
    msg_c[j] = 0;
  }
  for (int j = 0; j <= 15; j++) {
    U_Zell[j] = 0;
  }

  Serial.println("ESP32 gestartet. Warte auf Daten vom Pylontech-Akku...");
}

void loop() {
  currentMillis = millis();

  // Daten alle 10 Sekunden vom Akku anfordern
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    Serial.println("\nAnfrage an Pylontech-Akku gesendet.");
    Serial1.write(pylonask, sizeof(pylonask));

    // Empfangene Daten lesen
    int i = 0;
    while (Serial1.available()) {
      msg_c[i] = Serial1.read();
      i++;
      if (i >= 130) break;
    }

    // Empfangene Daten in Zahlen konvertieren
    for (int j = 0; j < 130; j++) {
      if (msg_c[j] >= '0' && msg_c[j] <= '9') {
        msg_i[j] = msg_c[j] - '0';
      } else if (msg_c[j] >= 'A' && msg_c[j] <= 'F') {
        msg_i[j] = msg_c[j] - 'A' + 10;
      } else {
        msg_i[j] = -1;
      }
    }

    // Batterie-Daten extrahieren
    I_Batt = msg_i[101] * 4096 + msg_i[102] * 256 + msg_i[103] * 16 + msg_i[104];
    U_Batt = msg_i[105] * 4096 + msg_i[106] * 256 + msg_i[107] * 16 + msg_i[108];
    C_Batt = msg_i[109] * 4096 + msg_i[110] * 256 + msg_i[111] * 16 + msg_i[112];
    C_Ges = msg_i[115] * 4096 + msg_i[116] * 256 + msg_i[117] * 16 + msg_i[118];
    SoC = (C_Batt * 1000) / C_Ges;

    // Zellenspannungen berechnen
    for (int j = 0; j <= 14; j++) {
      U_Zell[j] = msg_i[20 + (j * 4)] * 256 + msg_i[21 + (j * 4)] * 16 + msg_i[22 + (j * 4)];
    }

    // Zellspannungen analysieren
    U_Zell_min = U_Zell[0];
    U_Zell_max = U_Zell[0];
    U_Zell_mid = 0;
    for (int j = 0; j <= 14; j++) {
      if (U_Zell[j] < U_Zell_min) U_Zell_min = U_Zell[j];
      if (U_Zell[j] > U_Zell_max) U_Zell_max = U_Zell[j];
      U_Zell_mid += U_Zell[j];
    }
    U_Zell_mid /= 15;

    // Ergebnisse ausgeben
    Serial.println("=== Pylontech Akku-Daten ===");
    Serial.printf("Batterie-Strom: %ld (1/10 A)\n", I_Batt);
    Serial.printf("Batterie-Spannung: %ld mV\n", U_Batt);
    Serial.printf("Ladezustand: %ld mAh\n", C_Batt);
    Serial.printf("Gesamtkapazität: %ld mAh\n", C_Ges);
    Serial.printf("SoC: %ld %%\n", SoC);
    Serial.printf("Min. Zellenspannung: %d mV\n", U_Zell_min);
    Serial.printf("Durchschnittliche Zellenspannung: %ld mV\n", U_Zell_mid);
    Serial.printf("Max. Zellenspannung: %d mV\n", U_Zell_max);
    Serial.println("===========================");
  }
}
