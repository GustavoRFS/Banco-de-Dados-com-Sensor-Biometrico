#Resumo das funções dos módulos utilizados

_Esse arquivo foi feito para facilitar os membros do grupo e para manter sempre uma rápida referência para utilizar_

####FILE CLASS:FILE CLASS:
arquivo.peek() : usado para verificar o proximo byte, sem pular algum byte.
arquivo.seek(i) : seleciona a posição i;
arquivo.available(): Retorna se há bytes disponiveis para ler;

####SD CLASS:
SD.rmdir(dir): Apaga dir;
SD.remove(arquivo): Apaga arquivo;
SD.mkdir(dir): Cria dir;

####FPM10A (Sensor Biométrico):
sensor.verifyPassword() : verifica se o sensor foi encontrado;

sensor.getTemplateCount() : calcula a qntd de dedos salvos;
sensor.templateCount : (TA CERTO), mostra a qntd de dedos salvos;

sensor.getImage() : tira a foto do dedo;
sensor.image2Tz() : converte a imagem do dedo;
sensor.fingerFastSearch() : busca pelo dedo;
sensor.fingerID : Retorna o ID encontrado pela fastsearch;
sensor.confidence: Confiança do dedo encontrado;

sensor.deleteModel(ID) : deleta o ID;

sensor.loadModel(ID): Carrega o modelo

sensor.emptyDatabase() : Limpa a memoria do sensor;

*arquivo.peek() : usado para verificar o proximo byte, sem pular algum byte.
*arquivo.seek(i) : seleciona a posição i;
*arquivo.available(): Retorna se há bytes disponiveis para ler;

##SD CLASS:
*SD.rmdir(dir): Apaga o diretório;
*SD.remove(arquivo): Apaga arquivo;
*SD.mkdir(dir): Cria dir;

##FPM10A (Sensor Biométrico):
*sensor.verifyPassword() : verifica se o sensor foi encontrado;

*sensor.getTemplateCount() : calcula a qntd de dedos salvos;
*sensor.templateCount : (TA CERTO), mostra a qntd de dedos salvos;

*sensor.getImage() : tira a foto do dedo;
*sensor.image2Tz() : converte a imagem do dedo;
*sensor.fingerFastSearch() : busca pelo dedo;
*sensor.fingerID : Retorna o ID encontrado pela fastsearch;
*sensor.confidence: Confiança do dedo encontrado;

*sensor.deleteModel(ID) : deleta o ID;

*sensor.loadModel(ID): Carrega o modelo

*sensor.emptyDatabase() : Limpa a memoria do sensor;
