/*
 * ============================================================================
 * PROJETO: ROBÔ SEGUIDOR DE LINHA COM ESP32
 * ============================================================================
 *
 * DESCRIÇÃO
 * Sistema de navegação para robô móvel de dois motores, baseado na leitura
 * de sete sensores infravermelhos e no controle proporcional-derivativo-
 * integral (PID). O programa também utiliza uma máquina de estados para
 * executar manobras específicas, atravessar cruzamentos e tratar a detecção
 * de uma faixa de pedestres.
 *
 * PLATAFORMA E COMPONENTES
 * - Microcontrolador: ESP32
 * - Acionamento dos motores: ponte H com entradas de direção e PWM
 * - Sensores: sete sensores infravermelhos digitais
 * - Atuadores: dois motores DC
 *
 * ORGANIZAÇÃO DO PROGRAMA
 * 1. Mapeamento dos motores e parâmetros de velocidade
 * 2. Configuração do controlador PID
 * 3. Mapeamento dos sensores infravermelhos
 * 4. Funções de acionamento e controle dos motores
 * 5. Inicialização dos periféricos
 * 6. Leitura dos sensores e execução da máquina de estados
 *
 * ESTRATÉGIA DE CONTROLE
 * O acompanhamento da linha é realizado a partir de um erro estimado pela
 * posição dos sensores ativos. Esse erro alimenta o controlador PID, que
 * calcula a correção aplicada às velocidades dos motores.
 *
 * A máquina de estados coordena o comportamento do robô em situações
 * específicas, como cruzamentos, identificação de quadrados, manobras de
 * retorno e sequência de travessia da faixa de pedestres.
 *
 * OBSERVAÇÕES
 * - As leituras dos sensores são invertidas no loop por meio do operador !.
 * - Os valores de velocidade, ganhos PID e temporizações correspondem aos
 *   parâmetros definidos nesta versão do projeto.
 * - A lógica de movimentação foi preservada; esta documentação não representa
 *   uma validação do comportamento físico do robô.
 *
 * ============================================================================
 */

// ============================================================================
// 1. MAPEAMENTO DOS MOTORES E PARÂMETROS DE VELOCIDADE
// ============================================================================

#define PwmA 25 // ENA (Motor Direito)
#define IN1 26 // IN1
#define IN2 23 // IN2
#define IN3 22 // IN3
#define IN4 21 // IN4
#define PwmB 19 // ENB (Motor Esquerdo)

// Calibragem de velocidades
#define velA 130 // Velocidade para retas
#define velB 130
#define velCurva 85 // Velocidade para manobras gerais
#define velGiro 125 // Velocidade de giro para curvas de 90°
#define velGiroUmaRoda 160 // Velocidade para giro em uma roda após a ré


// ============================================================================
// 2. PARÂMETROS DO CONTROLADOR PID
// ============================================================================

float Kp = 25.0;
float Ki = 0.0;
float Kd = 15.0;

float P = 0, I = 0, D = 0, PID = 0;
float erroAnterior = 0;


// ============================================================================
// 3. MAPEAMENTO DOS SENSORES INFRAVERMELHOS
// ============================================================================

#define S1 36 // VP (Ponta Esquerda)
#define S2 39 // VN
#define S3 34
#define S4 35 // Central
#define S5 32
#define S6 33
#define S7 27 // Ponta Direita


// ============================================================================
// 4. FUNÇÕES DE CONTROLE DE MOVIMENTO
// ============================================================================

// Aciona os dois motores no sentido de avanço, com velocidades independentes.
void moverFrente(int velD, int velE) {
  analogWrite(PwmA, constrain(velD, 0, 255));
  analogWrite(PwmB, constrain(velE, 0, 255));
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN4, HIGH); digitalWrite(IN3, LOW);
}

// Aciona os dois motores no sentido de ré, com velocidades independentes.
void moverRe(int velD, int velE) {
  analogWrite(PwmA, constrain(velD, 0, 255));
  analogWrite(PwmB, constrain(velE, 0, 255));
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN4, LOW); digitalWrite(IN3, HIGH);
}

// Executa rotação para a direita, acionando os motores em sentidos opostos.
void girarDireita(int vel) {
  analogWrite(PwmA, constrain(vel, 0, 255));
  analogWrite(PwmB, constrain(vel, 0, 255));
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); // Motor Direito Ré
  digitalWrite(IN4, HIGH); digitalWrite(IN3, LOW); // Motor Esquerdo Frente
}

// Executa rotação para a esquerda, acionando os motores em sentidos opostos.
void girarEsquerda(int vel) {
  analogWrite(PwmA, constrain(vel, 0, 255));
  analogWrite(PwmB, constrain(vel, 0, 255));
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); // Motor Direito Frente
  digitalWrite(IN4, LOW); digitalWrite(IN3, HIGH); // Motor Esquerdo Ré
}

// Interrompe o acionamento PWM e coloca as entradas de direção em nível LOW.
void pararMotores() {
  analogWrite(PwmA, 0);
  analogWrite(PwmB, 0);
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

// Calcula a ação PID a partir do erro recebido e ajusta as velocidades.
void seguirLinha(float erro) {
  P = erro;
  I = I + erro;
  D = erro - erroAnterior;
  PID = (Kp * P) + (Ki * I) + (Kd * D);
  erroAnterior = erro;

  int velBase = (erro != 0) ? 80 : velA;
  int pmwMotorDireito = velBase - PID;
  int pmwMotorEsquerdo = velBase + PID;

  moverFrente(pmwMotorDireito, pmwMotorEsquerdo);
}


// ============================================================================
// 5. INICIALIZAÇÃO DO SISTEMA
// ============================================================================

void setup() {
  Serial.begin(115200);

  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  pinMode(S3, INPUT);
  pinMode(S4, INPUT);
  pinMode(S5, INPUT);
  pinMode(S6, INPUT);
  pinMode(S7, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(PwmA, OUTPUT);
  pinMode(PwmB, OUTPUT);
}


// ============================================================================
// 6. VARIÁVEIS DE CONTROLE DA MÁQUINA DE ESTADOS
// ============================================================================

int cont = 0;
int estado = 0;
unsigned long tempoInicio = 0;


// ============================================================================
// 7. LOOP PRINCIPAL: LEITURA, INTERPRETAÇÃO E CONTROLE
// ============================================================================

void loop() {
  float erro = 0;

  // Leitura dos sensores com inversão lógica.
  int sensor1 = !digitalRead(S1);
  int sensor2 = !digitalRead(S2);
  int sensor3 = !digitalRead(S3);
  int sensor4 = !digitalRead(S4);
  int sensor5 = !digitalRead(S5);
  int sensor6 = !digitalRead(S6);
  int sensor7 = !digitalRead(S7);

  // Contagem de sensores ativos para identificação de padrões.
  cont = sensor1 + sensor2 + sensor3 + sensor4 + sensor5 + sensor6 + sensor7;

  // ==========================================================================
  // ESTADO 0: ACOMPANHAMENTO NORMAL DA LINHA POR PID
  // ==========================================================================

  if (estado == 0) {
    if (sensor4 == 1) {
      if (sensor3 == 1) erro = -0.5;
      else if (sensor5 == 1) erro = +0.5;
      else erro = 0;
    } else if (sensor3 == 1) {
      erro = -1.0;
    } else if (sensor5 == 1) {
      erro = +1.0;
    } else if (sensor2 == 1) {
      erro = -2.0;
    } else if (sensor6 == 1) {
      erro = +2.0;
    } else if (sensor1 == 1) {
      erro = -3.0;
    } else if (sensor7 == 1) {
      erro = +3.0;
    }

    seguirLinha(erro);

    // Transições para situações especiais da pista.
    if((sensor1 == 1 && sensor2 == 1 && sensor3 == 1 && sensor4 == 1) || (sensor4 == 1 && sensor5 == 1 &&
    sensor6 == 1 && sensor7 == 1))
      estado = 3; // Encruzilhada
    else if(sensor1 == 1 && (sensor2 == 0 || sensor3 == 0) && sensor4 == 1 && sensor6 == 0 && sensor7 == 0)
      estado = 27; // Quadrado na esquerda
    else if(sensor1 == 0 && sensor2 == 0 && sensor4 == 1 && (sensor5 == 0 || sensor6 == 0) && sensor7 == 1)
      estado = 6; // Quadrado na direita
    else if(cont >= 6 && (sensor3 == 0 || sensor4 == 0 || sensor5 == 0))
      estado = 52; // Faixa de pedestres (pista invertida)

  // Estados auxiliares para alinhamento por rotação.
  } else if (estado == 1) {
    girarEsquerda(velGiro);
    if (sensor4 == 1) estado = 0;

  } else if (estado == 2) {
    girarDireita(velGiro);
    if (sensor4 == 1) estado = 0;

  // ==========================================================================
  // ESTADO 3: TRAVESSIA RETA DE CRUZAMENTO
  // ==========================================================================

  } else if (estado == 3) {
    moverFrente(100, 100);

    if (sensor1 == 0 && sensor2 == 0 && sensor6 == 0 && sensor7 == 0) {
      estado = 0;
    } else if(cont >= 6 && (sensor3 == 0 || sensor4 == 0 || sensor5 == 0)) {
      estado = 52; // Faixa de pedestres (pista invertida)
    }

  // ==========================================================================
  // ESTADOS 6 E 7: IDENTIFICAÇÃO E SAÍDA DO QUADRADO À DIREITA
  // ==========================================================================

  // Leitura do quadrado da direita.
  } else if(estado == 6){
    moverFrente(100, 100);

    if(sensor4 == 1 && sensor5 == 1 && sensor6 == 1 && sensor7 == 1)
      estado = 3;
    else if (sensor1 == 0 && sensor2 == 0 && (sensor3 == 1 || sensor4 == 1 || sensor5 == 1) && sensor6 == 0 &&
    sensor7 == 0) {
      estado = 7;
    } else if(cont >= 6 && (sensor3 == 0 || sensor4 == 0 || sensor5 == 0)) {
      estado = 52; // Faixa de pedestres (pista invertida)
    }

  // Após a passagem pelo quadrado da direita.
  } else if(estado == 7){
    if (sensor4 == 1) {
      if (sensor3 == 1) erro = -0.5;
      else if (sensor5 == 1) erro = +0.5;
      else erro = 0;
    } else if (sensor3 == 1) erro = -1.0;
    else if (sensor5 == 1) erro = +1.0;
    else if (sensor2 == 1) erro = -2.0;
    else if (sensor6 == 1) erro = +2.0;
    else if (sensor1 == 1) erro = -3.0;
    else if (sensor7 == 1) erro = +3.0;

    seguirLinha(erro);

    if(sensor4 == 1 && sensor5 == 1 && sensor6 == 1 && sensor7 == 1) {
      tempoInicio = millis();
      estado = 8; // Virar pra direita
    } else if(sensor1 == 1 && (sensor2 == 0 || sensor3 == 0) && sensor4 == 1 && sensor6 == 0 && sensor7 == 0) {
      estado = 48; // Leu quadrado na esquerda -> ré
    }

  // ==========================================================================
  // ESTADO 8: MANOBRA DE 90 GRAUS À DIREITA
  // ==========================================================================

  } else if(estado == 8){
    moverFrente(120, 120);
    delay(100);
    moverFrente(0, 150); // Giro com 1 roda (Direita para frente)
    delay(500);
    estado = 0;

  // ==========================================================================
  // ESTADOS 27, 28 E 29: IDENTIFICAÇÃO E MANOBRA À ESQUERDA
  // ==========================================================================

  // Leitura do quadrado da esquerda.
  } else if(estado == 27){
    moverFrente(100, 100);

    if(sensor1 == 1 && sensor2 == 1 && sensor3 == 1 && sensor4 == 1)
      estado = 3;
    else if (sensor1 == 0 && sensor2 == 0 && (sensor3 == 1 || sensor4 == 1 || sensor5 == 1) && sensor6 == 0 &&
    sensor7 == 0) {
      estado = 28;
    } else if(cont >= 6 && (sensor3 == 0 || sensor4 == 0 || sensor5 == 0)) {
      estado = 52; // Faixa de pedestres (pista invertida)
    }

  // Após a passagem pelo quadrado da esquerda.
  } else if(estado == 28){
    if (sensor4 == 1) {
      if (sensor3 == 1) erro = -0.5;
      else if (sensor5 == 1) erro = +0.5;
      else erro = 0;
    } else if (sensor3 == 1) erro = -1.0;
    else if (sensor5 == 1) erro = +1.0;
    else if (sensor2 == 1) erro = -2.0;
    else if (sensor6 == 1) erro = +2.0;
    else if (sensor1 == 1) erro = -3.0;
    else if (sensor7 == 1) erro = +3.0;

    seguirLinha(erro);

    if(sensor1 == 1 && sensor2 == 1 && sensor3 == 1 && sensor4 == 1) {
      tempoInicio = millis();
      estado = 29; // Virar pra esquerda
    } else if(sensor1 == 0 && sensor2 == 0 && sensor4 == 1 && (sensor5 == 0 || sensor6 == 0) && sensor7 == 1) {
      estado = 50; // Leu quadrado na direita -> ré
    }

  // Estado 29: manobra de 90 graus à esquerda.
  } else if(estado == 29){
    moverFrente(120, 120);
    delay(150);
    moverFrente(150, 0); // Giro com 1 roda (Direita para frente)
    delay(400);
    estado = 0;

  // ==========================================================================
  // ESTADOS 48 A 51: MANOBRAS DE RETORNO EM RÉ
  // ==========================================================================

  // Ré à esquerda: identificação do quadrado à esquerda.
  } else if(estado == 48){
    if(sensor1 == 1 && sensor2 == 1 && sensor3 == 1 && sensor4 == 1) {
      tempoInicio = millis();
      estado = 8;
    } else if(sensor1 == 0 && sensor2 == 0 && (sensor3 == 1 || sensor4 == 1 || sensor5 == 1) && sensor6 == 0 &&
    sensor7 == 0) {
      estado = 49; // Voltando de ré
    }

  // Retorno em ré e giro com uma roda.
  } else if(estado == 49){
    moverRe(150, 150);

    if(sensor1 == 1 && sensor2 == 1 && sensor3 == 1 && sensor4 == 1){
      pararMotores();
      delay(200);
      moverFrente(120, 120);
      delay(300);
      moverFrente(150, 0); // Giro com 1 roda (Direita para frente)
      delay(400);
      estado = 0;
    }

  // Ré à direita: identificação do quadrado à direita.
  } else if(estado == 50){
    moverFrente(100, 100);

    if(sensor4 == 1 && sensor5 == 1 && sensor6 == 1 && sensor7 == 1) {
      tempoInicio = millis();
      estado = 29;
    } else if (sensor1 == 0 && sensor2 == 0 && (sensor3 == 1 || sensor4 == 1 || sensor5 == 1) && sensor6 == 0 &&
    sensor7 == 0) {
      estado = 51; // Voltando de ré
    }

  // Retorno em ré e giro com uma roda.
  } else if(estado == 51){
    moverRe(120, 120);

    if(sensor4 == 1 && sensor5 == 1 && sensor6 == 1 && sensor7 == 1){
      pararMotores();
      delay(200);
      moverFrente(100, 100);
      delay(100);
      moverFrente(50, 120); // Giro com 1 roda (Esquerda para frente)
      delay(500);
      estado = 0;
    }

  // ==========================================================================
  // ESTADOS 52 A 58: DETECÇÃO E TRAVESSIA DA FAIXA DE PEDESTRES
  // ==========================================================================

  // Entrada na faixa de pedestres / pista invertida.
  } else if(estado == 52){
    moverFrente(150, 150);

    if(cont <= 3){
      estado = 53; // Seguir posicionando
    }

  // Posicionamento e acompanhamento da linha na faixa.
  } else if(estado == 53){
    if (sensor4 == 1) {
      if (sensor3 == 1) erro = -0.5;
      else if (sensor5 == 1) erro = +0.5;
      else erro = 0;
    } else if (sensor3 == 1) {
      erro = -1.0;
    } else if (sensor5 == 1) {
      erro = +1;
    } else if (sensor2 == 1) {
      erro = -2.0;
    } else if (sensor6 == 1) {
      erro = +2;
    } else if (sensor1 == 1) {
      erro = -3.0;
    } else if (sensor7 == 1) {
      erro = +3;
    }

    seguirLinha(erro);

    if(sensor1 == 0 && sensor2 == 0 && sensor3 == 0 && sensor4 == 0 && sensor5 == 0 && sensor6 == 0 && sensor7
    == 0)
      estado = 54; // Parar por 5s

  // Parada temporizada.
  } else if(estado == 54){
    moverFrente(0, 0);
    delay(5000);
    estado = 56; // Partida após parar

  // Deslocamento reto antes da faixa.
  } else if(estado == 56){
    moverFrente(125, 135);

    if(sensor1 == 0 && sensor2 == 0 && sensor3 == 0 && sensor4 == 0 && sensor5 == 0 && sensor6 == 0 && sensor7
    == 0)
      estado = 56;
    else
      estado = 57; // Andar reto na faixa

  // Deslocamento reto sobre a faixa.
  } else if(estado == 57){
    moverFrente(135, 135);
    delay(2000);
    estado = 58; // Andar reto após a faixa

  // Saída da faixa e retorno ao fluxo de cruzamento.
  } else if(estado == 58){
    moverFrente(135, 135);

    if(sensor1 == 0 && sensor2 == 0 && sensor3 == 0 && sensor4 == 0 && sensor5 == 0 && sensor6 == 0 && sensor7
    == 0)
      estado = 58;
    else
      estado = 3; // Detectou a linha -> atravessa reto e volta ao PID
  }
}
