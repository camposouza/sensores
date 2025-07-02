#include <Arduino.h>

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

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
//   DIRETIVAS DE SINCRONIZAÇÃO 
// ===============================

std::mutex mut;
std::condition_variable cond;
bool pode_escrever_velocidade = true;

// ============================
//      THREADS E OBJETOS
// ============================

Gerador gerador;
std::thread t1, t2, t3, t4, t5;

// ============================
//      FUNÇÕES DAS THREADS 
// ============================

// Gera os dados dos sensores
void t_geraDados() {
    gerador.geraDados(b_bruto);
}

void t_getLeituras() {
    while(true) {
        Sensores s = b_bruto.getValor();

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); 
    }
}

void t_ETLVelocidade() {
    while (true) {
        std::unique_lock<std::mutex> lock(mut);
        cond.wait(lock, [] { return pode_escrever_velocidade; });
        
        ETL(b_bruto, b_velocidade, "velocidade");

        Sensores s = b_velocidade.getValor(); 
        Serial.print("Velocidade: ");
        Serial.println(s.velocidade);

        pode_escrever_velocidade = false;
        lock.unlock(); // Libera o lock antes de notificar
        cond.notify_one();

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void t_ETLConsumo() {
    while (true) {
        std::unique_lock<std::mutex> lock(mut);
        cond.wait(lock, [] { return !pode_escrever_velocidade; });
        
        ETL(b_bruto, b_consumo, "consumo");

        Sensores s = b_consumo.getValor();
        Serial.print("Consumo: ");
        Serial.println(s.consumo);

        pode_escrever_velocidade = true;
        lock.unlock(); // Libera o lock antes de notificar
        cond.notify_one();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

// ============================
//          Datalogger
// ============================

Datalogger datalogger;

const String path = "/TesouroNoFinalDoArcoIris.txt";
String dadosParaGravar;

void t_Datalogger() {
    Serial.print("dadosParaGravar: ");
    Serial.println(dadosParaGravar);
    datalogger.lehBuffers(b_velocidade, b_consumo, dadosParaGravar);
    datalogger.concatenaArquivo(path, dadosParaGravar);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

void setup() {
    // Monitor Serial
    Serial.begin(9600);
    delay(1000);

    // Datalogger
    datalogger.setupDatalogger();
    datalogger.abreArquivo(path);

    // Threads
    Serial.println("Iniciando as threads...");

    t1 = std::thread(t_geraDados);
    t2 = std::thread(t_getLeituras);
    t3 = std::thread(t_ETLVelocidade);
    t4 = std::thread(t_ETLConsumo);
    t5 = std::thread(t_Datalogger);

    t1.detach();
    t2.detach();
    t3.detach();
    t4.detach();
    t5.detach();

}

void loop() {

     // Evita que o loop() consuma 100% da CPU se estiver vazio.
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}