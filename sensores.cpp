#include <iostream>
#include <thread>
#include <mutex>              // NOVO: Header para o mutex
#include <condition_variable> // NOVO: Header para a variável de condição

#include "BufferCircular.hpp"
#include "GeraDados.hpp"
#include "ETL.hpp"

// =========================
//          BUFFERS 
// =========================

#define BUFFER_SIZE 256

BufferCircular b_bruto(BUFFER_SIZE);       // Buffer escrito pela thread simulando os sensores
BufferCircular b_velocidade(BUFFER_SIZE);  // Buffer com os dados de velocidade
BufferCircular b_consumo(BUFFER_SIZE);     // Buffer com os dados de consumo


// ===============================
//   DIRETIVAS DE SINCRONIZAÇÃO 
// ===============================

std::mutex mut;                  
std::condition_variable cond;    
bool pode_escrever_velocidade = true;      // true = vez da velocidade escrever, false = vez do consumo


// ============================
//      FUNÇÕES DAS THREADS 
// ============================

// Gera os dados dos sensores
Gerador gerador;

void t_geraDados() {
    gerador.geraDados(b_bruto);
}

void t_getLeituras() {
    while(true) {
        Sensores s = b_bruto.getValor();
        gerador.printaDados(s.velocidade, s.consumo, 0);
    }
}

void t_ETLVelocidade() {
    while (true) {
        std::unique_lock<std::mutex> lock(mut);

        // Espera aqui até que 'pode_escrever_velocidade' seja true
        cond.wait(lock, [] { return pode_escrever_velocidade; });
        
        // Executa depois que acorda
        ETL(b_bruto, b_velocidade, "velocidade");

        // Passa a vez para a outra thread
        pode_escrever_velocidade = false;
        cond.notify_one();

    }
}

void t_ETLConsumo() {
    while (true) {
        std::unique_lock<std::mutex> lock(mut);

        // Espera aqui até que 'pode_escrever_velocidade' seja false
        cond.wait(lock, [] { return !pode_escrever_velocidade; });
        
        // Executa depois que acorda
        ETL(b_bruto, b_consumo, "consumo");

        // Passa a vez para a outra thread
        pode_escrever_velocidade = true;
        cond.notify_one();

    }
}


int main() {
    std::thread t1(t_geraDados);
    std::thread t2(t_getLeituras);
    std::thread t3(t_ETLVelocidade);
    std::thread t4(t_ETLConsumo);
    
    // Thread principal espera todas as outras finalizarem pra seguir
    t1.join(); 
    t2.join();
    t3.join();
    t4.join();
}