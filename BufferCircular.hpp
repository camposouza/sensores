#ifndef BUFFER_CIRCULAR_HPP
#define BUFFER_CIRCULAR_HPP

#include <vector>
#include <cstddef>
#include <iostream>

struct Sensores {
    double velocidade;  
    double consumo;
};

class BufferCircular {
public:
    /**
     * @brief Constrói o buffer circular com uma capacidade definida.
     * @param size A capacidade máxima do buffer.
     */
    BufferCircular(size_t size) : buffer_(size), capacity_(size), head_(0), is_full_(false) {}

    /**
     * @brief Adiciona um novo ponto de dados ao buffer.
     * Se o buffer estiver cheio, o dado mais antigo é sobrescrito.
     * @param data Dados dos Sensores a serem adicionado.
     */
    void add(const Sensores& data) {
        buffer_[head_] = data;
        head_ = (head_ + 1) % capacity_;
        if (head_ == 0 && !is_full_) {
            is_full_ = true;
        }

        std::cout << "add() " << buffer_[head_].consumo << std::endl;
    }
    
    /**
     * @brief Retorna o valor mais antigo adicionado ao buffer.
     * @return 
     */
    Sensores getValor() {
        if (!is_full_) {
            std::cout << "getValor() " << buffer_[head_].consumo << std::endl;
            return buffer_[0];
        }
        std::cout << "getValor() " << buffer_[head_].consumo << std::endl;
        return buffer_[head_];
    }

private:
    std::vector<Sensores> buffer_;
    size_t capacity_;
    size_t head_;
    bool is_full_;
};

#endif