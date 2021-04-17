/**
 * @file    snifferCan_cartao.cpp
 * @brief   Esse arquivo contem as funções relativas ao envio de dados ao cartão de memória
 * @author  Emanoel Gomes Santos
 * @date    Data de Criação: 08/04/2020
**/

/// Inclusões de bibliotecas importantes
#include "snifferCan_cartao.h"

/**
 * @brief  Função que formata uma quantidade x de quadro CAN para uma string
 * @param  texto: ponteiro para a string que irá ser formatado
 * @param  mensagem: Ponteiro para o array com as mensagens CANs
 * @param  quantidade: quantidade de mensagens can presentes no array
 * @param  formatado: boleano que define se o quadro será formatado
 * @return ERRO ou SUCESSO
 */
Terro snifferCanCartao_formataQuadroCANToString(char *texto, PTmensagemCAN mensagem, 
                                               Tuint16 quantidade, Tbool formatado){
  Tuint16 i,j;
  Tuint16 ultimaPos;  
  char *buffer;
  Tuint16 tamanhoBuffer = (
    (sizeof(char) * (sizeof(TmensagemCAN) * 2))  + 
    ((formatado) ? QUANTIDADE_ESPACO_TEXTO_FORMATADO : QUANTIDADE_SEPARADORES_TEXTO) + 
    1 + 1
  );


  // Reserva espaço de uma mensagem
  buffer = (char*)malloc(tamanhoBuffer);
  if(buffer == NULL){
    PRINTLN("aqui");
    return ERRO_ALOCACAO_MEMORIA;
  }    

 

  for(i=0; i<quantidade; i++){
    ultimaPos = 0;
    
    // Salva texto o tipo float
    (void)sprintf(buffer,"%0.1f", (float)((float)(mensagem[i].intervalo)/1000));
    
    // Recupera ultima posição do texto gerado
    ultimaPos = strlen(buffer);
    
    // Se deseja texto formatdo, então inserir os espaço, modulagem de acordo com tamanho 
    if(formatado){
      for(j=ultimaPos; j< TAMANHO_DEFINIDO_ESPACO_ENTRE_TEMPO_ID; j++){
        buffer[ultimaPos++] = ' ';
      }      
    }
    // Do contrario, apenas insere ';'
    else{
      buffer[ultimaPos++] = ';';  
    }      
    
    // Insere identificadores verificando se é o extendido ou o padrao
    if((mensagem[i].identificador.extendido & 0xFFFF0000) != 0){
      
      buffer[ultimaPos++] = HEX_TO_ASCII(((mensagem[i].identificador.extendido >> 28) & 0x0F));
      buffer[ultimaPos++] = HEX_TO_ASCII(((mensagem[i].identificador.extendido >> 24) & 0x0F));
      buffer[ultimaPos++] = HEX_TO_ASCII(((mensagem[i].identificador.extendido >> 20) & 0x0F));
      buffer[ultimaPos++] = HEX_TO_ASCII(((mensagem[i].identificador.extendido >> 16) & 0x0F));
      buffer[ultimaPos++] = HEX_TO_ASCII(((mensagem[i].identificador.extendido >> 12) & 0x0F));
      buffer[ultimaPos++] = HEX_TO_ASCII(((mensagem[i].identificador.extendido >>  8) & 0x0F));
      buffer[ultimaPos++] = HEX_TO_ASCII(((mensagem[i].identificador.extendido >>  4) & 0x0F));
      buffer[ultimaPos++] = HEX_TO_ASCII(((mensagem[i].identificador.extendido >>  0) & 0x0F));
      
    }
    else{      
      buffer[ultimaPos++] = HEX_TO_ASCII(((mensagem[i].identificador.padrao >>  8) & 0x0F));
      buffer[ultimaPos++] = HEX_TO_ASCII(((mensagem[i].identificador.padrao >>  4) & 0x0F));
      buffer[ultimaPos++] = HEX_TO_ASCII(((mensagem[i].identificador.padrao >>  0) & 0x0F));
    }
    
    // Se for o log formatado, inseri espaços, do contrario apenas inserir ';'
    if(formatado){
      buffer[ultimaPos++] = ' ';
      buffer[ultimaPos++] = ' ';
      buffer[ultimaPos++] = ' ';
      buffer[ultimaPos++] = ' '; 
      buffer[ultimaPos++] = ' '; 
      buffer[ultimaPos++] = ' ';
    }else{
      buffer[ultimaPos++] = ';';  
    }

    // Insere tamanho de dados do frame CAN
    buffer[ultimaPos++] = HEX_TO_ASCII(((mensagem[i].tamanho >> 4) & 0x0F));
    buffer[ultimaPos++] = HEX_TO_ASCII(((mensagem[i].tamanho >> 0) & 0x0F));
    
    // Se for o log formatado, inseri espaços, do contrario apenas inserir ';' 
    if(formatado){
      buffer[ultimaPos++] = ' ';
      buffer[ultimaPos++] = ' ';
      buffer[ultimaPos++] = ' ';
    }else{
      buffer[ultimaPos++] = ';';       
    }
     
    // Insere as mensagens de acordo com tamanho de dados do frame.  
    for(j=0; j<mensagem[i].tamanho; j++){
      // Se for o estilo formatado, então inserir espaço entre os bytes
      if(formatado){
        buffer[(3*j)+ultimaPos+0] = HEX_TO_ASCII(((mensagem[i].dados[j] >> 4) & 0x0F));
        buffer[(3*j)+ultimaPos+1] = HEX_TO_ASCII(((mensagem[i].dados[j] >> 0) & 0x0F));
        buffer[(3*j)+ultimaPos+2] = ' '; 
      }else{
        buffer[(2*j)+ultimaPos+0] = HEX_TO_ASCII(((mensagem[i].dados[j] >> 4) & 0x0F));
        buffer[(2*j)+ultimaPos+1] = HEX_TO_ASCII(((mensagem[i].dados[j] >> 0) & 0x0F));
      }
    }

    if(formatado){
      // Se for formatado, então inserir nova linha, do contrario inserir apenas ';'
      buffer[(3 *j)+ultimaPos+0] = '\r';
      buffer[(3 *j)+ultimaPos+1] = '\n';
      buffer[(3 *j)+ultimaPos+2] = '\0';      
    }else{
      // Se for formatado, então inserir nova linha, do contrario inserir apenas ';'
      buffer[(2*j)+ultimaPos+0] = ';';
    }

    // Verifica se é a primeira interação, se for então apenas copia buffer para texto.
    // Do contrario concatena buffer com texto existente
    if(PRIMEIRA_INTERACAO){
      (void)strcpy(texto,buffer);
    }else{
      (void)strcat(texto,buffer);
    }
  }
  // Libera buffer da memória, pois foi alocado dinamicamente
  free(buffer);

  return SUCESSO;
  
}

/**
 * @brief  Função que envia o texto para o cartão de memória
 * @param  texto: ponteiro para a string que irá ser enviado
 * @return ERRO ou SUCESSO
 */
Terro snifferCanCartao_envia(char *texto, char *nomeArquivo){
  Terro erro = SUCESSO;
  
  digitalWrite(LED_SISTEMA_PRONTO, HIGH);

  // Envia dados ao cartao
  erro = gerenciamentoCartao_escreve(texto,nomeArquivo,eModoAppend);
  if(erro != SUCESSO){
    return erro;
  }  
  
  

  // Se chegou até aqui entao tudo ocorreu com sucesso
  return erro;
}




