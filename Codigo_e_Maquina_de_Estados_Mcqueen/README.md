# McQueen — Código e Máquina de Estados

**Equipe MCQUEEN | Clube de Robótica do COLTEC/UFMG**

## Sobre

Diretório destinado à organização e à documentação dos códigos-fonte e dos materiais relacionados à lógica de funcionamento do robô McQueen, desenvolvido pela equipe MCQUEEN para a 12ª Competição de Robôs Autônomos (CoRA), realizada em 2025.

O conteúdo reúne diferentes versões do firmware e o diagrama da máquina de estados, preservando registros importantes da implementação e do desenvolvimento do sistema de controle.

## Firmware

O diretório reúne duas versões principais do código, identificadas pelas configurações de 200 RPM e 500 RPM.

| Versão | Característica |
|---|---|
| 200 RPM | Configuração desenvolvida com maior atenção ao controle do robô durante curvas e trajetórias. |
| 500 RPM | Configuração direcionada ao desempenho em arrancada, utilizada pela equipe na competição. |

A preservação das duas implementações permite consultar e comparar as configurações utilizadas durante o desenvolvimento do projeto.

## Máquina de estados

A máquina de estados representa a organização lógica do funcionamento do McQueen.

Essa estrutura permite organizar os diferentes estados de operação do robô e estabelecer as transições entre eles, contribuindo para a organização do comportamento programado.

O diagrama está disponível em dois formatos:

- **Draw.io:** arquivo editável, que permite consultar e modificar a representação;
- **JPEG:** versão exportada para visualização.

A manutenção dos dois formatos facilita tanto a consulta quanto a continuidade da documentação.

## Participação na 12ª CoRA

Durante a 12ª edição da Competição de Robôs Autônomos, a equipe MCQUEEN utilizou a versão de 500 RPM na prova de arrancada, conquistando a vitória nessa modalidade.

O resultado representou uma etapa importante do desenvolvimento e da aplicação prática dos conhecimentos envolvidos no projeto.

## Organização dos arquivos

| Arquivo ou diretório | Conteúdo |
|---|---|
| `McQueen_200_RPM.ino` | Versão do firmware configurada para 200 RPM. |
| `McQueen_500_RPM.ino` | Versão do firmware configurada para 500 RPM. |
| `Maquina_De_Estados/` | Arquivos relacionados ao diagrama da máquina de estados. |

## Objetivos da documentação

- Preservar as diferentes versões do firmware;
- Registrar a estrutura lógica do sistema;
- Disponibilizar o diagrama da máquina de estados;
- Facilitar a consulta e a comparação entre implementações;
- Manter referências técnicas para estudos e projetos futuros.

---

**Equipe MCQUEEN**  
Clube de Robótica do COLTEC/UFMG  
**12ª Competição de Robôs Autônomos (CoRA)**
