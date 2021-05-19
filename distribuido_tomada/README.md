## Projeto 3 - 2020/2 - FSE

| Aluno | Matricula |
|--|--|
|Elias Bernardo Marques Magalhães| 150009011 |
|Leonardo dos Santos Silva Barreiros | 150135521 | 

O objetivo deste trabalho é criar um sistema distribuído de automação residencial utilizando um computador (PC) como sistema computacional central e placas ESP32 como controladores distribuídos, interconectados via Wifi através do protocolo MQTT.

## Compilação e utilização

Primeiramente tenha a certeza de ter o idf instalado na sua máquina. Em seguida, na raiz da pasta `distribuido_tomada/`. Depois configure as varáveis de ambiente necessárias e faça o build do projeto:

> Para o menuconfig:
```
$ idf.py menuconfig
```

> E para o build
```
$ idf.py build
```
Na sequência, faça o flash da imagem para a esp:

```
$ idf.py -p /dev/ttyUSB0 -b 115200 flash
```
> Atenção: sua porta e bounding podem ser diferentes. Confira antes de executar o comando.

Caso queria monitorar a placa, rode o seguinte comando:

```
$ idf.py -p /dev/ttyUSB0 monitor
```
Pronto, todo o resto deve rodar de forma automática. Para simular o alteração na entrada, apenas pressione o botão de boot. 