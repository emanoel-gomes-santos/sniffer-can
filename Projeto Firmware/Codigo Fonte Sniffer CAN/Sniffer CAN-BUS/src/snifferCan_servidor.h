/**
 * @file    sniferCan_servidor.h
 * @brief   Esse arquivo contem o prototipo das funções relativas ao envio de dados ao servidor
 * @author  Emanoel Gomes Santos
 * @date    Data de Criação: 31/03/2020
**/
#ifndef SNIFFER_CAN_SERVIDOR_H_INCLUDED
#define SNIFFER_CAN_SERVIDOR_H_INCLUDED

/// Inclusões importantes
#include <stdio.h>
#include <stdlib.h>
#include <HTTPClient.h>
#include <string.h>

// Submódulos do sistema
#include "tipos.h"
#include "erros.h"
#include "fila_mensagem.h"
#include "snifferCan_wifi.h"
#include "gerenciamento_cartao.h"

// Funções exportadass
Terro snifferCanServidor_envia(char *texto, TwifiConfig wifi, char *url);
Terro snifferCanServidor_le(String url, TwifiConfig wifi, String *dadosLido);
Terro snifferCanServidor_formataQuadroCANToString(char *texto, PTmensagemCAN mensagem, 
                                                 Tuint16 quantidade, Tbool formatado);

Terro sniferCanServidor_conecta(char *url);
void sniferCanServidor_desconecta(void);                                                 

#endif // SNIFFER_CAN_SERVIDOR_H_INCLUDED
