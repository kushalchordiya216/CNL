#include <iostream>
#include <string>
#include <cstring>
#include <math.h>
#include <algorithm>
using namespace std;

class ErrorDetectionSender
{
protected:
    string message, encodedMessage;
    int tcp_sockfd, udp_sockfd;

public:
    virtual void encodeMessage(){};
    string acceptString()
    {
        cin.ignore();
        cout << "Enter message string:\n";
        getline(cin, this->message);
        return this->message;
    }
    void printEncodedString()
    {
        cout << "Encoded output is: \n";
        cout << this->encodedMessage << "\n";
    }
};

class Hamming : public ErrorDetectionSender
{
    // string message, encodedMessage;
    int numDataBits = 0, numRedundantBits = 0;

public:
    void calcRedundantBits()
    {
        this->numDataBits = this->message.size();
        do
        {
            if (this->numDataBits + this->numRedundantBits + 1 <= pow(2, this->numRedundantBits))
            {
                break;
            }
            this->numRedundantBits++;
        } while (true);
    }

    void placeRedundantBits()
    {
        string temp = this->message;
        reverse(temp.begin(), temp.end());
        for (int i = 1, j = 0, k = 0; i <= this->numDataBits + this->numRedundantBits; i++)
        {
            if (i == pow(2, k))
            {
                this->encodedMessage.insert(0, "b");
                k++;
            }
            else
            {
                this->encodedMessage.insert(0, temp.substr(j, 1));
                j++;
            }
        }
    }

    void calcParityBits()
    {
        int totalLength = this->encodedMessage.size();
        for (int i = 0; i < this->numRedundantBits; i++)
        {
            int index = pow(2, i);
            bool parityFlag = false;
            for (int j = index; j <= totalLength; j++)
            {
                if ((this->encodedMessage[totalLength - j] == '1') && (index & j))
                {
                    parityFlag = !parityFlag;
                }
            }
            if (parityFlag)
            {
                this->encodedMessage[totalLength - index] = '1';
            }
            else
            {
                this->encodedMessage[totalLength - index] = '0';
            }
        }
    }

    void encodeMessage()
    {
        calcRedundantBits();
        placeRedundantBits();
        calcParityBits();
    }
};

class CRC : public ErrorDetectionSender
{
    string polynomial, redundantBlock, temp;
    // message, encodedMessage
public:
    void appendRedundantBits()
    {
        cout << "Enter polynomial with which crc is to calulated:\n";
        cin >> this->polynomial;
        for (int i = 1; i < this->polynomial.size(); i++)
        {
            this->redundantBlock.append("0");
        }
        this->temp = this->message + redundantBlock;
        this->encodedMessage = this->temp;
    }

    string XOR(string s1, string s2)
    {
        for (int i = 0; i < s1.size(); i++)
        {
            if (s1[i] == s2[i])
            {
                s2[i] = '0';
            }
            else
            {
                s2[i] = '1';
            }
        }
        return s2;
    }

    void truncateZeros()
    {
        int i = 0;
        do
        {
            if (this->temp[i] == '0')
            {
                this->temp.erase(0, 1);
            }
            else
            {
                break;
            }
        } while (i < this->temp.size() - 1);
    }

    string calcRemainder()
    {
        int i = 0;
        do
        {
            if (temp[i] == '1')
            {
                string result = XOR(this->temp.substr(i, this->polynomial.size()), this->polynomial);
                temp.replace(i, this->polynomial.size(), result);
            }
            i++;
        } while (i <= temp.size() - this->polynomial.size());
        truncateZeros();
    }

    void substituteRemainder()
    {
        this->encodedMessage.replace(this->encodedMessage.size() - this->temp.size(), this->temp.size(), this->temp);
    }

    void encodeMessage()
    {
        appendRedundantBits();
        calcRemainder();
        substituteRemainder();
    }
};

int main()
{
    do
    {
        cout << "1.Hamming error detection\n2.CRC error detection\n3.Exit\n";
        char choice;
        cin >> choice;
        Hamming hamming;
        CRC crc;
        ErrorDetectionSender *eds;
        switch (choice)
        {
        case '1':
            eds = &hamming;
            break;
        case '2':
            eds = &crc;
            break;
        case '3':
            cout << "Exiting";
            exit(0);
        default:
            cout << "Invalid option\nDefaulting to hamming";
        }
        eds->acceptString();
        eds->encodeMessage();
        eds->printEncodedString();
    } while (true);
}