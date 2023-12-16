# T2 Sistemas digitais embrcados

### Neste projeto deseja-se criar um gerador de ondas utilizando esse conversor e um microcontrolador
ESP. 
O gerador de ondas é um dispositivo usado em laboratório para gerar ondas de determinado formato
(quadrado, triangulo, serra, senoide) com uma determinada frequencia. O estudante seleciona num botão a
onda e ajusta a frequencia. A saída do gerador pode ser observada num osciloscópio.

Neste trabalho, o microcontrolador deve enviar valores para os pinos de saida (que estão ligados aos pinos
D7..D0 do conversor) para gerar os valores analógicos correspondente e assim desenhar a onda adequada.
Os valores para descrever a onda podem ser armazenados internamente num vetor, um vetor para onda
quadrada, um vetor para onda serra, triangular, senoidal.

O software do ESP deve monitorar o terminal serial para
saber qual a opção desejada e após a seleção e assim escolher de
qual dos vetores os valores serão obtidos e ciclicamente enviar
para os pinos de saída esses dados.