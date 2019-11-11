// Zeyu Zhang/Mitul Magu
// 02/23/2019
// Project1

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;
const int PORT = 8080;

int send_file(const string& filename, int sock);
int recv_file(const string& filename, int sock);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Error argc less then 3!\n"
                  << "Usage: ./client_run <command> <filename>/<rpc_function>\n"
                  << "Upload a file ./client_ext upload filename\n"
                  << "Download a file ./client_ext download filename\n"
                  << "Delete a file ./client_ext delete filename\n"
                  << "Rename a file ./client_ext rename filename1 filename2\n"
                  << std::endl;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cout << "Socket creation error." << std::endl;
        return -1;
    }

    struct hostent *host;
    host = gethostbyname("127.0.0.1");

    struct sockaddr_in host_addr;
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = *(uint32_t *)host->h_addr;

    char str[32];
    const char *ip = inet_ntop(host->h_addrtype, &host_addr.sin_addr, str, sizeof(str));
    cout << "Host Address: " << str << endl;

    if (connect(sock, (struct sockaddr *)&host_addr, sizeof(host_addr)) < 0) {
        cout << "\nConnection failed.\n" << endl;
        return -1;
    }

    //  Decode message type
    string command = string(argv[1]);
    string send_str = command + " " + string(argv[2]);
    if (command == "upload") {
        send(sock , send_str.c_str(), send_str.size(), 0);
        cout << "send command to server: " << command << endl;

        if (send_file(string(argv[2]), sock) == 0)
            cout << "Upload file: " << string(argv[2]) << endl;

        return 0;
    } else if (command == "download") {
        send(sock , send_str.c_str(), send_str.size(), 0);
        cout << "send command to server: " << command << endl;

        if (recv_file(string(argv[2]), sock) == 0)
            cout << "Download file: " << string(argv[2]) << endl;

        return 0;
    } else if (command == "rename") {
        send_str += " " + string(argv[3]);
    } else if (command == "rpc") {
        string rpc_command = string(argv[2]);
        if (rpc_command == "add") {
            int i,j;
            cout << "Enter two integers to add: ";
            cin >> i >> j;
            send_str += " " + to_string(i) + " " + to_string(j);
        } else if (rpc_command == "sort") {
            string arr;
            cout << "Enter an integer array to sort: ";
            getline(cin,arr);
            send_str += " " + arr;
        } else if (rpc_command == "matrix_multiply") {
            int p, q, r;
            cout << "Enter #row and #column of matrix A and #column of matrix B: ";
            cin >> p >> q >> r;
            send_str += " " + to_string(p) + " " + to_string(q) + " " + to_string(r);

            for (int i=0; i<p; i++) {
                cout << "Enter " << i+1 << " row of matrix A(" << q << " numbers): ";
                for (int j=0; j<q; j++) {
                    double n;
                    cin >> n;
                    send_str += " " + to_string(n);
                }
            }
            for (int i=0; i<q; i++) {
                cout << "Enter " << i+1 << " row of matrix B(" << r << " numbers): ";
                for (int j=0; j<r; j++) {
                    double n;
                    cin >> n;
                    send_str += " " + to_string(n);
                }
            }

        }
    }


    // Send command to server
    send(sock , send_str.c_str(), send_str.size(), 0);
    cout << "send command to server: " << command << endl;

    // Receive the result from server
    char buffer[4096] = {0};
    recv(sock, buffer, sizeof(buffer), 0);
    cout << buffer << endl;





  return 0;
}


int send_file(const string& filename, int sock) {
    ifstream ifs(filename, ifstream::in | ifstream::binary);
    // Find length of file:
    ifs.seekg(0, ifs.end);
    size_t length = ifs.tellg();
    ifs.seekg(0, ios::beg);
    if (!ifs.good()) {
        cout << "File doesn't exist: " << filename << endl;
        return 1;
    }

    char buffer[1024] = {0};
    while (!ifs.eof() && length > 0) {
        size_t len = min(length, size_t(sizeof(buffer)));
        ifs.read(buffer, len);
        send(sock, buffer, len, 0);
        length -= len;
    }
    return 0;
}


int recv_file(const string& filename, int sock) {
    ofstream ofs;
    ofs.open(filename, ofstream::binary | ofstream::trunc);
    char buffer[1024] = {0};
    size_t length = 0;
    if (ofs.is_open()) {
        //  clear buffer
        memset(buffer, 0, sizeof(buffer));
        while ((length = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
            //  write data
            ofs.write(buffer, length);
            if (length < sizeof(buffer)) {
                break;
            }
        }
        cout << "Save file: " << filename << endl;
        ofs.close();
    } else {
        cout << "Failed to save file: " << filename << endl;
        return 1;
    }
    return 0;
}
