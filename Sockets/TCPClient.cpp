#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

using namespace std;

class TCPSocket
{
    int sock_descriptor, port_no;
    struct sockaddr_in server_addr;
    socklen_t len;
    char *ip_address;
    fstream fobj;

public:
    TCPSocket(char *ip_address, int port_no)
    {
        this->port_no = port_no;
        this->ip_address = ip_address;
        init();
    }

    ~TCPSocket()
    {
        cout << "Closing socket\n";
        close(sock_descriptor);
    }
    void init()
    {
        sock_descriptor = socket(AF_INET, SOCK_STREAM, 0);
        cout << "Spinning up a socket connection....\n";
        if (sock_descriptor < 0)
        {
            cout << "Socket creation failed!\n";
            exit(-1);
        }
        server_addr.sin_port = htons(this->port_no);
        server_addr.sin_addr.s_addr = inet_addr(this->ip_address);
        server_addr.sin_family = AF_INET;
        len = sizeof(server_addr);
        cout << "Connecting to server....\n";
        int connect_result = connect(sock_descriptor, (sockaddr *)&server_addr, len);
        if (connect_result == 0)
        {
            cout << "Socket connection successful";
        }
        else
        {
            exit(-2);
        }
    }

    void chat()
    {
        char intent[4] = {'c', 'h', 'a', 't'};
        send(sock_descriptor, intent, 4, 0);
        string message;
        cout << "\ntype message:";
        cin.ignore();
        getline(cin, message);
        char *send_buffer = new char[message.size() + 1];
        strcpy(send_buffer, message.c_str());
        fflush(stdin);
        char *recv_buffer = new char[256];
        send(sock_descriptor, send_buffer, message.size(), 0);
        recv(sock_descriptor, recv_buffer, 256, 0);
        cout << "\nServer: " << recv_buffer;
        delete send_buffer;
        delete recv_buffer;
    }

    void fileTransfer()
    {
        char intent[4] = {'f', 'i', 'l', 'e'};
        send(sock_descriptor, intent, 4, 0);

        cout << "Enter name of file you want:";
        cin.ignore();
        string fileName;
        getline(cin, fileName);
        char send_buffer[fileName.size()];
        strcpy(send_buffer, fileName.c_str());
        send(sock_descriptor, send_buffer, fileName.size(), 0);

        cout << "Recieving file size from server\n";
        char filesize[100];
        recv(sock_descriptor, filesize, sizeof(filesize), 0);
        string size(filesize);
        int fileSize = stoi(size);
        cout << "file Size:\n"
             << fileSize << "\n";
        char *recv_buffer = new char[fileSize];
        recv(sock_descriptor, recv_buffer, fileSize, 0);
        fobj.open("copied" + fileName, ios::out | ios::binary);
        if (fobj)
        {
            fobj.write(recv_buffer, fileSize);
            fobj.close();
        }
        else
        {
            cout << "new file could not be opened";
        }
    }

    void calculate()
    {
        send(sock_descriptor, "math", 4, 0);
        cout << "Enter expression:\n";
        cin.ignore();
        string expression;
        getline(cin, expression);
        send(sock_descriptor, expression.c_str(), expression.size(), 0);
        char *recv_buffer = new char[100];
        recv(sock_descriptor, recv_buffer, 100, 0);
        cout << "Answer is :" << recv_buffer << endl;
    }
};

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        cout << "Enter port ip and port in commandline";
        exit(-1);
    }
    TCPSocket t_socket(argv[1], stoi(argv[2]));
    while (true)
    {
        cout << "1.Chat\n2.File Transfer\n3.Arithmatic calculation\n4.Exit\n";
        char choice;
        cin >> choice;
        switch (choice)
        {
        case '1':
            t_socket.chat();
            break;
        case '2':
            t_socket.fileTransfer();
            break;
        case '3':
            t_socket.calculate();
            break;
        case '4':
            cout << "Goodbye!\n";
            exit(0);
        default:
            cout << "Invalid choice\n";
            break;
        }
    }
}