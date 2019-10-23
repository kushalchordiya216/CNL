#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <fstream>
#include <cstdlib>
#include <netinet/in.h>

void sendFile(char *FILE_NAME, char *BUFFER, int sockfd, struct sockaddr_in addr_con, socklen_t addr_len)
{
    int index = 0;
    std::fstream file_obj;
    file_obj.open(FILE_NAME, std::ios::in);
    if (file_obj)
    {
        std::cout << "File opened!\nReading into socket!\n";
    }
    else
    {
        std::cout << "File named" << FILE_NAME << "doesnt exist or could not be opened\nExiting!";
        return;
    }
    while (!file_obj.eof())
    {
        if (index < 2000)
        {
            file_obj.get(BUFFER[index]);
            index++;
        }
        else
        {
            sendto(sockfd, BUFFER, index, 0, (struct sockaddr *)&addr_con, addr_len);
            memset(BUFFER, '\0', 2000);
            index = 0;
        }
    }
    sendto(sockfd, BUFFER, 2000, 0, (struct sockaddr *)&addr_con, addr_len);
    memset(BUFFER, '\0', 2000);
}

int main(int argc, char **argv)
{
    int PORT_NO, socket_descriptor, index, nBytes;
    char BUFFER[2000] = "\0";
    char FILE_NAME[2000] = "\0";
    struct sockaddr_in addr_con;
    socklen_t addr_len = sizeof(addr_con);
    std::fstream file_obj;
    if (argc < 2)
    {
        std::cout << "Provide port no. in commandline args!\n";
        exit(1);
    }
    PORT_NO = std::stoi(argv[1]);
    addr_con.sin_family = AF_INET;
    addr_con.sin_port = htons(PORT_NO);
    addr_con.sin_addr.s_addr = INADDR_ANY;

    socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_descriptor == -1)
    {
        std::cout << "socket file descriptor not received\nExiting!";
        exit(1);
    }
    if (bind(socket_descriptor, (struct sockaddr *)&addr_con, sizeof(addr_con)))
    {
        std::cout << "Socket binding failed";
        exit(1);
    }
    while (true)
    {
        std::cout << "waiting for file name" << std::endl;
        bzero(FILE_NAME, 2000);
        nBytes = recvfrom(socket_descriptor, FILE_NAME, 32, 0, (struct sockaddr *)&addr_con, &addr_len);

        // int nBytes = recvfrom(socket_descriptor, FILE_NAME, 32, 0, (struct sockaddr *)&addr_con, &addr_len);
        if (!strcmp(FILE_NAME, "Done"))
        {
            break;
        }
        sendFile(FILE_NAME, BUFFER, socket_descriptor, addr_con, addr_len);
        std::cout << "File transferred!\n";
    }
    std::cout << "File transfer completed.\nClosing socket!";
    close(socket_descriptor);
    return 0;
}