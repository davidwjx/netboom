// netboom_sndr.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <io.h>
#include <string.h>
#include <fcntl.h>

#if defined __LINUX__
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#elif defined(WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif 

#define GROUP_IP "224.0.1.0"
//#define GROUP_IP "239.0.1.10"

int main()
{
#if defined(WIN32)
    
    int ret = 0;
    WSADATA wsaData;
    ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (ret != 0)
    {
        printf("Failed to initialize Winsocket: %d", ret);
        return 1;
    }
#endif

    // 1. ����ͨ�ŵ��׽���
    SOCKET fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (fd == -1)
    {
        perror("socket");
        exit(0);
    }

    // 2. �����鲥���� (�����Կ��Բ����÷��Ͷ��鲥����Ҳ����������)
    struct in_addr opt;
    // ���鲥��ַ��ʼ��������ṹ���Ա�м���
    inet_pton(AF_INET, GROUP_IP, &opt.s_addr);
    setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, (const char*) & opt, sizeof(opt));

    char buf[1024];
    char sendaddrbuf[64];

    socklen_t len = sizeof(struct sockaddr_in);
    struct sockaddr_in sendaddr;

    struct sockaddr_in cliaddr;
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(9999); // ���ն���Ҫ��9999�˿�
    // �����鲥��Ϣ, ��Ҫʹ���鲥��ַ, �������鲥����ʹ�õ��鲥��ַһ�¾Ϳ���
    inet_pton(AF_INET, GROUP_IP, &cliaddr.sin_addr.s_addr);

    // 3. ͨ��
    int num = 0;
    while (1)
    {
        memset(buf, 0, sizeof(buf));
        sprintf_s(buf, "hello, client...%d\n", num++);
        // ���ݹ㲥
        sendto(fd, buf, strlen(buf) + 1, 0, (struct sockaddr*)&cliaddr, len);
        printf("���͵��鲥������: %s\n", buf);
        memset(buf, 0, sizeof(buf));
        recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr*)&sendaddr, &len);
        printf("sendaddr: %s, port:%d\n", inet_ntop(AF_INET, &sendaddr.sin_addr.s_addr, sendaddrbuf, sizeof(sendaddrbuf)), sendaddr.sin_port);
        printf("���յ����鲥��Ϣ: %s\n", buf);
    }
    closesocket(fd);

#if defined(WIN32)
    WSACleanup();
#endif
    return 0;
}
