#include <iostream>
#include <cstring>
#include <math.h>

using namespace std;

class ErrorDetectionReceiver
{
protected:
    string message, encodedMessage;
    int tcp_sockfd, udp_sockfd;

public:
    void acceptString();
    virtual void checkForErrors(){};
    virtual void decodeMessage(){};
    void sendAck();
};

class Hamming : public ErrorDetectionReceiver
{
    int numDataBits = 0, numRedundantBits = 0;
    string parityCheck;

public:
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
                this->parityCheck.append("0");
            }
            else
            {
                this->parityCheck.append("1");
            }
        }
    }

    int binaryToInt()
    {
        int index = 0;
        for (int i = 0; i < this->parityCheck.size(); i++)
        {
            if (this->parityCheck[i] == '1')
            {
                index += pow(2, i);
            }
        }
        return index;
    }

    void decodeMessage()
    {
    }
    void correctError(int index)
    {
        if (index)
        {
            cout << "Error at index: " << index << "\nCorrecting error!\n";
            if (this->encodedMessage[this->encodedMessage.size() - index] == '1')
            {
                this->encodedMessage[this->encodedMessage.size() - index] = '0';
            }
            else
            {
                this->encodedMessage[this->encodedMessage.size() - index] = '1';
            }
        }
        else
        {
            cout << "No error!\n";
            cout << "Received word:  " << this->encodedMessage;
        }
    }

    void checkForErrors()
    {
        calcParityBits();
        correctError(binaryToInt());
    }
};

class CRC : public ErrorDetectionReceiver
{
    string polynomial, temp;
    // message, encodedMessage
public:
    void acceptPolynomial()
    {
        this->temp = this->encodedMessage;
        cout << "Enter polynomial for division:\n";
        getline(cin, this->polynomial);
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
            if (temp[i] == '0')
            {
                temp.erase(0, 1);
            }
            else
            {
                break;
            }
        } while (i < temp.size() - 1);
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
    void decodeMessage();
    void checkForErrors()
    {
        acceptPolynomial();
        calcRemainder();
        if (temp.size())
        {
            cout << "Error in received code, remainder not zero";
        }
        else
        {
            cout << "No error detected, remainder is zero!\n";
            cout << "Recieved word is: " << this->encodedMessage;
        }
    }
};

int main()
{
    Hamming hamming;
    CRC crc;
    ErrorDetectionReceiver *edr;
    while (true)
    {
        cout << "1.Hamming\n2.CRC\n3.Exit";
        char choice;
        cin >> choice;
        switch (choice)
        {
        case '1':
            edr = &hamming;
            break;
        case '2':
            edr = &crc;
            break;
        case '3':
            cout << "Exiting";
            exit(0);
        default:
            cout << "Invalid choice!\n";
        }
        edr->acceptString();
        edr->checkForErrors();
        // edr->sendAck();
        // edr->decodeMessage();
    }
}