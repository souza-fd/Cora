# Aranha — Robô Quadrúpede

**Clube de Robótica do COLTEC/UFMG**

## Sobre

Diretório destinado à organização e à documentação dos materiais relacionados ao projeto Aranha, um robô quadrúpede desenvolvido no contexto das atividades do Clube de Robótica do COLTEC/UFMG.

O projeto apresenta uma arquitetura de locomoção baseada em quatro pernas articuladas e um sistema de sensoriamento infravermelho destinado ao acompanhamento de linha.

Os arquivos disponíveis permitem consultar a implementação de software e estudar a integração entre sensores, servomotores e mecanismos de controle.

## Arquitetura do sistema

A implementação disponível reúne diferentes componentes responsáveis pelo sensoriamento, processamento das informações e controle da movimentação.

Entre os principais elementos identificados estão:

- Quatro pernas articuladas;
- Doze servomotores;
- Módulo controlador PWM PCA9685;
- Sete sensores infravermelhos TCRT5000;
- Comunicação I²C;
- Controle proporcional baseado nas leituras dos sensores.

## Sistema de locomoção

A estrutura mecânica utiliza quatro pernas, cada uma composta por três segmentos articulados.

O acionamento é realizado por servomotores controlados por meio do módulo PCA9685, responsável pela geração dos sinais PWM.

Essa configuração permite coordenar os movimentos das pernas e executar sequências de caminhada.

## Sensoriamento e controle

O acompanhamento da linha é realizado por sete sensores infravermelhos TCRT5000, distribuídos transversalmente na estrutura do robô.

As leituras são processadas pelo programa para estimar a posição relativa da linha e calcular o desvio em relação à trajetória.

A implementação utiliza uma abordagem de controle proporcional: o erro identificado pelos sensores é multiplicado por um ganho e utilizado para ajustar os parâmetros de movimentação.

Dessa maneira, o sistema relaciona as informações obtidas pelo sensoriamento às correções aplicadas ao movimento do robô.

## Aspectos de desenvolvimento

Um dos desafios relatados durante o desenvolvimento esteve relacionado às dimensões das pernas e à adequação da estrutura às condições da pista.

Essa questão evidencia a importância da integração entre modelagem mecânica, dimensões dos componentes e comportamento do robô durante a locomoção.

O material disponível também registra a participação do projeto na competição, na qual o Aranha recebeu reconhecimento na categoria de inovação.

## Organização dos arquivos

| Diretório ou arquivo | Conteúdo |
|---|---|
| `Codigo/` | Implementações do programa. |
| `Aranha_Original.ino` | Versão de referência do código. |
| `Aranha_Documentado.ino` | Versão com comentários e documentação complementar. |

## Observações

Os materiais deste diretório pertencem ao acervo de projetos do Clube de Robótica.

A documentação foi elaborada a partir dos arquivos disponíveis e das informações preservadas sobre o projeto. Portanto, detalhes que não estejam registrados no código ou nos materiais de referência não são tratados como especificações confirmadas.

## Objetivos da documentação

- Preservar as implementações disponíveis;
- Registrar a arquitetura identificada no código;
- Facilitar o estudo do sistema de locomoção;
- Documentar a integração entre sensoriamento e controle;
- Manter referências técnicas para consulta.

---

**Projeto Aranha**  
Clube de Robótica do COLTEC/UFMG
