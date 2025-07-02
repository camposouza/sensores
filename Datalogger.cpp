#include "Datalogger.hpp"
#include <string>

void Datalogger::setupDatalogger(){

    pinMode(SD_MISO, INPUT_PULLUP);
    pinMode(SD_CS, INPUT_PULLUP);
    pinMode(SD_MOSI, INPUT_PULLUP);
    pinMode(PWR_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, INPUT_PULLUP);
    
    if(!SD_MMC.begin("/sdcard", ONE_BIT_MODE)){
        Serial.println("Card Mount Failed");
        return;
    } else {
        Serial.println("Card Mount SUCCEED");
    }
}

void Datalogger::abreArquivo(String path){
    this->meu_arquivo = SD_MMC.open(path, FILE_READ);

    if(this->meu_arquivo.size() == 0) {
      Serial.println(path+" não existe. Criando e adicionando cabecalho...");
      
      String cabecalho = "Velocidade,Consumo";
      this->meu_arquivo = SD_MMC.open(path, FILE_WRITE);
      this->meu_arquivo.println(cabecalho);
    } else {
      Serial.println("Abrindo "+path+ " para concatenacao...")  ;
      this->meu_arquivo = SD_MMC.open(path, FILE_APPEND);
    }

    if (this->meu_arquivo){
        Serial.println("FILE OK !!!");
    } else {
        Serial.println("FILE NOT OK !!!");
    }
    this->meu_arquivo.close();
}


void Datalogger::concatenaArquivo(String path, String data) {
    this->meu_arquivo = SD_MMC.open(path, FILE_APPEND);

    if(!this->meu_arquivo ) {
        Serial.println("Failed to open file for appending");
        return;
    }

    if(this->meu_arquivo) {
        this->meu_arquivo.println(data);
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }

    this->meu_arquivo.close();
}


void Datalogger::lehBuffers(BufferCircular &b_velocidade, BufferCircular &b_consumo, String &dadosParaGravar) {
    // Lê dados de velocidade
    Sensores s_vel = b_velocidade.getValor();
    double velocidade = s_vel.velocidade;

    // Lê dados de consumo
    Sensores s_cons = b_consumo.getValor();
    double consumo = s_cons.consumo;

    // Monta stream de dados
    dadosParaGravar = String(velocidade) + "," + String(consumo);



}