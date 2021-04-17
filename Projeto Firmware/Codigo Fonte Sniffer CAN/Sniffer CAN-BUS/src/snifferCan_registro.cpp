/**
 * @file    snifferCan_registro.cpp
 * @brief   Esse arquivo contem as funções relativas ao envio do registro para 
 *          os "armazenadores", sendo: servidor ou cartão de memória
 * @author  Emanoel Gomes Santos
 * @date    Data de Criação: 08/04/2020
**/

/// Inclusões de bibliotecas importantes
#include "snifferCAN_registro.h"

// Definições importantes
#define PRIMEIRA_INTERACAO       (i==0)

// Variavel global 
char *textoEnvia; 

/**
 * @brief  Função que obtem o ponteiro para o tipo char * do texto 
 * @param  void
 * @return ponteiro para textoEnvia
 */
char *snifferCanRegistro_obtemPonteiroTexto(void){
  return textoEnvia;
}

/**
 * @brief  Função que formata os dados e os envia ao servidor
 * @param  mensagem: Ponteiro para o array com as mensagens CANs
 * @param  quantidade: quantidade de mensagens can presentes no array
 * @return ERRO ou SUCESSO
 */
Terro snifferCanRegistro_enviaDadosServidor(PTmensagemCAN mensagem, Tuint16 quantidade, 
                                            TwifiConfig wifi, char *url){
  Terro erro = SUCESSO;
  char *texto = snifferCanRegistro_obtemPonteiroTexto();
  Tuint16 tamanhoTexto;

  /*
  1 - aloca quantidade de bytes da estrutura TmensagemCAN -> sizeof(TmensagemCAN)
  2 -  Como cada neeble irá gerar um byte de texto, entao deverá ser multiplicado por 2 -> (sizeof(TmensagemCAN) * 2)
  3 - O contexto acima será pra uma mensagem can, mas estamos trabalhando com array de mensagens. por esse motivo,
      será mutiplicado pela quantidade -> (sizeof(TmensagemCAN) * 2) * quantidade)
  4 - Haverá um separador para cada mensagen, que sera o caracter ";", por esse motivo somado a quantidade de mensagens
      sendo -> (sizeof(TmensagemCAN) * 2) * quantidade) + quantidade
  5 - Deverá ser considerado o terminador do tipo texto "\0", que consome 1 byte. Por esse motivo a soma final 
      sendo -> ((sizeof(char) * (sizeof(TmensagemCAN) * 2) * quantidade) + quantidade + 1)
  */
  tamanhoTexto = (
    (sizeof(char) * (sizeof(TmensagemCAN) * 2) * quantidade) + 
    ( QUANTIDADE_SEPARADORES_TEXTO * quantidade) + 
    1
  );

  // Aloca espaço para texto  
  texto = (char*)malloc(tamanhoTexto);
  if(texto == NULL){
    return ERRO_ALOCACAO_MEMORIA;
  }
  
  // Formata o texto   
  erro = snifferCanServidor_formataQuadroCANToString(
    texto,
    mensagem,
    quantidade,
    FALSO // sempre falso, quem formata é o software do servidor
  );
  if(erro != SUCESSO){
    free(texto);
    return erro;
  }  

  // Envia dados formatados
  erro = snifferCanServidor_envia(texto, wifi, url);
  if(erro != SUCESSO){
    free(texto);
    return erro;
  }

  free(texto);

  return erro;
}

/**
 * @brief  Função que formata os dados e os envia ao cartão de memória
 * @param  mensagem: Ponteiro para o array com as mensagens CANs
 * @param  quantidade: quantidade de mensagens can presentes no array
 * @param  formatado: Flag que define se o log deverá ou nao ser formatado
 * @return ERRO ou SUCESSO
 */
Terro snifferCanRegistro_enviaDadosCartao(PTmensagemCAN mensagem, Tuint16 quantidade, 
                                          char *nomeArquivo, Tbool logFormatado, Tbool monitorSerial){
  Terro erro = SUCESSO;
  char *texto = snifferCanRegistro_obtemPonteiroTexto();
  Tuint16 tamanhoTexto;    
  /*
  1 - aloca quantidade de bytes da estrutura TmensagemCAN -> sizeof(TmensagemCAN)
  2 -  Como cada neeble irá gerar um byte de texto, entao deverá ser multiplicado por 2 -> (sizeof(TmensagemCAN) * 2)
  3 - O contexto acima será pra uma mensagem can, mas estamos trabalhando com array de mensagens. por esse motivo,
      será mutiplicado pela quantidade -> (sizeof(TmensagemCAN) * 2) * quantidade)
  4 - Haverá espaços para o texto formatado que esta definido em QUANTIDADE_ESPAÇO_TEXTO_FORMATADO
      sendo -> (sizeof(TmensagemCAN) * 2) * quantidade) + quantidade
  5 - Deverá ser considerado o terminador do tipo texto "\0", que consome 1 byte. Por esse motivo a soma final 
      sendo -> ((sizeof(char) * (sizeof(TmensagemCAN) * 2) * quantidade) + quantidade + 1)
  */
  tamanhoTexto = (
    (sizeof(char) * (sizeof(TmensagemCAN) * 2) * quantidade) + 
    (((logFormatado) ? QUANTIDADE_ESPACO_TEXTO_FORMATADO : QUANTIDADE_SEPARADORES_TEXTO) * quantidade) + 
    1
  );
 
  // Aloca espaço na memória para o texto
  texto = (char*)malloc(tamanhoTexto);
  if(texto == NULL){
    PRINTLN("texto = (char*)malloc(tamanhoTexto);");
    return ERRO_ALOCACAO_MEMORIA;
  }  
  
  // Formata o texto
  erro = snifferCanCartao_formataQuadroCANToString(
    texto,
    mensagem, 
    quantidade, 
    logFormatado
  );
  if(erro != SUCESSO){
    free(texto);
    return erro;
  }
     

  // Envia dados formatados ao cartão
  erro = snifferCanCartao_envia(texto,nomeArquivo);
  if(erro != SUCESSO){
    free(texto);
    return erro;
  }


  // Imprime log na tela do monitor serial
  if(monitorSerial){
    PRINT(texto);
  }

  // Se houve sucesso entao desalocar texto.
  free(texto);
  
  // Se chegou até aqui então houve sucesso
  return SUCESSO;
}