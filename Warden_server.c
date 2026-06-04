//client server on different hosts


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

//Used to get input of n bytes//

// void read_exact(int fd, void *buf, size_t n) {
//     size_t total = 0;
//     while (total < n) {
//         ssize_t r = read(fd, (char*)buf + total, n - total);
//         if (r <= 0) exit(1);
//         total += r;
//     }
// }

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

    int PORT1,PORT2;
    printf("Enter PORT for Prisoner1 To Connect : ");
    scanf("%d",&PORT1);
    getchar();
    printf("Enter PORT for Prisoner2 To Connect: ");
    scanf("%d",&PORT2);
    getchar();

    //Initializing Two Socket File descriptor for both the prisoners//

    int prisoner1_sock,prisoner2_sock,ConPrisoner1Sock,ConPrisoner2Sock;

    struct sockaddr_in prisoner1_addr,prisoner2_addr,ConPrisoner1_addr,ConPrisoner2_addr;
    socklen_t addr_len=sizeof(struct sockaddr_in);


    // Creating TCP Socket 

    prisoner1_sock=socket(AF_INET,SOCK_STREAM,0);
    prisoner2_sock=socket(AF_INET,SOCK_STREAM,0);

     if (prisoner1_sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    if (prisoner2_sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Binding Both sockets with address//

    prisoner1_addr.sin_family=AF_INET;
    prisoner1_addr.sin_addr.s_addr=INADDR_ANY;
    prisoner1_addr.sin_port=htons(PORT1);

    prisoner2_addr.sin_family=AF_INET;
    prisoner2_addr.sin_addr.s_addr=INADDR_ANY;
    prisoner2_addr.sin_port=htons(PORT2);

    if(bind(prisoner1_sock,(struct sockaddr*)&prisoner1_addr,sizeof(prisoner1_addr))<0){
         perror("Bind failed");
         close(prisoner1_sock);
        exit(EXIT_FAILURE);
    }

    if(bind(prisoner2_sock,(struct sockaddr*)&prisoner2_addr,sizeof(prisoner2_addr))<0){
         perror("Bind failed");
         close(prisoner2_sock);
         exit(EXIT_FAILURE);
    }

    //prining the ipaddress and the port number of the server//

    socklen_t len1 = sizeof(prisoner1_addr);

    getsockname(prisoner1_sock,(struct sockaddr*)&prisoner1_addr,&len1);

    printf("prisoner1 listening on %s:%d\n",
    inet_ntoa(prisoner1_addr.sin_addr), ntohs(prisoner1_addr.sin_port));

    socklen_t len2 = sizeof(prisoner1_addr);

    getsockname(prisoner2_sock,(struct sockaddr*)&prisoner2_addr,&len2);

    printf("prisoner1 listening on %s:%d\n",
    inet_ntoa(prisoner2_addr.sin_addr), ntohs(prisoner2_addr.sin_port));

         if (listen(prisoner1_sock, 5) < 0) {

        perror("Listen prisoner1 failed");
        close(prisoner1_sock);
        exit(EXIT_FAILURE);
    }

       if (listen(prisoner2_sock, 5) < 0) {
        perror("Listen prisoner2 failed");
        close(prisoner2_sock);
        exit(EXIT_FAILURE);
    }

    // Accepting the connection i.e handing over the fd from kernel to the application//

    socklen_t len3=sizeof(ConPrisoner1_addr);
    socklen_t len4=sizeof(ConPrisoner2_addr);
    

    ConPrisoner1Sock=accept(prisoner1_sock,(struct sockaddr*)&ConPrisoner1_addr,&len3);
    ConPrisoner2Sock=accept(prisoner2_sock,(struct sockaddr*)&ConPrisoner2_addr,&len4);
    
    if(ConPrisoner1Sock<0){
        perror("Accept failed");
        close(prisoner1_sock);
        exit(EXIT_FAILURE);
    }

    // Print Prisoner1 IP and port//
        printf("Prisoner1 connected from %s:%d\n",
        inet_ntoa(ConPrisoner1_addr.sin_addr), ntohs(ConPrisoner1_addr.sin_port));


    if(ConPrisoner2Sock<0){
        perror("Accept failed");
        close(prisoner2_sock);
        exit(EXIT_FAILURE);
    }


    // Print Prisoner1 IP and port//
        printf("Prisoner2 connected from %s:%d\n",
        inet_ntoa(ConPrisoner2_addr.sin_addr), ntohs(ConPrisoner2_addr.sin_port));


    //taking the input from the Prisoner1//

    // 0 - Defects 
    // 1 - CoOperates

    int Prisoner1Defects=-1;
    int Prisoner2Defects=-1;

    char BufferPisoner1[1024]={0};
    char BufferPisoner2[1024]={0};

    int Prisoner1BytesRead;

    // Reading info from PRISONER 1//

    while((Prisoner1Defects==-1) && (Prisoner1BytesRead=read(ConPrisoner1Sock,BufferPisoner1,sizeof(char)))>0){
        if(Prisoner1BytesRead==1){
            if(BufferPisoner1[0]=='0'){
                Prisoner1Defects=  1;
            }
            else if(BufferPisoner1[0]=='1'){
                Prisoner1Defects=0;
            }
        }
    }

     // Reading info from PRISONER 2//


    int Prisoner2BytesRead;

    while((Prisoner2Defects==-1) && (Prisoner2BytesRead=read(ConPrisoner2Sock,BufferPisoner2,sizeof(char)))>0){
        if(Prisoner2BytesRead==1){
            if(BufferPisoner2[0]=='0'){
                Prisoner2Defects=  1;
            }
            else if(BufferPisoner2[0]=='1'){
                Prisoner2Defects=0;
            }
        }
    }

    // Decision Making By the Server //

    char BufferWarden[1024]={0};

    if(Prisoner1Defects==0){

        if(Prisoner2Defects==0){
            
            BufferWarden[0]='1';
            sendmessage(ConPrisoner1Sock,BufferWarden,sizeof(char));
            sendmessage(ConPrisoner2Sock,BufferWarden,sizeof(char));

        }

        else{

            BufferWarden[0]='3';
            BufferWarden[1]='0';

            sendmessage(ConPrisoner1Sock,BufferWarden,sizeof(char));
            sendmessage(ConPrisoner2Sock,BufferWarden+sizeof(char),sizeof(char));


        }
    }

    else{

        if(Prisoner2Defects==1){

            BufferWarden[0]='2';
            sendmessage(ConPrisoner1Sock,BufferWarden,sizeof(char));
            sendmessage(ConPrisoner2Sock,BufferWarden,sizeof(char));

        }

        else{

            BufferWarden[0]='0';
            BufferWarden[1]='3';

            sendmessage(ConPrisoner1Sock,BufferWarden,sizeof(char));
            sendmessage(ConPrisoner2Sock,BufferWarden+sizeof(char),sizeof(char));

        }
    }

    printf("Message Delivered to both the prisoners.\n");
    close (ConPrisoner1Sock);
    close(ConPrisoner2Sock);
    close(prisoner1_sock);
    close(prisoner2_sock);

    

    
    

}
