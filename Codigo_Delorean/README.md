# DeLorean — Robô Seguidor de Linha

**Clube de Robótica do COLTEC/UFMG**

## Sobre

Diretório destinado à organização e à documentação dos códigos-fonte e materiais técnicos relacionados ao projeto DeLorean.

O material disponível apresenta a implementação de um robô seguidor de linha baseado no microcontrolador ESP32, com sensores infravermelhos, acionamento de motores por ponte H e estratégias de controle para acompanhamento de trajetórias.

O projeto também utiliza uma máquina de estados para organizar diferentes comportamentos durante a execução do percurso.

## Arquitetura do sistema

A implementação disponível reúne quatro partes principais:

- Unidade de processamento baseada em ESP32;
- Sistema de sensoriamento infravermelho;
- Acionamento de motores de corrente contínua;
- Lógica de controle e organização de estados.

A integração desses elementos permite interpretar as informações obtidas pelos sensores e controlar o deslocamento do robô.

## Sistema de sensoriamento

O código utiliza sete sensores infravermelhos para identificar a posição relativa da linha em relação ao robô.

Na implementação consultada, os sensores estão associados aos seguintes pinos:

| Sensor | Pino do ESP32 |
|---|---:|
| S1 | 36 |
| S2 | 39 |
| S3 | 34 |
| S4 | 35 |
| S5 | 32 |
| S6 | 33 |
| S7 | 27 |

As leituras são utilizadas pelo programa para identificar desvios e determinar as correções necessárias durante o acompanhamento da trajetória.

## Acionamento dos motores

O deslocamento é realizado por dois motores de corrente contínua controlados por uma ponte H.

O programa utiliza sinais PWM para regular o acionamento dos motores e sinais digitais para definir o sentido de rotação.

As rotinas de movimentação estão organizadas em funções específicas, responsáveis por comandos como avanço, recuo, giros e interrupção do movimento.

## Controle PID

A implementação utiliza um controlador PID para realizar ajustes na trajetória do robô.

O controle considera os componentes proporcional, integral e derivativo, relacionando o erro identificado pelos sensores ao ajuste aplicado aos motores.

Os parâmetros registrados na versão consultada são:

| Parâmetro | Valor |
|---|---:|
| Kp | 25,0 |
| Ki | 0,0 |
| Kd | 15,0 |

Esses valores correspondem à configuração registrada no código disponível e podem depender das características físicas do robô e das condições de funcionamento.

## Máquina de estados

Além do controle de trajetória, o programa utiliza uma máquina de estados para organizar comportamentos específicos.

Essa estrutura permite separar a navegação convencional de outras situações identificadas durante o percurso.

Entre os comportamentos presentes na implementação estão:

- Acompanhamento da linha;
- Correções de trajetória;
- Identificação de cruzamentos;
- Reconhecimento de padrões específicos;
- Execução de manobras;
- Tratamento de situações relacionadas à faixa de pedestres.

A utilização de estados permite estruturar a lógica do programa e organizar as respostas do robô de acordo com as condições identificadas.

## Organização dos arquivos

| Diretório | Conteúdo |
|---|---|
| `Codigo/` | Arquivos-fonte e diferentes versões da implementação. |
| `Documentacao/` | Documentos complementares e registros técnicos. |

As diferentes versões do código devem ser preservadas com seus nomes e conteúdos originais, permitindo identificar alterações e consultar implementações anteriores.

## Observações

Este diretório reúne materiais de um projeto do acervo do Clube de Robótica.

A documentação descreve os elementos identificados nos arquivos disponíveis, sem atribuir autoria ou participação individual que não esteja devidamente registrada.

As versões corrigidas devem ser consideradas implementações distintas, e não necessariamente versões validadas em hardware.

## Objetivos da documentação

- Preservar os códigos-fonte disponíveis;
- Registrar a arquitetura de sensoriamento e acionamento;
- Documentar os parâmetros de controle identificados;
- Facilitar o estudo da máquina de estados;
- Reunir referências para consulta e desenvolvimento.

---

**Projeto DeLorean**  
Clube de Robótica do COLTEC/UFMG
