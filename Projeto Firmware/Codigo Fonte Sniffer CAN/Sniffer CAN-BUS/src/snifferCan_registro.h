/**
 * @file    snifferCan_registro.h
 * @brief   Esse arquivo contem o prototipo das funções relativas ao envio de dados ao servidor
 * @author  Emanoel Gomes Santos
 * @date    Data de Criação: 31/03/2020
**/
#ifndef SNIFFER_CAN_REGISTRO_INCLUDED
#define SNIFFER_CAN_REGISTRO_INCLUDED

/// Inclusões importantes
#include <stdio.h>
#include <stdlib.h>
#include <HTTPClient.h>
#include <string.h>

/// Submódulos do sistema
#include "tipos.h"
#include "erros.h"
#include "fila_mensagem.h"
#include "snifferCan_cartao.h"
#include "snifferCan_servidor.h"
#include "snifferCan_wifi.h"

/// Funções exportadas
char *snifferCan_obtemPonteiroTexto(void);
Terro snifferCanRegistro_enviaDadosServidor(
    PTmensagemCAN mensagem, 
    Tuint16 quantidade,
    TwifiConfig wifi,
    char *url
);
Terro snifferCanRegistro_enviaDadosCartao(
    PTmensagemCAN mensagem, 
    Tuint16 quantidade, 
    char *nomeArquivo, 
    Tbool logFormatado,
    Tbool monitorSerial
);

#endif // SNIFFER_CAN_REGISTRO_INCLUDED
