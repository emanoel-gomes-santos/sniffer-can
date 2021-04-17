/**
 * @file    fila_mensagem.h
 * @brief   Esse arquivo contem o prototipo das funções relativas a implementação da estrutura de dados fila
 * @author  Emanoel Gomes Santos
 * @date    Data de Criação: 30/03/2020
**/
#ifndef FILA_MENSAGEM_H_INCLUDED
#define FILA_MENSAGEM_H_INCLUDED

/// Inclusões importantes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Inclusões de bibliotecas do módulo
#include "tipos.h"
#include "erros.h"

/**
 * Prototipos de funções exportadas
 */
/// Função que inializa uma fila
Terro filaMensagem_inicializaFila(PTfilaMensagem fila, Tuint32 tamanho);
/// Função que retorna o tamanho da fila
Tuint32 filaMensagem_tamanhoFila(PTfilaMensagem fila);
/// Função que enfilera uma celula na lista
Terro filaMensagem_enfileirar(PTfilaMensagem fila, TmensagemCAN mensagem);
/// Função que desinfilera uma celula da fila
Terro filaMensagem_desenfileirar(PTfilaMensagem fila, PTmensagemCAN mensagem);
/// Função que finaliza a fila desalocando a fila da memória
void filaMensagem_finalizaFila(PTfilaMensagem fila);

extern SemaphoreHandle_t sistema;



#endif // FILA_MENSAGEM_H_INCLUDED
