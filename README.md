# Trabalho 1 - LBP

Trabalho 1 da matéria de Programação 2, consiste em escrever um programa de processamento de imagens, especificamente de extensão PGM, que utiliza o algoritmo LBP (*Local Binary Patterns*) para identificar a semelhança entre imagens

## Arquivo file.c

Esse arquivo é responsável por abrir a imagem disponibilizada pelo usuário, tratar as informações nela contida e deixá-las disponíveis em uma struct chamada PGMImage, com todas as informações relevantes, como altura e largura da imagem, codificação, e maior nível de cinza. Também é responsável por limpar e livrar a memória ao fechar o programa

## Arquivo lbp.c 🎉

Esse arquivo é responsável pelo cálculo do algoritmo LBP, bem como pela construção do histograma desse mesmo algoritmo.

## Arquivo main.c

Por fim, esse arquivo é responsável por toda interação com o usuário, recebendo informações como a imagem a ser analisada, a pasta de comparação e o nome do arquivo para salvar a imagem tratada pelo LBP.