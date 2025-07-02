#include "GeraDados.hpp"

void Gerador::geraDados(BufferCircular &buffer){

    // --- Variáveis de Estado ---
    double tempo_total_s = 0.0;
    
    
    while (true) {
        // Calcula o tempo atual dentro da volta
        // Usa fmod porque o operador '%' em C++ é para inteiros
        double tempo_na_volta = fmod(tempo_total_s, TEMPO_VOLTA_S);

        // --- Cálculo do Consumo ---
        double tempo_no_ciclo_de_consumo = fmod(tempo_na_volta, CICLO_CONSUMO_S);
        double consumo;
        if (tempo_no_ciclo_de_consumo <= TEMPO_ACELERANDO_S) {
            // Fase de aceleração/consumo
            double progresso = tempo_no_ciclo_de_consumo / TEMPO_ACELERANDO_S;
            consumo = CONSUMO_MAX - progresso * (CONSUMO_MAX - CONSUMO_MIN_ACELERANDO);
        } else {
            // Fase sem consumo
            consumo = 0.0;
        }
        
        // --- Cálculo da Velocidade ---
        double tempo_no_ciclo_de_velocidade = fmod(tempo_na_volta, CICLO_VELOCIDADE_S);
        double velocidade;
        if (tempo_no_ciclo_de_velocidade <= TEMPO_ACELERANDO_S) {
            // Fase de aceleração (crescimento com raiz quadrada)
            double progresso = tempo_no_ciclo_de_velocidade / TEMPO_ACELERANDO_S;
            velocidade = VELOCIDADE_MIN + (VELOCIDADE_MAX - VELOCIDADE_MIN) * sqrt(progresso);
        } else {
            // Fase de desaceleração (queda linear)
            double progresso = (tempo_no_ciclo_de_velocidade - TEMPO_ACELERANDO_S) / TEMPO_ACELERANDO_S;
            velocidade = VELOCIDADE_MAX - progresso * (VELOCIDADE_MAX - VELOCIDADE_MIN);
        }
        
        // --- Adiciona os dados no Buffer Circular ---
        Sensores dados_sensores;
        dados_sensores.consumo = consumo;
        dados_sensores.velocidade = velocidade;
        buffer.add(dados_sensores);

        // --- Controle do Loop ---
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(TEMPO_LOOP_S * 1000)));
        tempo_total_s += TEMPO_LOOP_S;
    }
}


void Gerador::printaDados(double velocidade, double consumo, int tempo_total_s) {
        // -------- Formatação da Saída --------
    std::stringstream ss_vel;
    ss_vel << std::fixed << std::setprecision(2) << velocidade;
    std::string vel_str = ss_vel.str();
    
    std::stringstream ss_cons;
    ss_cons << std::fixed << std::setprecision(2) << consumo;
    std::string cons_str = ss_cons.str();
    
    // Imprime a saída formatada
    std::cout << "Tempo: " << std::fixed << std::setw(6) << std::setprecision(2) << tempo_total_s << "s | "
                << "Dados: [ '" << vel_str << "', '" << cons_str << "' ]"
                << std::endl;
}