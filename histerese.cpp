#include <OneWire.h>
#include <DallasTemperature.h>
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

LiquidCrystal_PCF8574 lcd(0x27);

int show = -1;
float temptotal = 0;
const int botao1 = 5;
const int botao2 = 6;
const int botao3 = 7;
const int PINO_ONEWIRE = 12;
const int PINO_RELE = 3;
//const int PINO_FAN = 2;

byte dotOff[] = { 0b00000, 0b01110, 0b10001, 0b10001,
                  0b10001, 0b01110, 0b00000, 0b00000 };
byte dotOn[] = { 0b00000, 0b01110, 0b11111, 0b11111,
                 0b11111, 0b01110, 0b00000, 0b00000 };

OneWire oneWire(PINO_ONEWIRE);
DallasTemperature sensor(&oneWire); 
DeviceAddress endereco_temp;

void setup() {
  int error;

  pinMode(botao1, INPUT);
  pinMode(botao2, INPUT);
  pinMode(botao3, INPUT);

  pinMode(PINO_RELE, OUTPUT);

  Serial.begin(115200);
  Serial.println("LCD...");
  sensor.begin();

  Wire.begin();
  Wire.beginTransmission(0x27);
  error = Wire.endTransmission();
  Serial.print("Error: ");
  Serial.print(error);

  if (error == 0) {
    Serial.println(": LCD found.");
    show = 25;
    lcd.begin(16, 2); 
    lcd.createChar(1, dotOff);
    lcd.createChar(2, dotOn);

  } else {
    Serial.println(": LCD not found.");
  }

}

void aumenta() {
  show++;
  if (show > 40) {
    show = 20;  // volta pra 20 quando chega em 40
  }
}

void diminui() {
  show--;
  if (show < 20) {
    show = 40;  // volta pra 40 se ir abaixo de 20
  }
}


void atualizaTemperatura() {
  sensor.requestTemperatures();             
  if (!sensor.getAddress(endereco_temp, 0)) { 
    Serial.println("SENSOR NAO CONECTADO"); 
  } else {
    temptotal = sensor.getTempC(endereco_temp);
  }
}

void atualizaContador() {
  int valor1 = digitalRead(botao1);
  int valor2 = digitalRead(botao2);

  if (valor1 == HIGH) {
    aumenta();
  }

  if (valor2 == HIGH) {
    diminui();
  }
}

void ativaRele() {
  if (temptotal < show) {
    digitalWrite(PINO_RELE, HIGH);
  } else if (temptotal > show) {
    digitalWrite(PINO_RELE, LOW);
  }
}

void loop() {
  bool reset = true;
  int valor3 = digitalRead(botao3);

  lcd.setBacklight(1);
  lcd.home();
  lcd.clear();
  lcd.setCursor(0, 0);  // atualiza a primeira linha
  lcd.print(reset);
  lcd.print(" Alvo: ");
  lcd.print(show);
  lcd.setCursor(0, 1);  // atualiza a segunda linha
  lcd.print("Temp: ");
  lcd.print(temptotal, 1);

  Serial.println(reset);

  atualizaContador();
  atualizaTemperatura();

  if (valor3 == HIGH) {
    reset = !reset;
  }

  if (reset == true) {
    ativaRele();
  }

  delay(500);
}
