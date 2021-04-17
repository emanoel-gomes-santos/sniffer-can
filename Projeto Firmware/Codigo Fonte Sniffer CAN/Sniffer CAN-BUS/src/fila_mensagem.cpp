/**
 * @file    fila_mensagem.cpp
 * @brief   Esse arquivo contem a implementação das funções relativas a implementação da estrutura de dados fila
 * @author  Emanoel Gomes Santos
 * @date    Data de Criação: 30/03/2020
**/

/// Inclusções de bibliotecas importantes
#include "fila_mensagem.h"

// Criação do semaforo
SemaphoreHandle_t mutex;
SemaphoreHandle_t sistema;

/**
 * @brief  Funçãoo que inicializa a fila , criando os ponteiros para o inicio e fim da fila
 *         Alem disso, cria-se o semaforo mutex para controle de região crítica
 * @param  fila: Ponteiro para fila que será criada
 * @param  tamanho: Tamanho da fila que deseja-se criar  
 * @return erro ou SUCESSO
 */
Terro filaMensagem_inicializaFila(PTfilaMensagem fila, Tuint32 tamanho){
  Tuint16 tentativas = 0;
  
  // Cria semaforo
  do{    
    mutex = xSemaphoreCreateMutex();
    tentativas ++;
  }while((mutex == NULL) && (tentativas < 100));
  if(mutex == NULL){
    return ERRO_CRIACAO_SEMAFORO;
  }

  do{    
    sistema = xSemaphoreCreateMutex();
    tentativas ++;
  }while((sistema == NULL) && (tentativas < 100));
  if(sistema == NULL){
    return ERRO_CRIACAO_SEMAFORO;
  }

  
  // Armazena o tamanho maximo da lista
  fila->capacidadeMax = tamanho;
  // Aloca a quantidade de dados maxima
  fila->mensagem = (PTmensagemCAN)malloc(sizeof(TmensagemCAN) * fila->capacidadeMax); // 13*5
  if(!(fila->mensagem)){
    return ERRO_ALOCACAO_MEMORIA;
  }
  // Definições iniciais dos ponteiros e tamanho
  fila->primeiro = 0;
	fila->ultimo =  0;
	fila->tamanhoAtual = 0; 

  return SUCESSO;
}

/**
 * @brief  Funçãoo que finaliza a fila, apenas desaloca ponteiro da fila da memória
 * @param  fila: Ponteiro para fila que será destruída 
 * @return erro ou SUCESSO
 */
void filaMensagem_finalizaFila(PTfilaMensagem fila){
  xSemaphoreTake(mutex,portMAX_DELAY);
  if(fila->mensagem != NULL){
    fila->primeiro = 0;
    fila->ultimo =  0;
    fila->tamanhoAtual = 0;     
    free((PTmensagemCAN)(fila->mensagem));
  }  
  xSemaphoreGive(mutex);
}

/**
 * @brief  Função verifica o tamanho da fila
 * @param  fila: Ponteiro para fila que sera criada
 * @return tamanho da fila
 */
Tuint32 filaMensagem_tamanhoFila(PTfilaMensagem fila){
  Tuint32 tamanho;

  // Aguarda semaforo esta liberado para entao mecher na fila
  xSemaphoreTake(mutex,portMAX_DELAY);
  tamanho = fila->tamanhoAtual;
  xSemaphoreGive(mutex);

  return tamanho;
}

/**
 * @brief  Função que verifica se a fila esta vazia
 * @param  fila: Ponteiro para fila que sera criada
 * @return VERDADEIR ou FALSO
 */
static Tbool verificaSeFilaEstaVazia(PTfilaMensagem fila){  
  Tbool filaVazia;
  
  // Aguarda semaforo esta liberado para entao mecher na fila
  xSemaphoreTake(mutex,portMAX_DELAY);  
  filaVazia = (fila->tamanhoAtual == 0);
  xSemaphoreGive(mutex);
  
  return filaVazia;
}

/**
 * @brief  Função que insere uma celula do tipo TmensagemCAN no fim da fila
 * @param  fila: Ponteiro para fila que sera atualizada
 * @param  mensagem: Dado do tipo TmensagemCAN que será armazenado
 * @return ERRO ou SUCESSO
 */
Terro filaMensagem_enfileirar(PTfilaMensagem fila, TmensagemCAN mensagem){

  // Aguarda semaforo esta liberado para entao mecher na fila
  xSemaphoreTake(mutex,portMAX_DELAY);

  if((fila->mensagem) == NULL){
    return ERRO_FILA_MENSAGEM_DESALOCADA;
  }
  
  // Armazena dado recebido na fila
  fila->mensagem[fila->ultimo] = mensagem;  
 
  // Se o ultimo lugar livre for a capacidade, então deverá rodar a roleta
  // exemplo: roleta tamanho 3 (0 a 2), se estiver na posição 2 devera ir para posição 0
  if((fila->ultimo + 1) == fila->capacidadeMax){
    fila->ultimo = 0;
  }else{
    // Incrementa ultimo que foi adicionado na fila
    fila->ultimo++;
  }   

  // Atualiza tamanho da fila +1
  fila->tamanhoAtual ++;
  
  // Verifica se esta na capacidade maxima, se estiver deverá incrementar o primeiro e o tamanho
  // da fila recebera capacidade maxima
  if(fila->tamanhoAtual == fila->capacidadeMax){
    fila->primeiro ++;
    fila->primeiro %= fila->capacidadeMax;
    fila->tamanhoAtual = (fila->capacidadeMax-1);
  }
  
  // Libera semaforo
  xSemaphoreGive(mutex);

  return SUCESSO;

}

/**
 * @brief  Função que insere uma celula do tipo TmensagemCAN no inicio da fila
 * @param  fila: Ponteiro para fila que sera atualizada
 * @param  mensagem: Ponteiro para receber o dado removido
 * @return ERRO ou SUCESSO
 */
Terro filaMensagem_desenfileirar(PTfilaMensagem fila, PTmensagemCAN mensagem){
  
  // Verifica se há itens na fila
  if(verificaSeFilaEstaVazia(fila)){
    return ERRO_FILA_VAZIA;
  }

  // Aguarda semaforo esta liberado para entao mecher na fila
  xSemaphoreTake(mutex,portMAX_DELAY);

  if((fila->mensagem) == NULL){
    return ERRO_FILA_MENSAGEM_DESALOCADA;
  }
  
  // Retira dados da fila e armazena na estrutura can
  (void)memcpy(mensagem, &fila->mensagem[fila->primeiro], sizeof(TmensagemCAN));

  // incrementa posição do primeiro da fila
  fila->primeiro ++;

  // Verifica se ultrapassou os limites da fila
  if(fila->primeiro == fila->capacidadeMax){
    fila->primeiro = 0;
  }
  
  // Atualiza tamanho atual da fila
  fila->tamanhoAtual --;

  // Libera semaforo
  xSemaphoreGive(mutex);
  
  return SUCESSO;
}

