//Prisoner1 client 1;



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define BUFFER_SIZE 1024

void sendmessage(int socket, char* buffer, size_t len){
            size_t sent = 0;
            while (sent < len) {
                ssize_t n = send(socket, buffer + sent, len - sent, 0);
                if (n <= 0) {
                    perror("send");
                    close(socket);
                    
                    exit(EXIT_FAILURE);
                }
                sent += n;
            }
}

int main(){

    int Prisoner1Sock;
    struct sockaddr_in Prisoner1Addr;
    char Prisoner1Buffer[1024];

    int PORT;
    char ipaddr[INET_ADDRSTRLEN];

    printf("Enter IP address: ");
    fgets(ipaddr, INET_ADDRSTRLEN, stdin);

    printf("Enter DESTINATION PORT: ");
    scanf("%d",&PORT);
    getchar();
    // remove newline
    ipaddr[strcspn(ipaddr, "\n")] = '\0';

    // Creating A TCP Socket//

    if((Prisoner1Sock=socket(AF_INET,SOCK_STREAM,0))<0){
        perror("Prisoner1 Failed");
        exit(EXIT_FAILURE);
    }

    // Adding Port Number and IP family of the final destination//

    Prisoner1Addr.sin_family=AF_INET;
    Prisoner1Addr.sin_port=htons(PORT);

    // Adding the IP address of the final destination / warden //

    if(inet_pton(AF_INET,ipaddr,&Prisoner1Addr.sin_addr)<=0){
        perror("Invalid Address");
        close(Prisoner1Sock);
        exit(EXIT_FAILURE);
    }

    // Connecting to the server //
    // Connect does implicit binding and it connects to any one of the local iP in the device 

     if(connect(Prisoner1Sock, (struct sockaddr*)&Prisoner1Addr,sizeof(Prisoner1Addr))<0){
        perror("Prisoner1 wasnt able to connect");
        close(Prisoner1Sock);
        exit(EXIT_FAILURE);
     }

     printf("You can only type 2 possible Inputs\n");
     fflush(stdout);
     printf(" 0 - DEFECT \n");
     fflush(stdout);
     printf(" 1 - COOPERATE \n");
     fflush(stdout);
     printf("What is Your Decision? : \n");
     fflush(stdout);

     fgets(Prisoner1Buffer,BUFFER_SIZE,stdin);
     char Input1=Prisoner1Buffer[0];
     int n=strlen(Prisoner1Buffer);

     while((n!=2) || (!(Input1=='0') && !(Input1=='1'))){

         printf("Stupid, Give Proper Answer, What is Your Decision? : \n");
         fflush(stdout);

         fgets(Prisoner1Buffer,BUFFER_SIZE,stdin);
         Input1=Prisoner1Buffer[0];
         n=strlen(Prisoner1Buffer);

     }

     sendmessage(Prisoner1Sock,Prisoner1Buffer,sizeof(char));

     int Output=-1;
     int Prisoner1BytesRead=0;

     while((Output==-1) && (Prisoner1BytesRead=read(Prisoner1Sock,Prisoner1Buffer,sizeof(char)))>0){
        if(Prisoner1BytesRead==1){
            if(Prisoner1Buffer[0]=='0'){
                Output= 0;
            }
            else if(Prisoner1Buffer[0]=='1'){
                Output=1;
            }
            else if(Prisoner1Buffer[0]=='2'){
                Output=2;
            }
            else if(Prisoner1Buffer[0]=='3'){
                Output=3;
            }
        }
    }

    printf("Hey! You are getting %d years of Imprisonment , Enjoy!!", Output);
    close(Prisoner1Sock);
    return 0;




}