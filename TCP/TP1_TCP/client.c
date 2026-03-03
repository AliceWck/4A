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
    int MSG_SIZE = 256;

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

    // Demander un fichier en envoyant son nom
    char filename[MSG_SIZE];
    printf("Entrez le nom du fichier à recevoir : ");
    fgets(filename, MSG_SIZE, stdin);
    filename[strcspn(filename, "\n")] = 0;  // Supprimer le retour à la ligne

    send(sockettine, filename, strlen(filename), 0);

    // Réception du fichier
    char buffer[MSG_SIZE];
    FILE *file = fopen("copie_du_fichier", "wb");
    if (file == NULL) {
        perror("Erreur ouverture fichier");
        close(sockettine);
        exit(EXIT_FAILURE);
    }

    int bytes_received;
    while ((bytes_received = recv(sockettine, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }

    printf("Fichier reçu et sauvegardé.\n");

    // Fermer le fichier et la connexion
    fclose(file);
    close(sockettine);
    return 0;
}