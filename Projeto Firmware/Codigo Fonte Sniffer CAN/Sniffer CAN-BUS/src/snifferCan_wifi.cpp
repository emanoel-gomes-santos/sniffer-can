/**
 * @file    snifferCAN_wifi.cpp
 * @brief   Arquivo com as funções relativas ao wifi
 * @author  Emanoel Gomes Santos 
 * @date    Data de Criação: 01/04/2020
 */
#include <snifferCAN_wifi.h>

// Definições importantes
#define MAX_TENTATIVAS_COMUNICACAO_WIFI  3//100
#define TIME_OUT_CONEXAO_INTERNET        200

/**
 * @brief  Função que conecta o wifi
 * @param  ssid: Login do wifi
 * @param  password: senha do wifi 
 * @return void
*/
static void snifferCANWiFiConectaWifi(char *ssid, char *password){  
  // Conecta na rede WI-FI
  WiFi.begin(ssid, password); 
}

/**
 * @brief   verifica o estado das conexões WiFI
 * @return  ERRO_CONEXAO_WIFI ou SUCESSO
 */
Terro snifferCANWiFi_verificaConexao(void){
  if((WiFi.status() != WL_CONNECTED)){
    return ERRO_CONEXAO_WIFI;
  }
  return SUCESSO;
}

/**
 * @brief   Função que inicializa a conexão com a rede wifi determinada
 * @param  ssid: Login do wifi
 * @param  password: senha do wifi 
 * @return erro ou SUCESSO
*/
Terro snifferCANWiFi_conecta(char *ssid, char *password, Tuint16 maxTentativas){
    Terro erro = SUCESSO;
    Tuint16 tentativas;

    PRINTLN("\n------Conexao WI-FI------");
    PRINT("Conectando-se na rede: ");
    PRINTLN(ssid);
    PRINT("Aguarde");
  
    tentativas = 0;

    do{
      snifferCANWiFiConectaWifi(ssid, password);
      erro = snifferCANWiFi_verificaConexao();
      if(erro != SUCESSO){
        PRINT(".");
        delay(1000); // somente aceita acima de 1 segundo
        tentativas ++;
      }      
      
    }while((erro != SUCESSO) && (tentativas < maxTentativas));
    
    if(erro != SUCESSO){
      return erro;     
    }   

    // Após o sucesso da conexão mostrar detalhes
    PRINTLN("\n");
    PRINT("Rede conectada: ");
    PRINTLN(ssid);
    PRINT("IP obtido: ");
    PRINTLN(WiFi.localIP());
    PRINTLN("Conectado com sucesso na rede WIFI!");

 
    return erro;
}

