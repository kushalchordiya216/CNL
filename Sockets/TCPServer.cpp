#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <math.h>
#include <string.h>
using namespace std;

class TCPSocket
{
    int sock_descriptor, client_sock, port_no;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(server_addr), client_len = sizeof(client_addr);
    char *ip_address;
    fstream fobj;

public:
    TCPSocket(int port_no)
    {
        this->port_no = port_no;
        init();
    }
    ~TCPSocket()
    {
        cout << "Closing socket\n";
        close(sock_descriptor);
    }
    void init()
    {
        cout << "Creating socket ....\n";
        sock_descriptor = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_descriptor < 0)
        {
            cout << "Socket creation failed!\n";
            exit(-1);
        }
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(this->port_no);
        len = sizeof(server_addr);
        cout << "Binding socket....\n";
        int bind_result = bind(sock_descriptor, (struct sockaddr *)&server_addr, len);

        if (bind_result < 0)
        {
            cout << "Socket binding failed";
            exit(-2);
        }

        cout << "Listening for socket connections....\n";

        if (listen(sock_descriptor, 10) < 0)
        {
            cout << "Error listening on port\n";
            exit(-3);
        }
        this->client_sock = accept(this->sock_descriptor, (struct sockaddr *)&client_addr, &client_len);
        if (this->client_sock < 0)
        {
            cout << "Error establishing connection with client\n";
            exit(-4);
        }
        else
        {
            cout << "Connection succesfully established\n";
        }
    }

    void chat()
    {
        string message;
        char *recv_buffer = new char[256];
        recv(client_sock, recv_buffer, 256, 0);
        cout << "Client: " << recv_buffer;
        cout << "\ntype message:";
        getline(cin, message);
        char *send_buffer = new char[256];
        strcpy(send_buffer, message.c_str());
        fflush(stdin);
        send(client_sock, send_buffer, message.size(), 0);
        delete recv_buffer;
        delete send_buffer;
    }

    void fileTransfer()
    {
        char *recv_buffer = new char[256];
        cout << "Waiting for file name from client....\n";
        recv(client_sock, recv_buffer, 256, 0);
        fobj.open(recv_buffer, ios::in | ios::binary);
        if (!fobj)
        {
            cout << "Error opening file\n";
            return;
        }

        long long int begin = fobj.tellg();
        fobj.seekg(0, ios::end);
        long long int end = fobj.tellg();
        long long int file_len = (end - begin);
        cout << "Sending file length to client....\n";
        string filesize = to_string(file_len);
        char fileSizeArr[filesize.size()];
        strcpy(fileSizeArr, filesize.c_str());
        send(client_sock, fileSizeArr, sizeof(fileSizeArr), 0);

        cout << "Reading file into buffer....\n";
        char *send_buffer = new char[file_len];
        fobj.seekg(ios::beg);
        fobj.read(send_buffer, file_len);
        cout << "Sending file to client....\n";
        send(client_sock, send_buffer, file_len, 0);
        fobj.close();
    }

    void calculate()
    {
        char *recv_buffer = new char[100];
        recv(client_sock, recv_buffer, 100, 0);
        char *token;
        vector<char *> operators;
        vector<int> operands;
        bool parity = true;
        while ((token = strtok_r(recv_buffer, " ", &recv_buffer)))
        {
            if (parity)
                operands.push_back(stoi(token));
            else
                operators.push_back(token);
            parity = !parity;
        }
        int result = operands[0];
        for (int i = 1, j = 0; i < operands.size(); i++, j++)
        {
            switch (operators[j][0])
            {
            case '+':
                result += operands[i];
                break;
            case '-':
                result -= operands[i];
                break;
            case '/':
                result *= operands[i];
                break;
            case '*':
                result /= operands[i];
                break;
            }
        }
        send(client_sock, to_string(result).c_str(), to_string(result).size(), 0);
    }

    void receiveIntent()
    {
        char intent[4];
        recv(client_sock, intent, 4, 0);
        if (intent[0] == 'c')
        {
            chat();
        }
        else if (intent[0] == 'f')
        {
            fileTransfer();
        }
        else if (intent[0] == 'm')
        {
            calculate();
        }
    }
};

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cout << "Enter port in commandline args";
        exit(-1);
    }
    TCPSocket t_socket(stoi(argv[1]));
    char intent[4];
    while (true)
    {
        t_socket.receiveIntent();
        cout << "Job finished" << endl;
    }
}