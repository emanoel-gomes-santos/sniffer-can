/**
 * @file    protocolo_can.h
 * @brief   Esse arquivo contem o prototipo das funções relativas a implementação 
 *          interface do cartao de memória micro SD
 * @author  Emanoel Gomes Santos
 * @date    Data de Criação: 30/03/2020
**/
#ifndef PROTOCOLO_CAN_H_INCLUDED
#define PROTOCOLO_CAN_H_INCLUDED

/// Inclusões importantes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// Submódulos do sistema
#include "tipos.h"
#include "erros.h"
#include "fila_mensagem.h"
#include "snifferCan_registro.h"
#include "snifferCan_wifi.h"


/// Funções exportadass
void  protocoloCAN_salvaRegistroCANFila(void * filaMensagem );
void  protocoloCAN_enviaRegistroCANFila(void * filaMensagem );
Terro protocoloCAN_inicializa(TaxaComunicacao taxa, 
                              TlistaFiltrosAndMascaras filtros, 
                              PTfilaMensagem filaMensagem, 
                              Tuint32 tamanhoFila);
void protocoloCan_entrarNoSistema(void);

// Referenia para a tarefa
extern TaskHandle_t salvaRegistroCANFila;
extern TaskHandle_t enviaRegistroCANFila;


#endif // PROTOCOLO_CAN_H_INCLUDED
