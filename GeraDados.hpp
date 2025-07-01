#ifndef GERA_DADOS_HPP
#define GERA_DADOS_HPP

#include <iostream>
#include <string>
#include <cmath>      // Para fmod e sqrt
#include <chrono>     // Para controle de tempo
#include <thread>     // Para std::this_thread::sleep_for
#include <iomanip>    // Para formatação da saída (setw, setprecision)
#include <sstream>    // Para conversão de double para string com precisão

#include "BufferCircular.hpp"

// =================================================================
//                          CONSTANTES
// =================================================================

const int    TAXA_AMOSTRAGEM_HZ     = 100;
const double TEMPO_LOOP_S           = 1.0 / TAXA_AMOSTRAGEM_HZ;

const double TEMPO_VOLTA_S          = 180.0; // 3 minutos por volta

// Consumo (J/km)
const double CONSUMO_MAX              = 20000.0;
const double CONSUMO_MIN_ACELERANDO   = 15000.0;
const double CICLO_CONSUMO_S          = TEMPO_VOLTA_S / 4.0; // 45s
const double TEMPO_ACELERANDO_S       = CICLO_CONSUMO_S / 2.0; // 22.5s

// Velocidade (km/h)
const double VELOCIDADE_MAX         = 30.0;
const double VELOCIDADE_MIN         = 3.0;
const double CICLO_VELOCIDADE_S     = TEMPO_VOLTA_S / 4.0; // 45s

class Gerador {
private:

public:
    /**
     * @brief 
     * 
     */
    void geraDados(BufferCircular &buffer);

    /**
     * @brief 
     * 
     */
    void printaDados(double velocidade, double consumo, int tempo_total_s);
};

#endif