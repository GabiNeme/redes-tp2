all:
	g++ -pthread servidor.cpp connect.cpp data.cpp -o servidor_dns

clean:
	rm servidor_dns
