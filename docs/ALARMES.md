
# Alarmes do controlador

## Objetivo

O controlador monitora as leituras dos sensores de nível e temperatura para identificar condições que exigem atenção.

Os alarmes são publicados no arquivo `telemetria.json`, no campo `alarmes`.

## Critérios dos alarmes

Os limites abaixo foram definidos como critérios didáticos para o projeto, pois o contrato não estabelece valores específicos para os alarmes.

| Código | Condição | Severidade |
|---|---|---|
| NIVEL_BAIXO | Nível abaixo de 30% | aviso |
| NIVEL_ALTO | Nível acima de 80% | aviso |
| TEMPERATURA_ALTA | Temperatura acima de 35 °C | critica |
| FALHA_NIVEL | Sensor de nível indisponível ou com falha | critica |
| FALHA_TEMPERATURA | Sensor de temperatura indisponível ou com falha | critica |

## Funcionamento

- Os alarmes são avaliados a cada execução do controlador.
- Mais de um alarme pode ser gerado simultaneamente.
- Quando as leituras estão normais, a lista de alarmes permanece vazia.
- Quando um sensor está indisponível ou apresenta falha, o controlador gera o alarme correspondente.
- Os alarmes são publicados com código, mensagem e severidade.

## Exemplo de telemetria

Quando o nível está em 20% e a temperatura em 40 °C, são gerados os alarmes `NIVEL_BAIXO` e `TEMPERATURA_ALTA`.

