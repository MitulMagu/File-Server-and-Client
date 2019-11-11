// Zeyu Zhang/Mitul Magu
// 02/23/2019
// Project1

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;



void split_commands(const string& s, vector<string>& v);
string rpc(vector<string>& commands);
void serving(int sock);

int send_file(const string& filename, int sock);
int recv_file(const string& filename, int sock);

const int PORT = 8080;

int main(int argc, char *argv[]) {
  if(argc != 1) {
    cerr << "Usage: " << argv[0] << " SRC DEST\n";
    return 1;
  }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        cerr << "Failed socket" << endl;
        exit(1);
    }

    struct sockaddr_in server_addr, client_addr;
    int addrlen = sizeof(server_addr);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Failed to bind" << endl;
        exit(1);
    }

    listen(sockfd, 5);

    char serveraddr_buf[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &server_addr.sin_addr, serveraddr_buf, INET_ADDRSTRLEN);
    cout << "Server Address: " << serveraddr_buf << endl;

    int clientfd = 0;
    while (1) {
        clientfd = accept(sockfd, (struct sockaddr*)&client_addr, (socklen_t*)&addrlen);
        if (clientfd < 0) {
            cerr << "Failed to accept client" << endl;
            exit(1);
        }

        serving(clientfd);
    }

  return 0;
}





void serving(int sock) {
    char recv_buff[4096] = {0};
    vector<string> commands;

    recv(sock, recv_buff, sizeof(recv_buff), 0);
    string recv_str(recv_buff);
    cout << "Message from client: " << recv_str << endl;


    string msg_send;
    split_commands(recv_str, commands);

    if(commands[0] == "upload") {
        if (recv_file(commands[1], sock) == 0)
            cout << "Receive file: " << commands[1] << endl;

        return;
    } else if(commands[0] == "download") {
        if (send_file(commands[1], sock) == 0)
            cout << "Send file: " << commands[1] << endl;

        return;
    } else if(commands[0] == "rename") {
        if(commands.size() < 3) {
            cerr << "3 commands needed to rename a file." << endl;
            exit(1);
        }
        if(!rename(commands[1].c_str(), commands[2].c_str())) {
            cout << "Rename " << commands[1] << " to " << commands[2] << " successfully" << endl;
        } else {
            cerr << "Failed to rename" << endl;
        }
    } else if(commands[0] == "delete") {
        if(!remove(commands[1].c_str())) {
            cout << "Delete " << commands[1] << " successfully" << endl;
        } else {
            cerr << "Failed to delete" << endl;
        }
    } else if(commands[0] == "rpc") {
        msg_send = rpc(commands);
    } else {
        cerr << "Bad command!" << endl;
    }

    send(sock, msg_send.c_str(), msg_send.size(), 0);
}


void split_commands(const string& s, vector<string>& v) {
    string::size_type pos1, pos2;
    pos2 = s.find(" ");
    pos1 = 0;
    while(string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2-pos1));

        pos1 = pos2 + 1;
        pos2 = s.find(" ", pos1);
    }
    if(pos1 != s.length())
        v.push_back(s.substr(pos1));
}

string rpc(vector<string>& commands) {
    string s = "";
    if(commands[1] == "calculate_pi") {
        int m = 1, f = 1;
        double pi = 0, cur = 1.0/m;
        do {
            pi += cur*f*4;
            f = -f;
            m += 2;
            cur = 1.0/m;
        } while(fabs(cur) >= 0.0000001);
        s += "pi=" + to_string(pi);
    } else if(commands[1] == "add") {
        int a = atoi(commands[2].c_str()) + atoi(commands[3].c_str());
        s += "i+j=" + to_string(a);
    } else if(commands[1] == "sort") {
        int n = commands.size()-2;
        int arr[n];
        for (int i=2; i<n+2; i++) {
            arr[i-2] = atoi(commands[i].c_str());
        }
        sort(arr, arr+n);
        s += "Sorted array:";
        for (int i=0; i<n; i++) {
            s += " " + to_string(arr[i]);
        }
    } else if(commands[1] == "matrix_multiply") {
        int p = atoi(commands[2].c_str());
        int q = atoi(commands[3].c_str());
        int r = atoi(commands[4].c_str());
        vector<vector<double>> matrixA;
        vector<vector<double>> matrixB;
        for (int i=0; i<p; i++) {
            vector<double> v;
            for (int j=0; j<q; j++) {
                v.push_back(atof(commands[5+q*i+j].c_str()));
            }
            matrixA.push_back(v);
        }
        int c = 5+p*q;
        for (int i=0; i<q; i++) {
            vector<double> v;
            for (int j=0; j<r; j++) {
                v.push_back(atof(commands[c+r*i+j].c_str()));
            }
            matrixB.push_back(v);
        }

        double matrixC[p][r] = {0};
        for (int i=0; i<p; i++) {
            for (int j=0; j<r; j++) {
                for (int k=0; k<q; k++) {
                    matrixC[i][j] += matrixA[i][k]*matrixB[k][j];
                }
            }
        }
        s += "Matrix C:\n";
        for (int i=0; i<p; i++) {
            for (int j=0; j<r; j++) {
                s += to_string(matrixC[i][j]) + " ";
            }
            s += "\n";
        }
    } else {
        s += "BAD rpc command!\n";
    }
    return s;
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
