#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>
#include <Servo.h>
#include <DHT.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);
char keys[4][4] = {{'1','2','3','A'},{'4','5','6','B'},{'7','8','9','C'},{'*','0','#','D'}};
byte rowPins[4] = {8, 7, 6, 5}, colPins[4] = {12, 11, 10, 9};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 4);

DHT dht(4, DHT11);
Servo panServo, tiltServo;

byte systemState = 0;
char inputPass[5]; 
byte passLen = 0;

int panAngle = 90, tiltAngle = 90;
unsigned long timer = 0, sensorTimer = 0;
byte t = 0, h = 0; 

// Protótipos
void showSplash();
void checkPassword();
void runDashboard();
void showHumidity();

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  dht.begin();
  panServo.attach(2); tiltServo.attach(3);
  panServo.write(panAngle); tiltServo.write(tiltAngle);
  pinMode(A3, INPUT);
  
  timer = millis();
}

void loop() {
  if (systemState == 0) showSplash();
  else if (systemState == 1) checkPassword();
  else if (systemState == 2) runDashboard();
  else showHumidity(); 
}

void showSplash() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 5);
  display.print(F(" PROJETO 1 BIMESTRE \n\n Tiago     Evelyn\n\n Mari      Ludmila")); 
  display.display();
  if (millis() - timer > 4000) systemState = 1;
}

void checkPassword() {
  display.clearDisplay();
  display.setCursor(10, 10);
  display.print(F("DIGITE A SENHA:\n\n    "));
  for (byte i = 0; i < passLen; i++) display.print('*');
  display.display();

  char key = keypad.getKey();
  if (key) {
    if (key == '*' || key == '#') {
      passLen = 0; 
      memset(inputPass, 0, sizeof(inputPass));
    } else if (passLen < 4) {
      inputPass[passLen++] = key;
    }
    
    if (passLen == 4) { 
      display.clearDisplay();
      display.setCursor(15, 25);
      
      if (strcmp(inputPass, "0401") == 0) { display.print(F("Bem-vinda, Mari!")); systemState = 2; }
      else if (strcmp(inputPass, "1909") == 0) { display.print(F("Bem-vinda, Lud!")); systemState = 2; }
      else if (strcmp(inputPass, "2711") == 0) { display.print(F("Bem-vinda, Evi!")); systemState = 2; }
      else if (strcmp(inputPass, "3011") == 0) { display.print(F("Bem-vindo, Ti!")); systemState = 2; }
      else if (strcmp(inputPass, "4321") == 0) { display.print(F("Lendo Umidade...")); systemState = 3; }
      else display.print(F("SENHA INCORRETA"));

      display.display();
      delay(1500);
      passLen = 0;
      memset(inputPass, 0, sizeof(inputPass));
    }
  }
}

void runDashboard() {
  if (keypad.getKey() == '*') {
    systemState = 1;
    return; 
  }

  int jX = analogRead(A0), jY = analogRead(A1);
// O valor 4 é a "marcha". Aumente para 6 ou 8 se quiser ainda mais rápido.
  if (jX < 400) panAngle += 4; else if (jX > 600) panAngle -= 4;
  if (jY < 400) tiltAngle -= 4; else if (jY > 600) tiltAngle += 4;
  panAngle = constrain(panAngle, 0, 180);
  tiltAngle = constrain(tiltAngle, 0, 180);
  panServo.write(panAngle); tiltServo.write(tiltAngle);

  if (millis() - sensorTimer > 1000) {
    h = dht.readHumidity();
    t = dht.readTemperature();
    sensorTimer = millis();
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(F("T:")); display.print(t); display.print(F("C  U:")); display.print(h); display.print(F("%\n\n"));
  
  display.print(F("Pan:")); display.print(panAngle); display.print(F("  Tilt:")); display.print(tiltAngle); display.print(F("\n\n"));
  display.print(digitalRead(A3) ? F("[!] MOVIMENTO [!]") : F("Nenhum mov."));
  
  display.display();
  delay(15); 
}

void showHumidity() {
  if (keypad.getKey() == '*') {
    systemState = 1;
    return;
  }

  if (millis() - sensorTimer > 2000) {
    h = dht.readHumidity();
    sensorTimer = millis();
  }
  
  display.clearDisplay();
  display.setCursor(15, 10);
  display.setTextSize(1);
  display.print(F("NIVEL DE UMIDADE"));
  
  display.setTextSize(2);
  display.setCursor(30, 25);
  display.print(h); display.print(F("%"));
  
  display.setTextSize(1);
  display.setCursor(0, 55);
  display.print(F("  Press * para sair"));
  display.display();
}
