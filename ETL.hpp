#include "BufferCircular.hpp"
#include <string>
#include <iostream>

/**
 * @brief Extrai dados de velocidade ou consumo do buffer de origem e carrega no buffer de destino
 * 
 * @param b_origem  buffer com os dados de origem
 * @param b_destino buffer de destino
 * @param medicao   Deve ser <velocidade> ou <consumo>
 */
void ETL(BufferCircular &b_origem, BufferCircular &b_destino, std::string medicao) {
    Sensores dados_lidos = b_origem.getValor();

    // O buffer de destino somente deve conter o dado da variável de interesse
    if (medicao == "velocidade") {
        // Zera os dados de consumo. Mantem os dados de velocidade
        dados_lidos.consumo = 0;
        b_destino.add(dados_lidos);

    } else if (medicao == "consumo") {
        // Zera os dados de velocidade. Mantem os dados de consumo
        dados_lidos.velocidade = 0;
        b_destino.add(dados_lidos);
    } else {
        // Input do usuário incorreto
        std::cout << "ERROR: medicao deve ser <velocidade> ou <consumo>" << std::endl;
    }
}