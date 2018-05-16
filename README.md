# DNS Server

DNS client & server application implemented in C using _Douglas E. Comer_'s **[CNAI](https://netbook.cs.purdue.edu)** Socket API.


#### A DNS server in C? Why?  
The purpose of this project is to get a solid grasp on the fundamentals of the **Socket API**. Writing such an application in C gives a basic understanding on how the **client** - **server** architecture works and overall on how to use the Socket API to establish communication between client and server applications with the goal of information exchange between the two.

## Getting Started

#### Requirements
The applications are currently developed to run only on Linux environments. In order to compile your own copy you should have **gcc** and **make** installed. To do so, on your Linux terminal execute the following command:

    sudo apt-get install gcc make


#### Compiling
On your terminal, navigate to the project's folder and issue the **make** command:

    make

This will compile the source code files alongside with the required dependencies and produce two executables, **servDNS** i.e. the DNS server and **clDNS**, the DNS client application.

## Usage

#### DNS Server
To start the server application, type the following command on your terminal. This will execute the the DNS server that listens by default on port **20000**

    ./servDNS

#### DNS Client
To make DNS queries to the DNS server, on a _separate_ terminal window you should use the following command:

	./clDNS dns-server port hostname

where **dns-server** is the _IP address_ or _hostname_ of the computer that DNS server is running on, **port** is the port the DNS server is listening on, and **hostname** is the _domain name_ the user is trying to resolve into an _IP address_

#### DNS Query Example
Request

	./clDNS localhost 20000 github.com

Response

	192.30.253.112 LOCAL
