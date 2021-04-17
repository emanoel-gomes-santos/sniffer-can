/**
 * @file    gerenciamento_cartao.cpp
 * @brief   Esse arquivo contem a implementação das funções relativas ao gerenciamento
 *          do cartao micro SD
 * @author  Emanoel Gomes Santos
 * @date    Data de Criação: 10/04/2020
**/

/// Inclusões de bibliotecas importantes
#include "gerenciamento_cartao.h"

/// String com o arquivo padrão de configurações
static const String conteudo_file_configuracoes = 
(
  "------------------------\nConfiguracoes do WIFI\n------------------------\nLogin: \"snifferCAN\"\nSenha: \"123456789\"\n\n------------------------\nLista de identificadores\n------------------------\nIdentificadores: \"7E0;7E8\"\n\n------------------------\nTaxa de Comunicacao\n------------------------\nTaxa: \"500KBPS\"\n\n------------------------\nURL Servidor\n------------------------\nURL Registros: \"---\"\nURL Taxa: \"---\"\nURL Filtros: \"---\"\n\n------------------------\nDeseja log formatado?\n------------------------\nLog Formatado: \"sim\"\n------------------------\nDeseja ativar monitor serial?\n------------------------\nMonitor Serial: \"sim\""
);
/// String com o arquivo padrão de system
static const String conteudo_file_system = 
(
  "Sniffer CAN / Versão 1.0 / EMANOEL GOMES SANTOS\nLast File: \"0\""
);

#define QUANTIDADE_TAXAS_CONHECIDAS   15
#define POS_TAXA_DEFAULT_500_KBPS     13

static const TtabelaTaxas tabela_taxas_conhecidas[QUANTIDADE_TAXAS_CONHECIDAS] = {
  {("4K096BPS"),TAXA_4K096BPS},
  {("5KBPS"   ),TAXA_5KBPS},
  {("10KBPS"  ),TAXA_10KBPS},
  {("20KBPS"  ),TAXA_20KBPS},
  {("31K25BPS"),TAXA_31K25BPS},
  {("33K3BPS" ),TAXA_33K3BPS},
  {("40KBPS"  ),TAXA_40KBPS},
  {("50KBPS"  ),TAXA_50KBPS},
  {("80KBPS"  ),TAXA_80KBPS},
  {("100KBPS" ),TAXA_100KBPS},
  {("125KBPS" ),TAXA_125KBPS},
  {("200KBPS" ),TAXA_200KBPS},
  {("250KBPS" ),TAXA_250KBPS},
  {("500KBPS" ),TAXA_500KBPS},
  {("1000KBPS"),TAXA_1000KBPS},
};

char * getStringTaxa(TaxaComunicacao taxa){
  return (char *)tabela_taxas_conhecidas[taxa].descricao;
}

/**
 * @brief  Função que escreve mensagem de erro no cartão para log de erros 
 * @param  msgErro: string com a mensagem de erro
 * @return erro ou SUCESSO
 */
Terro gerenciamentoCartao_escreveMensagemERRO(char *msgErro){
  Terro erro = SUCESSO;
  File arquivo;

  if(!SD.exists("/ERRO")){
    SD.mkdir("/ERRO");
    
    if(!SD.exists("/ERRO")){
      return ERRO_AO_CRIAR_DIRETORIO_CARTAO;
    }
  }

  arquivo = SD.open("/ERRO/log.txt", FILE_APPEND);
  if(!arquivo){
    return ERRO_ABRIR_CARTAO_PARA_ESCRITA;
  }

  arquivo.print(erro);

  arquivo.close();

  return erro;
}

/**
 * @brief  Função que inicializa o micro sd definido por um cs (pino do SPI)
 * @param  CS: pino de comunicação do SPI
 * @return ERRO ou SUCESSO
 */
static Terro inicializaCartaoMicroSD(Tuint8 cs){
  return ((SD.begin(cs)) ? SUCESSO : ERRO_CARTAO_NAO_ENCONTRADO);
}
/**
 * @brief  Função que finaliza o micro sd  
 * @return ERRO ou SUCESSO
 */
void gerenciamentoCartao_finaliza(void){
  SD.end();
}
/**
 * @brief  Função que inicializa o micro sd definido por um cs (pino do SPI)
 * @param  CS: pino de comunicação do SPI
 * @return ERRO ou SUCESSO
 */
Terro gerenciamentoCartao_inicializa(Tuint8 cs){
  Terro erro = SUCESSO;
  Tuint8 tentativas = 0;
  File arquivo;
  String info;
  
  PRINTLN();
  PRINTLN("------Inicializando cartao------");
  PRINT("Aguarde");

  tentativas = 0;
  do{
  
    erro = inicializaCartaoMicroSD(CS_PIN_MODULO_MICRO_SD);
    if(erro != SUCESSO){
      PRINT(".");
      delay(1000); // somente aceita acima de 1 segundo
      tentativas ++;
    }      
    
  }while((erro != SUCESSO) && (tentativas < MAX_TENTATIVAS_CARTAO));
  
  if(erro != SUCESSO){
    return erro;     
  } 

  // Verifica se existe a pasta SETUP
  if(!SD.exists("/SETUP")){
    // Cria diretório
    SD.mkdir("/SETUP");
    // Confere novamente após criação
    if(!SD.exists("/SETUP")){
      return ERRO_AO_CRIAR_DIRETORIO_CARTAO;
    }
  }
  // Confere se existe arquivo de configuração
  if(!SD.exists(NOME_ARQUIVO_CONFIGURACAO)){
    
    arquivo = SD.open(NOME_ARQUIVO_CONFIGURACAO, FILE_WRITE);
    if(!arquivo){
      return ERRO_ABRIR_CARTAO_PARA_ESCRITA;
    } 

    // Escreve conteúdo padrao
    arquivo.print(conteudo_file_configuracoes);
    arquivo.close();       

  }  
  // Confere se existe arquivo de configuração
  if(!SD.exists(NOME_ARQUIVO_REGISTRO_INTERNO)){
    
    arquivo = SD.open(NOME_ARQUIVO_REGISTRO_INTERNO, FILE_WRITE);
    if(!arquivo){
      return ERRO_ABRIR_CARTAO_PARA_ESCRITA;
    } 
    
    // Escreve conteúdo padrao
    arquivo.print(conteudo_file_system);
    arquivo.close();       

  }

  // Confere se existe pasta REGISTROS
  if(!SD.exists("/REGISTROS")){
    // Cria diretório
    SD.mkdir("/REGISTROS");
    // Confere novamente após criação
    if(!SD.exists("/REGISTROS")){
      return ERRO_AO_CRIAR_DIRETORIO_CARTAO;
    }

    arquivo = SD.open(NOME_ARQUIVO_REGISTRO_INTERNO,FILE_WRITE);
    if(!arquivo){
      return ERRO_ABRIR_CARTAO_PARA_ESCRITA;
    } 
    
    // Escreve conteúdo padrao
    arquivo.print(conteudo_file_system);
    arquivo.close();  
  } 
  // Após o sucesso da conexão mostrar detalhes
  PRINTLN("\n");
  PRINTLN("Cartão encontrado e pronto para ser utilizado!!!\r\n");

  return erro;
}

/**
 * @brief  Funçãoo que recupera o tamanho do arquivo desejado 
 * @param  caminho: Caminho do arquivo que se deseja obter o tamanho 
 * @return erro ou SUCESSO
 */
Terro gerenciamentoCartao_tamanhoArquivo(char *caminho, Tuint32 *tamanho){
  File arquivo;

  arquivo = SD.open(caminho,FILE_READ);
  if(!arquivo){
    return ERRO_ABRIR_CARTAO_PARA_LEITURA;
  }
  *tamanho = arquivo.size();

  arquivo.close();

  return SUCESSO;
}

/**
 * @brief  Funçãoo que cria um arquivo de acordo com caminho especificado
 * @param  caminho: Caminho do arquivo que se deseja obter o tamanho 
 * @return erro ou SUCESSO
 */
Terro gerenciamentoCartao_criaArquivo(char *caminho){  
  File arquivo;

  arquivo = SD.open(caminho,FILE_WRITE);
  if(!arquivo){
    return ERRO_ABRIR_CARTAO_PARA_LEITURA;
  }

  arquivo.close();

  // Se chegou até aqui entao tudo ok
  return SUCESSO;
}

/**
 * @brief  Funçãoo que verifica se ha espaço livre para provavel escrita no cartao
 * @return VERDADEIRO OU FALSO
 */
Tbool gerencimanentoCartao_haEspacoLivre(void){
  return ((SD.totalBytes() - SD.usedBytes()) > TAMANHO_BUFFER_10M);
}

/**
 * @brief  Função que escreve uma string no caminho especificado
 * @param  caminho: Caminho do arquivo que se deseja obter o tamanho 
 * @return erro ou SUCESSO
 */
Terro gerenciamentoCartao_escreve(char *texto, const char *caminho, TmodoEscrita modo){
  File arquivo;
  size_t tamanho;
  
  /// Somente se couber no cartao pode ser atualizado
  if(gerencimanentoCartao_haEspacoLivre()){ 

    arquivo = SD.open(caminho,((modo==eModoAppend) ? FILE_APPEND : FILE_WRITE));
    if(!arquivo){
      return ERRO_ABRIR_CARTAO_PARA_ESCRITA;
    }  

    tamanho = arquivo.print(texto);
    if(tamanho == 0){
      return ERRO_ABRIR_CARTAO_PARA_ESCRITA;
    }

    arquivo.close();   
    
    return SUCESSO;
  }
  
  return ERRO_LIMITE_EXCEDIDO_CARTAO_MEMORIA; 

}

/**
 * @brief  Função que escreve o ultimo arquivo de registro criado no diretorio system,
 *         para controle do sistema
 * @param  id: identificador do ultimo arquivo
 * @return erro ou SUCESSO
 */
Terro gerenciamentoCartao_atualizaLastFile(Tuint32 id){
  Terro erro = SUCESSO;
  File arquivo;
  char buffer[100];

  arquivo = SD.open(NOME_ARQUIVO_REGISTRO_INTERNO, FILE_WRITE);
  if(!arquivo){
    return ERRO_ABRIR_CARTAO_PARA_ESCRITA;
  }

  (void)sprintf(
    buffer, 
    "Sniffer CAN / Versão 1.0 / EMANOEL GOMES SANTOS\nLast File: \"%d\"",
    id
  );

  arquivo.print(buffer);

  arquivo.close();
 
  return erro;
}

/**
 * @brief  Função que busca uma informação no dump completo do arquivo de configuração,
 * @param  texto: dump completo do arquivo de configuração
 * @param  indice: indice que se deseja encontrar ex: "login:"
 * @param  infoEncontrada: Informação encontrada
 * @return erro ou SUCESSO
 */
Terro gerenciamentoCartao_escreveInformacao(String texto, const char *indice, String informacao){
  Tuint32 i;
  char buffer[500];
  
  i = 0;

  // Busca string indice
  while(texto[i] != '\0'){
    if(memcmp(&texto[i], indice, strlen(indice)) == 0 ){
      i += strlen(indice);
      break;
    }
    i++;
  }
  // Se o laço chegou até o final significa que nao encontrou indice
  if(texto[i]  == '\0'){
    return ERRO_ARQUIVO_CONFIGURACAO_CORROMPIDO;
  }

  texto[i] = ' ';
  i++;
  texto[i] = '\"';
  i++;

  (void)memcpy(&texto[i],&informacao[0], informacao.length());
  texto[i+informacao.length()+0] = '\"';
  texto[i+informacao.length()+1] = '\r';
  texto[i+informacao.length()+2] = '\n';

  texto.toCharArray(buffer,500);
  

  return gerenciamentoCartao_escreve(buffer,NOME_ARQUIVO_CONFIGURACAO, eModoWrite);

}

/**
 * @brief  Função que busca uma informação no dump completo do arquivo de configuração,
 * @param  texto: dump completo do arquivo de configuração
 * @param  indice: indice que se deseja encontrar ex: "login:"
 * @param  infoEncontrada: Informação encontrada
 * @return erro ou SUCESSO
 */
Terro gerenciamentoCartao_buscaInformacao(String texto, const char *indice, String *infoEncontrada){
  Terro erro = SUCESSO;
  Tuint32 i, posicao;
  char buffer[500];
  
  i = 0;
  posicao = 0;

  // Busca string Login
  while(texto[i] != '\0'){
    if(memcmp(&texto[i], indice, strlen(indice)) == 0 ){
      i += strlen(indice);
      break;
    }
    i++;
  }
  // Se o laço chegou até o final significa que nao encontrou "Login"
  if(texto[i]  == '\0'){
    return ERRO_ARQUIVO_CONFIGURACAO_CORROMPIDO;
  }
  // Tenta encontrar primeiras aspas
  while(texto[i] != '\0'){
    if(texto[i] == '\"'){ 
      i++;
      break; 
    }
    i++;
  }
  
  // Se o laço chegou até o final significa que nao encontrou as primeiras aspas obrigatória
  if(texto[i]  == '\0'){
    return ERRO_ARQUIVO_CONFIGURACAO_CORROMPIDO;
  }
  
  posicao = 0;
  // Armazena informação completa entre as aspas
  while((texto[i] != '\"') && (texto[i] != '\0')){
    buffer[posicao] = texto[i];
    i++;
    posicao ++;
  }


  if(texto[i] == '\"'){ 
    if(posicao == 0){
      (void)strcpy(buffer,"---");      
    }else{
      buffer[posicao] = '\0';    
    }
  }else{
    return ERRO_ARQUIVO_CONFIGURACAO_CORROMPIDO;
  }

  // SALVA INFORMAÇÃO ENCONTRADA
  *infoEncontrada = buffer;
    
  return erro;
}

/**
 * @brief  Função que obtem o id do ultimo arquivo registrado no dump
 * @param  idArquivo: variável que receberá o ultimo ID
 * @return erro ou SUCESSO
 */
Terro gerenciamentoCartao_obtemUltimoIdArquivoRegistro(Tuint16 *idArquivo){   
  Terro erro = SUCESSO;  
  File arquivo;
  String buffer;  
  String valor;
  const char lastString [] {"Last File:"};
  
  // Abre arquivo para leitura
  arquivo = SD.open(NOME_ARQUIVO_REGISTRO_INTERNO, FILE_READ);
  if(!arquivo){
    return ERRO_LEITURA_CARTAO;
  }

  buffer = arquivo.readString();
    
  // Busca informação do arquivo system Last File:
  erro = gerenciamentoCartao_buscaInformacao(buffer,lastString,&valor);
  if(erro != SUCESSO){
    return erro;
  }
  
  *idArquivo = ((Tuint32)valor.toInt());

  // Se chegou até aqui então tudo ocorreu bem
  return SUCESSO;
}

/**
 * @brief  Função que obtem o login e a senha recuperados do arquivo de configuração
 * @param  configuracaoWifi: Estrutura que armazenará a informação
 * @return erro ou SUCESSO
 */
Terro gerenciamentoCartao_obtemLoginSenha(PTwifiConfig configuracaoWifi){
  Terro erro = SUCESSO;
  File arquivo;
  String texto;
  const char login[] = {"Login:"};
  const char senha[] = {"Senha:"};
  String buffer;

  arquivo = SD.open(NOME_ARQUIVO_CONFIGURACAO, FILE_READ);
  if(!arquivo){
    return ERRO_LEITURA_CARTAO;
  }
  texto = arquivo.readString();
  arquivo.close(); 

  // Busca informação do login
  erro = gerenciamentoCartao_buscaInformacao(texto,login,&buffer);
  if(erro != SUCESSO){
    return erro;
  }
  

  buffer.toCharArray(configuracaoWifi->login, TAMANHO_MAXIMO_LOGIN);
  
  // Busca informação da senha
  erro = gerenciamentoCartao_buscaInformacao(texto,senha,&buffer);
  if(erro != SUCESSO){
    return erro;
  }
  buffer.toCharArray(configuracaoWifi->senha, TAMANHO_MAXIMO_SENHA);

  return erro;
}
/**
 * @brief  Função que obtem a URL do servidor recuperadoa do arquivo de configuração
 * @param  servidor: Estrutura que armazenará a informação
 * @return erro ou SUCESSO
 */
Terro gerenciamentoCartao_obtemURLServidor(PTservidor servidor){
  Terro erro = SUCESSO;
  File arquivo;
  String texto;
  const char url_registro[] = {"URL Registros:"};
  const char url_taxa[]     = {"URL Taxa:"};
  const char url_filtros[]  = {"URL Filtros:"};
  String buffer;
  char userAndPassword[TAMANHO_SENHA_E_SERIAL];

    // Cria string de user and password
  (void)sprintf(userAndPassword, "&%s&%s", SERIAL_SNIFFER_CAN,SENHA_SNIFFER_CAN);

  arquivo = SD.open(NOME_ARQUIVO_CONFIGURACAO, FILE_READ);
  if(!arquivo){
    return ERRO_LEITURA_CARTAO;
  }
  texto = arquivo.readString();
  arquivo.close(); 

  // Busca informação do login
  erro = gerenciamentoCartao_buscaInformacao(texto,url_registro,&buffer);
  if(erro != SUCESSO){
    return erro;
  } 
  buffer.toCharArray(servidor->reg, TAMANHO_MAXIMO_URL);

  (void)strcat(servidor->reg,userAndPassword);

  // Busca informação do login
  erro = gerenciamentoCartao_buscaInformacao(texto,url_taxa,&buffer);
  if(erro != SUCESSO){
    return erro;
  } 
  buffer.toCharArray(servidor->taxa, TAMANHO_MAXIMO_URL); 
  
  (void)strcat(servidor->taxa,userAndPassword);   

  // Busca informação do login
  erro = gerenciamentoCartao_buscaInformacao(texto,url_filtros,&buffer);
  if(erro != SUCESSO){
    return erro;
  } 
  buffer.toCharArray(servidor->filtro, TAMANHO_MAXIMO_URL);

  
  (void)strcat(servidor->filtro,userAndPassword);     

  return erro;
}


/**
 * @brief  Função que valida se a informação inserida no arquivo de configuração 
 *         é de fato um identificador válido
 * @param  id: identificador
 * @return VERDADEIRO OU FALSO
 */
static Tbool ehIdentificador(char *id){
  if((*id >= '0') && (*id <= '9') ){
    return VERDADEIRO;
  }
  if((*id >= 'A') && (*id <= 'F')){
    return VERDADEIRO;
  }
  if((*id == 'x') || (*id == 'X')){
    if(*id == 'x')*id -= 0x20; 
    return VERDADEIRO;
  }
  if((*id >= 'a') && (*id <= 'f')){
    *id -= 0x20;  
    return VERDADEIRO;
  }

  return FALSO;
}

/**
 * @brief  Função que verifica se o filtro é de mascará ou padrão 
 * @param  filtroString: string com filtro
 * @return VERDADEIRO OU FALSO
 */
static Tbool filtroMascarado(char *filtroString){
  Tuint8 i;

  for(i=0; i<strlen(filtroString); i++){
    // Se for x entao é mascarado
    if(filtroString[i] == 'X'){      
      return VERDADEIRO;
    }
  }

  return FALSO;  
}
/**
 * @brief  Função que converte uma string em um filtro 
 * @param  filtroString: string com filtro
 * @return filtro convertido em hexa
 */
static Tuint32 converteFiltroASCIItoHex(char *filtroString){
  Tuint8 i;
  Tuint32 filtro = 0;
  Tuint8 tamanho = strlen(filtroString);
  char buffer[TAMANHO_MAX_STRING_FILTRO];

  // Copio informação para buffer local para não alterar os dados.
  (void)strcpy(buffer,filtroString);

  for(i=0; i<tamanho; i++){
    // Se for x entao é mascarado
    if(buffer[i] == 'X'){
      buffer[i] = '0';
    }
    filtro |=  (Tuint32)(((Tuint32)(ASCII_TO_HEXA(buffer[i])) << ((4*(tamanho-1)) - (4 * i))));
  }
  return filtro;
  
}

Terro gerenciamentoCartao_formataListaFiltrosAndMascaras(PTlistaFiltrosAndMascaras lista, 
                                                         String listaPossiveisFiltros){
  Terro erro = SUCESSO;
  Tuint32 pos;
  Tuint32 i;
  char filtro[TAMANHO_MAX_STRING_FILTRO];
  Tuint8 quantFiltrosAndMask = 0;
  char buffer[QUANTIDADE_FILTROS_E_MASCARAS][TAMANHO_MAX_STRING_FILTRO];  
  Tuint8 quantidadeFiltrosPadrao = 0 ;
  Tuint8 quantidadeFiltrosExtendidos = 0;

 // Tratamento para situação de nenhum identificador configurado
  if(listaPossiveisFiltros.compareTo("XXX") == 0){
    lista->quantidade = 0;
    return SUCESSO; 
  }
  if(listaPossiveisFiltros.compareTo("") == 0){
    return ERRO_ARQUIVO_CONFIGURACAO_CORROMPIDO;
  }

  // Se nao houver o ponto na ultima posição entao significa que o arquivo esta corrompido
  /*
  if(listaPossiveisFiltros[listaPossiveisFiltros.length() - 1] != '.'){
    return ERRO_ARQUIVO_CONFIGURACAO_CORROMPIDO;
  }*/
  
  i = 0;
  pos = 0;
  while(listaPossiveisFiltros[i] != '\0'){
     
    if(listaPossiveisFiltros[i] != ';'){

      // Valida se realmente é um identificador  
      if(ehIdentificador(&listaPossiveisFiltros[i])){

        filtro[pos] = (char)listaPossiveisFiltros[i];    
        pos++;
        i++;
        
        // Trata situações: filtro de 29 bits so tem 8 caracteres
        // se for um filro de 29 bits entao o primeiro caracter devera ser obrigatoriamente menor que 2
        // se for um filro de 11 bits entao o primeiro caracter devera ser obrigatoriamente menor que 7
        if( (pos >= 8)                                               || 
            ((pos == 8) && ((filtro[0] > '1') || (filtro[0] < '0'))) ||
            ((pos == 3) && ((filtro[0] > '7') || (filtro[0] < '0')))
          ){
          return ERRO_ARQUIVO_CONFIGURACAO_CORROMPIDO;
        }

      }else{
        return ERRO_ARQUIVO_CONFIGURACAO_CORROMPIDO;      
      } 

      // Se o proximo byte é '\0' entao tratar e sair      
      if(listaPossiveisFiltros[i] == '\0'){
        
        // Finaliza filtro string
        filtro[pos] = '\0';
        // Verifica se algum filtro recebido é do tipo aberto, se sim apenas retornar
        if(strcmp(filtro,"XXX") == 0){
          lista->quantidade = 0;
          return SUCESSO; 
        }       

        // Copia para array de filtros
        (void)strcpy(buffer[quantFiltrosAndMask],filtro);
        quantFiltrosAndMask ++;
      }

      
    }
    // Se for terminador ou separador, finaliza string
    else {
       // Finaliza filtro string
       filtro[pos] = '\0';

      // Verifica se algum filtro recebido é do tipo aberto, se sim apenas retornar
       if(strcmp(filtro,"XXX") == 0){
        lista->quantidade = 0;
        return SUCESSO; 
       }       

       // Copia para array de filtros
       (void)strcpy(buffer[quantFiltrosAndMask],filtro);

       quantFiltrosAndMask++;
       
      
      // Verifica se ultrapassou quantidade maximas de identificadores 
      if(quantFiltrosAndMask == QUANTIDADE_FILTROS_E_MASCARAS){
         // Sair do laço
         break;
      }else {
        pos = 0;
        i++;
      }
    }
  }
  
  // Verifica se todos os filtros são extendido
  for(i=0; i<quantFiltrosAndMask; i++){
    if(strlen(buffer[i]) == (TAMANHO_MAX_STRING_FILTRO-1)){
      quantidadeFiltrosExtendidos ++;
    }
  }  
  // Verifica se todos os filtros são extendido
  for(i=0; i<quantFiltrosAndMask; i++){
    if(strlen(buffer[i]) == (TAMANHO_MAX_STRING_FILTRO-1)){
      quantidadeFiltrosPadrao ++;
    }
  }  

  // DEVERÁ TER SOMENTE filtros extendidos ou somente filtros padrao
  if((quantidadeFiltrosExtendidos != quantFiltrosAndMask) && (quantidadeFiltrosExtendidos != 0)){
    return ERRO_ARQUIVO_CONFIGURACAO_CORROMPIDO;
  }
  // DEVERÁ TER SOMENTE filtros extendidos ou somente filtros padrao
  if((quantidadeFiltrosPadrao != quantFiltrosAndMask) && (quantidadeFiltrosPadrao != 0)){
    return ERRO_ARQUIVO_CONFIGURACAO_CORROMPIDO;
  }  
  
  // Tratar filtros extendidos
  if(quantidadeFiltrosExtendidos == quantFiltrosAndMask){

    lista->tipo = eExtendido;

    // Encontra a mascara
    for(i=0; i<quantFiltrosAndMask; i++){ 

      if(filtroMascarado(buffer[i])){
          lista->mascara_1 = converteFiltroASCIItoHex(buffer[i]);
          lista->mask_1 = VERDADEIRO;     
          return SUCESSO; 
      }    
    }
    // Encontra os filtros
    for(i=0; i<quantFiltrosAndMask; i++){ 

      lista->filtros[lista->quantidade].valor = converteFiltroASCIItoHex(buffer[i]);      
      lista->quantidade ++;
      if(lista->quantidade == QUANTIDADE_FILTROS_MASCARA_1){
        return SUCESSO;
      }
    }
  }
  // Tratar filtros padrao
  else{

    lista->tipo = ePadrao;
    
    // Encontra a mascara
    for(i=0; i<quantFiltrosAndMask; i++){ 

      if(filtroMascarado(buffer[i])){
          lista->mascara_0 = converteFiltroASCIItoHex(buffer[i]);
          lista->mask_0 = VERDADEIRO;     
      }    
    }

    // Encontra os filtros
    for(i=0; i<quantFiltrosAndMask; i++){ 
      
      if(!filtroMascarado(buffer[i])){
        lista->filtros[lista->quantidade].valor = converteFiltroASCIItoHex(buffer[i]);        
        lista->quantidade ++;
        if(lista->quantidade == ((lista->mask_0) ? QUANTIDADE_FILTROS_MASCARA_1 : MAXIMA_QUANTIDADE_FILTROS)){
          return SUCESSO;
        }
      }
    }    

  } 

  return erro;  
}

/**
 * @brief  Função que obtem uma lista de filtros e mascaras
 * @param  lista: estrutura que irá armazenar os filtros e mascaras
 * @return erro ou SUCESSO
 */
Terro gerenciamentoCartao_obtemListaFiltrosAndMascaras(PTlistaFiltrosAndMascaras lista){
  Terro erro = SUCESSO;
  File arquivo;
  String texto;
  const char identificadores[] = {"Identificadores:"};
  String listaPossiveisFiltros;

  
  // Zera quantidade da lista
  lista->quantidade = 0;
  
  arquivo = SD.open(NOME_ARQUIVO_CONFIGURACAO, FILE_READ);
  if(!arquivo){
    return ERRO_LEITURA_CARTAO;
  }
  texto = arquivo.readString();
  arquivo.close(); 

  // Busca informação dos identificadores
  // exemplo: 7xX.
  erro = gerenciamentoCartao_buscaInformacao(texto,identificadores,&listaPossiveisFiltros);
  if(erro != SUCESSO){
    return erro;
  }

  // Formata a lista de identificadores de acordo com texto recebido
  erro = gerenciamentoCartao_formataListaFiltrosAndMascaras(lista,listaPossiveisFiltros);
  if(erro != SUCESSO){
    return erro;
  }
  
  return erro;
}

/**
 * @brief  Função que obtem uma lista de filtros e mascaras
 * @param  lista: estrutura que irá armazenar os filtros e mascaras
 * @return erro ou SUCESSO
 */
Terro gerenciamentoCartao_escreveListaFiltrosAndMascaras(String listaFiltrosAndMascaras){
  Terro erro = SUCESSO;
  File arquivo;
  String texto;
  const char identificadores[] = {"Identificadores:"};

  arquivo = SD.open(NOME_ARQUIVO_CONFIGURACAO, FILE_READ);
  if(!arquivo){
    return ERRO_LEITURA_CARTAO;
  }
  texto = arquivo.readString();
  arquivo.close(); 

  // Escreve informação de filtro e mascara
  erro = gerenciamentoCartao_escreveInformacao(texto,identificadores,listaFiltrosAndMascaras);
  if(erro != SUCESSO){
    return erro;
  }
  return erro;
}

Terro gerenciamentoCartao_verificaTaxa(PTaxaComunicacao taxa, String taxaTexto){
  Tuint16 i;

  for(i=0; i<QUANTIDADE_TAXAS_CONHECIDAS; i++){
    if( taxaTexto.toInt() == tabela_taxas_conhecidas[i].taxa){
      *taxa = tabela_taxas_conhecidas[i].taxa;
      break;
    }
  }
  // Significa que nao encontrou nenhuma taxa conhecida
  if(i == QUANTIDADE_TAXAS_CONHECIDAS){
    return ERRO_TAXA_DESCONHECIDA;
  }

  return SUCESSO;
}

/**
 * @brief  Função que formata a taxa de comunicação
 * @param  taxa: estrutura que irá armazenar a taxa de comunicação
 * @param  textoTaxa: texto com a taxa de comunicação
 * @return erro ou SUCESSO
 */
Terro gerenciamentoCartao_formataTaxa(PTaxaComunicacao taxa, String textoTaxa){
  Terro erro = SUCESSO;
  Tuint16 i;

  // Se for enviado texto erroneamente
  if(textoTaxa.compareTo("") == 0){
    return ERRO_ARQUIVO_CONFIGURACAO_CORROMPIDO;
  }


  for(i=0; i<QUANTIDADE_TAXAS_CONHECIDAS; i++){
    if(textoTaxa.compareTo(tabela_taxas_conhecidas[i].descricao) == 0){
      *taxa = tabela_taxas_conhecidas[i].taxa;
      break;
    }
  }
  // Significa que nao encontrou nenhuma taxa conhecida
  if(i == QUANTIDADE_TAXAS_CONHECIDAS){
    return ERRO_TAXA_DESCONHECIDA;
  }

  return erro;

}
/**
 * @brief  Função que obtem a taxa de comunicação apartir do arquivo de configuração
 * @param  taxa: estrutura que irá armazenar a taxa de comunicação
 * @return erro ou SUCESSO
 */
Terro gerenciamentoCartao_obtemTaxaComunicacao(PTaxaComunicacao taxa){
  Terro erro = SUCESSO;
  File arquivo;
  String texto;
  const char taxaComunicacao[] = {"Taxa:"};
  String buffer;
  
  arquivo = SD.open(NOME_ARQUIVO_CONFIGURACAO, FILE_READ);
  if(!arquivo){
    return ERRO_LEITURA_CARTAO;
  }
  texto = arquivo.readString();
  arquivo.close(); 

  // Busca informação do login
  erro = gerenciamentoCartao_buscaInformacao(texto,taxaComunicacao,&buffer);
  if(erro != SUCESSO){
    return erro;
  }   
  // Formata texto da taxa, se nao estiver na lista de taxas conhecidas entao atribuii taxa padrao
  erro = gerenciamentoCartao_formataTaxa(taxa, buffer);
  if(erro != SUCESSO){
    *taxa = TAXA_500KBPS;
  }

  return erro;
}
/**
 * @brief  Função que obtem a taxa de comunicação apartir do arquivo de configuração
 * @param  taxa: estrutura que irá armazenar a taxa de comunicação
 * @return erro ou SUCESSO
 */
Terro gerenciamentoCartao_escreveTaxaComunicacao(String taxa){
  Terro erro = SUCESSO;
  File arquivo;
  String texto;
  const char taxaComunicacao[] = {"Taxa:"};
  String buffer;
  Tuint8 i;
  
  arquivo = SD.open(NOME_ARQUIVO_CONFIGURACAO, FILE_READ);
  if(!arquivo){
    return ERRO_LEITURA_CARTAO;
  }
  texto = arquivo.readString();
  arquivo.close();   

  for(i=0; i<QUANTIDADE_TAXAS_CONHECIDAS; i++){
    if(taxa.toInt() == tabela_taxas_conhecidas[i].taxa){
      buffer = tabela_taxas_conhecidas[i].descricao;
      break;
    }
  }
  // Se chegou até o final e não encontrou, atribui taxa padrao
  if(i==QUANTIDADE_TAXAS_CONHECIDAS){
    buffer = tabela_taxas_conhecidas[POS_TAXA_DEFAULT_500_KBPS].descricao;
  }

  // Busca informação do login
  erro = gerenciamentoCartao_escreveInformacao(texto,taxaComunicacao,buffer);
  if(erro != SUCESSO){
    return erro;
  }   
  return erro;
}

/**
 * @brief  Função que obtem a informação se usuário deseja um log sem formatação 
 *         ou com formatação.
 * @param  formatado: variável que receberá se sera formatado ou nao
 * @return erro ou SUCESSO
 */
Terro gerenciamentoCartao_obtemDesejaFormatarLog(Tbool *formatado){
  Terro erro = SUCESSO;
  File arquivo;
  String texto;
  const char logFormatado[] = {"Log Formatado:"};
  String buffer;
  
  // Abre arquivo para leitura
  arquivo = SD.open(NOME_ARQUIVO_CONFIGURACAO, FILE_READ);
  if(!arquivo){
    return ERRO_LEITURA_CARTAO;
  }
  // Le arquivo inteiro e armazena em texto
  texto = arquivo.readString();
  // Fecha arquivo
  arquivo.close(); 

  // Busca informação do login
  erro = gerenciamentoCartao_buscaInformacao(texto,logFormatado,&buffer);
  if(erro != SUCESSO){
    return erro;
  }   
  
  // Somente podera ter 3 letras a resposta
  if(buffer.length() < 3){
    return ERRO_ARQUIVO_CONFIGURACAO_CORROMPIDO;
  }

  if( ((buffer[0] == 's') || (buffer[0] == 'S')) && 
      ((buffer[1] == 'i') || (buffer[1] == 'I')) && 
      ((buffer[2] == 'm') || (buffer[1] == 'M'))
    ){
      *formatado = VERDADEIRO;
    }else{
      *formatado = FALSO;
    }

  return erro;

}

/**
 * @brief  Função que obtem a informação se usuário deseja um log sem formatação 
 *         ou com formatação.
 * @param  formatado: variável que receberá se sera formatado ou nao
 * @return erro ou SUCESSO
 */
Terro gerenciamentoCartao_obtemDesejaMonitorSerial(Tbool *monitorSerial){
  Terro erro = SUCESSO;
  File arquivo;
  String texto;
  const char strMonitorSerial[] = {"Monitor Serial:"};
  String buffer;
  
  // Abre arquivo para leitura
  arquivo = SD.open(NOME_ARQUIVO_CONFIGURACAO, FILE_READ);
  if(!arquivo){
    return ERRO_LEITURA_CARTAO;
  }
  // Le arquivo inteiro e armazena em texto
  texto = arquivo.readString();
  // Fecha arquivo
  arquivo.close(); 

  // Busca informação do login
  erro = gerenciamentoCartao_buscaInformacao(texto,strMonitorSerial,&buffer);
  if(erro != SUCESSO){
    return erro;
  }   
  
  // Somente podera ter 3 letras a resposta
  if(buffer.length() < 3){
    return ERRO_ARQUIVO_CONFIGURACAO_CORROMPIDO;
  }

  if( ((buffer[0] == 's') || (buffer[0] == 'S')) && 
      ((buffer[1] == 'i') || (buffer[1] == 'I')) && 
      ((buffer[2] == 'm') || (buffer[1] == 'M'))
    ){
      *monitorSerial = VERDADEIRO;
    }else{
      *monitorSerial = FALSO;
    }

  return erro;

}
/**
 * @brief  Função que obtem as informações de configuração no cartao de memória
 * @param  configuracao: Estrutura com os definidores das configurações
 * @return ERRO ou SUCESSO
 */
Terro gerenciamentoCartao_obtemConfiguracao(PTconfiguracao configuracao){
  Terro erro = SUCESSO;

  // Obtem login e senha do wifi
  erro = gerenciamentoCartao_obtemLoginSenha(&(configuracao->wifi));
  if(erro != SUCESSO){
    return erro;
  }
  PRINTF("LOGIN: %s\r\n", configuracao->wifi.login);
  PRINTF("SENHA: %s\r\n", configuracao->wifi.senha);

  // Obtem a taxa de comunicação 
  erro = gerenciamentoCartao_obtemTaxaComunicacao(&(configuracao->taxa));
  if(erro != SUCESSO){
    return erro;
  }  
  PRINTF("TAXA: %d\r\n", configuracao->taxa);

  // Obtem a lista de identificadores para filtro 
  erro = gerenciamentoCartao_obtemListaFiltrosAndMascaras(&(configuracao->filtAndMask));
  if(erro != SUCESSO){
    return erro;
  }  
  if(configuracao->filtAndMask.mask_0){
    PRINTF("MASCARA 0: 0x%08X\r\n",configuracao->filtAndMask.mascara_0);
  }
  if(configuracao->filtAndMask.mask_1){
    PRINTF("MASCARA 0: 0x%08X\r\n",configuracao->filtAndMask.mascara_1);
  }  
  
  for(int i=0; i<configuracao->filtAndMask.quantidade; i++){
    PRINTF("IDENTIFICADORES %02d: 0x%08X\r\n", (i+1),configuracao->filtAndMask.filtros[i].valor);
  }
  // Obtem a lista de identificadores para filtro 
  erro = gerenciamentoCartao_obtemDesejaFormatarLog(&(configuracao->logFormatado));
  if(erro != SUCESSO){
    return erro;
  }     

  PRINTF("Log formatado? %d\r\n", configuracao->logFormatado);

  // Obtem a lista de identificadores para filtro 
  erro = gerenciamentoCartao_obtemDesejaMonitorSerial(&(configuracao->monitorSerial));
  if(erro != SUCESSO){
    return erro;
  }     

  PRINTF("Monitor Serial? %d\r\n", configuracao->monitorSerial);
  

  // Obtem id do ultimo arquivo armazenado no cartao de memória
  erro = gerenciamentoCartao_obtemUltimoIdArquivoRegistro(&(configuracao->idArquivo));
  if(erro != SUCESSO){
    return erro;
  }  

  PRINTF("Ultimo arquivo: %d\r\n", configuracao->idArquivo);

  erro = gerenciamentoCartao_obtemURLServidor(&(configuracao->servidor));
  if(erro != SUCESSO){
    return erro;
  }  

  PRINTF("URL Registros: %s\r\n", configuracao->servidor.reg);

  PRINTF("URL Filtros: %s\r\n", configuracao->servidor.filtro);

  PRINTF("URL Taxa: %s\r\n", configuracao->servidor.taxa);


  return erro;
}

