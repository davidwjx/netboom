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

    // 1. 创建通信的套接字
    SOCKET fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (fd == -1)
    {
        perror("socket");
        exit(0);
    }

    // 2. 设置组播属性 (经测试可以不设置发送端组播属性也能正常发送)
    struct in_addr opt;
    // 将组播地址初始化到这个结构体成员中即可
    inet_pton(AF_INET, GROUP_IP, &opt.s_addr);
    setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, (const char*) & opt, sizeof(opt));

    char buf[1024];
    char sendaddrbuf[64];

    socklen_t len = sizeof(struct sockaddr_in);
    struct sockaddr_in sendaddr;

    struct sockaddr_in cliaddr;
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(9999); // 接收端需要绑定9999端口
    // 发送组播消息, 需要使用组播地址, 和设置组播属性使用的组播地址一致就可以
    inet_pton(AF_INET, GROUP_IP, &cliaddr.sin_addr.s_addr);

    // 3. 通信
    int num = 0;
    while (1)
    {
        memset(buf, 0, sizeof(buf));
        sprintf_s(buf, "hello, client...%d\n", num++);
        // 数据广播
        sendto(fd, buf, strlen(buf) + 1, 0, (struct sockaddr*)&cliaddr, len);
        printf("发送的组播的数据: %s\n", buf);
        memset(buf, 0, sizeof(buf));
        recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr*)&sendaddr, &len);
        printf("sendaddr: %s, port:%d\n", inet_ntop(AF_INET, &sendaddr.sin_addr.s_addr, sendaddrbuf, sizeof(sendaddrbuf)), sendaddr.sin_port);
        printf("接收到的组播消息: %s\n", buf);
    }
    closesocket(fd);

#if defined(WIN32)
    WSACleanup();
#endif
    return 0;
}
