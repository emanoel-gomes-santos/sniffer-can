/**
 * @file    sniferCan_servidor.cpp
 * @brief   Esse arquivo contem as funções relativas ao envio de dados ao servidor
 * @author  Emanoel Gomes Santos
 * @date    Data de Criação: 31/03/2020
**/

/// Inclusões de bibliotecas importantes
#include "snifferCan_servidor.h"

// Variavel da estrutura do http
HTTPClient http;

/**
 * @brief  Função que conecta ao servidor
 * @param  texto: ponteiro para a string que irá ser formatado
 * @param  mensagem: Ponteiro para o array com as mensagens CANs
 * @param  quantidade: quantidade de mensagens can presentes no array
 * @return ERRO ou SUCESSO
 */
Terro sniferCanServidor_conecta(char *url){
  // Especifica o destino para a requisição HTTP 
  if(!http.begin(url)){
    return ERRO_CONEXAO_SERVIDOR;
  }
  if(!http.connect()){
    return ERRO_CONEXAO_SERVIDOR;
  }  
  
  // Configura 
  http.setReuse(VERDADEIRO);  
  return SUCESSO;

}

void sniferCanServidor_desconecta(void){
  // Especifica o destino para a requisição HTTP 
  http.end();
}

/**
 * @brief  Função que formata uma quantidade x de quadro CAN para uma string
 * @param  texto: ponteiro para a string que irá ser formatado
 * @param  mensagem: Ponteiro para o array com as mensagens CANs
 * @param  quantidade: quantidade de mensagens can presentes no array
 * @return ERRO ou SUCESSO
 */
Terro snifferCanServidor_formataQuadroCANToString(char *texto, PTmensagemCAN mensagem, 
                                                 Tuint16 quantidade, Tbool formatado){
  Tuint16 i,j;
  Tuint16 ultimaPos;  
  char *buffer;
  Tuint16 tamanhoBuffer = (
    (sizeof(char) * (sizeof(TmensagemCAN) * 2))  + 
    ((formatado) ? QUANTIDADE_ESPACO_TEXTO_FORMATADO : QUANTIDADE_SEPARADORES_TEXTO) + 
    1
  );

  // Reserva espaço de uma mensagem
  buffer = (char*)malloc(tamanhoBuffer);
  if(buffer == NULL){
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

    // Se for formatado, então inserir nova linha, do contrario inserir apenas ';'
    buffer[(((formatado) ? (3) : (2))*j)+ultimaPos+0] = (formatado)? ('\n') : (';');
    buffer[(((formatado) ? (3) : (2))*j)+ultimaPos+1] = '\0';

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
 * @brief  Funçãoo que envia uma string formatada ao servidor 
 * @param  texto: Ponteiro para texto a ser enviado
 * @return ERRO ou SUCESSO
 */
Terro snifferCanServidor_envia(char *texto, TwifiConfig wifi, char *url){
  Terro erro = SUCESSO;     
  int status;

  // Verifica conexao com a internet ates de tentar enviar os dados
  erro = snifferCANWiFi_verificaConexao();
  if(erro != SUCESSO){
    erro = snifferCANWiFi_conecta(wifi.login,wifi.senha, 1);
    if(erro != SUCESSO){
      // Desaloca texto      
      return erro;
    }
  }
  // Especifica cabeçalho
  http.addHeader("Content-Type", "text/plain");
  http.addHeader("Connection", "keep-alive");

  http.setReuse(VERDADEIRO);

  // Envia texto
  status = http.POST((Tuint8*)texto, strlen(texto));

  // Verifica se foi com algum tipo de erro
  if(status < 1){
    // Se não envio após todas as tentativas, entao retornar erro
    PRINTLN(status);
    http.setReuse(FALSO);
    sniferCanServidor_desconecta();
    return ERRO_ENVIO_DADOS_SERVIDOR;
  }
  // Se não houve erro então foi com sucesso
  else{
    //PRINTLN(texto);
  }

  sniferCanServidor_desconecta();

 // Se chegou até aqui entao tudo ocorreu com sucesso
  return SUCESSO;
}
Terro snifferCanServidor_le(String url, TwifiConfig wifi, String *dadosLido){
  Terro erro = SUCESSO;
  int status;

  // Verifica conexao com a internet ates de tentar enviar os dados
  erro = snifferCANWiFi_verificaConexao();
  if(erro != SUCESSO){
    erro = snifferCANWiFi_conecta(wifi.login,wifi.senha, 2);
    if(erro != SUCESSO){
      // Desaloca texto      
      return erro;
    }
  }  
  
  // Le texto
  status = http.GET();

  // Verifica se foi enviado com sucesso
  if(status < 1){
    sniferCanServidor_desconecta();
    // Se não envio após todas as tentativas, entao retornar erro
    return ERRO_ENVIO_DADOS_SERVIDOR;
  } 
  *dadosLido = http.getString();

  sniferCanServidor_desconecta();

  return erro;
}