# Banco de Dados Com Sensor Biométrico
Segunda etapa do trabalho prático em grupo com arduino da disciplina Introdução aos Algoritmos, no curso de Ciência da Computação na UFLA - Universidade Federal de Lavras.
Grupo: Caio Rocha, Gustavo Ribeiro, João Paulo de Paiva Lima e Pedro Paulo

*Clique [aqui]() para ver a Primeira Etapa*

O trabalho exigia o uso de Ponteiros, de ordernação e utilizar arquivos.

Utilizando um módulo de Cartão SD, Um módulo de Sensor Biométrico e o monitor Serial o arduino será capaz de:
* Adicionar pessoas com o comando "add_pessoa"

* Remover pessoas com o comando "remover_pessoa"

* Listas pessoas:
 * Em ordem de ID com o comando "listar_pessoas"
 * Em ordem "pseudo-alfabética" (Apenas a primeira letra sendo ordenada, preferimos assim por conta da baixa capacidade de processamento do arduino) com o comando "listar_pessoas --alf"
 * Do mais novo para o mais velho com o comando "listar_pessoas --idade"
 * Em ordem do número de matrícula com o comando "listar_pessoas --matricula"

* Mostrar dados de uma pessoa, inserindo o dedo no sensor biométrico ou com o comando "mostrar_dados"
