// SERVER APPLICATION!!!!!!!!!!!!!!!

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdbool.h>

void SendFileToClient(int ClientSocket, char * FileName)
{
    int fd = 0;
    struct stat sobj;
    char Buffer[1024] = {'\0'};
    int BytesRead = 0;
    char Header[64] = {'\0'};

    printf("Filename is : %s :%ld\n",FileName,strlen(FileName));

    fd = open(FileName,O_RDONLY);

    if(fd < 0)
    {
        printf("Unable to open file\n");
        // Send error msg to client
        write(ClientSocket,"ERR\n",4);

        return;
    }

    stat(FileName,&sobj);

    // Header = "OK 1700"
    snprintf(Header,sizeof(Header),"OK %ld\n",(long)sobj.st_size);

    // Write header to client

    write(ClientSocket,Header,strlen(Header));

    // Send actual file contents
    while((BytesRead = read(fd,Buffer,sizeof(Buffer))) > 0)
    {
        // Send the data to client
        write(ClientSocket,Buffer,BytesRead);
    }

    close(fd);
}

/////////////////////////////////////////////////////////////////////////
//
//  Command line arguement Application
//  1st Arguement : Port number
//  ./server 9000
//  argv[0]  argv[1]
//
/////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    int ServerSocket = 0;
    int ClientSocket = 0;
    int Port = 0;
    int iRet = 0;

    char Filename[50] = {'\0'};
    
    __pid_t pid = 0;

    struct sockaddr_in ServerAddr; // Server fd
    struct sockaddr_in ClientAddr; // Client fd

    socklen_t AddrLen = sizeof(ClientAddr);
    
    if((argc < 2) || (argc > 2))
    {
        printf("Unable to proceed as invalid number of arguement\n");
        printf("Please provide the port number\n");

        return -1;
    }

    // Port number of server
    Port = atoi(argv[1]);

    ///////////////////////////////////////////////////////////
    //  Step 1 : Create TCP socket
    ///////////////////////////////////////////////////////////
                          //IPV4  //Talking betweet two socket
    ServerSocket = socket(AF_INET,SOCK_STREAM,0);

    if(ServerSocket < 0)
    {
        printf("Unable to create server socket\n");
        return -1;
    }

    ///////////////////////////////////////////////////////////
    //  Step 2 : Bind socket to ip and port
    ///////////////////////////////////////////////////////////

    memset(&ServerAddr,0,sizeof(ServerAddr));

    // Initialize the structure
    ServerAddr.sin_family = AF_INET; // IPV4 protocal to follow
    ServerAddr.sin_port = htons(Port); // Convert port to actual OS understandble port number
    ServerAddr.sin_addr.s_addr = INADDR_ANY; // It will listen from all network interface eg:- Ethernet,Wifi etc

    iRet = bind(ServerSocket,(struct sockaddr *)&ServerAddr,sizeof(ServerAddr));

    if(iRet == -1)
    {
        printf("Unable to bind\n");

        close(ServerSocket);
        
        return -1;
    }

    ///////////////////////////////////////////////////////////
    //  Step 3 : Listen for client connections
    ///////////////////////////////////////////////////////////

    iRet = listen(ServerSocket/*Server fd*/,11/* 11 = number of clients*/);

    if(iRet == -1)
    {
        printf("Server unable to listen the request\n");

        close(ServerSocket);

        return -1;
    }

    printf("Server is running on port number : %d\n",Port);

    ///////////////////////////////////////////////////////////
    //  Loop which accepts client request continiously
    ///////////////////////////////////////////////////////////

    // Loop to accept multiple client request
    while(1)
    {
        ///////////////////////////////////////////////////////////
        //  Step 4 : Accept the client requests 
        ///////////////////////////////////////////////////////////
        
        memset(&ClientAddr,0,sizeof(ClientAddr));

        printf("Server is Waiting for client request\n");

        ClientSocket = accept(ServerSocket, // Server fd
                              (struct sockaddr *)&ClientAddr, // Client socket structure address
                              &AddrLen   // address Sizeof that client structure
                            );
        
        if(ClientSocket < 0)
        {
            printf("Unable to accept client request\n");
            
            continue; // Continue the while loop if one client fails to connect
        }

        // inet_ntoa() Convert Internet number in IN to ASCII representation. The return value
        // is a pointer to an internal array containing the string.
        printf("Client gets connected : %s\n",inet_ntoa(ClientAddr.sin_addr));

        ///////////////////////////////////////////////////////////
        //  Step 5 : Create new process to handle client request 
        ///////////////////////////////////////////////////////////

        pid = fork();

        if(pid < 0)
        {
            printf("Unable to create a new process for client request\n");

            close(ClientSocket);

            continue;
        }

        // New process gets created to handle a client request
        if(pid == 0)
        {
            printf("New process is created for client request\n");

            close(ServerSocket);

            iRet = read(ClientSocket,Filename,sizeof(Filename));

            printf("Request file by client : %s\n",Filename);
            
            Filename[strcspn(Filename,"\r\n")] = '\0';

            SendFileToClient(ClientSocket,Filename);

            close(ClientSocket);

            printf("File tranfser done & client disconnected\n");

            exit(0); // this will kill the child process avoiding infinite loop
        } // End of if (fork)
        else
        {
            close(ClientSocket);
        }

    }// End of while

    close(ServerSocket);

    return 0;
}// End of main