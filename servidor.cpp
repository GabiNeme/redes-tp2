#include "connect.h"
#include "data.h"

#include <arpa/inet.h>
#include <pthread.h>
#include <vector>
#include <cstring>
#include <fstream>

#define BUFSZ 100


Data hosts_data;
std::vector<sockaddr_storage> dns_servers;
char log_answer;
std::string answer;

struct request_thread_data{
    int serverSocket;
    struct sockaddr_storage cliaddr;
    std::string hostname;
};

std::string request_other_servers(int serverSocket,std::string hostname);
void * answer_request(void * request_data);
void * listen_to_msgs(void * serverSocket);
void save_dns_server(std::string cmd_line);
void read_input_file(std::string filename);

/*
    Servidor DNS
*/

int main(int argc, char **argv) {

    //Verifica argumentos
    if (argc != 2 && argc !=3) {
        printf("Para iniciar o servidor DNS digite como argumento a " 
            "porta de conexão e, opcionalmente, arquivo com hostnames e ips: \n");
        printf("%s  <porta> [startup] \n", argv[0]);
        exit(EXIT_FAILURE);
    }else if (argc == 3){ //foi informado um arquivo contendo comandos
        read_input_file(argv[2]);
    }

    //cria socket
    struct sockaddr_storage addr_storage;
    int serverSocket = init_server(argv[1], &addr_storage);
    struct sockaddr *addr = (struct sockaddr *)(&addr_storage);

    //adiciona ao socket o endereço local
    if (bind(serverSocket, addr, sizeof(addr_storage)) != 0) {
        logError("Failed to bind.");
    }

    //cria thread responsável por receber mensagens de outros servidores
    pthread_t tid;
    pthread_create(&tid, NULL, listen_to_msgs, &serverSocket);


    //lê linha de comandos
    while(1){
        printf("> ");
        std::string cmd_line;

        getline(std::cin, cmd_line);

        if(cmd_line.rfind("add ",0) == 0){              //comando add
            hosts_data.add(cmd_line);

        }else if(cmd_line.rfind("search ",0) == 0){     //comando search
            //verifica se possui esse hostname salvo
            std::string resp = hosts_data.search(cmd_line);

            if (resp != ""){ //não conhece hostname informado, então pede aos outros servidores DNS
                resp = request_other_servers(serverSocket, resp);
                if (resp == "-1"){
                    std::cout << "Resposta não encontrada." << std::endl;
                }else{
                    std::cout << resp << std::endl;
                }
            }
        }else if(cmd_line.rfind("link ",0) == 0){       //comando link
            save_dns_server(cmd_line);
        
        }else{                                          //comando errado
            std::cout << "Comando não identificado. As opções disponíveis são:" << std::endl;
            std::cout << "\tadd <hostname> <ip>" << std::endl;
            std::cout << "\tsearch <hostname>" << std::endl;
            std::cout << "\tlink <ip> <porta>" << std::endl;
        }
    }
  
    return 0;
}

void read_input_file(std::string filename){
    std::ifstream infile(filename);
    std::string line;

    while (std::getline(infile, line)){
        std::cout << "> " << line << std::endl;
        if(line.rfind("add ",0) == 0){          //add
            hosts_data.add(line);
        }else if(line.rfind("link ",0) == 0){   //link
            save_dns_server(line);
        }else{                                  //linha não reconhecida
            std::cout << "ERRO: Linha não reconhecida: '" << line << "'" << std::endl;
        }
    }
}



void save_dns_server(std::string cmd_line){

    //verifica se a estrutura da linha está correta
   size_t n = std::count(cmd_line.begin(), cmd_line.end(), ' ');
    if (n != 2){
        std::cout << "Comando incorreto, o uso deve ser:" << std::endl;
        std::cout << "\tlink <ip> <porta>" << std::endl;
        return;
    }

    //lê valores
    std::istringstream iss(cmd_line); 
    std::string ip, porta;
    iss >> cmd_line;
    iss >> ip;
    iss >> porta;

    struct sockaddr_storage s;

    //tenta criar um endereço a partir dos dados passados
    if (addr_parse(ip.c_str(),porta.c_str(), &s) == 0 ){    //se conseguiu criar, adiciona ao array de servidores
        dns_servers.push_back(s);
        std::cout << "Link salvo com sucesso." << std::endl;
    }else{  //caso contrário, informa ao usuário endereço inválido
        std::cout << "Não foi possível reconhecer o endereço informado como "
                "um endereço válido. Tente novamente." << std::endl;
    }
}



std::string request_other_servers(int serverSocket,std::string hostname){
    //percorre cada servidor conectado
    for(auto it = dns_servers.begin(); it != dns_servers.end(); ++it){
        struct sockaddr_storage oserver_addr_st = (*it);

        std::string s = "1" + hostname; 
        const char * msg = s.c_str();

        log_answer = 'W'; //marca que está esperando resposta
        //envia requisição
        sendto(serverSocket, msg, strlen(msg), 
            MSG_CONFIRM, (const struct sockaddr *) &oserver_addr_st, 
                sizeof(oserver_addr_st));
        
        
        while (log_answer == 'W'); //aguarda chegada da resposta
        if(answer != "-1"){ //lê variável global recém alterada
            return answer;
        }
        
    }
    return "-1";
}

void * answer_request(void * request_data){
    struct request_thread_data * r_data = (struct request_thread_data *) request_data;
    
    //verifica se possui esse hostname salvo
    std::string resp = hosts_data.search_hostname(r_data->hostname);

    if (resp == "-1"){ //não possui hostname salvo, pede aos outros servidores DNS
        resp = request_other_servers(r_data->serverSocket, r_data->hostname);
    }

    std::string s = "2" + resp;
    const char * msg = s.c_str();
    //envia resposta obtida aos servidor que requereu
    sendto(r_data->serverSocket,msg, strlen(msg), 
        MSG_CONFIRM, (const struct sockaddr *) &r_data->cliaddr, 
            sizeof(r_data->cliaddr)); 
    free(r_data);
    return request_data;
}

void * listen_to_msgs(void * serverSocket){
    int * s_socket = (int *) serverSocket;

    while(1){
        struct sockaddr_storage cliaddr; //armazenará endereço do servidor que enviou a mensagem
        memset(&cliaddr, 0, sizeof(cliaddr)); 

        unsigned int len = sizeof(cliaddr);
        char buffer[BUFSZ];

        int n = recvfrom(*s_socket, (char *)buffer, BUFSZ,  
                    MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
                    &len); 

        if (n > 0) { //se recebeu mensagem corretamente
            buffer[n] = '\0';
            std::string s = buffer;
            s.erase(0,1);
            if (buffer[0] == '1' ){         //requisição
                struct request_thread_data *r_data = (struct request_thread_data *) malloc(sizeof(*r_data));

                r_data->serverSocket = *s_socket;
                r_data->cliaddr = cliaddr;
                r_data->hostname = s;
                
                //cria nova thread para responder à requisição
                pthread_t tid;
                pthread_create(&tid, NULL, answer_request, r_data);

            }else if (buffer[0] == '2') {   //resposta
                answer = s;         //escreve resposta na variável pública
                log_answer = 'R';   //libera a leitura da variável
            }
        }
    }
}