#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <cstdlib>

void receiveFile(int socket_descriptor, struct sockaddr_in addr_connection, socklen_t address_len, char *FILE_NAME, char *BUFFER)
{
    char *NEW_FILE_NAME = new char[std::strlen(FILE_NAME) + std::strlen("Copied")];
    std::strcpy(NEW_FILE_NAME, "Copied");
    std::strcat(NEW_FILE_NAME, FILE_NAME);
    std::fstream file_obj;
    file_obj.open(NEW_FILE_NAME, std::ios::out);
    while (file_obj)
    {
        std::cout << "waiting for data from server";
        recvfrom(socket_descriptor, BUFFER, 2000, 0, (struct sockaddr *)&addr_connection, &address_len);
        int index = 0;
        //file_obj.write(BUFFER, 2000);
        while (index < 2000)
        {
            if (BUFFER[index] == '\0')
            {
                std::cout << "Completed file transfer\n";
                file_obj.close();
                return;
            }
            file_obj.write(BUFFER + sizeof(char) * index, 1);
            index++;
        }
    }
}

int main(int argc, char **argv)
{
    int socket_descriptor, PORT_NO;
    struct sockaddr_in addr_connection
    {
    };
    socklen_t address_len = sizeof(addr_connection);
    char BUFFER[2000] = "\0", FILE_NAME[2000] = "\0";
    if (argc < 3)
    {
        std::cout << "Enter hostname and port no in Commandline arguments";
        exit(1);
    }
    addr_connection.sin_family = AF_INET;
    addr_connection.sin_port = htons(std::stoi(argv[2]));
    addr_connection.sin_addr.s_addr = inet_addr(argv[1]);
    socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_descriptor == -1)
    {
        std::cout << "Socket descriptor not returned\n";
        exit(1);
    }
    while (true)
    {
        std::cout << "Enter name of file to be requested\n";
        std::cin >> FILE_NAME;
        int len = std::strlen(FILE_NAME);
        sendto(socket_descriptor, FILE_NAME, 32, 0, (struct sockaddr *)&addr_connection, address_len);
        if (!std::strcmp(FILE_NAME, "Done"))
        {
            break;
        }
        std::cout << "Name sent\n";
        receiveFile(socket_descriptor, addr_connection, address_len, FILE_NAME, BUFFER);
    }
}
