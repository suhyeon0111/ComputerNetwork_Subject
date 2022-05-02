#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#define BUFFER_SIZE 1024
int main(int argc, char *argv[])
{
    int sock;                   // UDP 서버 쪽 소켓 디스크립터
    struct sockaddr_in srvAddr; // 서버 주소
    struct sockaddr_in cliAddr; // 클라이언트 주소
    unsigned short srvPort;
    char buffer[BUFFER_SIZE];
    char server_buffer[BUFFER_SIZE];
    int sentSize, rcvSize;
    unsigned int srvAddrLen, cliAddrLen;
    int ret, i;

    if (argc != 2)
    {
        printf("Usage: %s Port\n", argv[0]);
        exit(0);
    }

    srvPort = atoi(argv[1]);
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock < 0)
    {
        printf("socket() failed\n");
        exit(0);
    }

    // 서버 주소 설정. 이후 바인드 시스템 콜에서 사용됨
    memset(&srvAddr, 0, sizeof(srvAddr));        // 주소를 0으로 초기화
    srvAddr.sin_family = AF_INET;                // IPv4
    srvAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 서버 IP는 지정하지 않음
    srvAddr.sin_port = htons(srvPort);           // 서버 포트번호 지정
    // 소켓과 서버 주소를 바인딩
    ret = bind(sock, (struct sockaddr *)&srvAddr, sizeof(srvAddr));

    if (ret < 0)
    {
        printf("bind() failed\n");
        exit(0);
    }
    printf("Server is running.\n");

    while (1)
    { // 무한루프를 돌면서 메시지 송수신
        cliAddrLen = sizeof(cliAddr);
        rcvSize = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&cliAddr,
                           &cliAddrLen); // sock 소켓을 통해 buffer로 메시지 수신

        if (rcvSize < 0)
        {
            printf("Error in recvfrom()\n");
            exit(0);
        }

        printf("[Client/%s:%d] %s\n", inet_ntoa(cliAddr.sin_addr),
               ntohs(cliAddr.sin_port), buffer);

        if (!strcmp(buffer, "quit"))
            break;                         // 입력받은 데이터가 quit이면 종료
                                           //------------------------------------------------------------------------------//
        fgets(buffer, BUFFER_SIZE, stdin); // 키보드에서 buffer로
        buffer[strlen(buffer) - 1] = '\0'; // 한 줄씩 입력 받음

        sentSize = sendto(sock, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&cliAddr,
                          sizeof(cliAddr)); // sock 소켓을 통해 변환한 데이터 전송

        if (sentSize != strlen(buffer) + 1)
        {
            printf("sendto() sent a different number of bytes than expected");
            exit(0);
        }
        if (!strcmp(buffer, "quit"))
        {
            break;
        }
    }
    close(sock);
    printf("UDP Server is Closed.\n");
}