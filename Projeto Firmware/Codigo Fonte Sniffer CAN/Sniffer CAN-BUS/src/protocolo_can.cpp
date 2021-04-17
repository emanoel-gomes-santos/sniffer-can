/**
 * @file    protocolo_can.cpp
 * @brief   Esse arquivo contem as funções relativas ao tratamento das mensagens CAN
 * @author  Emanoel Gomes Santos
 * @date    Data de Criação: 31/03/2020
**/

// Inclusões de bibliotecas do arduino
#include <mcp_can.h>
#include <SPI.h>
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
//#include "esp_int_wdt.h"

/// Inclusões de bibliotecas importantes
#include "protocolo_can.h"

// Definições de tamanho
#define QUANTIDADE_MENSAGENS_POR_BLOCO  50   // nao alterar (suporta so 200 mensagens)
#define TEMPO_ENTRE_ENVIOS_REQUISICOES  500  // pode ser alterado na faixa de 200 a 1000 (por segurança)
#define TENTATIVAS_ENVIO_BLOCO_MENSAGEM 2       
#define HA_MENSAGEM_NO_BUFFER(x)        (x>0)
#define TAMANHO_MAXIMO_ARQUIVO          20000 
#define TENTATIVAS_INICIALIZAR_CAN      5
#define TEMPO_ENTRE_PISCA_LED           50

// Declarações de variáveis
#define CAN_INT           4                              // Set INT to pin 4
#define CS_PIN_MCP_2515   15

MCP_CAN CAN(CS_PIN_MCP_2515);                                     // Set CS to pin 5

Tbool executando = VERDADEIRO;
// Referenia para a tarefa
TaskHandle_t salvaRegistroCANFila;
TaskHandle_t enviaRegistroCANFila;

/**
 * @brief  Função que desativa flag para entrar no sistema
 * @return void
 */
void protocoloCan_sairDoSistema(void){
  xSemaphoreTake(sistema,portMAX_DELAY);
  // Apenas se esta executando que atualiza variavel executando, e finaliza fila
  if(executando){
    executando = FALSO;  
  }
  xSemaphoreGive(sistema);

}
/**
 * @brief  Função que ativa flag para entrar no sistema
 * @return void
 */
void protocoloCan_entrarNoSistema(void){
  executando = VERDADEIRO;  
}

/**
 * @brief  Função que configura os filtros escolhidos pelo usuário
 * @param  lista: Estrutura com a lista de filtros
 * @return void
 */
void protocoloCAN_configuraFiltro(TlistaFiltrosAndMascaras lista){  
  Tuint8 i;
  Tuint32 filtroDefault;

  // Se não existe nenhum filtro e nenhuma mascara, entao retornar e nao configurar nada
  if((lista.quantidade == 0) && (!lista.mask_0) && (!lista.mask_1)){
    return;
  }
  // Verifica qual tipo de filtro
  if(lista.tipo == ePadrao){
    filtroDefault = 0x7FF; 
  }else{
    filtroDefault = 0xFFFF;     
  }
 
  // Trata situação que não ha mascaras
  if((!lista.mask_0) && (!lista.mask_1)){
    
    // Se for padrao ativa primeira mascara
    if(lista.tipo == ePadrao){
      CAN.init_Mask(0,lista.tipo,filtroDefault);
      CAN.init_Mask(1,lista.tipo,filtroDefault);

        
      // Insere os filtros
      for(i=0; i<lista.quantidade; i++){
        CAN.init_Filt(i,lista.tipo,lista.filtros[i].valor);
      }
      for(i=lista.quantidade; i<MAXIMA_QUANTIDADE_FILTROS; i++){
        CAN.init_Filt(i, lista.tipo, lista.filtros[i%lista.quantidade].valor);
      }         
    }

    else{
      CAN.init_Mask(1,lista.tipo,filtroDefault);
        
      // Insere os filtros
      for(i=2; i<lista.quantidade; i++){
        CAN.init_Filt(i,lista.tipo,lista.filtros[(i-2)].valor);
      }
      for(i=(lista.quantidade+2); i<MAXIMA_QUANTIDADE_FILTROS; i++){
        CAN.init_Filt(i,lista.tipo,0x00000000);
      }  

    }
    
  }

  //////////////////////////////////////////////////////////////////////////////////////
  // Trata situação que so existe uma mascara
  else if(((!lista.mask_0) && (lista.mask_1)) || ((lista.mask_0) && (!lista.mask_1))){    
    

    // Tratas situação de filtro padrao com mascara 0
    if(lista.mask_0){

      CAN.init_Mask(0 , lista.tipo, 0x700);
      for(i=0; i<QUANTIDADE_FILTROS_MASCARA_0; i++){
        CAN.init_Filt(i,lista.tipo,lista.mascara_0);
      } 

      // Se existe filtro setado
      if(lista.quantidade > 0){
        CAN.init_Mask(1,lista.tipo,filtroDefault);

        for(i=0; i<lista.quantidade; i++){          
          CAN.init_Filt((i+2),lista.tipo,lista.filtros[i].valor);          
        }     

        // Restante das mascaras sao zeradas
        for(i=(lista.quantidade+2); i<MAXIMA_QUANTIDADE_FILTROS; i++){
          // (i+2), pois os slots da mascara zero (slot 0 e 1)  considera-se utilizados
          CAN.init_Filt(i,lista.tipo,0x00000000);  
        }        
      }
      else{
        // Mascara 1 recebe filtro default 0x7FF
        CAN.init_Mask(1,lista.tipo,filtroDefault);
        
        // Restante das mascaras sao zeradas
        for(i=2; i<MAXIMA_QUANTIDADE_FILTROS; i++){
          CAN.init_Filt(i,lista.tipo,0x00000000);
        }      
      }
    }


    else{ // lista.mask_1

      CAN.init_Mask(1 , lista.tipo, 0x1FFF0000);
      for(i=2; i<QUANTIDADE_FILTROS_MASCARA_1; i++){
        CAN.init_Filt(i,lista.tipo,lista.mascara_1);
      } 
      
    }
    

  }
 
}

/**
 * @brief  Função que inicializa o protocolo CAN, inicializando a fila de mensagens
 * @param  taxa: taxa de comunicação CAN
 * @param  filtros: Ponteiro os filtros que se deseja configurar
 * @param  fila: Ponteiro para fila que sera criada
 * @param  tamanhoFila: Tamanho da fila de mensagens
 * @return ERRO ou SUCESSO
 */
Terro protocoloCAN_inicializa(TaxaComunicacao taxa, TlistaFiltrosAndMascaras filtros, PTfilaMensagem filaMensagem, Tuint32 tamanhoFila){
  Terro erro = SUCESSO;
  Tuint16 tentativas = 0;

  PRINT("\n------Protocolo CAN------\r\n\n");
  PRINT("Inicializando CAN...\r\n");

  // Initialize MCP2515 trabalhnado em 8MHZ com taxa de 500KBS e mascara e filtros desabilitados
  while(CAN.begin(MCP_STDEXT, taxa, MCP_20MHZ) != CAN_OK){
    PRINTLN(".");
    delay(500);
  }
  // Configura os filtros 
  protocoloCAN_configuraFiltro(filtros);
  
  // Seta can como normal para executar processo
  CAN.setMode(MCP_LISTENONLY);

  // Configura pino 4 como entrada de dados (interrupção)
  pinMode(CAN_INT, INPUT);                            
  
  // Inicializa fila
  do{
    
    erro = filaMensagem_inicializaFila(filaMensagem, tamanhoFila);
    if(erro != SUCESSO){
      PRINTLN("ERRO AO INICIALIZAR A FILA! TENTANDO NOVAMENTE...");
      tentativas ++;
    }
  }while((erro != SUCESSO) && (tentativas < TENTATIVAS_INICIALIZAR_CAN)) ;

  if(erro != SUCESSO){
    return ERRO_INICIALIZACAO_CAN;
  }

  /*
  heap_caps_print_heap_info(MALLOC_CAP_DEFAULT);
  */
  // Se chegou até aqui então mostrar sucesso 
  PRINT("CAN inicializada com sucesso!\r\n\n");

  return SUCESSO;

}

/**
 * @brief  Função que recupera a informação do ultimo arquivo escrito no cartao
 *         Isso será usado para controle dos nomes dos arquivos
 * @param  idArquivo: id do ultimo registro armazenado
 * @param  nomeArquivo: Nome do proximo registro
 * @return ERRO ou SUCESSO
 */
Terro protocoloCAN_recuperaInformacaoUltimoArquivo(Tuint32 *idArquivo, char *nomeArquivo){
  Terro erro = SUCESSO;
  Tuint32 tamanhoArquivo = 0;
  
  // Se for a primeira utilização do produto, entao encrementa idArquivo e salva ultimo criado
  if(*idArquivo == 0){

    (*idArquivo)++;

    (void)sprintf(nomeArquivo, ((char *)"/REGISTROS/LOG-%04d.txt"), *idArquivo);

    // Abre apenas para criar
    erro = gerenciamentoCartao_criaArquivo(nomeArquivo);
    if(erro != SUCESSO){
      PRINTLN("ERRO AO CRIAR CARTAO");
    }

    // Atualiza last file
    erro = gerenciamentoCartao_atualizaLastFile(*idArquivo);
    if(erro != SUCESSO){ 
      PRINTLN("ERRO NO ENVIO DOS DADOS!!!");
    }

    // Criou novo arquivo e retornou
    return SUCESSO;

  }
  // Atribui nome do arquivo com identificador recebido do cartao
  (void)sprintf(nomeArquivo, ((char *)"/REGISTROS/LOG-%04d.txt"), (*idArquivo)); 

  // Verifica-se o tamanho do arquivo com nome "nomeArquivo"
  erro = gerenciamentoCartao_tamanhoArquivo(nomeArquivo, &tamanhoArquivo);
  if(erro != SUCESSO){
    return erro;
  }

  // Somente se o arquivo for maior que tamanho 0 que iremos criar um novo arquivo
  if(tamanhoArquivo > 0){

    (*idArquivo)++;
    
    // Cria path com id do arquivo incrementado, ou seja, proximo arquivo.
    (void)sprintf(nomeArquivo, ((char *)"/REGISTROS/LOG-%04d.txt"), (*idArquivo));

    // Abre apenas para criar
    erro = gerenciamentoCartao_criaArquivo(nomeArquivo);
    if(erro != SUCESSO){
      PRINTLN("ERRO AO CRIAR CARTAO");
    }
    // Atualiza last file
    erro = gerenciamentoCartao_atualizaLastFile((*idArquivo));
    if(erro != SUCESSO){
      PRINTLN("ERRO NO ENVIO DOS DADOS!!!");
    }
  }

  // Se chegou aqui entao tudo ocorreu bem
  return SUCESSO;
}

/**
 * @brief  Função que será executada em um loop infinito dentro de um processo.
 *         Essa função irá identificar se existe uma mensagem no buffer do MCP2515,
 *         se houver irá salvar a mensagem na fila de mensagens CAN
 * @param  filaMensagem: Ponteiro para a estrutura de fila da mensagem CAN
 * @return void
 */
void protocoloCAN_salvaRegistroCANFila(void * descritor ){
  Terro erro = SUCESSO;
  TmensagemCAN mensagem;
  Tempo inicio;
  PTdescritorSniffer desc = (PTdescritorSniffer)descritor;
  //Tempo teste_inicial, teste_final;


  inicio = micros();
  
  // Loop infinito   
  while(executando){

    // Alimenta cao de guarda na mao. Tive que fazer isso para nao parar a função so pra alimenta-lo
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed=1;
    TIMERG0.wdt_wprotect=0;    

    // Verifica se chegou alguma mensagens no buffer do MCP2515
        
    erro = (Terro)CAN.readMsgBuf_2(
      (INT32U*)&mensagem.identificador.extendido,         
      (INT8U*)&mensagem.tamanho, 
      (INT8U*)&(mensagem.dados[0])      
    );      

    // Recebeu mensagem, entao colocar na fila
    if(erro == CAN_OK){


      //teste_inicial = micros();
      mensagem.intervalo = (micros() - inicio);

      // Insere dado recebido na fila de mensagens CAN
      erro = filaMensagem_enfileirar(
        (PTfilaMensagem)&(desc->filaMensagem), 
        mensagem
      );
      if(erro != SUCESSO){
        // Se ocorreu algum erro, então acender led de CAN e sai do sistema
        digitalWrite(LED_ERRO_CAN,HIGH);                      
        PRINTLN("FALHA AO ENFILEIRAR!");
        protocoloCan_sairDoSistema();
        continue;
      }

      //teste_final = micros();
      //PRINTF("tempo enfileiramento: %d\r\n", (teste_final - teste_inicial));

      // Define um novo inicio para o intervalo de tempo para a proxima interação
      inicio = micros(); 



    }
  }  

  filaMensagem_finalizaFila((PTfilaMensagem)&(desc->filaMensagem));
  vTaskDelete(salvaRegistroCANFila);
}


/**
 * @brief  Função que será executada em um loop infinito dentro de um processo.
 *         Essa função irá retirar dados da fila e os enviar para os armazenadores, 
 *         sendo eles: servidor ou cartão de memória
 * @param  filaMensagem: Ponteiro para a estrutura de fila da mensagem CAN
 * @return void
 */
void protocoloCAN_enviaRegistroCANFila(void * descritor){  
  Terro erro = SUCESSO;
  Tuint16 controleMensagemBloco = 0;
  Tuint32 controleTamanhoArquivo = 0;  
  Tempo inicio;  
  Tuint16 tentativasEnvio = 0;     
  PTmensagemCAN mensagemTx;
  Tuint32 idArquivo = 0;
  char nomeArquivo[TAMANHO_BUFFER_MENSAGEM_REGISTRO];
  PTdescritorSniffer desc = (PTdescritorSniffer)descritor;
  /*
  UBaseType_t uxHighWaterMark;

  uxHighWaterMark = uxTaskGetStackHighWaterMark (NULL);
  PRINTF("PRIMEIRA CHAMADA: %d\r\n", uxHighWaterMark);
  */

  //Tempo teste_1,teste_2,total;

  // Recebe id do ultimo arquivo e incrementa 1 para gerar o proximo arquivo
  idArquivo = desc->configuracao.idArquivo;

  // Verifica qual o ultimo arquivo do sistema para que possa decidir-se:
  // Se o ultimo arquivo esta com tamanho zero, significa que ele apenas foi criado
  // mas nao foi escrito, nesse caso devo escrever os novos frames nesse mesmo arquivo
  // Caso contrario, o arquivo ja esta com um tamanho maior que zero, sigifnica que devo
  // criar um novo arquivo.
  erro = protocoloCAN_recuperaInformacaoUltimoArquivo(&idArquivo,nomeArquivo); 
  if(erro != SUCESSO){
    digitalWrite(LED_ERRO_CARTAO_MEMORIA,HIGH);  
    // Sair do sistema
    protocoloCan_sairDoSistema(); 
  }
 
  // ================ aloca espaço para buffer de mensagem ===================   
  mensagemTx = (PTmensagemCAN)malloc(sizeof(TmensagemCAN) * QUANTIDADE_MENSAGENS_POR_BLOCO);

  // Define tempo inicial para ser usado posteriormente  
  inicio = millis();

  // Conecta ao servidor
  if(desc->configuracao.wifi.conectado == VERDADEIRO){
    erro = sniferCanServidor_conecta(desc->configuracao.servidor.reg);
    if(erro != SUCESSO){
      desc->configuracao.wifi.conectado = FALSO;
    }
  }

  while(executando){    

    // Alimenta cao de guarda na mao. Tive que fazer isso para nao parar a função so pra alimenta-lo
    TIMERG1.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG1.wdt_feed=1;
    TIMERG1.wdt_wprotect=0;  
    
    // Se nao houver mensagem
    if(controleMensagemBloco == 0){
      digitalWrite(LED_SISTEMA_PRONTO,  HIGH);
    }

    // Verifica se há mensagens a serem desenfileiradas ou se ha mensagens a serem enviadas
    if((filaMensagem_tamanhoFila((PTfilaMensagem)&(desc->filaMensagem)) > 0) || (controleMensagemBloco > 0)){           

      // Desenfileira a mensagem e insere em um buffer local      
      erro =  filaMensagem_desenfileirar((PTfilaMensagem)&(desc->filaMensagem), &mensagemTx[controleMensagemBloco]);      
      if(erro == SUCESSO){
        // Se deu sucesso no enfileiramento da mensagem entao encrementa o contador
        controleMensagemBloco ++;        
        controleTamanhoArquivo ++;        
      }

      // Pisca led para indicar funcionamento do sistema
      if(((millis() - inicio) % TEMPO_ENTRE_PISCA_LED) == 0){
        digitalWrite(LED_SISTEMA_PRONTO,      !digitalRead(LED_SISTEMA_PRONTO));
      }
            
      /*   
      Verifica se o contador de mensagens ultrapassou o limite de mensagens suportadas pelo buffer local
      ou se tempo limite entre envios foi estourado. Alem disso, são enviadas mensagens se somente se
      houver mensagem no buffer local
      */     
      if( ((controleMensagemBloco == QUANTIDADE_MENSAGENS_POR_BLOCO)    || 
          ((millis() - inicio) > TEMPO_ENTRE_ENVIOS_REQUISICOES) ) &&  
           (HA_MENSAGEM_NO_BUFFER(controleMensagemBloco))
        ){ 

        if(controleTamanhoArquivo > TAMANHO_MAXIMO_ARQUIVO){
          controleTamanhoArquivo = 0;
          idArquivo ++;

          (void)sprintf(nomeArquivo, ((char *)"/REGISTROS/LOG-%04d.txt"), idArquivo);

          // Abre apenas para criar
          erro = gerenciamentoCartao_criaArquivo(nomeArquivo);
          if(erro != SUCESSO){
            // Se ocorreu algum erro, então acender led de cartão de memória e sai do sistema
            digitalWrite(LED_ERRO_CARTAO_MEMORIA,HIGH);            
            PRINTLN("ERRO NO ENVIO DOS DADOS!!!");         
            // Sair do sistema
            protocoloCan_sairDoSistema();
            continue;
          }
          
          // Atualiza last file
          erro = gerenciamentoCartao_atualizaLastFile(idArquivo);
          if(erro != SUCESSO){

            // Se ocorreu algum erro, então acender led de cartão de memória e sai do sistema
            digitalWrite(LED_ERRO_CARTAO_MEMORIA,HIGH);              
            PRINTLN("ERRO NO ENVIO DOS DADOS!!!");         
            // Sair do sistema
            protocoloCan_sairDoSistema();
            continue;
          }

        }
        
        //total = millis();
        //teste_1 = millis();

        tentativasEnvio = 0;
        // Envia dados para cartao micro SD
        do{

          erro = snifferCanRegistro_enviaDadosCartao(
            (PTmensagemCAN)&mensagemTx[0],
            controleMensagemBloco, 
            nomeArquivo,
            desc->configuracao.logFormatado,
            desc->configuracao.monitorSerial
          );
          if(erro != SUCESSO){
            PRINTF("ERRO NA TENTATIVA DE ENVIO AO CARTAO %d\r\n", tentativasEnvio);             
          }          
          tentativasEnvio ++;
          
        }while((erro != SUCESSO) && (tentativasEnvio < TENTATIVAS_ENVIO_BLOCO_MENSAGEM));        
        
        if(erro != SUCESSO){
          // Se ocorreu algum erro, então acender led de cartão de memória e sai do sistema
          digitalWrite(LED_ERRO_CARTAO_MEMORIA,HIGH);
          digitalWrite(LED_SISTEMA_PRONTO,LOW);
          PRINTLN("ERRO NO ENVIO DOS DADOS AO CARTAO DE MEMORIA!!!");  
          // Desconectar do servidor
          sniferCanServidor_desconecta();        
          protocoloCan_sairDoSistema();  
          continue; 
        }

        //PRINTF("TEMPO CARTAO: %d\r\n",(millis() - teste_1));
        //teste_2 = millis();        
        
        // Somente se foi conectado ao wifi, tenta enviar ao servidor
        if(desc->configuracao.wifi.conectado == VERDADEIRO){
          
          tentativasEnvio = 0;
          // Envia dados para servidor via http
          do{
            erro = snifferCanRegistro_enviaDadosServidor(
              (PTmensagemCAN)&mensagemTx[0],
              controleMensagemBloco,
              desc->configuracao.wifi,
              desc->configuracao.servidor.reg
            );
            if(erro != SUCESSO){
              PRINTF("ERRO NA TENTATIVA DE ENVIO AO SERVIDOR%d\r\n", tentativasEnvio);          
            }          
            tentativasEnvio ++;
            
          }while((erro != SUCESSO) && (tentativasEnvio < TENTATIVAS_ENVIO_BLOCO_MENSAGEM));        
          if(erro != SUCESSO){
            if(erro == ERRO_CONEXAO_WIFI){
              desc->configuracao.wifi.conectado = FALSO;              
            }
            // Se ocorreu algum erro, então acender led do servidor sai do sistema
            digitalWrite(LED_ERRO_SERVIDOR,HIGH);                                   
            PRINTLN("ERRO NO ENVIO DOS DADOS AO SERVIDOR!!!");          
          }
        }

        //PRINTF("TEMPO SERVIDOR: %d\r\n",(millis() - teste_2));
        //PRINTF("TEMPO TOTAL: %d\r\n",(millis() - total));
       
        // Inicializa novamente o contador de tempo        
        inicio = millis();

        // Zera contador de mensagens recebidas
        controleMensagemBloco = 0;      

      }
    }
    /*
    uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    PRINTF("SEGUNDA CHAMADA: %d\r\n", uxHighWaterMark);  
    */  
  }

  // Desconectar do servidor
  sniferCanServidor_desconecta(); 
  gerenciamentoCartao_finaliza();
 
  free(mensagemTx);
  vTaskDelete(enviaRegistroCANFila);
  
}