# Trabalho prático 2
### Redes de Computadores - 2020/1 - DCC023

O presente trabalho prático visa criar mini servidores DNS responsáveis por localizar e traduzir endereços digitados nos navegadores para endereços IPs.

Tais servidores DNS se comunicam através do protocolo UDP.

Para tanto, cada servidor pode ser conectado a outro através do comando link, além de ser possível pesquisar por determinado hostname e adicionar novas entradas para hostnames e IPs.

### Aluna:
Gabriela Peres Neme

## Compilar

```
make
```

### Rodar o servidor DNS
```
./servidor/dns <porta> [startup]
```
O arquivo opcional startup pode conter comandos para adicionar novas entradas no servidor DNS e para criar links entre servidores. Cada comando deve estar em uma linha e seguir a mesma estrutura descrita a seguir.
Um exemplo foi informado no arquivo input.txt.

### Comandos para os servidores (linha de comando)
#### Add
Cria uma entrada relacionando um hostname a um endereço IP, retornando ao usuário uma mensagem de sucesso. Caso seja feita uma inserção para um mesmo hostname a entrada deve ser atualizada, avisando o usuaŕio da atualização.
```
add <hostname> <ip>
```

#### Search
Procura pelo endereço IP associado ao hostname informado. Caso o dado não exista no servidor atual, são enviadas requisições para os servidores conectados ao atual.
```
search <hostname>
```

#### Link
Faz uma ligação unilateral com outro servidor DNS, informando o endereço IP e a porta de conexão.
```
link <ip> <porta>
```