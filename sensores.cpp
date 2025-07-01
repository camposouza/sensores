#include <iostream>
#include <thread>

#include "BufferCircular.hpp"
#include "GeraDados.hpp"
#include "ETL.hpp"


// =====================================
//            BUFFERS USADOS
// =====================================

#define BUFFER_SIZE 256

BufferCircular b_bruto(BUFFER_SIZE);       // Buffer escrito pela thread simulando os sensores
BufferCircular b_velocidade(BUFFER_SIZE);  // Buffer com os dados de velocidade
BufferCircular b_consumo(BUFFER_SIZE);     // Buffer com os dados de consumo

// Gera os dados aleatorios dos sensores
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
    ETL(b_bruto, b_velocidade, "velocidade");

    // Thread dorme para não trazer dados desatualizados
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void t_ETLConsumo() {
    ETL(b_bruto, b_consumo, "consumo");

    // Thread dorme para não trazer dados desatualizados
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

int main() {
    std::thread t1(t_geraDados);
    std::thread t2(t_getLeituras);
    t1.join(); 
    t2.join();
}
