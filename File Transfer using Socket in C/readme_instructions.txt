Server:

gcc server.c -o server -lpthread

./server bird.jpg




Client:


gcc client.c -o ./client
./client 192.168.1.6
