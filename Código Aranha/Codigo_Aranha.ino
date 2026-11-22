#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>


Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();


// ============================================================================
//  CONFIGURAÇÕES GERAIS E CONSTANTES
// ============================================================================


// Frequência de atualização do servo (Hz)
const int SERVO_FREQUENCIA = 60;


// ============================================================================
//  MAPEAMENTO DE CANAIS DOS SERVOS (PCA9685)
// ============================================================================
// PFE = Perna Frontal Esquerda
// PFD = Perna Frontal Direita
// PTE = Perna Traseira Esquerda
// PTD = Perna Traseira Direita


#define PTE_MEIO_CH   8
#define PTE_BASE_CH   10
#define PTE_PONTA_CH  11


#define PFD_MEIO_CH   6
#define PFD_BASE_CH   5
#define PFD_PONTA_CH  4


#define PFE_MEIO_CH   13
#define PFE_BASE_CH   14
#define PFE_PONTA_CH  15


#define PTD_MEIO_CH   2
#define PTD_BASE_CH   1
#define PTD_PONTA_CH  0


// Mapeamento [perna][segmento]: base, meio, ponta
const int servoCanais[4][3] = {
  {PFE_BASE_CH, PFE_MEIO_CH, PFE_PONTA_CH},
  {PFD_BASE_CH, PFD_MEIO_CH, PFD_PONTA_CH},
  {PTE_BASE_CH, PTE_MEIO_CH, PTE_PONTA_CH},
  {PTD_BASE_CH, PTD_MEIO_CH, PTD_PONTA_CH}
};


// ============================================================================
//  PARÂMETROS DE MOVIMENTO / POSTURA
// ============================================================================


const int ANGULO_NEUTRO        = 90;   // Posição central do servo
const int ANGULO_BASE_FRENTE   = 135;
const int ANGULO_MEIO_FRENTE   = 45;
const int ANGULO_ELEVACAO      = 10;   // Altura de levantamento da perna
const int AMPLITUDE_PASSO      = 90;   // Alcance de cada passo
const int ATRASO_PASSO_MS      = 20;   // Tempo de atraso entre estágios


// ============================================================================
//  OFFSETS DE AJUSTE DE ÂNGULO
// ============================================================================


const int OFFSET_GARRA         = 50;
const int OFFSET_FRENTE        = 15;
const int OFFSET_TRAS          = 20;
const int AJUSTE_TRAS_EXTRA    = 15;


// ============================================================================
//  CONFIGURAÇÃO DOS SENSORES (TCRT5000)
// ============================================================================


#define NUM_SENSORES 7
const int pinosSensores[NUM_SENSORES]   = {8, 7, 6, 5, 4, 3, 2};
const int pesosSensores[NUM_SENSORES]   = {-9, -7, -3, 0, 3, 7, 9};


float ganhoProporcional = 15.0;
float erroAnterior = 0;


// ============================================================================
//  MAPEAMENTO DE PULSO PWM (0–180° → 100–500)
// ============================================================================
const int PULSO_MIN = 100;
const int PULSO_MAX = 500;


// ============================================================================
//  SETUP
// ============================================================================
void setup() {
  Serial.begin(9600);
  Serial.println("=== Robô Aranha - Seguidor de Linha com Controle Inverso ===");


  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQUENCIA);
  delay(500);


  // Inicializa sensores como entrada
  for (int i = 0; i < NUM_SENSORES; i++) {
    pinMode(pinosSensores[i], INPUT);
  }


  colocarEmPe();
  delay(2000);
}


// ============================================================================
//  LOOP PRINCIPAL
// ============================================================================
void loop() {
  // Leitura dos sensores
  int leituraSensores[NUM_SENSORES];
  for (int i = 0; i < NUM_SENSORES; i++) {
    leituraSensores[i] = digitalRead(pinosSensores[i]);
  }


  // Cálculo do erro
  float erro = calcularErroLinha();


  // Direção estimada (debug)
  String direcao = "Frente";
  if (erro > 0.5) direcao = "Direita";
  else if (erro < -0.5) direcao = "Esquerda";
  else if (erro == 0) direcao = "Alinhado";


  // Log Serial
  Serial.print("Sensores: ");
  for (int i = 0; i < NUM_SENSORES; i++) {
    Serial.print(leituraSensores[i]);
    if (i < NUM_SENSORES - 1) Serial.print(" | ");
  }
  Serial.print("  -> Erro: ");
  Serial.print(erro);
  Serial.print("  -> Direcao: ");
  Serial.println(direcao);


  // Função de movimento (opcional)
  seguirLinha();
  //colocarEmPe();


  delay(25);
}


// ============================================================================
//  COLOCAR ROBÔ EM PÉ
// ============================================================================
void colocarEmPe() {
  for (int perna = 0; perna < 4; perna++) {
    switch (perna) {
      case 0:
        definirAngulosPerna(perna, ANGULO_BASE_FRENTE, ANGULO_NEUTRO, ANGULO_NEUTRO + OFFSET_GARRA);
        break;
      case 1:
        definirAngulosPerna(perna, ANGULO_MEIO_FRENTE, ANGULO_NEUTRO, ANGULO_NEUTRO + OFFSET_GARRA);
        break;
      case 2:
        definirAngulosPerna(perna, ANGULO_MEIO_FRENTE, ANGULO_NEUTRO + OFFSET_TRAS, ANGULO_NEUTRO - OFFSET_GARRA);
        break;
      case 3:
        definirAngulosPerna(perna, ANGULO_BASE_FRENTE, ANGULO_NEUTRO + OFFSET_TRAS + AJUSTE_TRAS_EXTRA, ANGULO_NEUTRO - OFFSET_GARRA);
        break;
    }
  }
}


// ============================================================================
//  CÁLCULO DO ERRO (SENSORES DE LINHA)
// ============================================================================
float calcularErroLinha() {
  int totalAtivo = 0;
  int somaPesos = 0;


  for (int i = 0; i < NUM_SENSORES; i++) {
    int leitura = digitalRead(pinosSensores[i]);
    if (leitura == LOW) {
      somaPesos += pesosSensores[i];
      totalAtivo++;
    }
  }


  if (totalAtivo == 0) return erroAnterior;


  float erro = ((float)somaPesos / totalAtivo);
  erroAnterior = erro;
  return erro;
}


// ============================================================================
//  CONTROLE P INVERSO (MOVIMENTO PROPORCIONAL)
// ============================================================================
void seguirLinha() {
  float erro = calcularErroLinha();
  float ajuste = ganhoProporcional * abs(erro);


  int amplitudeEsquerda = 0;
  int amplitudeDireita = 0;


  if (erro < 0) {
    amplitudeEsquerda = ajuste + 1;
  } else if (erro > 0) {
    amplitudeDireita = ajuste + 1;
  }


  passoPersonalizado(amplitudeEsquerda, amplitudeDireita);
}


// ============================================================================
//  FUNÇÃO COMPLETA DE CAMINHADA PERSONALIZADA
//  (mantida 100% original — apenas nomes e constantes foram ajustados)
// ============================================================================
void passoPersonalizado(int ampEsquerda, int ampDireita) {


  // PERNAS DIANTEIRAS
  definirAngulosPerna(1, ANGULO_MEIO_FRENTE, ANGULO_NEUTRO - OFFSET_FRENTE, ANGULO_NEUTRO + OFFSET_GARRA);
  delay(ATRASO_PASSO_MS);


  definirAngulosPerna(1, ANGULO_ELEVACAO, ANGULO_NEUTRO, ANGULO_NEUTRO + OFFSET_GARRA);
  delay(300);


  definirAngulosPerna(1, ANGULO_ELEVACAO, ANGULO_NEUTRO, ANGULO_NEUTRO + OFFSET_GARRA);
  delay(ATRASO_PASSO_MS + 20);


  definirAngulosPerna(1, ANGULO_ELEVACAO, ANGULO_NEUTRO - AMPLITUDE_PASSO + ampDireita, ANGULO_NEUTRO + OFFSET_GARRA);
  delay(ATRASO_PASSO_MS + 100);


  definirAngulosPerna(1, ANGULO_MEIO_FRENTE, ANGULO_NEUTRO - AMPLITUDE_PASSO + ampDireita, ANGULO_NEUTRO + OFFSET_GARRA);
  delay(ATRASO_PASSO_MS );


  definirAngulosPerna(0, ANGULO_BASE_FRENTE, ANGULO_NEUTRO + OFFSET_FRENTE, ANGULO_NEUTRO - OFFSET_FRENTE);
  delay(ATRASO_PASSO_MS);


  definirAngulosPerna(0, (ANGULO_BASE_FRENTE * 2) + ANGULO_ELEVACAO, ANGULO_NEUTRO, ANGULO_NEUTRO - OFFSET_FRENTE + OFFSET_GARRA);
  delay(150);


  definirAngulosPerna(0, (ANGULO_BASE_FRENTE * 2) + ANGULO_ELEVACAO, ANGULO_NEUTRO, ANGULO_NEUTRO + OFFSET_GARRA - OFFSET_FRENTE);
  delay(ATRASO_PASSO_MS);


  definirAngulosPerna(0, (ANGULO_BASE_FRENTE * 2) + ANGULO_ELEVACAO, ANGULO_NEUTRO - AMPLITUDE_PASSO + ampEsquerda, ANGULO_NEUTRO + OFFSET_GARRA - OFFSET_FRENTE);
  delay(ATRASO_PASSO_MS);


  definirAngulosPerna(0, ANGULO_BASE_FRENTE, ANGULO_NEUTRO - AMPLITUDE_PASSO + ampEsquerda, ANGULO_NEUTRO + OFFSET_GARRA - OFFSET_FRENTE);
  delay(ATRASO_PASSO_MS + 200);


  // APOIO TRASEIRO
  definirAngulosPerna(1, ANGULO_MEIO_FRENTE, ANGULO_NEUTRO, ANGULO_NEUTRO + OFFSET_GARRA);
  definirAngulosPerna(0, ANGULO_BASE_FRENTE, ANGULO_NEUTRO, ANGULO_NEUTRO + OFFSET_GARRA - OFFSET_FRENTE);
  definirAngulosPerna(3, ANGULO_BASE_FRENTE, ANGULO_NEUTRO + OFFSET_TRAS + AMPLITUDE_PASSO, ANGULO_NEUTRO - OFFSET_GARRA);
  definirAngulosPerna(2, ANGULO_MEIO_FRENTE + 20, ANGULO_NEUTRO + OFFSET_TRAS + AMPLITUDE_PASSO, ANGULO_NEUTRO - OFFSET_GARRA);
  delay(ATRASO_PASSO_MS + 200);


  // PERNAS TRASEIRAS
  definirAngulosPerna(3, (ANGULO_BASE_FRENTE * 2) - ANGULO_ELEVACAO, ANGULO_NEUTRO + OFFSET_TRAS + AMPLITUDE_PASSO, ANGULO_NEUTRO - OFFSET_GARRA);
  delay(ATRASO_PASSO_MS);


  definirAngulosPerna(3, (ANGULO_BASE_FRENTE * 2) - ANGULO_ELEVACAO, ANGULO_NEUTRO + OFFSET_TRAS + AMPLITUDE_PASSO, ANGULO_NEUTRO - OFFSET_GARRA);
  delay(ATRASO_PASSO_MS);


  definirAngulosPerna(3, (ANGULO_BASE_FRENTE * 2) - ANGULO_ELEVACAO, ANGULO_NEUTRO + OFFSET_TRAS, ANGULO_NEUTRO - OFFSET_GARRA);
  delay(ATRASO_PASSO_MS);


  definirAngulosPerna(3, (ANGULO_BASE_FRENTE * 2) - ANGULO_ELEVACAO, ANGULO_NEUTRO + OFFSET_TRAS, ANGULO_NEUTRO - OFFSET_GARRA);
  delay(ATRASO_PASSO_MS);


  definirAngulosPerna(3, ANGULO_BASE_FRENTE, ANGULO_NEUTRO + OFFSET_TRAS + 20, ANGULO_NEUTRO - OFFSET_GARRA);
  delay(ATRASO_PASSO_MS);


  definirAngulosPerna(2, ANGULO_ELEVACAO + 15, ANGULO_NEUTRO + OFFSET_TRAS + AMPLITUDE_PASSO, ANGULO_NEUTRO - OFFSET_GARRA);
  delay(ATRASO_PASSO_MS);


  definirAngulosPerna(2, ANGULO_ELEVACAO + 15, ANGULO_NEUTRO + OFFSET_TRAS + AMPLITUDE_PASSO, ANGULO_NEUTRO - OFFSET_GARRA);
  delay(ATRASO_PASSO_MS);


  definirAngulosPerna(2, ANGULO_ELEVACAO + 15, ANGULO_NEUTRO + OFFSET_TRAS, ANGULO_NEUTRO - OFFSET_GARRA);
  delay(ATRASO_PASSO_MS);


  definirAngulosPerna(2, ANGULO_ELEVACAO + 15, ANGULO_NEUTRO + OFFSET_TRAS, ANGULO_NEUTRO - OFFSET_GARRA);
  delay(ATRASO_PASSO_MS);


  definirAngulosPerna(2, ANGULO_MEIO_FRENTE + 20, ANGULO_NEUTRO + OFFSET_TRAS, ANGULO_NEUTRO - OFFSET_GARRA);
  delay(ATRASO_PASSO_MS + 100);
}


// ============================================================================
//  DEFINIÇÃO DE ÂNGULOS PARA CADA PERNA
// ============================================================================
void definirAngulosPerna(int perna, int angBase, int angMeio, int angPonta) {
  // Espelha o movimento para as pernas do lado direito
  if (perna == 1 || perna == 3) {
    angMeio  = 180 - angMeio;
    angPonta = 180 - angPonta;
  }


  pwm.setPWM(servoCanais[perna][0], 0, anguloParaPulso(angBase)); // Base
  pwm.setPWM(servoCanais[perna][1], 0, anguloParaPulso(angMeio)); // Meio
  pwm.setPWM(servoCanais[perna][2], 0, anguloParaPulso(angPonta)); // Ponta
}


// ============================================================================
//  CONVERSÃO DE ÂNGULO PARA PULSO PWM
// ============================================================================
long anguloParaPulso(int angulo) {
  angulo = constrain(angulo, 0, 180);
  return map(angulo, 0, 180, PULSO_MIN, PULSO_MAX);
}



