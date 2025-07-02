#include <Arduino.h>
#include "BufferCircular.hpp"
#include "GeraDados.hpp"
#include "ETL.hpp"
#include "Datalogger.hpp"

// =========================
//          BUFFERS 
// =========================

#define BUFFER_SIZE 16

BufferCircular b_bruto(BUFFER_SIZE);
BufferCircular b_velocidade(BUFFER_SIZE);
BufferCircular b_consumo(BUFFER_SIZE);

// ===============================
//   DIRETIVAS DE SINCRONIZAÇÃO (MODIFICADO)
// ===============================

SemaphoreHandle_t sem_velocidade;
SemaphoreHandle_t sem_consumo;

// ============================
//      OBJETOS GLOBAIS
// ============================

Gerador gerador;
Datalogger datalogger;
const String path = "/TesouroNoFinalDoArcoIris.txt";
String dadosParaGravar;

// ============================
//      FUNÇÕES DAS TAREFAS 
// ============================

void t_geraDados(void* parameter) {
    gerador.geraDados(b_bruto);
    vTaskDelete(NULL); 
}

void t_getLeituras(void* parameter) {
    while(true) {
        Sensores s = b_bruto.getValor();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void t_ETLVelocidade(void* parameter) {
    while (true) {
        // Espera até que o semáforo seja liberado
        if (xSemaphoreTake(sem_velocidade, portMAX_DELAY) == pdTRUE) {
            
            ETL(b_bruto, b_velocidade, "velocidade");

            Sensores s = b_velocidade.getValor(); 
            Serial.print("Velocidade: ");
            Serial.println(s.velocidade);

            // Libera o semáforo da tarefa de consumo
            xSemaphoreGive(sem_consumo);
        }
        
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

void t_ETLConsumo(void* parameter) {
    while (true) {
        // Espera até que o semáforo seja liberado
        if (xSemaphoreTake(sem_consumo, portMAX_DELAY) == pdTRUE) {
        
            ETL(b_bruto, b_consumo, "consumo");

            Sensores s = b_consumo.getValor();
            Serial.print("Consumo: ");
            Serial.println(s.consumo);

            // Libera o semáforo da tarefa de velocidade
            xSemaphoreGive(sem_velocidade);
        }
        
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void t_Datalogger(void* parameter) {
    while(true) { 
        datalogger.lehBuffers(b_velocidade, b_consumo, dadosParaGravar);
        datalogger.concatenaArquivo(path, dadosParaGravar);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(9600);
    delay(1000); // Garante que o Serial esteja pronto

    sem_velocidade = xSemaphoreCreateBinary();
    sem_consumo = xSemaphoreCreateBinary();

    // Arbitrariamente, definimos que a primeira tarefa a rodar
    // será a de velocidade
    if (sem_velocidade != NULL) {
        xSemaphoreGive(sem_velocidade);
    }

    datalogger.setupDatalogger();
    datalogger.abreArquivo(path);

    xTaskCreate(t_geraDados, "GeraDados", 2048, NULL, 1, NULL);
    xTaskCreate(t_getLeituras, "GetLeituras", 2048, NULL, 1, NULL);
    xTaskCreate(t_ETLVelocidade, "ETL_Velo", 4096, NULL, 2, NULL);
    xTaskCreate(t_ETLConsumo, "ETL_Cons", 4096, NULL, 2, NULL);
    xTaskCreate(t_Datalogger, "Datalogger", 8192, NULL, 3, NULL);
}

void loop() {
     vTaskDelay(1000 / portTICK_PERIOD_MS);
}