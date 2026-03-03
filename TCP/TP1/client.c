#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define PORT 8080

int main() {
    int sockettine;
    int msgSize = 25;

    sockettine = socket(AF_INET, SOCK_STREAM, 0);

    if (sockettine < 0) {
        perror("Erreur socket");
        exit(EXIT_FAILURE);
    }

    printf("%d\n", sockettine);

    // Permettre reuse immédiat de la socket
    int reuse = 1;
    setsockopt(sockettine, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    // Initialisation structure sockaddre_in
    struct sockaddr_in my_addr;
    memset((char*)&my_addr, 0, sizeof(my_addr));

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(PORT);
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Init le champ sin_addr avec @ précise (loopback ici car on est sur la même machine, mais sinon @server)


    // Demande de connexion du client : connect(sockettine, (struct sockaddr *)&my_addr, sizeof(my_addr));
    if (connect(sockettine, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) {
        perror("Erreur connect");
        exit(EXIT_FAILURE);
    }

    // Définitino d'un buffer de taille 256. Ensuite, on copie la char dedans, et on l'envoie
    char buffer[msgSize];
    strcpy(buffer, "salut la tcheam");
    send(sockettine, buffer, strlen(buffer),0);
}