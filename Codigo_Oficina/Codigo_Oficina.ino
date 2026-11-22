/*
 * ============================================================================
 * PROJETO: Seguidor de Linha com Cinco Sensores e Dois Motores
 * ============================================================================
 *
 * DESCRICAO
 * ---------
 * Programa de controle de um robo seguidor de linha que utiliza cinco
 * sensores digitais para identificar a posicao da linha e duas saidas PWM
 * para controlar a velocidade dos motores esquerdo e direito.
 *
 * ESTRATEGIA DE CONTROLE
 * ----------------------
 * As leituras dos sensores sao avaliadas em uma sequencia de prioridades:
 *
 * 1. Sensor central ativo: deslocamento em linha reta.
 * 2. Sensor extremo esquerdo ativo: curva acentuada para a esquerda.
 * 3. Sensor extremo direito ativo: curva acentuada para a direita.
 * 4. Sensor interno esquerdo ativo: curva leve para a esquerda.
 * 5. Sensor interno direito ativo: curva leve para a direita.
 * 6. Nenhum dos sensores ativo: parada dos motores.
 *
 * A estrutura if / else if faz com que apenas a primeira condicao verdadeira
 * seja executada em cada iteracao do loop.
 *
 * CONFIGURACAO DE HARDWARE
 * -----------------------
 * Sensores digitais:
 *   - Extremo esquerdo: pino 2
 *   - Interno esquerdo: pino 3
 *   - Central:          pino 5
 *   - Interno direito:  pino 6
 *   - Extremo direito:  pino 7
 *
 * Motores:
 *   - PWM do motor esquerdo: pino 10
 *   - PWM do motor direito:  pino 11
 *
 * Os sensores sao configurados como INPUT_PULLUP. Portanto, o programa
 * considera uma leitura LOW como deteccao/acionamento do sensor.
 *
 * PARAMETROS DE OPERACAO
 * ----------------------
 *   - Velocidade em linha reta: 200
 *   - Curva leve: combinacao de 150 e 250
 *   - Curva acentuada: combinacao de 120 e 255
 *   - Intervalo entre iteracoes: 50 ms
 *   - Comunicacao serial: 9600 baud
 *
 * OBSERVACAO
 * ----------
 * Este arquivo documenta a implementacao apresentada. Os pinos, valores de
 * velocidade, prioridades e comandos de controle foram mantidos.
 * A resposta fisica do robo depende dos sensores, dos motores, do driver e
 * da montagem utilizada.
 *
 * ============================================================================
 */


// ============================================================================
// 1. DEFINICAO DOS PINOS DOS SENSORES
// ============================================================================

const int sensor_esquerda_maior = 2;
const int sensor_esquerda_menor = 3;
const int sensor_meio = 5;
const int sensor_direita_menor = 6;
const int sensor_direita_maior = 7;


// ============================================================================
// 2. DEFINICAO DAS SAIDAS PWM DOS MOTORES
// ============================================================================

const int MOTOR_ESQUERDO_PWM_PIN = 10;
const int MOTOR_DIREITO_PWM_PIN = 11;


// ============================================================================
// 3. VARIAVEIS DE ESTADO DOS SENSORES
// ============================================================================

int estadoBotaoEE;
int estadoBotaoE;
int estadoBotaoMeio;
int estadoBotaoD;
int estadoBotaoDD;


// ============================================================================
// 4. PARAMETROS DE VELOCIDADE
// ============================================================================

// Valores aplicados aos motores durante o deslocamento em linha reta.
const int VELOCIDADE_RETA = 200;

// Diferenca de velocidade utilizada nas curvas leves.
const int VELOCIDADE_CURVA_ALTA = 250;
const int VELOCIDADE_CURVA_BAIXA = 150;

// Diferenca de velocidade utilizada nas curvas acentuadas.
const int VELOCIDADE_CURVA_MUITO_ALTA = 255;
const int VELOCIDADE_CURVA_MUITO_BAIXA = 120;


// ============================================================================
// 5. INICIALIZACAO DO SISTEMA
// ============================================================================

void setup() {
  // Configura os cinco sensores com os resistores pull-up internos.
  pinMode(sensor_esquerda_maior, INPUT_PULLUP);
  pinMode(sensor_esquerda_menor, INPUT_PULLUP);
  pinMode(sensor_meio, INPUT_PULLUP);
  pinMode(sensor_direita_menor, INPUT_PULLUP);
  pinMode(sensor_direita_maior, INPUT_PULLUP);

  // Configura as saidas de controle PWM dos motores.
  pinMode(MOTOR_ESQUERDO_PWM_PIN, OUTPUT);
  pinMode(MOTOR_DIREITO_PWM_PIN, OUTPUT);

  // Inicializa a comunicacao serial para acompanhar o estado do robo.
  Serial.begin(9600);
}


// ============================================================================
// 6. CICLO PRINCIPAL DE CONTROLE
// ============================================================================

void loop() {
  // --------------------------------------------------------------------------
  // 6.1. AQUISICAO DAS LEITURAS
  // --------------------------------------------------------------------------
  // Cada sensor e lido individualmente. Com INPUT_PULLUP, LOW representa
  // a condicao considerada ativa por esta implementacao.

  estadoBotaoEE = digitalRead(sensor_esquerda_maior);
  estadoBotaoE = digitalRead(sensor_esquerda_menor);
  estadoBotaoMeio = digitalRead(sensor_meio);
  estadoBotaoD = digitalRead(sensor_direita_menor);
  estadoBotaoDD = digitalRead(sensor_direita_maior);


  // --------------------------------------------------------------------------
  // 6.2. SELECAO DO MOVIMENTO POR PRIORIDADE
  // --------------------------------------------------------------------------
  // A ordem das condicoes determina qual leitura sera considerada quando
  // mais de um sensor estiver ativo simultaneamente.

  // PRIORIDADE 1: SENSOR CENTRAL - MOVIMENTO RETILINEO
  if (estadoBotaoMeio == LOW) {
    Serial.println("FRENTE");

    analogWrite(MOTOR_ESQUERDO_PWM_PIN, VELOCIDADE_RETA);
    analogWrite(MOTOR_DIREITO_PWM_PIN, VELOCIDADE_RETA);
  }

  // PRIORIDADE 2: SENSOR EXTREMO ESQUERDO - CURVA ACENTUADA
  else if (estadoBotaoEE == LOW) {
    Serial.println("CURVA MUITO FORTE ESQUERDA");

    analogWrite(MOTOR_ESQUERDO_PWM_PIN, VELOCIDADE_CURVA_MUITO_BAIXA);
    analogWrite(MOTOR_DIREITO_PWM_PIN, VELOCIDADE_CURVA_MUITO_ALTA);
  }

  // PRIORIDADE 3: SENSOR EXTREMO DIREITO - CURVA ACENTUADA
  else if (estadoBotaoDD == LOW) {
    Serial.println("CURVA MUITO FORTE DIREITA");

    analogWrite(MOTOR_ESQUERDO_PWM_PIN, VELOCIDADE_CURVA_MUITO_ALTA);
    analogWrite(MOTOR_DIREITO_PWM_PIN, VELOCIDADE_CURVA_MUITO_BAIXA);
  }

  // PRIORIDADE 4: SENSOR INTERNO ESQUERDO - CURVA LEVE
  else if (estadoBotaoE == LOW) {
    Serial.println("CURVA LEVE ESQUERDA");

    analogWrite(MOTOR_ESQUERDO_PWM_PIN, VELOCIDADE_CURVA_BAIXA);
    analogWrite(MOTOR_DIREITO_PWM_PIN, VELOCIDADE_CURVA_ALTA);
  }

  // PRIORIDADE 5: SENSOR INTERNO DIREITO - CURVA LEVE
  else if (estadoBotaoD == LOW) {
    Serial.println("CURVA LEVE DIREITA");

    analogWrite(MOTOR_ESQUERDO_PWM_PIN, VELOCIDADE_CURVA_ALTA);
    analogWrite(MOTOR_DIREITO_PWM_PIN, VELOCIDADE_CURVA_BAIXA);
  }

  // PRIORIDADE 6: NENHUM SENSOR ATIVO - PARADA
  else {
    Serial.println("PARADO");

    analogWrite(MOTOR_ESQUERDO_PWM_PIN, 0);
    analogWrite(MOTOR_DIREITO_PWM_PIN, 0);
  }


  // --------------------------------------------------------------------------
  // 6.3. INTERVALO ENTRE CICLOS
  // --------------------------------------------------------------------------
  // Aguarda antes de realizar uma nova leitura dos sensores.
  delay(50);
}
