#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define PORT 8080

int main() {
    int sockettine; // un nom qui déchire, descripteur socket serveur
    int msgSize=256;

    sockettine = socket(AF_INET, SOCK_STREAM, 0);

    if (sockettine < 0) {
        perror("Erreur socket");
        exit(EXIT_FAILURE);
    }

    printf("%d\n", sockettine);

    // Permettre reuse immédiat de la socket
    int reuse = 1;
    setsockopt(sockettine, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    // Initialisation structure sockaddr_in et de ses param
    struct sockaddr_in my_addr;
    memset((char*)&my_addr, 0, sizeof(my_addr));

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY; // Init le champ sin_addr : @ globale

    // Lien entre structure sockaddr_in et la socket (lier socket à @)
    if (bind(sockettine, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) {  // (struct format_voulu *)&format_actuel (avec & pointeur)
        perror("Erreur bind");
        exit(EXIT_FAILURE);
    }

    // Nb max de connexion en attente sur la socket, attendre connexions
    listen(sockettine, 7);

    // Boucle infinie (-> daemon)
    while(1){
        // Créer nouvelle adresse
        struct sockaddr_in new_addr;
        int addr_len = sizeof(new_addr);
        // Fonction accept pour traiter la demande de connexion (mise en variable pour pouvoir la print)
        int socket_client = accept(sockettine, (struct sockaddr *)&new_addr, &addr_len);
        if (socket_client < 0) {
            perror("Erreur accept");
            continue;
        }
        printf("Socket client fd = %d\n", socket_client);

        // Récupérer IP et port 
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &new_addr.sin_addr, client_ip, sizeof(client_ip));

        int client_port = ntohs(new_addr.sin_port);

        printf("Client IP : %s, Port : %d\n", client_ip, client_port);


        // Créer un processus fils pour gérer le client
        pid_t pid = fork();
        if (pid == 0) {  // Processus fils
            close(sockettine); // Le fils n'a pas besoin du socket serveur

            // Récupérer le nom du fichier envoyé par le client
            char filename[msgSize];
            memset(filename, 0, msgSize);
            recv(socket_client, filename, sizeof(filename), 0);
            filename[strcspn(filename, "\n")] = 0;  // Enlever le retour à la ligne

            // Ouvrir le fichier demandé
            FILE *file = fopen(filename, "rb");
            if (file == NULL) {
                perror("Fichier non trouvé");
                close(socket_client);
                exit(EXIT_FAILURE);
            }

            // Lire et envoyer le fichier au client
            char buffer[msgSize];
            size_t bytes_read;
            while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
                send(socket_client, buffer, bytes_read, 0);
            }

            // Fermer le fichier et la connexion
            fclose(file);
            close(socket_client);
            exit(EXIT_SUCCESS);
        } else if (pid > 0) {  // Processus père
            close(socket_client);  // Le père n'a pas besoin du socket client
        } else {
            perror("Erreur fork");
            exit(EXIT_FAILURE);
        }
    }

    // Fermer la socket serveur
    close(sockettine);
    return 0;
}
