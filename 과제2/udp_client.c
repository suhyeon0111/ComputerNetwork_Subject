#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
    int sock;                   // UDP 클라이언트 쪽 소켓 디스크립터
    struct sockaddr_in srvAddr; // 서버 주소
    char *srvIp = NULL;
    unsigned short srvPort;
    char buffer[BUFFER_SIZE];
    char recv_buffer[BUFFER_SIZE];
    int sentSize, rcvSize;
    unsigned int srvAddrLen;

    if (argc != 3)
    {
        printf("Usage: %s IP_addr Port\n", argv[0]);
        exit(0);
    }

    srvIp = argv[1]; // 서버 IP를 명령 실행줄에서 입력 받음
    srvPort = atoi(argv[2]);
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock < 0)
    {
        printf("socket() failed\n");
        exit(0);
    }
    // 서버 주소 설정. 이후 서버로 데이터 전송 시 사용됨
    memset(&srvAddr, 0, sizeof(srvAddr));       // 주소를 0으로 초기화
    srvAddr.sin_family = AF_INET;               // IPv4
    srvAddr.sin_addr.s_addr = inet_addr(srvIp); // 서버 IP 지정
    srvAddr.sin_port = htons(srvPort);          // 서버 포트 번호 지정

    printf("Client is running.\n");

    while (1)
    {                                      // 무한루프를 돌면서 메시지 송수신
        fgets(buffer, BUFFER_SIZE, stdin); // 키보드에서 buffer로
        buffer[strlen(buffer) - 1] = '\0'; // 한 줄씩 입력 받음
        // 마지막에 널 문자 추가

        sentSize = sendto(sock, buffer, strlen(buffer) + 1, 0,
                          (struct sockaddr *)&srvAddr, sizeof(srvAddr));

        if (sentSize != strlen(buffer) + 1)
        {
            printf("sendto() sent a different number of bytes than expected\n");
            exit(0);
        }
        if (!strcmp(buffer, "quit"))
            break;

        srvAddrLen = sizeof(srvAddr);
        rcvSize = recvfrom(sock, buffer, BUFFER_SIZE, 0,
                           (struct sockaddr *)&srvAddr, &srvAddrLen);

        if (rcvSize < 0)
        {
            printf("Error in recvfrom()\n");
            exit(0);
        }
        printf("[Server/%s:%d] %s\n", inet_ntoa(srvAddr.sin_addr),
               ntohs(srvAddr.sin_port), buffer); // 수신한 데이터를 화면에 출력

        if (!strcmp(buffer, "quit"))
            break; // 수신한 데이터가 quit라면 종료
    }

    close(sock);
    printf("UDP Client is Closed.\n");
}