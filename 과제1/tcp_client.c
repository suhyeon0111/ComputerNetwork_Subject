#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define BUFFER_SIZE 1024
int main(int argc, char *argv[])
{
    int cSock;
    struct sockaddr_in srvAddr;
    char *srvIp = NULL;
    unsigned short srvPort;
    char buffer[BUFFER_SIZE];
    int sentSize, rcvSize;
    int ret;
    if (argc != 3)
    {
        printf("Usage: %s IP_addr Port\n", argv[0]);
        exit(0);
    }
    srvIp = argv[1];         // 서버 IP를 명령 실행줄에서 입력 받음
    srvPort = atoi(argv[2]); // 서버 포트를 명령 실행줄에서 입력 받음
    // TCP 클라이언트 용 소켓 생성
    cSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (cSock < 0)
    {
        printf("socket() failed\n");
        exit(0);
    }
    // 서버 주소 설정. 이후 서버로 접속 시 connect() 시스템 콜에서 사용됨
    memset(&srvAddr, 0, sizeof(srvAddr));       // 주소를 0으로 초기화
    srvAddr.sin_family = AF_INET;               // IPv4
    srvAddr.sin_addr.s_addr = inet_addr(srvIp); // 서버 IP 지정
    srvAddr.sin_port = htons(srvPort);          // 서버 포트 번호 지정
    // 서버에 TCP 연결 수행
    ret = connect(cSock, (struct sockaddr *)&srvAddr, sizeof(srvAddr));
    if (ret < 0)
    {
        printf("Client cannot connect to the Server.\n");
        exit(0);
    }
    printf("Client is running.\n");
    printf("Enter the word to translate into capitals\n");
    while (1)
    { // 무한루프를 돌면서 메시지 송수신
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strlen(buffer) - 1] = '\0';
        sentSize = write(cSock, buffer, strlen(buffer) + 1);
        if (sentSize != strlen(buffer) + 1)
        {
            printf("write() sent a different number of bytes than expected\n");
            exit(0);
        }
        if (!strcmp(buffer, "quit"))
            break;
        rcvSize = read(cSock, buffer, BUFFER_SIZE);
        if (rcvSize < 0)
        {
            printf("Error in read()\n");
            exit(0);
        }
        printf("[Server/%s:%d] %s\n", inet_ntoa(srvAddr.sin_addr),
               ntohs(srvAddr.sin_port), buffer);
    }
    close(cSock);
    printf("TCP Client is Closed.\n");
}