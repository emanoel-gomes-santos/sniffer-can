/**
 * @file    snifferCAN_wifi.h
 * @brief   Arquivo com o prototipo das funções relativos ao wifi
 * @author  Emanoel Gomes Santos 
 * @date    Data de Criação: 01/05/2019
 */
#ifndef SNIFFER_CAN_WIFI_H_INCLUDED
#define SNIFFER_CAN_WIFI_H_INCLUDED

// Inclusões de bibliotecas do sistema
#include <wifi.h>

// Inclusões de bibliotecas do módulo
#include "tipos.h"
#include "erros.h"

/// Funções exportadas
Terro snifferCANWiFi_conecta(char* ssid, char* password, Tuint16 tentativas);
Terro snifferCANWiFi_verificaConexao(void);

#endif // SNIFFER_CAN_WIFI_H_INCLUDED
