/*
 * Trabalho de Introdução aos Algoritmos
 * Banco de dados com Sensor Biométrico
 * Copyright 2019 by Gustavo Ribeiro da Fonseca Santos, Caio Cesar da Rocha, Pedro Paulo Araujo Carvalho e João Paulo de Paiva Lima
 * 
 * Os arquivos que serão utilizados nesse código serão apenas um por cada pessoa cadastrada.
 * Cada pessoa cadastrada tem um ID e o nome do arquivo com os dados dessa pessoa será ID.dat
 *
 */
  
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Fingerprint.h>

SoftwareSerial serialSensor(2,3); //Configura a comunicação serial com o Sensor Biométrico
Adafruit_Fingerprint sensor = Adafruit_Fingerprint(&serialSensor); //Configura o Sensor Biométrico

//Função que verifica as conexões dos dispositivos:
void verificarDispositivos(){                        
  Serial.println(F("Verificando dispositivos")); //Mostra no monitor Serial que está sendo executada a verificação

  //sensor.verifyPassword verifica se o leitor biométrico está conectado corretamente
  //SD.begin verifica se o Cartão SD conseguiu ser lido
  if (!sensor.verifyPassword() and !SD.begin(10)){ 
    Serial.println(F("Erro no SD e no Sensor Biometrico"));
    while (!sensor.verifyPassword() and !SD.begin(10));
  }
  else if (!sensor.verifyPassword()){
    Serial.println(F("Erro no Sensor Biometrico"));
    while (!sensor.verifyPassword());
  }
  else if(!SD.begin(10)){
    Serial.println(F("Erro ao ler o SD"));
    while(!SD.begin(10));
  }
  delay(500);
}

//Função que transforma os caracteres minusculos em maiusculos:
void charUpperCase(char &letra){
  if (letra>='a' and letra<='z'){
    letra=letra-32;
  }
}

//FUNÇÕES PARA LEITURA DO MONITOR SERIAL:

//Função que lê o monitor serial até que seja apertado enter (quebra de linha == '\n'):
void lerSerialChar(char vet[],int tam){
  Serial.setTimeout(2000);
  char caractere_atual;
  byte i=0;
  while (!Serial.available());    //Enquanto nada é digitado, o programa não fica em um loop vazio
  while (Serial.available() and i<tam){  //Quando algo é digitado, sera lido caractere por caractere até alcançar 50 caracteres
    caractere_atual=Serial.read();
    if (caractere_atual=='\n' or i==tam-1){   //Se o caractere for uma quebra de linha, a função será encerrada
     
      while (Serial.available()){ //Trecho que ignora os bytes restantes no monitor serial, jogando os fora
        Serial.read();
      }       
      
      return;  //Finaliza a função
    }
    vet[i]=caractere_atual;       //Se o caractere não for quebra de linha, o programa continua e segue adicionando os caracteres para a string
    i++;
    delay(10);
  }
}

//Função identica a anterior, para retornar uma string:
String lerSerial(){
  Serial.setTimeout(2000); //Define um intervalo máximo para ler um byte no monitor serial
  char caractere_atual;
  String resposta="";
  byte i=0;
  while (!Serial.available()); //Enquanto nada é digitado, o programa não fica em um loop vazio
  while (Serial.available()){
    caractere_atual=Serial.read();
    if (caractere_atual=='\n'){  //Se o caractere for uma quebra de linha, a string será retornada
      while (Serial.available()){ //Trecho que ignora os bytes restantes no monitor serial, jogando os fora
        Serial.read();
      }
      
      return resposta;
    }
    resposta+=caractere_atual;  //Se o caractere não for quebra de linha, o programa continua e segue adicionando os caracteres para a string
    i++;
    delay(10);
  }
}

//Função semelhante as anteriores para retornar um valor inteiro
int lerSerialInt(){
  Serial.setTimeout(2000); //Define um intervalo máximo para ler um byte no monitor serial
  int retorno;
  while (!Serial.available()); //Enquanto nada é digitado, o programa não fica em um loop vazio
  retorno=Serial.parseInt();
  while (Serial.available()){ //Trecho que ignora os bytes restantes no monitor serial, jogando os fora
    Serial.read();
  }
  return retorno;
}


//FUNÇÕES DE ORDENAÇÃO:
//Selection sort que ordenará tanto os valores de idade e os numeros de matricula
void selectionSort(unsigned long int vet[],byte ordem[],byte tam){
  unsigned long int menor;    
  for (byte i=0;i<tam-1;i++){  //"For" que pega um numero, para que esse numero seja comparado com os proximos
    menor=i;  //Menor recebe o indice do menor numero
    for (byte j=i+1;j<tam;j++){  //"For" que pega os numeros que serão comparados com a variavel menor
      if (vet[j]<vet[menor]){  
        menor=j;  //Se o numero no indice j for menor que a o numero no indice da variavel menor, o indice j vira menor
      }
    }
    unsigned long int aux=vet[i]; 
    vet[i]=vet[menor];           //Faz a troca no vetor dos dados obtidos nos arquivos
    vet[menor]=aux;
    aux=ordem[i];
    ordem[i]=ordem[menor];       //Faz a troca no vetor com os indices de ordem
    ordem[menor]=aux;
  }
}

//Selection sort que fará a ordenação PSEUDO-ALFABETICA, que ordena apenas a primeira letra do nome
void ordemAlfabetica(char vet[],byte ordem[],byte &tam){
  for (byte i=0;i<tam-1;i++){  //Tem estrutura identica ao selectionSort anterior, porem adaptado para funcionar com vetor de char
    byte menor=i;
    for (byte j=i+1;j<tam;j++){
      if (vet[j]<vet[menor]){
        menor=j;
      }
    }
    char auxChar=vet[i];
    vet[i]=vet[menor];  //Faz a troca dos dados do vetor de char
    vet[menor]=auxChar;
    
    byte aux=ordem[i];
    ordem[i]=ordem[menor];  //Faz a troca no vetor com os indices de ordem
    ordem[menor]=aux;
  }
}


//FUNÇÕES DE ESCRITA E LEITURA DE DADOS:

//Função que adiciona os dados de uma pessoa:
void addPessoa(){
  /* 
   * O Reconhecimento biométrico funciona da seguinte forma:
   * Ele tira a imagem do dedo, depois converte para um formato que o sensor consiga trabalhar
   * Depois pede para que o dedo seja inserido novamente para confirmação e converte novamente
   * Feito isso, será criado o modelo da imagem do dedo e depois, caso nenhum erro aconteça, 
   * serão requisitados os dados da pessoa, o modelo do dedo é salvo na memoria do sensor
   * e depois os dados serão salvos no SD 
   */
  int processoAtual;  //Variavel que vai receber os retornos das funções do sensor biométrico
  
  sensor.getTemplateCount();  //Pede para o sensor contar a quantidade de IDS cadastrados
  byte id = sensor.templateCount+1;  //Recebe a quantidade de IDS cadastrados e soma um, que será o ID onde o próximo dedo será cadastrado
  
  //Mostra as seguintes mensagens no monitor serial
  Serial.println(F("Insira o dedo no sensor biometrico"));
  Serial.println(F("Aguardando dedo"));

  //Começa o processo de reconhecimento do dedo, criação de modelo, recebimento de dados da pessoa, salva os dados no SD e o modelo na memoria do sensor
  for (byte i=1;i<=2;i++){
    processoAtual=sensor.getImage();
    while (processoAtual == FINGERPRINT_NOFINGER){  //Se a imagem tirada não detectar nenhum dedo, o programa aguarda ser inserido um dedo
      processoAtual=sensor.getImage();  //Pega a imagem no sensor biométrico
    }
    
    if (processoAtual==FINGERPRINT_OK){  //Se a imagem encontrar um dedo, o processo segue
      Serial.println(F("Imagem do dedo tirada!"));
    }
    
    else {  //Se a imagem der algum erro, o subprograma é encerrado
      Serial.println(F("Erro, tente novamente!"));
      return;  //Encerra o programa
    }
    
    processoAtual=sensor.image2Tz(i); //Processo de conversão 
    //Note que a função tem como parâmetro i, que mostra se é a primeira ou a segunda vez
    //Que está ocorrendo a conversão
    
    if (processoAtual == FINGERPRINT_OK){  //O processo ocorreu com sucesso
      Serial.println(F("Imagem convertida com sucesso!"));
    }
    else if (processoAtual==FINGERPRINT_IMAGEMESS){  //O processo falhou e será finalizado
      Serial.println(F("Imagem confusa, Tente novamente!"));
      return;  //Finaliza a função
    }
    else{  //O processo falhou e será finalizado
      Serial.println(F("Erro, tente novamente!"));
      return;  //Finaliza a função
    }
    
    if (i==1){ //Se for a primeira execução da estrutura "for", espera que o usuário remova o dedo e coloque o mesmo dedo novamente
      Serial.println(F("Remova o dedo"));
      while (sensor.getImage() != FINGERPRINT_NOFINGER); //Espera para que a pessoa remova o dedo
      
      Serial.println(F("Coloque o mesmo dedo novamente"));
    }
  }
  
  //Se o sensor conseguir reconhecer um dedo corretamente, o subprograma chega até aqui
  //Agora, será criado o modelo do dedo 
  
  processoAtual=sensor.createModel();
  
  if (processoAtual == FINGERPRINT_OK){  //Criação do modelo ocorreu com sucesso
    Serial.println(F("Modelo criado com sucesso"));
  }
  else{  //Criação do modelo falhou
    Serial.println(F("Erro ao criar o modelo!"));
    
    return; //A função será encerrada
  }  
  
  /* 
   * Agora será o processo de receber os dados
   * Os dados só serão salvos quando a função tiver a confirmação de que
   * o processo de salvar o modelo do dedo na memória do sensor biométrico
   * obtiver sucesso, pois caso os dados fossem salvos agora e depois, na hora
   * de salvar o modelo do dedo, fosse obtido um erro, teríamos um problema de
   * ter o arquivo de um ID no cartão SD, porém não teríamos esse ID
   * na memória do sensor biométrico.
   */
  
  Serial.println(F("Agora, insira os dados da pessoa"));

  String teste="";  //Variável que será utilizada posteriormente para testar os valores
  char nomeCompleto[45]={' '};
  String sexo;
  byte idade;
  String matricula;
  String telefone;
  
  Serial.println(F("Insira o Nome Completo da pessoa:"));
  lerSerialChar(nomeCompleto,45); //Recebe o nome completo
  while (nomeCompleto==' '){
    Serial.println(F("Nome inserido inválido"));
    lerSerialChar(nomeCompleto,45);
  }
  
  charUpperCase(nomeCompleto[0]);
  
  Serial.println(F("Insira o sexo da pessoa: "));
  teste=lerSerial(); //Recebe um teste de string, para ser verificada se é uma entrada valida
  teste.toUpperCase();
  while ((not teste=="MASCULINO") or (not teste=="FEMININO") or (not teste=="M") or (not teste=="F")){  //Se o texto digitado não se referir à um sexo 
    Serial.println(F("Sexo inserido invalido, insira novamente:"));  //Será retornado uma mensagem de erro e ficará em um loop até que seja inserido um valor valido
    teste=lerSerial();
    teste.toUpperCase();
  }
  
  //Para manter uma padronização na escrita no arquivo, foi feito o trecho a seguir
  if (teste=="MASCULINO" or teste=="M"){  
    sexo="Masculino";
  }
  else{
    sexo="Feminino";
  }
  
  
  Serial.println(F("Insira a idade da pessoa: "));
  byte testeInt=lerSerialInt();
  while (testeInt>150){
    Serial.println(F("Idade inserida invalida, insira novamente:"));
    testeInt=lerSerialInt();
  }
  idade=testeInt;
  
  
  Serial.println(F("Insira o numero de matricula: "));
  teste=lerSerial();
  while (teste.length()!=9){  //Matriculas na ufla têm 9 numeros, caso a matricula não tenha esse tamanho, será retornada uma mensagem de erro
    Serial.println(F("Numero de matricula invalida, insira novamente:"));
    teste=lerSerial();
  }
  matricula=teste;
  
  Serial.println(F("Insira o numero de telefone: "));
  telefone=lerSerial();
  
  processoAtual= sensor.storeModel(id);  //Salva o modelo do dedo na memória do sensor biométrico
  
  if (processoAtual == FINGERPRINT_OK){  //Se o dedo foi salvo com sucesso, a função prossegue
    Serial.println(F("Dados salvos com sucesso!"));
  }
  
  else if (processoAtual == FINGERPRINT_ENROLLMISMATCH){ //Se o dedo não foi salvo com sucesso, será mostrada uma mensagem de erro,
    Serial.println(F("Os dedos inseridos não combinam"));//Os dados não serão salvos no SD e será retornada uma mensagem de erro
    sensor.deleteModel(id); //Garante que se o processo alocou um espaço na memória do sensor biométrico, esse espaço será desalocado
    return;
  }
  else{
    Serial.println(F("Erro ao salvar o modelo na memória do Sensor biométrico"));
    sensor.deleteModel(id); //Garante que se o processo alocou um espaço na memória do sensor biométrico, esse espaço será desalocado
    return;
  }
  
  /*
   * Se o modelo for salvo com sucesso na memoria flash do sensor o subprograma
   * chega até aqui e salva os dados no cartão SD.
   * Isso foi feito dessa forma para evitar que caso o dedo não salve
   * Os dados do cartão SD não fiquem dessincronizados com o do sensor
   */
  
  File dadosPessoa=SD.open(String(id)+".dat",FILE_WRITE);  //Abre o arquivo
  dadosPessoa.println(nomeCompleto);  //Escreve as variaveis, uma em cada linha do arquivo
  dadosPessoa.println(sexo);
  dadosPessoa.println(idade);
  dadosPessoa.println(matricula);
  dadosPessoa.println(telefone);
  dadosPessoa.close();
  
  return; //Se nenhum erro ocorrer, o subprograma é encerrado
}

//Função que mostra todos os dados de uma pessoa de acordo com o ID, lendo essas variaveis direto do arquivo para o monitor serial, sem passar por variaveis
void mostrarDados(byte id){
  if (!SD.exists(String(id)+".dat")){ //Caso algum erro ocorra na leitura do arquivo de dados será retornada uma mensagem de erro e a função é encerrada
    Serial.println(F("Erro, não foi possível ler os dados dessa pessoa"));
    return;
  }
  //Se o arquivo foi encontrado, a função segue
  File arquivo=SD.open(String(id)+".dat");
  Serial.print(F("ID: "));
  Serial.println(id);
  
  Serial.print(F("Nome Completo: "));
  char caractereAtual=arquivo.read();  
  while (caractereAtual!='\n'){
    Serial.print(caractereAtual);  //Mostra no monitor serial os caracteres do nome da pessoa
    caractereAtual=arquivo.read();
  }
  
  Serial.println();
  
  Serial.print(F("Sexo: "));
  caractereAtual=arquivo.read();
  while (caractereAtual!='\n'){
    Serial.print(caractereAtual); //Mostra no monitor serial os caracteres do sexo da pessoa
    caractereAtual=arquivo.read();
  }
  
  Serial.println();
  
  Serial.print(F("Idade: "));
  caractereAtual=arquivo.read();  
  while (caractereAtual!='\n'){
    Serial.print(caractereAtual); //Mostra no monitor serial os caracteres da idade da pessoa
    caractereAtual=arquivo.read();
  }
  
  Serial.println();
  
  Serial.print(F("Matricula: "));
  caractereAtual=arquivo.read(); 
  while (caractereAtual!='\n'){
    Serial.print(caractereAtual); //Mostra no monitor serial os caracteres da matricula da pessoa
    caractereAtual=arquivo.read();
  }

  Serial.println();
  
  Serial.print(F("Telefone: "));
  caractereAtual=arquivo.read();
  while (caractereAtual!='\n'){
    Serial.print(caractereAtual); //Mostra no monitor serial os caracteres do telefone da pessoa
    caractereAtual=arquivo.read();
  }
  
  Serial.println();
  
  arquivo.close();
}

//Função que reconhece a pessoa pelo dedo inserido e, usado de outra função, mostra os dados da pessoa reconhecida
void reconhecer(){  //O processo sensor.getImage() foi feito na função loop. Se esse processo retornar FINGERPRINT_OK, essa função é iniciada
  Serial.println(F("Imagem Tirada!"));
  int processoAtual;
  processoAtual=sensor.image2Tz();

  if (processoAtual == FINGERPRINT_OK){ //Se a imagem for convertida, o processo continua
    Serial.println(F("Imagem convertida com sucesso!"));
  }
  else if (processoAtual==FINGERPRINT_IMAGEMESS){ //Senão, é mostrada alguma mensagem de erro e a função será encerrada
    Serial.println(F("Imagem confusa, Tente novamente!"));
    return;
  }
  else{
    Serial.println(F("Erro, tente novamente!"));
    return;
  }
  
  processoAtual=sensor.fingerFastSearch(); //Busca a imagem do dedo na memória do Sensor Biométrico
  
  if (processoAtual==FINGERPRINT_OK){ //Se o dedo for encontrado, o processo continua
    Serial.println(F("Dedo encontrado com sucesso!"));
  }
  else if (processoAtual==FINGERPRINT_NOTFOUND){ //Senão, será retornado uma mensagem de erro e a função será encerrada
    Serial.println(F("O dedo não foi encontrado!"));
    
    return;
  }
  else{
    Serial.println(F("Erro, tente novamente!"));
    
    return;
  }
  byte id=sensor.fingerID; //Recebe o ID encontrado na busca
  mostrarDados(id); //Mostra os dados desse ID
}


//Função que remove os dados de uma pessoa
void removerPessoa(byte id,byte qntdIDS){
  //Pede confirmação para o usuário   
  Serial.println(F("Os dados perdidos nao poderao ser recuperados!Deseja prosseguir?(S/N)"));
  String resposta="";
  resposta=lerSerial();
  resposta.toUpperCase();
  
  if (resposta=="S" or resposta=="SIM"){   //Se a resposta for sim, a função segue
    Serial.println(F("Limpando dados"));
  }
  else if (resposta=="N" or resposta=="NAO"){ //Se for não, a função encerra
    Serial.println(F("Operacao cancelada!"));
    return;
  }
  else{ //Qualquer outra resposta será dada uma mensagem de erro e a função encerra
    Serial.println(F("Comando desconhecido!"));
    Serial.println(F("Operacao cancelada!"));
    return;
  }
  
  int processoAtual=sensor.deleteModel(id);  //Deleta o dedo na memória do sensor
  
  if (processoAtual==FINGERPRINT_OK){  //Informa se foi o processo ocorreu com sucesso
    Serial.println(F("Modelo do dedo apagado com sucesso"));
  }
  else{ //Se der erro, uma mensagem será exibida e a função será encerrada
    Serial.println(F("Erro ao apagar dedo"));
    return;
  }
  
  if (SD.exists(String(id)+".dat")){
    SD.remove(String(id)+".dat");
  }
  
  //Percorre por todos os ids sucessores do id removido, a fim de evitar espaços vazios entre os dados
  for (byte i=id+1;i<=qntdIDS;i++){
    //Move os modelos sucessores do removido para um slot antecessor ao cadastrado
    //EX:IDS={1,2,3,4,5,6,7,8,9,10};
    //Deletando 5 a memoria ficaria assim: IDS={1,2,3,4,Vazio,6,7,8,9,10}
    //O que esse trecho faz é mover o 6 para o 5, o 7 para o 6 e assim por diante
    //E depois renomea os arquivos com os dados desses respectivos IDS
    sensor.loadModel(i);
    sensor.storeModel(i-1);
    sensor.deleteModel(i);
    
    
    //O trecho a seguir renomea os arquivos para sincronizar os dados do sensor biométrico com os salvos no Cartão SD
    File arqVelho=SD.open(String(i)+".dat",FILE_READ);  //Arquivo velho que terá seus dados copiados para outro
    File arqNovo=SD.open(String(i-1)+".dat",FILE_WRITE);  //Arquivo com o novo nome
  
    while (arqVelho.available()){
      arqNovo.write(arqVelho.read());  //Copia os dados do arquivo velho para o novo
    }
  
    arqVelho.close();  //Fecha os arquivos
    arqNovo.close();
  
    SD.remove(String(i)+".dat"); //Remove o arquivo antigo, que teve seus dados copiados para um novo
  }  
 
  Serial.println(F("Pronto!"));
}

//As 4 funções a seguir lêem os dados e criam os vetores para serem ordenados por outra função e mostrados na tela por uma função posterior
void listarPessoasAlf(){
  sensor.getTemplateCount();  //Calcula quantos dedos tem cadastrado no sensor biométrico
  
  byte tam=sensor.templateCount;  //Recebe a quantidade de dedos cadastrados
  
  char *primeiraLetra=new char[tam];  //Cria o vetor com as primeiras letras dos nomes através de ponteiro
  byte *ordem=new byte[tam];  //Cria o vetor que guardará os IDS que serão ordenados
  
  for (int i=0;i<tam;i++){
    File arquivo = SD.open(String(i+1)+".dat",FILE_READ); //Cria a variavel do arquivo
    primeiraLetra[i]=arquivo.read();
    arquivo.close();
    
    ordem[i]=i+1; //Enumera vetor de ordem, para que seja alterado posteriormente
  }
  
  ordemAlfabetica(primeiraLetra,ordem,tam);
  delete[] primeiraLetra; //Desaloca o ponteiro
  primeiraLetra=NULL; //Invalida o ponteiro
  listarPessoas(ordem,tam); //Mostra a lista das pessoas na ordem pseudo alfabetica  
  delete[] ordem;
  ordem=NULL;
}

void listarPessoasID(){
  sensor.getTemplateCount(); //Calcula a quantidade de dedos salvos no Sensor Biométrico
  
  byte tam=sensor.templateCount; //Recebe a quantidade de dedos salvos no sensor
  byte *ordem=new byte [tam];  //Aloca o ponteiro ordem
  for (int i=0;i<tam;i++){
    ordem[i]=i+1;  //Cria o vetor de ordem
  }
  
  listarPessoas(ordem,tam);
  delete[] ordem; //Desaloca o ponteiro ordem
  ordem=NULL; //Anula o ponteiro
}

void listarPessoasIdade(){
  sensor.getTemplateCount();  //Faz a contagem de quantos IDS tem na memória do sensor biometrico
  
  byte tam=sensor.templateCount; //Recebe a quantidade de IDS salvos
  
  byte *ordem=new byte [tam];  //Aloca dinamicamente o vetor de ordem dos ids
  unsigned long int *idade=new unsigned long int [tam];  //Aloca dinamicamente o vetor com as idades
  for (int i=0;i<tam;i++){
    File arquivo =SD.open(String(i+1)+".dat");
    for (int j=0;j<2;j++){ //Ignora duas variaveis salvas no arquivo, pulando para a variavel idade
      char caractereAtual=arquivo.read();
      while (caractereAtual!='\n'){  
        caractereAtual=arquivo.read();
      }
    }
    idade[i]=arquivo.parseInt();  //Lê a idade no arquivo
    arquivo.close();
    ordem[i]=i+1;  //Coloca os elementos no vetor de ordenação
  }
  selectionSort(idade,ordem,tam);  //Ordena os vetores
  delete[] idade; //Desaloca o ponteiro idade
  idade=NULL; //Anula o ponteiro
  listarPessoas(ordem,tam); //Lista no monitor serial, utilizando do vetor ordem
  delete[] ordem; //Desaloca o ponteiro ordem
  ordem=NULL;  //Desaloca o ponteiro
}

void listarPessoasMatricula(){
  sensor.getTemplateCount();  //Faz a contagem de quantos IDS tem na memória do sensor biometrico
  
  byte tam=sensor.templateCount; //Recebe a quantidade de IDS salvos
  
  byte *ordem=new byte [tam];  //Aloca dinamicamente o vetor de ordem dos ids
  unsigned long int *matricula=new unsigned long int [tam];  //Aloca dinamicamente o vetor com as idades
  for (int i=0;i<tam;i++){
    File arquivo =SD.open(String(i+1)+".dat");
    for (int j=0;j<3;j++){    //Ignora 3 variaveis do arquivo, pulando para o trecho onde está salvo o número de matrícula 
      char caractereAtual=arquivo.read();
      while (caractereAtual!='\n'){
        caractereAtual=arquivo.read();
      }
    }
    matricula[i]=arquivo.parseInt(); //Lê a matricula
    arquivo.close();
    ordem[i]=i+1; //Coloca os elementos no vetor de ordenação
  }
  selectionSort(matricula,ordem,tam);
  delete[] matricula;
  matricula=NULL;
  listarPessoas(ordem,tam);
  delete[] ordem;
  ordem=NULL;
}  

  
//Função que lista as pessoas no monitor serial, de acordo com alguma das ordenações feita anteriormente:
//Esta função lê o arquivo direto para o monitor serial, a fim de evitar grande consumo de memória
void listarPessoas(byte ordem[],byte tam){ 
  Serial.print(F("| ID|"));
  Serial.print(F("Nome completo                                |"));
  Serial.print(F("Idade|"));
  Serial.println(F("Matricula|"));
  
  //Estrutura de repetição que percorrerá por todos os arquivos
  //mostrando o ID,Nome,Idade e Matricula das pessoas salvas nesses arquivos
  for (byte i=0;i<tam;i++){ 
    byte id=ordem[i];  //ID vindo da lista ordenada anteriormente será mostrado na tela, além de ser o que da os nomes dos arquivos
    //O próximo trecho serve apenas para mostrar o ID de uma forma organizada no monitor serial, dando a quantidade de espaços necessária para ocupar 3 algarismos
    Serial.print(F("|"));
    if (id<10){
      Serial.print(F("  "));
    }
    else if (id<100 and id>=10){
      Serial.print(F(" "));
    }
    Serial.print(id);
    Serial.print(F("|"));
    
    if (SD.exists(String(id)+".dat")){ //Garante que o arquivo existe para poder ser lido
      File arquivo = SD.open(String(id)+".dat",FILE_READ);  //Abre o arquivo do ID da ordem que foi escolhida
    
      byte cont=0;
      //O próximo trecho lê o nome completo gravado no arquivo e o mostra no monitor serial
      char caractereAtual=arquivo.read();
      while (caractereAtual!='\n'){
        Serial.print(caractereAtual);
        caractereAtual=arquivo.read();
        cont++;
      }
      for (byte j=cont-1;j<45;j++){
        Serial.print(F(" "));
      }
      Serial.print(F("|"));
      delay(20);
      //O próximo trecho lê o sexo da pessoa gravado no arquivo, mas não o salva em nenhuma variável nem mostra no monitor serial
      caractereAtual=arquivo.read();
      while (caractereAtual!='\n'){ 
        caractereAtual=arquivo.read();
      }
    
      //O próximo trecho lê a idade e a mostra no monitor serial
      caractereAtual=arquivo.read();
      cont=0;
      while (caractereAtual!='\n'){
        Serial.print(caractereAtual);
        caractereAtual=arquivo.read();
        cont++;
      }
      for (byte j=cont-1;j<5;j++){
        Serial.print(F(" "));
      }
      Serial.print(F("|"));
      delay(20);
      //O próximo trecho lê o numero de matricula e o mostra no monitor serial
      caractereAtual=arquivo.read();
      cont=0;
      while (caractereAtual!='\n'){
        Serial.print(caractereAtual);
        caractereAtual=arquivo.read();
        cont++;
      }
      for (int j=cont-1;j<9;j++){
        Serial.print(F(" "));
      }
      Serial.println(F("|"));
      delay(20);
      arquivo.close();
    }
    else{
      Serial.println(F("Erro ao ler os dados desse ID                |     |         |"));
    }
  }//Fim da estrutura de repetição que percorre por todos os arquivos
  
  Serial.println();
  Serial.println(F("Pronto!"));
}

//FUNÇÕES PADRÃO DO ARDUINO:
void setup(){
  Serial.begin(9600);
  sensor.begin(57600);
  SD.begin(10);
  
  verificarDispositivos();


  //Mostra no monitor serial os possíveis comandos:
  Serial.println(F("Dispositivos prontos!"));
  Serial.println(F("Lista de comandos:"));
  Serial.println(F("add_pessoa = Cadastra uma pessoa"));
  Serial.println(F("remover_pessoa = Remove o cadastro de uma pessoa"));
  Serial.println(F("listar_pessoas = Mostra os nomes das pessoas cadastradas em ordem de ID"));
  Serial.println(F("listar_pessoas --alf = Mostra os nomes das pessoas cadastradas em uma ordem pseudo-alfabetica"));
  Serial.println(F("listar_pessoas --idade = Mostra os nomes das pessoas cadastradas das mais novas para as mais velhas"));
  Serial.println(F("listar_pessoas --matricula = Mostra os nomes das pessoas cadastras em ordem crescente de matricula"));
  Serial.println(F("mostrar_dados = Mostra os dados de uma pessoa especifica informando o nome completo"));
  Serial.println(F("Insira um comando listado!"));
}

void loop(){
  sensor.getTemplateCount(); //Calcula quantos dedos estão salvos na memória do sensor biométrico
  byte qntdIDS=sensor.templateCount; //Recebe a quantidade calculada
  
  if (sensor.getImage()==FINGERPRINT_OK){ //Se algum dedo for inserido
    reconhecer(); //Ele será buscado e, se encontrado, mostrará os dados da pessoa encontrada
  }
  
  if (Serial.available()){
    String resposta = lerSerial();
    if (resposta == "add_pessoa"){
      addPessoa();
    }
    else if (resposta == "remover_pessoa"){
      Serial.println(F("Insira o ID dessa pessoa")); 
      byte id=lerSerialInt();
      if (id>=1 and id<=qntdIDS){
        removerPessoa(id,qntdIDS);
      }
      else{
        Serial.println(F("Erro, pessoa nao cadastrada"));
      }
    }
    else if (resposta == "listar_pessoas"){
      if (qntdIDS>0){
        listarPessoasID();
      }
      else{
        Serial.println(F("Não há pessoas cadastradas para serem listadas"));
      }
    }  
    else if (resposta == "listar_pessoas --alf"){
      if (qntdIDS>0){
        listarPessoasAlf();
      }
      else{
        Serial.println(F("Não há pessoas cadastradas para serem listadas"));
      }
    }
    else if (resposta == "listar_pessoas --idade"){
      if (qntdIDS>0){
        listarPessoasIdade();
      }
      else{
        Serial.println(F("Não há pessoas cadastradas para serem listadas"));
      }
    }
    else if (resposta == "listar_pessoas --matricula"){
      if (qntdIDS>0){
        listarPessoasMatricula();
      }
      else{
        Serial.println(F("Não há pessoas cadastradas para serem listadas"));
      }      
    }  
    else if (resposta == "mostrar_dados"){
      Serial.println(F("Insira o ID da pessoa"));
      byte id=lerSerialInt();
      if (id>=1 and id<=qntdIDS){
        mostrarDados(id);
      }
      else{
        Serial.println(F("Erro, pessoa nao cadastrada"));
      }
    }
    else{
      Serial.println(F("Comando desconhecido!"));
    }
  }
}
