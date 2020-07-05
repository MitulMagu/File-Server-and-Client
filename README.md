# Server and Client for File Transfer and Remote Computation

Developers: Mitul Magu, Zeyu Zhang

## Detailed Instructions to Execute Code
1. Install a c++ compiler in your machine. We use g++ and it works fine.
2. Once the compiler has been installed for c++ code, change the directory to where the file belongs
3. As an example, if the Server code belongs in the Codes folder in your Desktop, type the command: cd Desktop/Codes 
4. Once the directory is shown in your command line, enter the command: g++ -std=c++11 -o ser server.cpp
5. Press enter and type the name of executable: ./ser
6. This will execute the server program once you press enter. The server will wait for any instructions from the client side.
7. Repeat the procedure for the client code and while entering the name of the executable in step 5, include the arguments to be sent to the server side.

## Working with Docker

1. Load client image: docker load -i p1-client.tar
2. Load server image: docker load -i p1-server.tar
3. Run client image with host network config: docker run -it --network=host client:3.0 /bin/bash
4. Run server image with host network config: docker run -it --network=host server:3.0 /bin/bash
5. Go to clieng/server directory respectively: cd client/server
6. Run the server program first: ./ser
7. Run the client program with proper arguments: ./cli <command> <filename/rpc_function> [options]

## Sending Instructions from the Client to the Server
1. Typing ./cli upload <filename> will upload a file currently on the client folder to the server folder.
2. Typing ./cli download <filename> will download a file currently on the server folder to the client folder.
3. Typing ./cli delete <filename> will delete the file specified currently on the server folder.
4. Typing ./cli rename <filename1> <filename2> will rename a file specified in <filename1> on the server folder to the name specified in <filename2>.
5. To execute one of the rpc commands, type rpc after the name of the executable and then follow it with the specific command.
6. Typing ./cli rpc calculate_pi will return the value of pi from the server.
7. Typing ./cli rpc add will prompt the user to enter two numbers. Simply type the two number with space between them and the result will be sent by the server on pressing enter.
8. Typing ./cli rpc sort will prompt the user to enter an array. Simply type the array elements with spaces between them and the sorted array will be sent by the server on pressing enter.
9. Typing ./cli rpc matrix_multiply will prompt the user to enter the number of rows and columns in matrix A and the number of columns in matrix B. Enter the elements for both the matrices one by one and the resulting C matrix (multiplication of matrix A and matrix B) will be sent from the server to the client.
  
More information can be found here: https://mitulmagu.wordpress.com/2019/11/11/file-server-and-client/
