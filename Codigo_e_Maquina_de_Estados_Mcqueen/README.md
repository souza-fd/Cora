# McQueen — Firmware e Máquina de Estados

**Equipe McQueen | Clube de Robótica do COLTEC/UFMG**

## 1. Apresentação

Este diretório reúne os códigos-fonte e a documentação técnica relacionados ao funcionamento do robô McQueen, desenvolvido pela equipe McQueen para a 12ª Competição de Robôs Autônomos (CoRA), realizada em 2025.

O material contempla diferentes versões do firmware e a representação da máquina de estados finitos utilizada na organização da lógica de controle do robô.

A documentação tem como finalidade preservar os registros da implementação, facilitar a compreensão do funcionamento do sistema e manter referências técnicas para consultas e desenvolvimentos futuros.

## 2. Firmware

O projeto reúne duas versões principais do firmware, identificadas pelas configurações de 200 RPM e 500 RPM.

| Versão | Característica |
|---|---|
| 200 RPM | Configuração desenvolvida com maior atenção ao comportamento do robô durante curvas e trajetórias. |
| 500 RPM | Configuração direcionada ao desempenho em arrancada, utilizada pela equipe na competição. |

A manutenção das duas implementações permite consultar e comparar as configurações adotadas durante o desenvolvimento, preservando diferentes etapas do processo de ajuste do robô.

Os arquivos utilizam a extensão `.ino`, correspondente ao ambiente de desenvolvimento Arduino, e contêm a implementação do firmware responsável pelo controle do sistema embarcado.

## 3. Máquina de estados finitos

A lógica de funcionamento do McQueen é representada por uma máquina de estados finitos (MEF), estrutura utilizada para organizar o comportamento do robô a partir de estados definidos e transições condicionadas às leituras dos sensores.

Cada estado representa uma condição ou operação específica do sistema. As transições determinam quando o robô deve abandonar o estado atual e executar uma nova ação, de acordo com as condições identificadas durante o percurso.

Essa organização permite estruturar diferentes situações de navegação, como acompanhamento de linha, identificação de cruzamentos, execução de curvas, reconhecimento de faixas de pedestres e tratamento de bifurcações.

### 3.1. Estrutura geral

O diagrama apresenta os estados identificados por códigos, como E0, E1, E2 e assim sucessivamente. As setas representam as transições entre esses estados, enquanto as sequências binárias indicam as condições associadas às mudanças de estado.

A representação está organizada em cinco conjuntos funcionais:

- Partida;
- Deslocamento para a direita;
- Deslocamento para a esquerda;
- Faixa de pedestres;
- Bifurcação em T.

Essa divisão facilita a visualização das diferentes etapas de operação e das decisões tomadas pelo sistema durante a navegação.

### 3.2. Sequência de partida

A sequência inicial organiza a preparação do robô antes do início do deslocamento.

O fluxo apresentado começa no estado E0, identificado como uma condição de parada por dois segundos. Em seguida, o sistema passa pelo estado E1, associado à leitura dos sensores. Depois, alcança E2, correspondente à partida, e E3, responsável pelo acompanhamento da linha.

A partir dessa sequência, o robô inicia sua operação de navegação e passa a utilizar as leituras dos sensores para determinar as próximas transições.

### 3.3. Deslocamento para a direita

O conjunto de estados destinado às manobras para a direita descreve as etapas de identificação da condição de percurso, execução da curva e recuperação do acompanhamento da linha.

Entre os estados representados, destacam-se:

- **Seguir linha:** mantém o acompanhamento da trajetória;
- **Encruzilhada:** representa a identificação de uma interseção;
- **Leitura dos quadrantes à direita:** reúne estados associados à interpretação das leituras dos sensores nessa região;
- **Parada de leitura:** representa a interrupção da leitura dos quadrantes em determinadas etapas da sequência;
- **Virada para a direita:** corresponde à execução da manobra;
- **Procurando linha à direita:** representa a etapa de busca pela trajetória após a curva.

O diagrama também apresenta sequências específicas de estados para diferentes etapas da manobra, identificadas como DIR2, DIR3 e DIR4. Essas sequências detalham a continuidade do procedimento e as etapas de recuperação do acompanhamento da linha.

### 3.4. Deslocamento para a esquerda

A estrutura destinada às manobras para a esquerda apresenta uma organização correspondente à sequência de deslocamento para a direita.

São representadas etapas de leitura dos quadrantes à esquerda, identificação de condições de percurso, execução da curva e procura pela linha após a manobra.

Entre os estados identificados estão:

- **Leitura dos quadrantes à esquerda;**
- **Parada de leitura dos quadrantes;**
- **Virada para a esquerda;**
- **Procura pela linha à esquerda.**

Assim como no conjunto anterior, o diagrama apresenta sequências complementares, identificadas como ESQ2, ESQ3 e ESQ4, que representam diferentes etapas do procedimento de manobra.

A separação dessas sequências permite documentar individualmente o comportamento do sistema durante as operações de mudança de direção.

### 3.5. Reconhecimento de faixa de pedestres

O conjunto destinado à faixa de pedestres representa uma sequência específica de identificação e tratamento dessa condição durante o percurso.

O fluxo parte do estado E3, associado ao acompanhamento da linha, e apresenta uma etapa de identificação de possível pista invertida, seguida pela confirmação dessa condição.

Na sequência, o diagrama representa:

1. Identificação de uma possível pista invertida;
2. Confirmação da condição identificada;
3. Posicionamento do robô;
4. Parada temporizada de cinco segundos;
5. Início do procedimento na faixa;
6. Deslocamento em linha reta;
7. Deslocamento sobre a faixa;
8. Identificação da passagem pela faixa;
9. Retorno ao estado de acompanhamento da linha.

Essa sequência organiza o comportamento do robô diante da faixa de pedestres, estabelecendo etapas próprias para identificação, posicionamento, parada e retomada do percurso.

### 3.6. Tratamento de bifurcação em T

O conjunto de estados referente à bifurcação em T apresenta uma lógica de interpretação das leituras dos quadrantes para identificar as condições encontradas no percurso.

O diagrama reúne estados associados ao acompanhamento da linha e à leitura dos quadrantes à direita e à esquerda. A partir dessas condições, são representadas possibilidades distintas de transição:

- Identificação de condição associada ao quadrante direito;
- Identificação de condição associada ao quadrante esquerdo;
- Reconhecimento de uma encruzilhada;
- Parada para leitura dos quadrantes.

Essa estrutura permite organizar as decisões do sistema diante de uma bifurcação, relacionando as leituras dos sensores às possibilidades de continuidade do percurso.

## 4. Organização da documentação da máquina de estados

O diagrama da máquina de estados está disponibilizado em dois formatos:

| Formato | Finalidade |
|---|---|
| Draw.io | Arquivo editável, destinado à consulta, manutenção e atualização do diagrama. |
| JPEG | Representação exportada para visualização e consulta rápida. |

A disponibilização dos dois formatos preserva tanto a representação visual quanto a possibilidade de continuidade da documentação.

## 5. Participação na 12ª CoRA

Na 12ª edição da Competição de Robôs Autônomos, realizada em 2025, a equipe McQueen utilizou a versão de 500 RPM na prova de arrancada, conquistando a vitória nessa modalidade.

A participação na competição representou a aplicação prática dos conhecimentos envolvidos no desenvolvimento do robô, incluindo programação embarcada, eletrônica, controle de motores, leitura de sensores e organização da lógica de navegação.

## 6. Objetivos da documentação

- Preservar as diferentes versões do firmware;
- Registrar a estrutura lógica de controle do McQueen;
- Documentar os estados e as sequências de operação representadas no diagrama;
- Facilitar a consulta e a comparação entre implementações;
- Manter registros técnicos do desenvolvimento e da participação na competição;
- Disponibilizar referências para estudos, manutenção e projetos futuros.

## 7. Licenciamento e atribuição

Os códigos-fonte e materiais reunidos neste diretório devem manter suas respectivas referências de autoria e origem.

A utilização, modificação ou redistribuição dos materiais deve respeitar as condições estabelecidas por seus autores e eventuais licenças associadas. A ausência de uma licença explícita não representa autorização irrestrita de uso ou redistribuição.

---

**Equipe McQueen**  
Clube de Robótica do COLTEC/UFMG  
**12ª Competição de Robôs Autônomos (CoRA) — 2025**
