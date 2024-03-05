#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/socket.h>
#include  <netdb.h>
#include  <string.h>
#include  <unistd.h>
#include  <stdbool.h>
#include "./simpleSocketAPI.h"


#define SERVADDR "127.0.0.1"        // Définition de l'adresse IP d'écoute
#define SERVPORT "0"                // Définition du port d'écoute, si 0 port choisi dynamiquement
#define LISTENLEN 1                 // Taille de la file des demandes de connexion
#define MAXBUFFERLEN 1024           // Taille du tampon pour les échanges de données
#define MAXHOSTLEN 64               // Taille d'un nom de machine
#define MAXPORTLEN 64               // Taille d'un numéro de port

#define PORTFTP "21"                // numéro de port de connexion


int main(){
    int ecode;                       // Code retour des fonctions
    char serverAddr[MAXHOSTLEN];     // Adresse du serveur
    char serverPort[MAXPORTLEN];     // Port du server
    int descSockRDV;                 // Descripteur de socket de rendez-vous
    int descSockCOM;                 // Descripteur de socket de communication
    struct addrinfo hints;           // Contrôle la fonction getaddrinfo
    struct addrinfo *res;            // Contient le résultat de la fonction getaddrinfo
    struct sockaddr_storage myinfo;  // Informations sur la connexion de RDV
    struct sockaddr_storage from;    // Informations sur le client connecté
    socklen_t len;                   // Variable utilisée pour stocker les 
				                     // longueurs des structures de socket
    char buffer[MAXBUFFERLEN];       // Tampon de communication entre le client et le serveur

    char username[50];               // nom d'utilisateur pour la connexion
    int sockserveurcmd;              //socket comande serveur 
    int ip1, ip2, ip3, ip4;          // stockage ip envoyer provisoir
    int p1, p2 ;                     // stockage port envoyer provisoir
    char addripcl[MAXHOSTLEN];       // stockage addresse ip client    
    char  portcl[MAXPORTLEN];        // stockage port client   
    int sockactive;                  // socket active du client
    char pasv[50] ="PASV\r\n";       // commende pasive
    char addripsv[MAXHOSTLEN];       // stockage addresse ip serveur    
    char  portsv[MAXPORTLEN];        // stockage port serveur 
    int sockpassive;                 // socket passive du serveur
    char msg[50] ="200 OK\r\n";      // code confirmation client



    // Initialisation de la socket de RDV IPv4/TCP
    descSockRDV = socket(AF_INET, SOCK_STREAM, 0);
    if (descSockRDV == -1) {
         perror("Erreur création socket RDV\n");
         exit(2);
    }
    // Publication de la socket au niveau du système
    // Assignation d'une adresse IP et un numéro de port
    // Mise à zéro de hints
    memset(&hints, 0, sizeof(hints));
    // Initialisation de hints
    hints.ai_flags = AI_PASSIVE;      // mode serveur, nous allons utiliser la fonction bind
    hints.ai_socktype = SOCK_STREAM;  // TCP
    hints.ai_family = AF_INET;        // seules les adresses IPv4 seront présentées par 
				                      // la fonction getaddrinfo

     // Récupération des informations du serveur
     ecode = getaddrinfo(SERVADDR, SERVPORT, &hints, &res);
     if (ecode) {
         fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(ecode));
         exit(1);
     }
     // Publication de la socket
     ecode = bind(descSockRDV, res->ai_addr, res->ai_addrlen);
     if (ecode == -1) {
         perror("Erreur liaison de la socket de RDV");
         exit(3);
     }
     // Nous n'avons plus besoin de cette liste chainée addrinfo
     freeaddrinfo(res);

     // Récuppération du nom de la machine et du numéro de port pour affichage à l'écran
     len=sizeof(struct sockaddr_storage);
     ecode=getsockname(descSockRDV, (struct sockaddr *) &myinfo, &len);
     if (ecode == -1)
     {
         perror("SERVEUR: getsockname");
         exit(4);
     }
     ecode = getnameinfo((struct sockaddr*)&myinfo, sizeof(myinfo), serverAddr,MAXHOSTLEN, 
                         serverPort, MAXPORTLEN, NI_NUMERICHOST | NI_NUMERICSERV);
     if (ecode != 0) {
             fprintf(stderr, "error in getnameinfo: %s\n", gai_strerror(ecode));
             exit(4);
     }
     printf("L'adresse d'ecoute est: %s\n", serverAddr);
     printf("Le port d'ecoute est: %s\n", serverPort);

     // Definition de la taille du tampon contenant les demandes de connexion
     ecode = listen(descSockRDV, LISTENLEN);
     if (ecode == -1) {
         perror("Erreur initialisation buffer d'écoute");
         exit(5);
     }

	len = sizeof(struct sockaddr_storage);
     // Attente connexion du client
     // Lorsque demande de connexion, creation d'une socket de communication avec le client
     descSockCOM = accept(descSockRDV, (struct sockaddr *) &from, &len);
     if (descSockCOM == -1){
         perror("Erreur accept\n");
         exit(6);
     }
    // Echange de données avec le client connecté

    /*****
     * Testez de mettre 220 devant BLABLABLA ...
     * **/
    strcpy(buffer, "220 BLABLABLA\n");
    write(descSockCOM, buffer, strlen(buffer));

    /*******
     * 
     * A vous de continuer !
     * 
     * *****/

    //lecture du message entré par le client permetant de se connecté au serveur visé ("username"@"adresseduserveur")
    ecode=read(descSockCOM,buffer,MAXBUFFERLEN);

    //la fonction read renvoie -1 en cas d'erreur
    if (ecode==-1){
        perror("Erreur de lecture de la socket pour se connecté au serveur");
        exit(7);
    }

    // \0 correspond a la suppresion des caractères nul de fin du buffer
    buffer[ecode]='\0';
    printf("Message recu: %s\n",buffer);

    //séparer le "username" de l' "adresse" du serveur
    sscanf(buffer,"%[^@]@%s",username,serverAddr);
    printf("Username :%s\n Serveur : %s", username, serverAddr);

    //création de la socket de connexion du serveur
    ecode = connect2Server(serverAddr, PORTFTP, &sockserveurcmd);
    if (ecode==-1){
        perror("Erreur de la connexion au serveur\n");
        exit(8);
    }
    printf("Connexion Serveur réussie\n");


    //lecture du message retour apres connexion du serveur
    ecode=read(sockserveurcmd,buffer,MAXBUFFERLEN);
    if (ecode==-1){
        perror("Erreur lecture de la socket du serveur\n");
        exit(8);
    }
    buffer[ecode]='\0';
    printf("Message recu du serveur: %s\n",buffer);

    //envoie de username du client au serveur
    //sprintf permet de mettre une variable dans le buffer
    sprintf(buffer,"%s\r\n",username);

    ecode=write(sockserveurcmd,buffer,strlen(buffer));
    if (ecode==-1){
        perror("Erreur écriture dans la socket du serveur\n");
        exit(8);
    }
    printf("Message envoyé au serveur: %s\n", buffer);

    //lecture du message retour apres connexion du serveur
    ecode=read(sockserveurcmd,buffer,MAXBUFFERLEN);
    if (ecode==-1){
        perror("Erreur lecture de la socket du serveur\n");
        exit(8);
    }
    buffer[ecode]='\0';
    printf("Message recu du serveur: %s\n",buffer);    

    //transmet au client le message du serveur

      ecode=write(descSockCOM,buffer,strlen(buffer));
    if (ecode==-1){
        perror("Erreur écriture dans la socket du client\n");
        exit(7);
    }
    printf("Message envoyé au client: %s\n", buffer);

    // lecture du mot de passe du client (adresse email)

     ecode=read(descSockCOM,buffer,strlen(buffer));
    if (ecode==-1){
        perror("Erreur lecture dans la socket client\n");
        exit(10);
    }
    buffer[ecode]='\0';
    printf("Message recu du client: %s\n",buffer);  

    // transmet le mot de passe au serveur

    ecode=write(sockserveurcmd,buffer,strlen(buffer));
    if (ecode==-1){
        perror("Erreur écriture dans la socket du serveur \n");
        exit(7);
    }
    printf("Message envoyé au serveur: %s\n", buffer);

    // lecture du retour serveur 

    ecode=read(sockserveurcmd,buffer,MAXBUFFERLEN);
    if (ecode==-1){
        perror("Erreur lecture dans socket \n");
        exit(10);
    }
    buffer[ecode]='\0';
    printf("Message recu du serveur: %s\n",buffer);  

    //transmet au client le message du serveur 

    ecode=write(descSockCOM,buffer,strlen(buffer));
    if (ecode==-1){
        perror("Erreur écriture dans la socket du client \n");
        exit(7);
    }
    printf("Message envoyé au client aaaaa: %s\n", buffer);

    // lecture message client 

    ecode=read(descSockCOM,buffer,MAXBUFFERLEN);
    if (ecode==-1){
        perror("Erreur lecture dans socket client \n");
        exit(10);
    }
    buffer[ecode]='\0';
    printf("Message recu du client: %s\n",buffer);  

    // transmet au serveur message client 

    ecode=write(sockserveurcmd,buffer,strlen(buffer));
    if (ecode==-1){
        perror("Erreur écriture dans la socket du serveur \n");
        exit(7);
    }
    printf("Message envoyé au serveur : %s\n", buffer);

    // lecture du serveur 

    ecode=read(sockserveurcmd,buffer,MAXBUFFERLEN);
    if (ecode==-1){
        perror("Erreur lecture dans socket serveur \n");
        exit(10);
    }
    buffer[ecode]='\0';
    printf("Message recu du serveur: %s\n",buffer); 

    // transmet au client le message du serveur 

    ecode=write(descSockCOM,buffer,strlen(buffer));
    if (ecode==-1){
        perror("Erreur écriture dans la socket du client \n");
        exit(7);
    }
    printf("Message envoyé au client: %s\n", buffer);

    // lecture message client 

    ecode=read(descSockCOM,buffer,MAXBUFFERLEN);
    if (ecode==-1){
        perror("Erreur lecture dans socket client \n");
        exit(10);
    }
    buffer[ecode]='\0';
    printf("Message recu du client: %s\n",buffer);  

    // distinction adresse IP client du port client 

    sscanf(buffer,"PORT %d,%d,%d,%d,%d,%d", &ip1,&ip2,&ip3,&ip4,&p1,&p2);
    sprintf(addripcl,"%d.%d.%d.%d", ip1,ip2,ip3,ip4);
    p1 = p1*256 +p2 ;
    sprintf(portcl,"%d", p1);
    printf("IP Client :%s\n Port Client : %s", addripcl, portcl);

    //Connexion avec le client
    ecode = connect2Server(addripcl, portcl, &sockactive);


    //envoie passif au serveur pour la connexion
    ecode=write(sockserveurcmd,pasv,strlen(pasv));
    if (ecode==-1){
        perror("Erreur écriture dans la socket du serveur \n");
        exit(7);
    }
    printf("Message envoyer au serveur: %s\n",pasv);

    // lecture du serveur 
    ecode=read(sockserveurcmd,buffer,MAXBUFFERLEN);
    if (ecode==-1){
        perror("Erreur lecture dans socket serveur \n");
        exit(10);
    }
    buffer[ecode]='\0';
    printf("Message recu du serveur: %s\n",buffer);

    // distinction adresse IP serveur du port serveur 
    sscanf(buffer,"227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", &ip1,&ip2,&ip3,&ip4,&p1,&p2);
    sprintf(addripsv,"%d.%d.%d.%d", ip1,ip2,ip3,ip4);
    p1 = p1*256 +p2 ;
    sprintf(portsv,"%d", p1);
    printf("IP Serveur :%s\n Port Serveur : %s", addripsv, portsv);

    //Connexion avec le client
    ecode = connect2Server(addripsv, portsv, &sockpassive);
    if (ecode==-1){
        perror("Erreur connexion serveur en mode passive \n");
        exit(10);
    }

    //envoie de la confirmation client
    write(descSockCOM, msg, strlen(msg));

    // lecture du client 
    ecode=read(descSockCOM,buffer,MAXBUFFERLEN);
    if (ecode==-1){
        perror("Erreur lecture dans socket Client \n");
        exit(10);
    }
    buffer[ecode]='\0';
    printf("Message recu du Client: %s\n",buffer);

    // transmet au serveur message client 

    ecode=write(sockserveurcmd,buffer,strlen(buffer));
    if (ecode==-1){
        perror("Erreur écriture dans la socket du serveur \n");
        exit(7);
    }
    printf("Message envoyé au serveur : %s\n", buffer);

    // lecture du serveur 
    ecode=read(sockserveurcmd,buffer,MAXBUFFERLEN);
    if (ecode==-1){
        perror("Erreur lecture dans socket serveur \n");
        exit(10);
    }
    buffer[ecode]='\0';
    printf("Message recu du serveur: %s\n",buffer);

    // transmet au client message serveur 

    ecode=write(descSockCOM,buffer,strlen(buffer));
    if (ecode==-1){
        perror("Erreur écriture dans la socket du Client \n");
        exit(7);
    }
    printf("Message envoyé au Client : %s\n", buffer);

    // boucle lire ls en entier 

    do {
        //lecture donne ls serveur

        ecode= read(sockpassive,buffer,MAXBUFFERLEN);
        if (ecode==-1){
        perror("probleme de lecture du serveur \n");
        exit(10);
        }
        buffer[ecode]='\0';
        printf(" %s\n",buffer);

        // envoie des donnee au client 
        write(sockactive,buffer,strlen(buffer));

    }while(read(sockpassive,buffer,MAXBUFFERLEN)!=0);

    close(sockactive);
    close(sockpassive);

    // lecture confirmation du serveur

    ecode=read(sockserveurcmd,buffer,MAXBUFFERLEN);
    if (ecode==-1){
        perror("Erreur lecture dans socket serveur \n");
        exit(10);
    }
    buffer[ecode]='\0';
    printf("Message recu du serveur: %s\n",buffer);

    // transmet au client message serveur 

    ecode=write(descSockCOM,buffer,strlen(buffer));
    if (ecode==-1){
        perror("Erreur écriture dans la socket du Client \n");
        exit(7);
    }
    printf("Message envoyé au Client : %s\n", buffer);


    //Fermeture de la connexion
    close(sockserveurcmd);
    close(descSockCOM);
    close(descSockRDV);
    printf("connexion terminee\n");
}

