// netboom_sndr.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <io.h>
#include <string.h>
#include <fcntl.h>
#include "getopt.h"

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


#define GROUPCAST_IP "224.0.1.0"
//#define GROUP_IP "239.0.1.10"
#define BROADCAST_IP "255.255.255.255"
typedef struct
{
    int     type;
    int     port;
    int     timeout;	// seconds
    int     fps;
}NetBoomConfigParams;

NetBoomConfigParams g_nbcfgs;

static void display_help(void)
{
    char helpStringBuffer[1024];

    sprintf_s(helpStringBuffer,
        "Usage: netboom_sndr [OPTION]\n"
        "\n"
        " Parameters  in a configuration:\n"
        "  -T       1: unicast  2: multicast   4: broadcast\n"
        "  -P  port number\n"
        "  -d  duration in seconds\n"
        "  -f  fps\n"
        "  -h  help\n"
    );
    printf(helpStringBuffer);
    exit(EXIT_SUCCESS);
}

void process_options(int argc, char* argv[], NetBoomConfigParams* configP)
{
    int opt_num = 0;
    for (;;) {
        static char const* short_options = "T:h:d:f:P:m:";
        int c = getopt(argc, argv, (char*)short_options);
        if (c == EOF)
            break;
        if (optarg)
            opt_num = atoi(optarg);

        switch (c) {
        case 'T':
            configP->type = opt_num;
            break;

        case 'P':
            configP->port = opt_num;
            break;
        case 'd':
            configP->timeout = opt_num;
            break;
        case 'f':
            configP->fps = opt_num;
            break;

        case 'h':
            display_help();
            exit(0);
            break;


        default:
            display_help();
            exit(0);
            break;
        }

    }

    argc -= optind;
    argv += optind;

}


int main(int argc, char* argv[])
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

    g_nbcfgs.port = 9999;
    process_options(argc, argv, &g_nbcfgs);

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
    switch (g_nbcfgs.type)
    {
    case 1:
        inet_pton(AF_INET, GROUPCAST_IP, &opt.s_addr);
        break;

    case 2:
        inet_pton(AF_INET, GROUPCAST_IP, &opt.s_addr);
        setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, (const char*)&opt, sizeof(opt));
        break;

    case 4:
        inet_pton(AF_INET, BROADCAST_IP, &opt.s_addr);
        setsockopt(fd, SOL_SOCKET, SO_BROADCAST, (const char*)&opt, sizeof(opt));
        break;

    default:
        inet_pton(AF_INET, BROADCAST_IP, &opt.s_addr);
        setsockopt(fd, SOL_SOCKET, SO_BROADCAST, (const char*)&opt, sizeof(opt));
    }

    

    char buf[1024];
    char sendaddrbuf[64];

    socklen_t len = sizeof(struct sockaddr_in);
    struct sockaddr_in sendaddr;

    struct sockaddr_in cliaddr;
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(g_nbcfgs.port); // 接收端需要绑定9999端口
    // 发送组播消息, 需要使用组播地址, 和设置组播属性使用的组播地址一致就可以
    //inet_pton(AF_INET, GROUPCAST_IP, &cliaddr.sin_addr.s_addr);
    memcpy(&cliaddr.sin_addr.s_addr, &opt.s_addr, sizeof(opt));

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
