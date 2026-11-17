// TESTE DAS RODAS - as duas devem girar PARA FRENTE
// Pinagem do professor. Rodas fora do chão no primeiro teste.

#define ENA 6    // motor A
#define IN1 8
#define IN2 7
#define ENB 11   // motor B
#define IN3 9
#define IN4 10

#define VELOCIDADE 50      // 0 a 255 (50 pode ser fraco demais)

// Se uma roda girar pra trás, mude a dela pra true
#define INVERTER_A false
#define INVERTER_B false

void motorA(bool frente, int vel) {
  if (INVERTER_A) frente = !frente;
  digitalWrite(IN1, frente ? HIGH : LOW);
  digitalWrite(IN2, frente ? LOW : HIGH);
  analogWrite(ENA, vel);
}

void motorB(bool frente, int vel) {
  if (INVERTER_B) frente = !frente;
  digitalWrite(IN3, frente ? HIGH : LOW);
  digitalWrite(IN4, frente ? LOW : HIGH);
  analogWrite(ENB, vel);
}

void parar() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

void setup() {
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  parar();
}

void loop() {
  motorA(true, VELOCIDADE);
  motorB(true, VELOCIDADE);
  delay(3000);

  parar();
  delay(2000);
}
