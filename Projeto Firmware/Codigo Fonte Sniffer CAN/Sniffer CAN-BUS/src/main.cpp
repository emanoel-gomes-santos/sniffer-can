/**
 * @file    main.cpp
 * @brief   Esse arquivo contem as funções principais e de configuração do sistema
 * @author  Emanoel Gomes Santos
 * @date    Data de Criação: 30/03/2020
**/

/// Bibliotecas do esp32
#include <mcp_can.h>
#include <SPI.h>
#include <pthread.h>
#include <Wire.h>
#include "time.h"

#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

/// Bibliotecas do módulo Snnifer CAN/BUS
#include "erros.h"
#include "tipos.h"
#include "fila_mensagem.h"
#include "protocolo_can.h"
#include "gerenciamento_cartao.h"

// Definições importantes
#define TENTATIVAS_INICIALIZA_CONEXAO_WIFI  10

// Variáveis globais
TmensagemCAN mensagem;
Terro erro;
TdescritorSniffer descritor;
struct tm data;


/**
 * @brief  Função que executa as configurações iniciais do sistema
 * @return void
 */
void setup(){
  // ------------------------------------------------------------------------------------------//
  //                        CONFIGURAÇÕES INICIAIS NECESSÁRIAS                                 //
  // ------------------------------------------------------------------------------------------// 

  // Configura porta serial para debug
  Serial.begin(TAXA_TRANSFERENCIA_SERIAL_ESP32);
  
  //Cria a estrutura temporaria para funcao abaixo.
  timeval tv;
  // Encontrar uma forma de obter a hora exata
  tv.tv_sec = 1587141960;
  settimeofday(&tv, NULL);

  // Define pino 2 como saída para utilizar o LED interno
  pinMode(PINO_LED_INTERNO, OUTPUT);

  pinMode(LED_ERRO_CARTAO_MEMORIA, OUTPUT);
  pinMode(LED_ERRO_WIFI,           OUTPUT);
  pinMode(LED_ERRO_SERVIDOR,       OUTPUT);
  pinMode(LED_ERRO_CAN,            OUTPUT);
  pinMode(LED_SISTEMA_PRONTO,      OUTPUT);
 
  // Garante que todos os leds estão apagados
  digitalWrite(LED_ERRO_CARTAO_MEMORIA, LOW);
  digitalWrite(LED_ERRO_WIFI,           LOW);
  digitalWrite(LED_ERRO_SERVIDOR,       LOW);
  digitalWrite(LED_ERRO_CAN,            LOW);
  digitalWrite(LED_SISTEMA_PRONTO,      LOW);
  

  // ------------------------------------------------------------------------------------------//
  //                    OBTEM INFORMAÇÕES DE CONFIGURAÇÃO NO CARTAO                            //
  // ------------------------------------------------------------------------------------------// 
  
  // Inicializa cartão micro SD
  erro = gerenciamentoCartao_inicializa(CS_PIN_MODULO_MICRO_SD);
  if(erro != SUCESSO){
    // Se ocorreu algum erro, então acender led de cartão de memória e sai do sistema
    digitalWrite(LED_ERRO_CARTAO_MEMORIA,HIGH);
    PRINTLN("CARTÃO INVALIDO OU NAO ENCONTRADO"); 
    return;
  }  

  PRINTLN("----DADOS DO CARTAO DE MEMORIA---");

  // Limpa todo buffer de configuração
  (void)memset((Tuint8*)&(descritor.configuracao), 0x00, sizeof(Tconfiguracao));

  // Obtem todas as informações necessárias para inicializar sistema
  erro = gerenciamentoCartao_obtemConfiguracao((PTconfiguracao)&(descritor.configuracao));
  if(erro != SUCESSO){
    // Se ocorreu algum erro, então acender led de cartão de memória e sai do sistema
    digitalWrite(LED_ERRO_CARTAO_MEMORIA,HIGH);
    PRINTLN("DADOS DO CARTÃO CORROMPIDOS OU CARTÃO INVALIDO"); 
    return;
  } 

  
  // ------------------------------------------------------------------------------------------//
  //                                  REALIZA CONEXÃO COM WIFI                                 //
  // ------------------------------------------------------------------------------------------//  
  erro = snifferCANWiFi_conecta(
    descritor.configuracao.wifi.login, 
    descritor.configuracao.wifi.senha,
    TENTATIVAS_INICIALIZA_CONEXAO_WIFI
  );
  if(erro != SUCESSO){
    // Se ocorreu algum erro, então acender led do wifi. não sai do sistema, pois funciona
    // mesmo se wifi nao estiver funcionando
    digitalWrite(LED_ERRO_WIFI,HIGH);  
    PRINTLN("FALHA AO CONECTAR AO WIFI");  
    digitalWrite(LED_ERRO_SERVIDOR,HIGH);  
    PRINTLN("FALHA AO CONECTAR AO SERVIDOR");      
    descritor.configuracao.wifi.conectado = FALSO;    
  }

  // ------------------------------------------------------------------------------------------//
  //            LE DADOS DO SERVIDOR SE CONSEGUIR SE CONECTAR AO WIFI                          //
  // ------------------------------------------------------------------------------------------//   
  // Se conseguiu conectar a internet, tenta conectar ao servidor
  else{
    String buffer;
    String url;
    descritor.configuracao.wifi.conectado = VERDADEIRO;

    PRINTLN("\n----DADOS DO SERVIDOR---");

    PRINTLN("Aguarde...\r\n");    

    // LER FILTROS DO SERVIDOR --------------------
    erro = sniferCanServidor_conecta(descritor.configuracao.servidor.filtro);
    if(erro == SUCESSO){

      // Le filtro servidor
      erro = snifferCanServidor_le(
        url,
        descritor.configuracao.wifi,
        &buffer
      );
      // Se ocorreu algum erro, mostrar erro
      if(erro != SUCESSO){
          digitalWrite(LED_ERRO_SERVIDOR,HIGH);  
          PRINTLN("FALHA AO CONECTAR AO SERVIDOR");  
          descritor.configuracao.wifi.conectado = FALSO;
      }
      // Se ocorreu tudo bem, salvar filtros
      else{
        
        PRINT("FILTROS:");
        PRINTLN(buffer);

        // Recupera informação de filtros e mascaras do servidor e configura descritor de configuração
        erro = gerenciamentoCartao_formataListaFiltrosAndMascaras(
          (PTlistaFiltrosAndMascaras)&(descritor.configuracao.filtAndMask),
          buffer
        );
        if(erro != SUCESSO){
          digitalWrite(LED_ERRO_SERVIDOR,HIGH);  
          PRINTLN("FALHA AO CONECTAR AO SERVIDOR");  
          descritor.configuracao.wifi.conectado = FALSO;  
        }
        /*
        // Atualiza cartão com a informação recebida pelo servidor
        erro = gerenciamentoCartao_escreveListaFiltrosAndMascaras(buffer);
        if(erro != SUCESSO){
          PRINTLN("FALHA AO ESCREVER INFORMACAO DO SERVIDOR NO CARTAO DE MEMORIA");  
        }*/
      }

      /// TENTA LER TAXA DO SERVIDOR -----------
      erro = sniferCanServidor_conecta(descritor.configuracao.servidor.taxa);
      if(erro == SUCESSO){
          
        // Le taxa no servidor
        erro = snifferCanServidor_le(
          url,
          descritor.configuracao.wifi,
          &buffer
        );
        // Se ocorreu algum erro mostrar e acender led
        if(erro != SUCESSO){
          digitalWrite(LED_ERRO_SERVIDOR,HIGH);  
          PRINTLN("FALHA AO CONECTAR AO SERVIDOR");  
          descritor.configuracao.wifi.conectado = FALSO;           
        }
        // Se ocorreu tudo bem, entao salvar taxa lida do servidor
        else{
            
          PRINTF("TAXA: %s\r\n", getStringTaxa((TaxaComunicacao)(buffer.toInt())));
          //PRINTLN(buffer);

          // Recupera informação de filtros e mascaras do servidor e configura descritor de configuração
          erro = gerenciamentoCartao_verificaTaxa(
            (PTaxaComunicacao)&(descritor.configuracao.taxa),
            buffer
          );
          if(erro != SUCESSO){
            descritor.configuracao.taxa = TAXA_500KBPS; // recebe taxa default
            digitalWrite(LED_ERRO_SERVIDOR,HIGH);
            delay(1000);
            digitalWrite(LED_ERRO_SERVIDOR,LOW);  
            delay(1000);
            digitalWrite(LED_ERRO_SERVIDOR,HIGH);  
            delay(1000);
            digitalWrite(LED_ERRO_SERVIDOR,LOW);           
            PRINTLN("TAXA INVALIDA RECEBIDA DO SERVIDOR!");
          }
          /*
          
          // Atualiza cartão com a informação recebida pelo servidor
          erro = gerenciamentoCartao_escreveTaxaComunicacao(buffer);
          if(erro != SUCESSO){
            PRINTLN("FALHA AO ESCREVER INFORMACAO DO SERVIDOR NO CARTAO DE MEMORIA");  
          } */     
        }
      }else{
        digitalWrite(LED_ERRO_SERVIDOR,HIGH);  
        PRINTLN("FALHA AO CONECTAR AO SERVIDOR");
      }

    }else{
      digitalWrite(LED_ERRO_SERVIDOR,HIGH);  
      PRINTLN("FALHA AO CONECTAR AO SERVIDOR");  
    }
   

   

  }

  // ------------------------------------------------------------------------------------------//
  //                            INICIALIZAÇÃO DO PROTOCOLO CAN                                 //
  // ------------------------------------------------------------------------------------------//
  erro = protocoloCAN_inicializa(
    descritor.configuracao.taxa, 
    descritor.configuracao.filtAndMask, 
    (PTfilaMensagem)&(descritor.filaMensagem),
    TAMANHO_MAXIMO_BUFFER_FILA
);
  if(erro != SUCESSO){
    // Se ocorreu algum erro, então acender led da can. Sai do sistema, pois sem a CAN inicialziada com uscesso
    // nada funciona
    digitalWrite(LED_ERRO_CAN,HIGH);      
    return;    
    PRINTLN("FALHA NA INICIALIZAÇÃO DO PROTOCOLO CAN");
  }
  // SE CHEGOU AQUI ENTÃO OCORREU TUDO BEM

  // Se chegou até aqui então esta tudo correto. apenas sinaliza com LED INTERNO do ESP32 
  PRINTLN("\n\n\nExecutando...");

  digitalWrite(PINO_LED_INTERNO,HIGH);   
  digitalWrite(LED_SISTEMA_PRONTO,HIGH);   

  // Ativa flag das funções principais
  protocoloCan_entrarNoSistema();  
  
  /*
    Cria uma tarefa que será executada na função protocoloCAN_salvaRegistroCANFila, com prioridade 2
    e execução no núcleo 0.
    salvaRegistroCANFila: Captura os dados da linha CAN via SPI (MCP2515) e salva em uma fila de mensagens CAN
  */
  xTaskCreatePinnedToCore(
    // Função que implementa a tarefa
    protocoloCAN_salvaRegistroCANFila,
    // Nome da tarefa
    "salvaRegistroCANFila", 
    // Numero de bytes a serem alocados para uso com a pilha da tarefa
    // Fazendo teste com a função uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    // percebe-se que a quantidade maxima de palavras usadas da pilha é de 1324.
    // Com isso, 4KB (4096) é mais do que suficiente
    TAMANHO_BUFFER_4K, // 
    // Parametro de entrada da tarefa, nesse caso sera justamente um ponteiro para fila de mensagem
    (PTdescritorSniffer)&descritor,       
    // Prioridade da tarefa(0 à N)
    2,
    // Referencia para a tarefa (não é utilizado entao null)
    &salvaRegistroCANFila,       
    // Nucleo que estará sendo executado o proceso = 0
    NUCLEO_ZERO
  ); 
                    
  delay(500); //tempo para a tarefa iniciar
  /*
    Cria uma tarefa que será executada na função protocoloCAN_enviaRegistroCANFila, com prioridade 1
    e execução no núcleo 1.
    enviaRegistroCANFila: Envia os dados armazenados na estrutura de fila para cartão de memória ou servidor
  */
  xTaskCreatePinnedToCore(
    // Função que implementa a tarefa
    protocoloCAN_enviaRegistroCANFila, 
    // Nome da tarefa
    "enviaRegistroCANFila", 
    // Numero de bytes a serem alocados para uso com a pilha da tarefa
    (TAMANHO_BUFFER_20K),      
    // Parametro de entrada da tarefa, nesse caso sera justamente um ponteiro para fila de mensagem
    (PTdescritorSniffer)&descritor,       
    // Prioridade da tarefa(0 à N)
    1,
    // Referencia para a tarefa (não é utilizado entao null)
    &enviaRegistroCANFila,    
    // Nucleo que estará sendo executado o proceso = 1   
    NUCLEO_UM
  );

  delay(500); //tempo para a tarefa iniciar  
  
}


/**
 * @brief  Função que executa o loop infinito do esp32. Nesse caso não será usado pois o programa 
 *         está sendo rodado nas tarefas definidas no setup
 * @return void
 */

void loop(){

/*
TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
TIMERG0.wdt_feed=1;
TIMERG0.wdt_wprotect=0;
*/
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/