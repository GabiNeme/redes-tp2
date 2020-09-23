#include "server_connect.h"
#include "data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BUFSZ 100

int main(int argc, char **argv) {

    Data hosts_data;

    if (argc != 2 && argc !=3) {
        printf("Para iniciar o servidor DNS digite como argumento a " 
            "porta de conexão e, opcionalmente, arquivo com hostnames e ips: \n");
        printf("%s  <porta> [startup] \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //cria socket
    struct sockaddr_storage addr_storage;
    int serverSocket = init_server(argv[1], &addr_storage);
    struct sockaddr *addr = (struct sockaddr *)(&addr_storage);

    //adiciona ao socket o endereço local
    if (bind(serverSocket, addr, sizeof(addr_storage)) != 0) {
        logError("Failed to bind.");
    }


    while(1){
        printf("> ");
        std::string cmd_line;

        getline(std::cin, cmd_line);

        if(cmd_line.rfind("add ",0) == 0){
            hosts_data.add(cmd_line);
        }else if(cmd_line.rfind("search ",0) == 0){
            int resp;
            resp = hosts_data.search(cmd_line);

            if (resp == 0){ //requisição correta, pedir aos outros servidores DNS

            }
        
        }else if(cmd_line.rfind("link ",0) == 0){

        }else{
            std::cout << "Comando não identificado. As opções disponíveis são:" << std::endl;
            std::cout << "\tadd <hostname> <ip>" << std::endl;
            std::cout << "\tsearch <hostname>" << std::endl;
            std::cout << "\tlink <ip> <porta>" << std::endl;
        }



    }


    if (strcmp(argv[1], "5111") == 0){
        char hello[BUFSZ] = "Hello from server"; 


        struct sockaddr_storage oserver_addr_st;

        addr_parse("0.0.0.0", "5112", &oserver_addr_st);

        sendto(serverSocket,(const char *)hello, strlen(hello), 
            MSG_CONFIRM, (const struct sockaddr *) &oserver_addr_st, 
                sizeof(oserver_addr_st)); 


        printf("msg sent\n");
    }


    if (strcmp(argv[1], "5112") == 0){
        struct sockaddr_storage cliaddr; 

        memset(&cliaddr, 0, sizeof(cliaddr)); 
        unsigned int len;
        int n; 
  
        len = sizeof(cliaddr);  //len is value/resuslt 

        char buffer[BUFSZ];

        addrtostr(addr, buffer,BUFSZ);
        printf("%s\n",buffer);

        n = -1;
        
        n = recvfrom(serverSocket, (char *)buffer, BUFSZ,  
                    MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
                    &len); 
        
        
        buffer[n] = '\0'; 
        printf("Client : %s\n", buffer); 
    }
    // //prepara para receber conexões
    // if (listen(serverSocket, 10) != 0) {
    //     logError("Failed to listen.");
    // }

    // while (1) {

    //     //recebe conexão do cliente
    //     struct sockaddr_storage client_storage;
    //     struct sockaddr *client_addr = (struct sockaddr *)(&client_storage);
    //     socklen_t client_addrlen = sizeof(client_storage);

    //     int client_socket = accept(serverSocket, client_addr, &client_addrlen);
    //     if (client_socket == -1) {
    //         logError("Failed to accept new client connection");
    //     }

    //     //envia ao cliente o tamanho da palavra
    //     char buf[BUFSZ];
    //     memset(buf, 0, BUFSZ);
    //     buf[0] = 1;
    //     buf[1] = strlen(word);
    //     size_t count = send(client_socket, buf, 2 , 0);
    //     if (count != 2) {
    //         logError("send size of word");
    //     }

    //     //estrutura de dados para armazenar as letras adivinhadas
    //     struct Forca Forca;
    //     init_forca(&Forca);
        
    //     while(1){
    //         // recebe palpite do cliente
    //         memset(buf, 0, 2);
    //         recv(client_socket, buf, 2, 0);
    //         char c = buf[1];
    //         unsigned int oc_count;
            
    //         //verifica se a letra está na palavra
    //         check_char_in_word(&Forca, c, &oc_count, buf);
            
    //         //idetifica se cliente já adivinhou a palavra
    //         //e envia mensagem 4 se sim
    //         if (guessed_all(&Forca)){
    //             buf[0] = 4;
    //             count = send(client_socket, buf, 1, 0);
    //             if (count != 1) {
    //                 logError("send end of game");
    //             }
    //             break;
    //         }

    //         //envia a resposta ao palpite ao cliente
    //         count = send(client_socket, buf, oc_count+2, 0);
    //         if (count != oc_count+2) {
    //             logError("send size of word");
    //         }

    //     }


    //     free(Forca.guessed);
    //     close(client_socket);
    // }

    return 0;
}