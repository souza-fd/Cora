/*
 * ============================================================================
 * PROJETO: ROBÔ ARANHA — SEGUIDOR DE LINHA
 * ============================================================================
 *
 * DESCRIÇÃO GERAL
 * ----------------------------------------------------------------------------
 * Firmware de controle de um robô quadrúpede (quatro pernas), equipado com
 * servomotores acionados por um módulo controlador PCA9685 e um conjunto de
 * sete sensores infravermelhos TCRT5000 para detecção e acompanhamento de
 * uma linha.
 *
 * O programa realiza a leitura dos sensores, calcula uma estimativa do desvio
 * em relação à linha por meio de uma média ponderada e utiliza esse erro para
 * determinar ajustes nas amplitudes aplicadas à sequência de caminhada.
 *
 * ARQUITETURA DO SISTEMA
 * ----------------------------------------------------------------------------
 *  1. Sensoriamento: sete entradas digitais fornecem as leituras da linha.
 *  2. Processamento: os sensores ativos contribuem com pesos posicionais para
 *     o cálculo do erro médio.
 *  3. Controle: o erro é multiplicado pelo ganho proporcional e convertido
 *     em ajustes para os lados esquerdo ou direito da caminhada.
 *  4. Acionamento: o PCA9685 recebe os comandos PWM destinados aos servos das
 *     quatro pernas, cada uma representada por três segmentos articulados.
 *  5. Monitoramento: informações de leitura, erro e direção estimada são
 *     transmitidas pela comunicação serial.
 *
 * COMPONENTES E BIBLIOTECAS
 * ----------------------------------------------------------------------------
 *  - Placa compatível com o ambiente Arduino.
 *  - Módulo PCA9685 para geração dos sinais PWM dos servomotores.
 *  - 12 servomotores: base, meio e ponta de cada uma das quatro pernas.
 *  - 7 sensores infravermelhos TCRT5000 com saída digital.
 *  - Biblioteca Wire, utilizada na comunicação I2C.
 *  - Biblioteca Adafruit_PWMServoDriver, para controle do PCA9685.
 *
 * ORGANIZAÇÃO DAS PERNAS
 * ----------------------------------------------------------------------------
 *  PFE: perna frontal esquerda.
 *  PFD: perna frontal direita.
 *  PTE: perna traseira esquerda.
 *  PTD: perna traseira direita.
 *
 * Os canais do PCA9685 são organizados na matriz servoCanais, na ordem
 * [perna][segmento], sendo os segmentos: base, meio e ponta.
 *
 * LÓGICA DE DETECÇÃO DA LINHA
 * ----------------------------------------------------------------------------
 *  Os sete sensores possuem pesos posicionais de -9 a +9. Quando a leitura
 *  de um sensor é LOW, seu peso é incluído na soma e o sensor é contabilizado
 *  como ativo. O erro corresponde à média dos pesos dos sensores ativos.
 *  Caso nenhum sensor esteja ativo, o programa conserva o último erro válido.
 *
 * CONTROLE DE MOVIMENTO
 * ----------------------------------------------------------------------------
 *  A função seguirLinha() calcula o módulo do erro multiplicado pelo ganho
 *  proporcional. O sinal do erro define qual lado recebe o ajuste. Os valores
 *  resultantes são enviados à função passoPersonalizado(), que executa a
 *  sequência programada de posicionamento das pernas.
 *
 * INICIALIZAÇÃO E EXECUÇÃO
 * ----------------------------------------------------------------------------
 *  Na inicialização, a comunicação serial e o PCA9685 são configurados, os
 *  sensores são definidos como entradas e o robô assume a postura inicial.
 *  Em seguida, o loop principal repete a leitura, o cálculo do erro, o registro
 *  das informações e a execução do movimento de acompanhamento da linha.
 *
 * PARÂMETROS PRINCIPAIS DESTA VERSÃO
 * ----------------------------------------------------------------------------
 *  Frequência dos servos:       60 Hz
 *  Quantidade de sensores:      7
 *  Ganho proporcional:          15.0
 *  Faixa angular considerada:   0 a 180 graus
 *  Faixa de pulso configurada:  100 a 500
 *  Intervalo do loop:           25 ms, além dos atrasos da sequência de passo
 *
 * NOTAS DE IMPLEMENTAÇÃO
 * ----------------------------------------------------------------------------
 *  - Os pinos, canais, ângulos, pesos, tempos e ganhos abaixo correspondem aos
 *    parâmetros registrados nesta versão do projeto.
 *  - A interpretação de LOW como sensor ativo depende do módulo utilizado e
 *    de sua configuração elétrica.
 *  - A calibração mecânica dos servos e a montagem física podem exigir ajustes
 *    específicos; os parâmetros não são apresentados como universais.
 *  - A sequência de caminhada é definida diretamente em passoPersonalizado().
 *
 * ============================================================================
 */

/*
 * Projeto: Robô Aranha - Seguidor de Linha
 *
 * Descrição:
 * Controle de um robô quadrúpede equipado com servomotores e um
 * conjunto de sete sensores para acompanhamento de uma linha.
 *
 * O programa utiliza o módulo PCA9685 para controlar os servos,
 * calcula o desvio da linha a partir das leituras dos sensores e
 * aplica um ajuste proporcional às amplitudes do movimento.
 *
 * Bibliotecas:
 * - Wire
 * - Adafruit PWM Servo Driver
 *
 * Observação:
 * Os parâmetros de ângulo, deslocamento e temporização correspondem
 * aos valores definidos para esta versão do projeto.
 */

// 1. BIBLIOTECAS E INSTÂNCIA DO CONTROLADOR PWM
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// 2. CONFIGURAÇÃO GERAL DO PCA9685
const int SERVO_FREQUENCIA = 60;

// 3. MAPEAMENTO DOS CANAIS PWM POR PERNA E SEGMENTO
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

const int servoCanais[4][3] = {
  {PFE_BASE_CH, PFE_MEIO_CH, PFE_PONTA_CH},
  {PFD_BASE_CH, PFD_MEIO_CH, PFD_PONTA_CH},
  {PTE_BASE_CH, PTE_MEIO_CH, PTE_PONTA_CH},
  {PTD_BASE_CH, PTD_MEIO_CH, PTD_PONTA_CH}
};

// 4. PARÂMETROS DE POSTURA E MOVIMENTO
const int ANGULO_NEUTRO        = 90;
const int ANGULO_BASE_FRENTE   = 135;
const int ANGULO_MEIO_FRENTE   = 45;
const int ANGULO_ELEVACAO      = 10;
const int AMPLITUDE_PASSO      = 90;
const int ATRASO_PASSO_MS      = 20;

// 5. OFFSETS DE AJUSTE DOS SEGMENTOS
const int OFFSET_GARRA         = 50;
const int OFFSET_FRENTE        = 15;
const int OFFSET_TRAS          = 20;
const int AJUSTE_TRAS_EXTRA    = 15;

// 6. CONFIGURAÇÃO DOS SENSORES E PESOS POSICIONAIS
#define NUM_SENSORES 7
const int pinosSensores[NUM_SENSORES]   = {8, 7, 6, 5, 4, 3, 2};
const int pesosSensores[NUM_SENSORES]   = {-9, -7, -3, 0, 3, 7, 9};

float ganhoProporcional = 15.0;
float erroAnterior = 0;

// 7. CONVERSÃO ENTRE ÂNGULO E PULSO PWM
const int PULSO_MIN = 100;
const int PULSO_MAX = 500;

// 8. INICIALIZAÇÃO DOS PERIFÉRICOS E POSTURA INICIAL
void setup() {
  Serial.begin(9600);
  Serial.println("=== Robô Aranha - Seguidor de Linha com Controle Inverso ===");

  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQUENCIA);
  delay(500);

  for (int i = 0; i < NUM_SENSORES; i++) {
    pinMode(pinosSensores[i], INPUT);
  }

  colocarEmPe();
  delay(2000);
}

// 9. CICLO PRINCIPAL: LEITURA, MONITORAMENTO E MOVIMENTO
void loop() {

  int leituraSensores[NUM_SENSORES];
  for (int i = 0; i < NUM_SENSORES; i++) {
    leituraSensores[i] = digitalRead(pinosSensores[i]);
  }

  // Estima o desvio atual e prepara a indicação textual para o monitor serial.
  float erro = calcularErroLinha();

  String direcao = "Frente";
  if (erro > 0.5) direcao = "Direita";
  else if (erro < -0.5) direcao = "Esquerda";
  else if (erro == 0) direcao = "Alinhado";

  Serial.print("Sensores: ");
  for (int i = 0; i < NUM_SENSORES; i++) {
    Serial.print(leituraSensores[i]);
    if (i < NUM_SENSORES - 1) Serial.print(" | ");
  }
  Serial.print("  -> Erro: ");
  Serial.print(erro);
  Serial.print("  -> Direcao: ");
  Serial.println(direcao);

  seguirLinha();

  delay(25);
}

// 10. POSICIONAMENTO INICIAL DAS QUATRO PERNAS
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

// 11. CÁLCULO DO ERRO MÉDIO PONDERADO DA LINHA
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

  // Sem sensores ativos, mantém a última estimativa disponível.
  if (totalAtivo == 0) return erroAnterior;

  float erro = ((float)somaPesos / totalAtivo);
  erroAnterior = erro;
  return erro;
}

// 12. CONTROLE PROPORCIONAL DO DESLOCAMENTO
void seguirLinha() {
  float erro = calcularErroLinha();
  // A intensidade do ajuste é proporcional ao módulo do erro calculado.
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

// 13. SEQUÊNCIA PERSONALIZADA DE CAMINHADA
void passoPersonalizado(int ampEsquerda, int ampDireita) {

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

  definirAngulosPerna(1, ANGULO_MEIO_FRENTE, ANGULO_NEUTRO, ANGULO_NEUTRO + OFFSET_GARRA);
  definirAngulosPerna(0, ANGULO_BASE_FRENTE, ANGULO_NEUTRO, ANGULO_NEUTRO + OFFSET_GARRA - OFFSET_FRENTE);
  definirAngulosPerna(3, ANGULO_BASE_FRENTE, ANGULO_NEUTRO + OFFSET_TRAS + AMPLITUDE_PASSO, ANGULO_NEUTRO - OFFSET_GARRA);
  definirAngulosPerna(2, ANGULO_MEIO_FRENTE + 20, ANGULO_NEUTRO + OFFSET_TRAS + AMPLITUDE_PASSO, ANGULO_NEUTRO - OFFSET_GARRA);
  delay(ATRASO_PASSO_MS + 200);

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

// 14. APLICAÇÃO DOS ÂNGULOS AOS SERVOS DE UMA PERNA
void definirAngulosPerna(int perna, int angBase, int angMeio, int angPonta) {

  // Inverte os ângulos dos segmentos meio e ponta nas pernas direitas.
  if (perna == 1 || perna == 3) {
    angMeio  = 180 - angMeio;
    angPonta = 180 - angPonta;
  }

  pwm.setPWM(servoCanais[perna][0], 0, anguloParaPulso(angBase));
  pwm.setPWM(servoCanais[perna][1], 0, anguloParaPulso(angMeio));
  pwm.setPWM(servoCanais[perna][2], 0, anguloParaPulso(angPonta));
}

// 15. CONVERSÃO DE ÂNGULO EM PULSO PWM PARA O PCA9685
long anguloParaPulso(int angulo) {
  angulo = constrain(angulo, 0, 180);
  return map(angulo, 0, 180, PULSO_MIN, PULSO_MAX);
}
