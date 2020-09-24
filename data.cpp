#include "data.h"

Data::Data(){

}

void Data::add(std::string cmd_line){

    //verifica se a estrutura da linha está correta
    size_t n = std::count(cmd_line.begin(), cmd_line.end(), ' ');
    if (n != 2){
        std::cout << "Comando incorreto, o uso deve ser:" << std::endl;
        std::cout << "\tadd <hostname> <ip>" << std::endl;
        return;
    }

    //lê valores
    std::istringstream iss(cmd_line); 
    std::string hostname, ip;
    iss >> cmd_line;
    iss >> hostname;
    iss >> ip;

    if(this->hostname_ip_table.find(hostname) ==  this->hostname_ip_table.end() ){  //hostname adicionado
        std::cout << "Entrada adicionada com sucesso." << std::endl;
    }else{                                                                          //hostname atualizado
        std::cout << "Essa entrada já existia e foi atualizada com sucesso." << std::endl;
    }

    this->hostname_ip_table[hostname] = ip;
}

std::string Data::search(std::string cmd_line){

    //verifica se a estrutura da linha está correta
    size_t n = std::count(cmd_line.begin(), cmd_line.end(), ' ');
    if (n != 2 && cmd_line.length() <= 7){
        std::cout << "Comando incorreto, o uso deve ser:" << std::endl;
        std::cout << "\tsearch <hostname>" << std::endl;
        return "";
    }

    //lê valores
    std::istringstream iss(cmd_line); 
    std::string hostname;
    iss >> cmd_line;
    iss >> hostname;

    if(this->hostname_ip_table.find(hostname) !=  this->hostname_ip_table.end() ){  //tem hostname salvo
        std::cout << hostname_ip_table[hostname] << std::endl;  //imprime para usuário
        return "";
    }else{  //não tem hostname salvo
        return hostname; //retorna seu valor
    }

}

std::string Data::search_hostname(std::string hostname){
    if(this->hostname_ip_table.find(hostname) !=  this->hostname_ip_table.end() ){ //tem hostname salvo
        return hostname_ip_table[hostname]; //retorna IP
    }else{
        return "-1";
    }
}