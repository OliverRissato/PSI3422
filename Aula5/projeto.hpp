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
#include <cekeikon.h>

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 1   // how many pending connections queue will hold
#define MAXDATASIZE 1460 // max number of bytes we can get at once 

#define HALFSPEED 75
#define SPEED 90
#define TFOWARD 3200
#define TSTOP 1000
#define TTURN 1000

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

class DEVICE {
public:
    virtual void sendBytes(int nBytesToSend, BYTE *buf)=0;
    virtual void receiveBytes(int nBytesToReceive, BYTE *buf)=0;
    void sendUint(uint32_t m);
    void receiveUint(uint32_t& m);
    void sendVb(const vector<BYTE>& vb);
    void receiveVb(vector<BYTE>& vb);
    void sendImg(const Mat_<COR>& img);
    void receiveImg(Mat_<COR>& img);
    void receiveImgComp(Mat_<COR> &img);
    void sendImgComp(const Mat_<COR>& img);
    /*
    */
};


class SERVER : public DEVICE {
private:
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    //char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

public:
    SERVER();
    ~SERVER();
    void sendBytes(int nBytesToSend, BYTE *buf);
    void waitConnection();
    void receiveBytes(int nBytesToReceive, BYTE *buf);
    //void sendUint(uint32_t m);
    //void receiveUint(uint32_t& m);
};

class CLIENT : public DEVICE {
private:
    int sockfd, numbytes, new_fd;  
    //char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
public:
    CLIENT(string endereco);
    ~CLIENT();
    void sendBytes(int nBytesToSend, BYTE *buf);
    void receiveBytes(int nBytesToReceive, BYTE *buf);
    //void sendUint(uint32_t m);
    //void receiveUint(uint32_t& m);
};

SERVER::SERVER(){
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    printf("server: waiting for connections...\n");

}

SERVER::~SERVER(){
    close(new_fd);  // parent doesn't need this
}

void SERVER::waitConnection(){
    sin_size = sizeof their_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1) {
        perror("accept");
    }

    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
    printf("server: got connection from %s\n", s);
}

void SERVER::sendBytes(int nBytesToSend, BYTE *buf){
    uint32_t count = 0;
    while((count += send(new_fd, &buf[count], nBytesToSend, 0)) != nBytesToSend){

    }
}

void SERVER::receiveBytes(int nBytesToReceive, BYTE *buf){
    uint32_t count = 0;
    while ((count += recv(new_fd, &buf[count], nBytesToReceive, 0)) != nBytesToReceive){
        ;
    }
    
}

CLIENT::CLIENT(string endereco){
    const char *cstr = endereco.c_str();
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(cstr, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure
}

CLIENT::~CLIENT(){
    close(sockfd);
}

void CLIENT::sendBytes(int nBytesToSend, BYTE *buf){
    uint32_t count = 0;
    while((count += send(sockfd, &buf[count], nBytesToSend, 0)) != nBytesToSend){
        ;
    }
}

void CLIENT::receiveBytes(int nBytesToReceive, BYTE *buf){
    uint64_t count = 0;
    while ((count += (uint64_t)recv(sockfd, &buf[count], nBytesToReceive, 0)) != nBytesToReceive){
    }
}

bool testaBytes(BYTE* buf, BYTE b, int n) {
    //Testa se n bytes da memoria buf possuem valor b
    bool igual=true;
    for (unsigned i=0; i<n; i++)
        if (buf[i]!=b) { igual=false; break; }
    return igual;
}

void DEVICE::sendUint(uint32_t m){
    m = htonl(m);
    sendBytes(4,(BYTE*)&m);
}

void DEVICE::receiveUint(uint32_t& m){
    receiveBytes(4,(BYTE*)&m);
    m = ntohl(m);
}

void DEVICE::sendVb(const vector<BYTE>& vb){
    BYTE buf[vb.size()];
    int i = 0;
    sendUint((uint32_t)vb.size());
    for(auto it = std::begin(vb); it != std::end(vb); ++it){
        buf[i] = *it;
        i++;
    }
    sendBytes((uint32_t)vb.size(), buf);
}

void DEVICE::receiveVb(vector<BYTE>& vb){
    uint32_t len = 0;
    uint32_t count = 0;
    receiveUint(len);
    BYTE buf[len];
    receiveBytes(len, buf);
    for(auto it = std::begin(vb); it != std::end(vb); ++it){
        vb.erase(it);
        it = vb.insert(it, buf[count]);
        count++;
    }
    if (count < len){
        while (count < len){
            vb.push_back(buf[count]);
            count++;
        }
    }
}

void DEVICE::sendImg(const Mat_<COR>& img){
    Mat_<COR> img_send;
    img.copyTo(img_send);
    if (img_send.isContinuous()==false)
        perror("sendImage: notContinous");
    
    sendUint(img_send.rows); 
    sendUint(img_send.cols);
    sendBytes(3*img_send.total(), img_send.data);
}

void DEVICE::receiveImg(Mat_<COR>& img){
    uint32_t nl = 0;
    uint32_t nc = 0;
    receiveUint(nl);
    receiveUint(nc);
    Mat_<COR> img_rec;
    img_rec.create(nl,nc); 
    receiveBytes(3*img_rec.total(), img_rec.data);
    img_rec.copyTo(img);
}

void DEVICE::sendImgComp(const Mat_<COR>& img){
    vector<BYTE> vb;
    vector<int> param{CV_IMWRITE_JPEG_QUALITY,80};
    imencode(".jpg",img,vb,param);
    sendVb(vb);
}

void DEVICE::receiveImgComp(Mat_<COR> &img){
    vector<BYTE> vb;
    receiveVb(vb);
    Mat_<COR> img_rec=imdecode(vb,1);
    img_rec.copyTo(img);
}

bool testaVb(const vector<BYTE> vb, BYTE b) {
 //Testa se todos os bytes de vb possuem valor b
 bool igual=true;
 for (unsigned i=0; i<vb.size(); i++)
 if (vb[i]!=b) { igual=false; break; }
 return igual;
}

Mat_<COR> teclado(uint32_t estado) {
    Mat_<COR> a(240,240,COR(255,255,255));

    int thickness = 2;
    // linhas verticais
    line(a, Point(0,0), Point(0,240), Scalar(0,0,0),thickness);
    line(a, Point(80,0), Point(80,240), Scalar(0,0,0),thickness);
    line(a, Point(160,0), Point(160,240), Scalar(0,0,0),thickness);
    line(a, Point(240-1,0), Point(240-1,240), Scalar(0,0,0),thickness);

    // linhas horizontais
    line(a, Point(0,0), Point(240,0), Scalar(0,0,0),thickness);
    line(a, Point(0,80), Point(240,80), Scalar(0,0,0),thickness);
    line(a, Point(0,160), Point(240,160), Scalar(0,0,0),thickness);
    line(a, Point(0,240-1), Point(240,240-1), Scalar(0,0,0),thickness);

    //ponto central
    line(a, Point(120,120), Point(120,120), Scalar(0,0,255),10);

    // setas
    arrowedLine(a, Point(120,60), Point(120,20), Scalar(0,0,255),thickness); //N
    arrowedLine(a, Point(60,60), Point(20,20), Scalar(0,0,255),thickness); //NO
    arrowedLine(a, Point(180,60), Point(220,20), Scalar(0,0,255),thickness); //NE
    arrowedLine(a, Point(120,180), Point(120,220), Scalar(0,0,255),thickness); //S
    arrowedLine(a, Point(60,180), Point(20,220), Scalar(0,0,255),thickness); //SO
    arrowedLine(a, Point(180,180), Point(220,220), Scalar(0,0,255),thickness); //SE
    //return E
    line(a, Point(60, 140), Point(60,100), Scalar(0,0,255),thickness); 
    line(a, Point(60, 100), Point(20,100), Scalar(0,0,255),thickness); 
    arrowedLine(a, Point(20, 100), Point(20,140), Scalar(0,0,255),thickness); 
    //return D
    line(a, Point(180, 140), Point(180,100), Scalar(0,0,255),thickness); 
    line(a, Point(180, 100), Point(220,100), Scalar(0,0,255),thickness); 
    arrowedLine(a, Point(220, 100), Point(220,140), Scalar(0,0,255),thickness); 

    switch (estado)
    {
    case 9:
        arrowedLine(a, Point(60,60), Point(20,20), Scalar(0,255,0),thickness);
        break;
    
    case 8:
        arrowedLine(a, Point(120,60), Point(120,20), Scalar(0,255,0),thickness); //N
        break;
    case 7:
        arrowedLine(a, Point(180,60), Point(220,20), Scalar(0,255,0),thickness); //NE
        break;

    case 6:
        //return E
        line(a, Point(60, 140), Point(60,100), Scalar(0,255,0),thickness); 
        line(a, Point(60, 100), Point(20,100), Scalar(0,255,0),thickness); 
        arrowedLine(a, Point(20, 100), Point(20,140), Scalar(0,255,0),thickness); 
        break;
    case 5:
        //ponto central
        line(a, Point(120,120), Point(120,120), Scalar(0,255,0),10);
        break;
    case 4:
        //return D
        line(a, Point(180, 140), Point(180,100), Scalar(0,255,0),thickness); 
        line(a, Point(180, 100), Point(220,100), Scalar(0,255,0),thickness); 
        arrowedLine(a, Point(220, 100), Point(220,140), Scalar(0,255,0),thickness); 
        break;
    case 3:
        arrowedLine(a, Point(60,180), Point(20,220), Scalar(0,255,0),thickness); //SO
        break;
    case 2:
        arrowedLine(a, Point(120,180), Point(120,220), Scalar(0,255,0),thickness); //S
        break;
    case 1:
        arrowedLine(a, Point(180,180), Point(220,220), Scalar(0,255,0),thickness); //SE
        break;
    default:
        break;
    }


    return(a);
}

uint32_t estado=0;
void on_mouse(int event, int c, int l, int flags, void* userdata) {
    if (event==EVENT_LBUTTONDOWN) {
        if ( 0<=l && l<80 && 0<=c && c<80 ) estado=9;
        else if ( 0<=l && l<80 && 80<=c && c<160 ) estado=8;
        else if ( 0<=l && l<80 && 160<=c && c<240 ) estado=7;
        else if ( 80<=l && l<160 && 0<=c && c<80 ) estado=6;
        else if ( 80<=l && l<160 && 80<=c && c<160 ) estado=5;
        else if ( 80<=l && l<160 && 160<=c && c<240 ) estado=4;
        else if ( 160<=l && l<240 && 0<=c && c<80 ) estado=3;
        else if ( 160<=l && l<240 && 80<=c && c<160 ) estado=2;
        else if ( 160<=l && l<240 && 160<=c && c<240 ) estado=1;
        else estado=0;
    } else if (event==EVENT_LBUTTONUP) {
        estado=0;
    }
}

