/**
 * @file    tipos.h
 * @brief   Esse arquivo contem as definições dos tipos proprios utilizado no sistema
 * @author  Emanoel Gomes Santos
 * @date    Data de Criação: 30/03/2020
**/

#ifndef TIPO_H_INCLUDED
#define TIPO_H_INCLUDED

#include <Arduino.h>
#include <mcp_can.h>
#include <SD.h>

#define SERIAL_SNIFFER_CAN  "User=EA12345678"
#define SENHA_SNIFFER_CAN   "Password=AE123456"
#define TAMANHO_SENHA_E_SERIAL (sizeof(SERIAL_SNIFFER_CAN) + sizeof(SENHA_SNIFFER_CAN) + 2 + 1) 
//  mais 2 por causa de dois "&" comercials e +1 por causa do \0

#define TAXA_TRANSFERENCIA_SERIAL_ESP32  115200

/// Definição boleana
#define VERDADEIRO true
#define FALSO      false
/// Definições de escrita na tela serial
#define PRINTLN       Serial.println
#define PRINTF        Serial.printf
#define PRINT         Serial.print

// Definições de taxas CAN
#define TAXA_4K096BPS  CAN_4K096BPS
#define TAXA_5KBPS     CAN_5KBPS
#define TAXA_10KBPS    CAN_10KBPS
#define TAXA_20KBPS    CAN_20KBPS
#define TAXA_31K25BPS  CAN_31K25BPS
#define TAXA_33K3BPS   CAN_33K3BPS
#define TAXA_40KBPS    CAN_40KBPS
#define TAXA_50KBPS    CAN_50KBPS
#define TAXA_80KBPS    CAN_80KBPS
#define TAXA_100KBPS   CAN_100KBPS
#define TAXA_125KBPS   CAN_125KBPS
#define TAXA_200KBPS   CAN_200KBPS
#define TAXA_250KBPS   CAN_250KBPS
#define TAXA_500KBPS   CAN_500KBPS
#define TAXA_1000KBPS  CAN_1000KBPS


#define MAX_MASCARA_FILTRO_SUPORTADA   2

/// Definiçoes gerais
#define TAMANHO_BUFFER_10M                (10*1024*1024)
#define TAMANHO_BUFFER_200K               (200*1024)
#define TAMANHO_BUFFER_100K               (100*1024)
#define TAMANHO_BUFFER_120K               (120*1024)
#define TAMANHO_BUFFER_80K                (80*1024)
#define TAMANHO_BUFFER_60K                (60*1024)
#define TAMANHO_BUFFER_40K                (40*1024)
#define TAMANHO_BUFFER_20K                (20*1024)
#define TAMANHO_BUFFER_10K                (10*1024)
#define TAMANHO_BUFFER_8K                 (8*1024)
#define TAMANHO_BUFFER_4K                 (4*1024)
#define TAMANHO_BUFFER_2K                 (2*1024)
#define TAMANHO_BUFFER_1K                 (1*1024)
#define TAMANHO_MAXIMO_BUFFER_FILA        TAMANHO_BUFFER_4K// nao modificar tamanho maximo suportado 4k (4096 * 17)
#define NUCLEO_ZERO                       0
#define NUCLEO_UM                         1
#define PINO_LED_INTERNO                  2

#define TAMANHO_MAX_DADOS_QUADRO_CAN      8

// Servidor
#define URL_HTTP_SERVIDOR_SNNIFER_CAN  \
  "https://tcc-eng-comp-webapp.azurewebsites.net/api/Esp32?Authorization=XiREf7U5HdmxMwHcyLKdwdEDLqvkv2PSFKBnUaFDE94CYRVygjggtVrfxJz5kYeB"
#define TAMANHO_MAXIMO_URL    1000
/// WIFI
#define SSID_WIFI      ((const char*)"Emanoel")
#define PASSWORD_WIFI  ((const char*)"147852369")
#define TAMANHO_MAXIMO_SENHA   (64+1)
#define TAMANHO_MAXIMO_LOGIN   (32+1)

/// Cartão
#define CS_PIN_MODULO_MICRO_SD     5
#define MAX_TENTATIVAS_CARTAO      5
#define TENTATIVAS_ESCRITA_CARTAO  10

#define MAXIMA_QUANTIDADE_IDENTIFICADORES  6
#define MAXIMA_QUANTIDADE_MASCARAS         2
#define QUANTIDADE_FILTROS_MASCARA_0       2
#define QUANTIDADE_FILTROS_MASCARA_1       4
#define MAXIMA_QUANTIDADE_FILTROS          (QUANTIDADE_FILTROS_MASCARA_0+QUANTIDADE_FILTROS_MASCARA_1)
#define QUANTIDADE_FILTROS_E_MASCARAS      (MAXIMA_QUANTIDADE_MASCARAS+MAXIMA_QUANTIDADE_FILTROS)
#define TAMANHO_MAX_STRING_FILTRO          (8+1)
#define TAMANHO_MAX_BUFFER_FILTRO          (TAMANHO_MAX_STRING_FILTRO*MAXIMA_QUANTIDADE_FILTROS)
#define NOME_ARQUIVO_CONFIGURACAO          ("/SETUP/configuracao.txt")
#define NOME_ARQUIVO_REGISTRO_INTERNO      ("/SETUP/system.nel")
#define NOME_ARQUIVO_REGISTRO_PADRAO       ("/REGISTROS/LOG-0000.txt")
#define TAMANHO_BUFFER_MENSAGEM_REGISTRO   ((strlen(NOME_ARQUIVO_REGISTRO_PADRAO)) + 1)
#define QUANTIDADE_MAXIMA_REGISTROS_CARTAO (0xFFFF)

/// Definidores de formatação do texto a serem enviados
#define TAMANHO_DEFINIDO_ESPACO_ENTRE_TEMPO_ID   20
#define QUANTIDADE_ESPACO_TEXTO_FORMATADO       (17 + TAMANHO_DEFINIDO_ESPACO_ENTRE_TEMPO_ID)
#define QUANTIDADE_SEPARADORES_TEXTO             4

/// Definições de funções
#define HEX_TO_ASCII(hexa)      ((hexa <= 0x09)  ? (hexa + '0') : ((hexa - 0x0A) + 'A'))
#define ASCII_TO_HEXA(ascii)    ((ascii >= 'A') ? (ascii - 0x37) : (ascii - 0x30))

// Definições importantes
#define PRIMEIRA_INTERACAO       (i==0) 

// Definições de erro
#define  LED_ERRO_CAN              26
#define  LED_ERRO_CARTAO_MEMORIA   27
#define  LED_ERRO_WIFI             32
#define  LED_ERRO_SERVIDOR         33

// Definição do led para sistema pronto
#define  LED_SISTEMA_PRONTO        25

/// Tipo inteiro  de 64 bits sem sinal
typedef unsigned long long int Tuint64;
/// Tipo inteiro  de 32 bits sem sinal
typedef unsigned int Tuint32;
/// Tipo inteiro de 16 bits sem sinal
typedef unsigned short int  Tuint16;
/// Tipo inteiro de 8 bits sem sinal
typedef unsigned char Tuint8;
/// Tipo erro
typedef Tuint8 Terro;
/// Tipo boleano
typedef bool Tbool;

typedef unsigned long Tempo;

typedef enum EmodoEscritaCartao {
  eModoWrite,
  eModoAppend
}TmodoEscrita;

typedef TmodoEscrita *PTmodoEscrita;

// Tipo de mensagem can
typedef union Uidentificador {
  Tuint16 padrao;
  Tuint32 extendido;
}Tidentificador;

typedef Tidentificador *PTidentificador;

typedef struct SmensagemCAN {
  Tidentificador identificador;
  Tuint8 dados[TAMANHO_MAX_DADOS_QUADRO_CAN];
  Tuint8 tamanho;
  Tempo intervalo;
}TmensagemCAN;

typedef TmensagemCAN * PTmensagemCAN;

typedef struct SwifiConfig {
  char login[TAMANHO_MAXIMO_LOGIN];
  char senha[TAMANHO_MAXIMO_SENHA];
  Tbool conectado;
}TwifiConfig;

typedef TwifiConfig *PTwifiConfig; 


// Tipo taxa de comunicação
typedef Tuint8 TaxaComunicacao;
typedef TaxaComunicacao *PTaxaComunicacao;

typedef enum EtipoFiltro {
  ePadrao,
  eExtendido,  
}TtipoFiltro;

typedef struct Sfiltro{
  Tuint32 valor;  
}Tfiltro;

typedef struct SlistaFiltrosAndMascaras {
  // Definidor da mascara 0
  Tuint32 mascara_0;
  // Existe mascara 0 configurada?
  Tbool mask_0;  
  // Definidor da mascara 1
  Tuint32 mascara_1;
  // Existe mascara 0 configurada?
  Tbool mask_1;
  // Lista de filtros possiveis
  Tfiltro filtros[MAXIMA_QUANTIDADE_FILTROS];
  // Quantidade de filtros configurados
  Tuint16 quantidade;
  // Tipo do filtro
  TtipoFiltro tipo;
}TlistaFiltrosAndMascaras;

typedef TlistaFiltrosAndMascaras *PTlistaFiltrosAndMascaras;

typedef struct Sservidor {
  char reg[TAMANHO_MAXIMO_URL];
  char taxa[TAMANHO_MAXIMO_URL];
  char filtro[TAMANHO_MAXIMO_URL];  
}Tservidor;

typedef Tservidor *PTservidor;

typedef struct Sconfiguracao{
  // Lista de identificadores que se deseja filtrar
  TlistaFiltrosAndMascaras filtAndMask;
  // Taxa de comunicação do módulo
  TaxaComunicacao taxa;
  // Login e senha do wifi
  TwifiConfig wifi;
  // Deseja log formatado?
  Tbool logFormatado;
  // Deseja monitor serial?
  Tbool monitorSerial;
  // identificador do ultimo arquivo
  Tuint16 idArquivo;
  // URL do servidor
  Tservidor servidor;
}Tconfiguracao;

typedef Tconfiguracao *PTconfiguracao;

// Estrutura de dados para a fila de mensagem CAN
typedef struct SfilaMensagem{
  /// Ponteiro para frente celula ta fila
  Tuint32 capacidadeMax;
  /// Ponteiro para o tipo mensagem CAN
  PTmensagemCAN mensagem;
  // Posição primeiro
  Tuint32  primeiro;
  // Posição ultimo
  Tuint32 ultimo;
  // Tamanho lista
  Tuint32 tamanhoAtual;

}TfilaMensagem;

// Definição do ponteiro
typedef TfilaMensagem* PTfilaMensagem;


typedef struct SdescritorSniffer{
  Tconfiguracao configuracao;
  TfilaMensagem filaMensagem;
}TdescritorSniffer;

typedef TdescritorSniffer *PTdescritorSniffer;


typedef struct StabelaTaxas{
  char descricao[50];
  TaxaComunicacao taxa;  
}TtabelaTaxas;

typedef TtabelaTaxas *PTtabelaTaxas;

#endif // TIPO_H_INCLUDED