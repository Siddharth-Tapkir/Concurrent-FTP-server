// CLIENT APPLICATION!!!!!!!!!!!!!!!

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

int ReadLine(int Sock, char *line, int max)
{
    int i = 0;
    int n = 0;
    char ch = '\0';

    while(i < max - 1)
    {
        n = read(Sock,&ch,1);

        if(n <= 0)
        {
            break;
        }
        
        line[i++] = ch;
        
        if(ch == '\n')
        {
            break;
        }
    }// End of while

    return i;
}

/////////////////////////////////////////////////////////////////////////
//
//  Command line arguement Application
//  1st Arguement : IP address 
//  2nd Arguement : Port number
//  3rd Arguement : Target file name
//  4th Arguement : New file name
//
//  ./client  127.0.0.1   9000     Demo.txt   a.txt
//  argv[0]   argv[1]     argv[2]  argv[3]    argv[4]
//
/////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    char Header[64] = {'\0'};
    int Sock = 0;
    int iRet = 0;
    struct sockaddr_in ServerAddr;

    char *ip = NULL;         //argv[1]
    int Port = 0;            //argv[2]
    char *filename = NULL;   //argv[3]
    char *outfilename = NULL;//argv[4]

    if((argc < 5) || (argc > 5))
    {
        printf("Unable to proceed as invalid number of arguements\n");

        printf("Please provide below arguements\n");

        printf("1st Arguement : IP address\n");
        printf("2nd Arguement : Port number\n");
        printf("3rd Arguement : Target file name\n");
        printf("4th Arguement : New file name\n");
        return -1;
    }

    // Store command line arguements into the variables

    ip = argv[1];
    Port = atoi(argv[2]);
    filename = argv[3];
    outfilename = argv[4];

    /////////////////////////////////////////////////////////
    // Step 1 : Create TCP socket
    /////////////////////////////////////////////////////////

    Sock = socket(AF_INET // Follow IPV4 protocal
                ,SOCK_STREAM, // Communicate between two sockets
                0);
    
    if(Sock < 0)
    {
        printf("Unable to create the client socket\n");
        return -1;
    }

    /////////////////////////////////////////////////////////
    // Step 2 : Connect with server
    /////////////////////////////////////////////////////////

    memset(&ServerAddr,0,sizeof(ServerAddr));

    ServerAddr.sin_family = AF_INET; // Initialize IPV4 into structure
    ServerAddr.sin_port = htons(Port);

    //Convert the IP address into binary format
    inet_pton(AF_INET,ip,&ServerAddr.sin_addr);

    iRet = connect(Sock,(struct sockaddr *)&ServerAddr,sizeof(ServerAddr));

    if(iRet == -1)
    {
        printf("Unable to connect with server\n");
        
        close(Sock);

        return -1;
    }

    /////////////////////////////////////////////////////////
    // Step 3 : Send file name
    /////////////////////////////////////////////////////////
    write(Sock,filename,strlen(filename));
    write(Sock,"\n",1);

    /////////////////////////////////////////////////////////
    // Step 4 : Read the header
    /////////////////////////////////////////////////////////
    iRet = ReadLine(Sock,Header,sizeof(Header));

    if(iRet <= 0)
    {
        printf("Server gets disconnected abnormally\n");
        close(Sock);
        return -1;
    }

    long filesize = 0;

    sscanf(Header,"OK %ld",&filesize);
    printf("File size is : %ld\n",filesize);

    /////////////////////////////////////////////////////////
    // Step 5 : Create new file
    /////////////////////////////////////////////////////////

    int outfd = 0;

    outfd = open(outfilename,O_CREAT | O_WRONLY | O_TRUNC,0777);

    if(outfd < 0)
    {
        printf("Unable to create downloaded file\n");
        return -1;
    }

    char Buffer[1024] = {'\0'};
    long recevied = 0;
    long remaining = 0;
    int n = 0;
    int toRead = 0;

    while(recevied < filesize)
    {
        remaining = filesize - recevied;
        
        if(remaining > 1024)
        {
            toRead = 1024;
        }
        else
        {
            toRead = remaining;
        }

        n = read(Sock,Buffer,toRead);

        if(n == 0)
        {
            break;
        }

        write(outfd,Buffer,n);

        recevied = recevied + n;
    }// End of while

    close(outfd);
    close(Sock);

    if(recevied == filesize)
    {
        printf("Download complete...\n");
        return 0;
    }
    else
    {
        printf("Download failed...\n");
        return -1;
    }
    return 0;
}// End of main