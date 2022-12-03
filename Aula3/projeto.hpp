//projeto.hpp
#include <cekeikon.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

class SERVER 
{
    const char* PORT="3490"; // the port users will be connecting to
    const int BACKLOG=1; // how many pending connections queue will hold
    int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    static void *get_in_addr(struct sockaddr *sa) 
    {
        if (sa->sa_family == AF_INET) 
        {
            return &(((struct sockaddr_in*)sa)->sin_addr);
        }      
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }

    public:
    SERVER() 
    {
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE; // use my IP

        if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) 
        {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        }

        // loop through all the results and bind to the first we can
        for(p = servinfo; p != NULL; p = p->ai_next) 
        {
            if ((sockfd = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1) 
            {
                perror("server: socket");
            continue;
            }

            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) 
            {
                perror("setsockopt");
                exit(1);
            }
            if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
            {
                close(sockfd);
                perror("server: bind");
                continue;
            }
            break;
        }

        freeaddrinfo(servinfo); // all done with this structure
        if (p == NULL) 
        {
            fprintf(stderr, "server: failed to bind\n");
            exit(1);
        }
        if (listen(sockfd, BACKLOG) == -1) 
        {
            perror("listen");
            exit(1);
        }
    }

    ~SERVER() 
    {
        close(new_fd);
    }

    void waitConnection() 
    {
        printf("server: waiting for connections...\n");
        while (1) 
        {
            sin_size = sizeof their_addr;
            new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        
            if (new_fd == -1) 
            {
                perror("accept");
                continue;
            } else break;
        }

        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);
        close(sockfd); // doesn't need the listener anymore
    }

    void sendBytes(int nBytesToSend, const BYTE *buf) 
    {
        while(nBytesToSend > 0)
        {
            if (send(new_fd, &buf[nBytesToSend-1], 1, 0) == -1) perror("send");
            nBytesToSend--;
        }
        
    }

    void receiveBytes(int nBytesToReceive, BYTE *buf) 
    {
        while (nBytesToReceive > 0)
        {
            if (recv(new_fd, &buf[nBytesToReceive-1], 1, 0) == -1) perror("recv");
            nBytesToReceive--;
        }
        
    }

    void sendUint(uint32_t m)
    {
        if (send(new_fd, (BYTE*)&m, 4, 0) == -1) perror("send");
    }

    void receiveUint(uint32_t& m)
    {
        if (recv(new_fd, (BYTE*)&m, 4, 0) == -1) perror("recv");
        printf("server: received '%d'\n", m);
    }

    void sendVb(const vector<BYTE>& vb)
    {

        cout << vb.size() << endl;

        int size = vb.size();

        if (send(new_fd, (BYTE*)&size, 4, 0) == -1) perror("sendsize");

        sendBytes(size, vb.data());
        
    }

    void receiveVb(vector<BYTE>& st)
    {
        int size;

        if (recv(new_fd, (BYTE*)&size, 4, 0) == -1) perror("recvsize");

        st.resize(size);

        receiveBytes(size, st.data());
        
        //printf("server: received '%s'\n", st);
    }

}; //Fim de SERVER

class CLIENT 
{
    const char* PORT="3490"; // the port client will be connecting to
    int sockfd, numbytes;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    static void *get_in_addr(struct sockaddr *sa) 
    {
        if (sa->sa_family == AF_INET) 
        {
            return &(((struct sockaddr_in*)sa)->sin_addr);
        }      
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }
    
    public:
    CLIENT(char* endereco) 
    {
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        if ((rv = getaddrinfo(endereco, PORT, &hints, &servinfo)) != 0) 
        {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        }

        // loop through all the results and connect to the first we can
        for(p = servinfo; p != NULL; p = p->ai_next)   
        {
            if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
            {
                perror("client: socket");
                continue;
            }

            if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
            {
                perror("client: connect");
                close(sockfd);
                continue;
            }
            break;
        }

        if (p == NULL) 
        {
            fprintf(stderr, "client: failed to connect\n");
        }

        inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
        printf("client: connecting to %s\n", s);

        freeaddrinfo(servinfo); // all done with this structure
    }

    ~CLIENT() 
    {
        close(sockfd);
    }

    void sendBytes(int nBytesToSend, const BYTE *buf) 
    {
        while(nBytesToSend > 0)
        {
            if (send(sockfd, &buf[nBytesToSend-1], 1, 0) == -1) perror("send");
            nBytesToSend--;
        }
        
    }

    void receiveBytes(int nBytesToReceive, BYTE *buf) 
    {
        while (nBytesToReceive > 0)
        {
            if (recv(sockfd, &buf[nBytesToReceive-1], 1, 0) == -1) perror("recv");
            nBytesToReceive--;
        }
        
    }

    void sendUint(uint32_t m)
    {
        if (send(sockfd, (BYTE*)&m, 4, 0) == -1) perror("send");
    }

    void receiveUint(uint32_t& m)
    {
        if (recv(sockfd, (BYTE*)&m, 4, 0) == -1) perror("recv");
        printf("server: received '%d'\n", m);
    }

    void sendVb(const vector<BYTE>& vb)
    {
        int size = vb.size();

        if (send(sockfd, (BYTE*)&size, 4, 0) == -1) perror("sendsize");

        sendBytes(size, vb.data());
        
    }

    void receiveVb(vector<BYTE>& st)
    {
        int size;

        if (recv(sockfd, (BYTE*)&size, 4, 0) == -1) perror("recvsize");

        cout << size << endl;

        st.resize(size);

        receiveBytes(size, st.data());
        
        //printf("server: received '%s'\n", st);
    }

}; //Fim de CLIENT


bool testaBytes(BYTE* buf, BYTE b, int n) {
    //Testa se n bytes da memoria buf possuem valor b
    bool igual=true;
    for (unsigned i=0; i<n; i++)
        if (buf[i]!=b) { igual=false; break; }
    return igual;
}


bool testaVb(const vector<BYTE> vb, BYTE b) 
{
    //Testa se todos os bytes de vb possuem valor b
    bool igual=true;
    for (unsigned i=0; i<vb.size(); i++)
        if (vb[i]!=b) { igual=false; break; }
    return igual;
}