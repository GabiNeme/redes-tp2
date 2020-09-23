all:
	g++ servidor.cpp server_connect.cpp data.cpp -o servidor

clean:
	rm servidor
