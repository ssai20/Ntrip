#include <iostream>
#include <stdio.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>

using namespace std;
#define SIZE 1000
#define PORT 2101


string base64Encoder(string input_str, int len_str) {
    
    char char_set[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    char *res_str = (char *) malloc(SIZE * sizeof(char));
    
    int index, no_of_bits = 0, padding = 0, val = 0, count = 0, temp;
    int i, j, k = 0;
    
    for (i = 0; i < len_str; i += 3) {
        val = 0, count = 0, no_of_bits = 0;
        
        for (j = i; j < len_str && j <= i + 2; j++) {
            val = val << 8;
            val = val | input_str[j];
            count++;
        }
        
        no_of_bits = count * 8;
        padding = no_of_bits % 3;
        
        while (no_of_bits != 0) {
            if (no_of_bits >= 6) {
                temp = no_of_bits - 6;
                
                index = (val >> temp) & 63;
                no_of_bits -= 6;
            } else {
                temp = 6 - no_of_bits;
                
                // append zeros to right if bits are less than 6
                index = (val << temp) & 63;
                no_of_bits = 0;
            }
            res_str[k++] = char_set[index];
        }
    }
    
    for (i = 1; i <= padding; i++) {
        res_str[k++] = '=';
    }
    
    res_str[k] = '\0';
    string a = res_str;
    return a;
    
}


int main() {
    
    addrinfo hints;
    addrinfo* addrResult = NULL;
    
    int connectSocket = 1;
    int result;
    int BUFFER_BYTES = 2048;
    
    char recvBuffer[BUFFER_BYTES];
    std::string myString;

    
    //    cout<<"Enter mountpoint, please:"<<endl;
    //    string mountpoint;
    //    cin>>mountpoint;
    //    cout<<endl;
    string mountpoint = "Agviser";
    //    bie001
    //    AlabamaSylacauga
    //    ASKJA
    //    DMBeardServices
    //    AGSSIAAP
    //    agtekrtk
    string input_str = ":";
    int len_str;
    
    len_str = input_str.length();
    string pwd = base64Encoder(input_str, len_str);
    string header = "GET /" + mountpoint + " HTTP/1.0\r\n" + \
    "User-Agent: NTRIP u-blox\r\n" + \
    "Accept: */*\r\n" + \
    "Authorization: Basic " + pwd + "\r\n" + \
    "Connection: close\r\n\r\n";
    struct hostent *h;
    if ((h = gethostbyname("www.rtk2go.com")) == NULL) { // Lookup the hostname
        cout << "cannot look up hostname" << endl;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    
    result = getaddrinfo("3.23.52.207", "2101", &hints, &addrResult);
    if (result != 0)
    {
        cout<<"getaaddrinfo failed! with error: "<<result<<endl;
        return 1;
    }
    
    connectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (connectSocket < 0)
    {
        cout<<"socket creation failed"<<endl;
        freeaddrinfo(addrResult);
        return 1;
    }
    
    result = connect(connectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result < 0)
    {
        cout<< "Unable connect to NtripCaster"<<endl;;
        close(connectSocket);
        connectSocket = 1;
        freeaddrinfo(addrResult);
        return 1;
    }
    
    char sendBuffer[header.length() + 1];
    strcpy(sendBuffer, header.c_str());
    result = send(connectSocket, sendBuffer, strlen(sendBuffer), 0);
    if (result < 0)
    {
        cout<<"sent failed, error: "<<result<<endl;
        close(connectSocket);
        freeaddrinfo(addrResult);
        return 1;
    }
    
    cout<<"Sent: "<<result<<" bytes"<<endl;
    
    sockaddr_in info;
    int infoLenth = sizeof(info);
    //int nData = recvfrom(connectSocket, recvBuffer, BUFFER_BYTES, 0, (struct sockaddr*)&info, &infoLenth);
    do
    {
        //        memset(recvBuffer, 0, BUFFER_BYTES);
        result = recv(connectSocket, recvBuffer, BUFFER_BYTES, 0);
        if (result > 0)
        {
            cout<<"Recieved "<<result<<" bytes"<<endl;
            cout<<"Recieved data "<<recvBuffer<<endl;
            //myString.append(recvBuffer, result);
            //            if (result == BUFFER_BYTES) continue;
        }
        else if (result == 0)
        {
            cout<<"Connection closed"<<endl;
        }
        else
        {
            cout<<"recv failed with error"<<endl;
        }
    } while(result>0);
//    std::cout << myString << "\n";

    close(connectSocket);
    freeaddrinfo(addrResult);
    return 0;
}

