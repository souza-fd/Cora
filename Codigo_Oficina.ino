/*
  Programa de 5 Sensores para 2 Motores
  Agora com 2 níveis de curva (menor e maior)
*/

// --- Pinos dos Botões ---
const int sensor_esquerda_maior = 2; 
const int sensor_esquerda_menor = 3;
const int sensor_meio = 5;
const int sensor_direita_menor = 6;
const int sensor_direita_maior = 7;

// --- Pinos dos Motores ---
const int MOTOR_ESQUERDO_PWM_PIN = 10;
const int MOTOR_DIREITO_PWM_PIN = 11;

// --- Estados ---
int estadoBotaoEE, estadoBotaoE, estadoBotaoMeio, estadoBotaoD, estadoBotaoDD;

// --- Velocidades ---
const int VELOCIDADE_RETA = 200;

const int VELOCIDADE_CURVA_ALTA = 250;   
const int VELOCIDADE_CURVA_BAIXA = 150;

const int VELOCIDADE_CURVA_MUITO_ALTA = 255; 
const int VELOCIDADE_CURVA_MUITO_BAIXA = 120;

void setup() {

  pinMode(sensor_esquerda_maior, INPUT_PULLUP);
  pinMode(sensor_esquerda_menor, INPUT_PULLUP);
  pinMode(sensor_meio, INPUT_PULLUP);
  pinMode(sensor_direita_menor, INPUT_PULLUP);
  pinMode(sensor_direita_maior, INPUT_PULLUP);

  pinMode(MOTOR_ESQUERDO_PWM_PIN, OUTPUT);
  pinMode(MOTOR_DIREITO_PWM_PIN, OUTPUT);

  Serial.begin(9600);
}

void loop() {

  // LER BOTÕES
  estadoBotaoEE = digitalRead(sensor_esquerda_maior);
  estadoBotaoE = digitalRead(sensor_esquerda_menor);
  estadoBotaoMeio = digitalRead(sensor_meio);
  estadoBotaoD = digitalRead(sensor_direita_menor);
  estadoBotaoDD = digitalRead(sensor_direita_maior);

  // --- LÓGICA COM PRIORIDADES ---

  // PRIORIDADE 1 - IR RETO
  if (estadoBotaoMeio == LOW) {
    Serial.println("FRENTE");
    analogWrite(MOTOR_ESQUERDO_PWM_PIN, VELOCIDADE_RETA);
    analogWrite(MOTOR_DIREITO_PWM_PIN, VELOCIDADE_RETA);
  }

  // PRIORIDADE 2 - CURVA MAIOR ESQUERDA (sensor_esquerda_maior)
  else if (estadoBotaoEE == LOW) {
    Serial.println("CURVA MUITO FORTE ESQUERDA");
    analogWrite(MOTOR_ESQUERDO_PWM_PIN, VELOCIDADE_CURVA_MUITO_BAIXA);
    analogWrite(MOTOR_DIREITO_PWM_PIN, VELOCIDADE_CURVA_MUITO_ALTA);
  }

  // PRIORIDADE 3 - CURVA MAIOR DIREITA (sensor_direita_maior)
  else if (estadoBotaoDD == LOW) {
    Serial.println("CURVA MUITO FORTE DIREITA");
    analogWrite(MOTOR_ESQUERDO_PWM_PIN, VELOCIDADE_CURVA_MUITO_ALTA);
    analogWrite(MOTOR_DIREITO_PWM_PIN, VELOCIDADE_CURVA_MUITO_BAIXA);
  }

  // PRIORIDADE 4 - CURVA MENOR ESQUERDA
  else if (estadoBotaoE == LOW) {
    Serial.println("CURVA LEVE ESQUERDA");
    analogWrite(MOTOR_ESQUERDO_PWM_PIN, VELOCIDADE_CURVA_BAIXA);
    analogWrite(MOTOR_DIREITO_PWM_PIN, VELOCIDADE_CURVA_ALTA);
  }

  // PRIORIDADE 5 - CURVA MENOR DIREITA
  else if (estadoBotaoD == LOW) {
    Serial.println("CURVA LEVE DIREITA");
    analogWrite(MOTOR_ESQUERDO_PWM_PIN, VELOCIDADE_CURVA_ALTA);
    analogWrite(MOTOR_DIREITO_PWM_PIN, VELOCIDADE_CURVA_BAIXA);
  }

  // PRIORIDADE 6 - NENHUM BOTÃO
  else {
    Serial.println("PARADO");
    analogWrite(MOTOR_ESQUERDO_PWM_PIN, 0);
    analogWrite(MOTOR_DIREITO_PWM_PIN, 0);
  }

  delay(50);
}
