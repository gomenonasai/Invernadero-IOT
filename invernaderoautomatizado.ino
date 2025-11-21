#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---- PINES ----
#define PIN_SOIL_DO  D6 
#define PIN_RELAY    D5 
#define PIN_UV       A0 // ML8511 OUT

// ---- CONFIGURACIONES ----
LiquidCrystal_I2C lcd(0x27, 16, 2); // Si no funciona, probar 0x3F

// Tiempo de riego (ms)
unsigned long pumpTime = 4000;  
unsigned long lastPump = 0;
unsigned long minPumpInterval = 15000; // 15 seg entre riegos para evitar "bomba loca"

void setup() {
  Serial.begin(115200);

  // Pines
  pinMode(PIN_SOIL_DO, INPUT);
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, LOW); // RELÉ APAGADO

  // LCD I2C
  Wire.begin(D2, D1); // SDA, SCL
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("Invernadero ON");
  delay(1200);
  lcd.clear();
}

void loop() {

  // -------------------------
  // Lecturas de sensores
  // -------------------------
  int uvRaw = analogRead(PIN_UV); 
  int soilState = digitalRead(PIN_SOIL_DO);  
  bool sueloSeco = (soilState == HIGH); 
  // Nota: si tu módulo funciona invertido y muestra "HUMEDO" cuando está seco,
  // cambia a: bool sueloSeco = (soilState == LOW);

  // -------------------------
  // Control Bomba
  // -------------------------
  if (sueloSeco && (millis() - lastPump > minPumpInterval)) {
    Serial.println("Suelo SECO -> Activando bomba...");
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("RIEGO ACTIVADO");

    digitalWrite(PIN_RELAY, HIGH); // Enciende relé
    delay(pumpTime);
    digitalWrite(PIN_RELAY, LOW); // Apaga

    Serial.println("Bomba apagada.");
    lastPump = millis();
  }

  // -------------------------
  // Impresión en MONITOR SERIE
  // -------------------------
  Serial.print("UV RAW: ");
  Serial.print(uvRaw);
  Serial.print(" | HUMEDAD: ");
  Serial.println(sueloSeco ? "SECO" : "HUMEDO");

  // -------------------------
  // Mostrar en LCD
  // -------------------------
  lcd.setCursor(0,0);
  lcd.print("UV:");
  lcd.print(uvRaw);
  lcd.print("     ");

  lcd.setCursor(0,1);
  lcd.print("Suelo:");
  lcd.print(sueloSeco ? "SECO   " : "HUMEDO ");

  delay(800);
}
