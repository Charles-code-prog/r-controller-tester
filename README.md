# Testador de Controle Cartesiano

Este projeto contém o código de controle para um **testador de controle cartesiano**, utilizado para verificar e acionar diversos atuadores, sensores e relés de forma automatizada por meio de comandos via porta serial.

A seguir estão listados os **comandos disponíveis para acionamento e desligamento** de cada saída. O sistema aceita comandos no formato:

- `COMANDO_1` → Liga a saída correspondente
- `COMANDO_0` → Desliga a saída correspondente

A resposta serial retornada para cada comando também é listada abaixo:

---

## 🧭 Tabela de Comandos Seriais

| **Pino** | **Comando para ligar** | **Mensagem ON**  | **Comando para desligar**  | **Mensagem OFF**  |
|---------:|------------------------|------------------|----------------------------|-------------------|
| 18       | `P_1`                  | `PRE_ON;`        | `P_0`                      | `PRE_OFF;`        |
| 19       | `KR_1`                 | `RST_ON;`        | `KR_0`                     | `RST_OFF;`        |
| 20       | `K1_1`                 | `AV2_ON;`        | `K1_0`                     | `AV2_OFF;`        |
| 21       | `K2_1`                 | `AV1_ON;`        | `K2_0`                     | `AV1_OFF;`        |
| 22       | `K3_1`                 | `TR2_ON;`        | `K3_0`                     | `TR2_OFF;`        |
| 23       | `K4_1`                 | `TR1_ON;`        | `K4_0`                     | `TR1_OFF;`        |
| 24       | `K5_1`                 | `TRC_ON;`        | `K5_0`                     | `TRC_OFF;`        |
| 25       | `K6_1`                 | `BT2_ON;`        | `K6_0`                     | `BT2_OFF;`        |
| 45       | `K7_1`                 | `BT1_ON;`        | `K7_0`                     | `BT1_OFF;`        |
| 27       | `K8_1`                 | `3V1_ON;`        | `K8_0`                     | `3V1_OFF;`        |
| 28       | `K9_1`                 | `3V2_ON;`        | `K9_0`                     | `3V2_OFF;`        |
| 29       | `K10_1`                | `IR1_ON;`        | `K10_0`                    | `IR1_OFF;`        |
| 30       | `K11_1`                | `IR2_ON;`        | `K11_0`                    | `IR2_OFF;`        |
| 31       | `K12_1`                | `GRE_ON;`        | `K12_0`                    | `GRE_OFF;`        |
| 32       | `K13_1`                | `YEL_ON;`        | `K13_0`                    | `YEL_OFF;`        |
| 33       | `K14_1`                | `RED_ON;`        | `K14_0`                    | `RED_OFF;`        |
| 34       | `RNA_1`                | `RNA_ON;`        | `RNA_0`                    | `RNA_OFF;`        |

---

## 📌 Observações

- Os comandos devem ser enviados exatamente conforme a tabela, via porta serial (ex.: Monitor Serial do Arduino).
- Cada comando recebe uma confirmação por mensagem no mesmo canal serial, facilitando logs e automação.
- Esse sistema foi desenvolvido com foco em **testes manuais ou automatizados de bancada**.

## Saídas dos Sensores
---

| Sensor | Definição                   | Mensagem Serial enviada |
| ------ | --------------------------- | ----------------------- |
| S11    | 35 (BERÇO 1 RECUADO)        | BED1\_OFF;              |
| S12    | 36 (BERÇO 1 AVANÇADO)       | BED1\_ON;               |
| S21    | 37 (BERÇO 2 RECUADO)        | BED2\_OFF;              |
| S22    | 38 (BERÇO 2 AVANÇADO)       | BED2\_ON;               |
| S31    | 39 (PISTÃO CAMERA RECUADO)  | PIST\_OFF;              |
| S32    | 40 (PISTÃO CAMERA AVANÇADO) | PIST\_ON;               |



