#include <Wire.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <HCSR04.h>

// ---------- Pinos ----------
#define ENA   6                                                                                                                                                                                                                                                                                                                                                                                               
#define IN1   7
#define IN2   8
#define ENB   11
#define IN3   9
#define IN4   10
#define PINO_SERVO 3
#define TRIG  4                                                                                                                                                                                                                                                                                                                                                                                                                                      
#define ECHO  5

// ---------- Ajustes ----------
#define LCD_ENDERECO     0x27   // se o LCD não acender, tente 0x3F
#define VELOCIDADE       50    // 0 a 255
#define VELOCIDADE_CURVA 50      

#define DIST_OBSTACULO   25     // cm: abaixo disso o robô para e desvia
#define TEMPO_RE         350    // ms de ré
#define TEMPO_CURVA_90   450    // ms para girar ~90 graus (calibre no chão)
#define SERVO_CENTRO     90
#define SERVO_ESQUERDA   160
#define SERVO_DIREITA    20

LiquidCrystal_I2C lcd(LCD_ENDERECO, 16, 2);
UltraSonicDistanceSensor sensor(TRIG, ECHO);
Servo servo;

unsigned long ultimaAtualizacaoLcd = 0;

// ---------- Motores ----------
// Se um lado girar ao contrário, troque IN1<->IN2 (esquerdo)
// ou IN3<->IN4 (direito) nos #define acima.
void motorEsquerdo(int vel) {
  digitalWrite(IN1, vel > 0 ? HIGH : LOW);
  digitalWrite(IN2, vel < 0 ? HIGH : LOW);
  analogWrite(ENA, abs(vel));
}

void motorDireito(int vel) {
  digitalWrite(IN3, vel > 0 ? HIGH : LOW);
  digitalWrite(IN4, vel < 0 ? HIGH : LOW);
  analogWrite(ENB, abs(vel));
}

void frente()   { motorEsquerdo(VELOCIDADE);        motorDireito(VELOCIDADE); }
void re()       { motorEsquerdo(-VELOCIDADE);       motorDireito(-VELOCIDADE); }
void esquerda() { motorEsquerdo(-VELOCIDADE_CURVA); motorDireito(VELOCIDADE_CURVA); }
void direita()  { motorEsquerdo(VELOCIDADE_CURVA);  motorDireito(-VELOCIDADE_CURVA); }
void parar()    { motorEsquerdo(0);                 motorDireito(0); }

// ---------- Sensor ----------
// A biblioteca devolve -1 quando não há eco. Trato como "livre" (400 cm).
float medirDistancia() {
  float d = sensor.measureDistanceCm();
  Serial.println(d);               // -1 = sem eco
  if (d <= 0) return 400.0;
  return d;
}

// Aponta o servo, espera ele chegar e mede (média de 3 leituras).
float medirEm(int angulo) {
  servo.write(angulo);
  delay(450);
  float soma = 0;
  for (int i = 0; i < 3; i++) {
    soma += medirDistancia();
    delay(60);
  }
  return soma / 3.0;
}

// ---------- LCD ----------
void mostrar(const char* estado, float distancia) {
  lcd.setCursor(0, 0);
  lcd.print(estado);
  lcd.print("          ");          // limpa sobras da linha
  lcd.setCursor(0, 1);
  lcd.print("Dist: ");
  if (distancia >= 400) {
    lcd.print("sem eco  ");
  } else {
    lcd.print(distancia, 1);
    lcd.print(" cm   ");
  }
}

// ---------- Desvio ----------
void desviar() {
  parar();
  mostrar("Obstaculo!", medirDistancia());
  delay(200);

  re();
  delay(TEMPO_RE);
  parar();

  mostrar("Olhando...", 0);
  float distEsq = medirEm(SERVO_ESQUERDA);
  float distDir = medirEm(SERVO_DIREITA);
  servo.write(SERVO_CENTRO);

  if (distEsq >= distDir) {
    mostrar("Virando esq", distEsq);
    esquerda();
  } else {
    mostrar("Virando dir", distDir);
    direita();
  }
  delay(TEMPO_CURVA_90);
  parar();

  servo.write(SERVO_CENTRO);
  delay(300);
}

void setup() {
  Serial.begin(9600);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  parar();

  servo.attach(PINO_SERVO);
  servo.write(SERVO_CENTRO);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Robo pronto");
  delay(1500);
}

void loop() {
  float distancia = medirDistancia();

  if (distancia < DIST_OBSTACULO) {
    desviar();
  } else {
    frente();
    // atualiza o LCD a cada 200 ms para não atrasar o sensor
    if (millis() - ultimaAtualizacaoLcd >= 200) {
      mostrar("Andando", distancia);
      ultimaAtualizacaoLcd = millis();
    }
  }
  delay(40);
}
