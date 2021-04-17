/**
 * @file    gerenciamento_cartao.h
 * @brief   Esse arquivo contem o prototipo das funções relativas ao gerenciamento
 *          do cartao micro SD
 * @author  Emanoel Gomes Santos
 * @date    Data de Criação: 10/04/2020
**/
#ifndef GERENCIAMENTO_CARTAO_H_INCLUDED
#define GERENCIAMENTO_CARTAO_H_INCLUDED

/// Inclusões importantes
#include <stdio.h>
#include <stdlib.h>
#include <SD.h>
#include <string.h>

// Submódulos do sistema
#include "tipos.h"
#include "erros.h"

/// Funções exportadas

// Funções auxiliares
Terro gerenciamentoCartao_obtemListaFiltrosAndMascaras(PTlistaFiltrosAndMascaras lista);
Terro gerenciamentoCartao_escreveListaFiltrosAndMascaras(String listaFiltrosAndMascaras);
Terro gerenciamentoCartao_obtemTaxaComunicacao(PTaxaComunicacao taxa);
Terro gerenciamentoCartao_escreveTaxaComunicacao(String taxa);
Terro gerenciamentoCartao_formataTaxa(PTaxaComunicacao taxa, String textoTaxa);
Terro gerenciamentoCartao_obtemLoginSenha(PTwifiConfig configuracaoWifi);
Terro gerenciamentoCartao_obtemDesejaFormatarLog(Tbool *formatado);
Terro gerenciamentoCartao_obtemUltimoIdArquivoRegistro(Tuint16 *idArquivo);
Terro gerenciamentoCartao_obtemURLServidor(PTservidor servidor);
Terro gerenciamentoCartao_formataListaFiltrosAndMascaras(PTlistaFiltrosAndMascaras lista, 
                                                         String listaPossiveisFiltros);
                                                         
Terro gerenciamentoCartao_atualizaLastFile(Tuint32 id);
Terro gerenciamentoCartao_tamanhoArquivo(char *caminho, Tuint32 *tamanho);

Terro gerenciamentoCartao_escreveMensagemERRO(char *erro);
Terro gerenciamentoCartao_verificaTaxa(PTaxaComunicacao taxa, String taxaTexto);

// Funções principais
Terro gerenciamentoCartao_obtemConfiguracao(PTconfiguracao configuracao);
Terro gerenciamentoCartao_inicializa(Tuint8 cs);
void gerenciamentoCartao_finaliza(void);
Terro gerenciamentoCartao_escreve(char *texto, const char *caminho, TmodoEscrita mode);
Terro gerenciamentoCartao_criaArquivo(char *caminho);
char * getStringTaxa(TaxaComunicacao taxa);
#endif // GERENCIAMENTO_CARTAO_H_INCLUDED
