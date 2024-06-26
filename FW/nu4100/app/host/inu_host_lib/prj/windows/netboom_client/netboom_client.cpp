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
#include "getopt.h"

#define GROUPCAST_IP "224.0.1.0"
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
        "Usage: netboom_client [OPTION]\n"
        "\n"
        " Parameters  in a configuration:\n"
        "  -T       1: unicast  2: multicast   4: broadcast\n"
        "  -P  port number\n"
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

    int ret = 0;
#if defined(WIN32)

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
    SOCKET fd, new_socket;
    if (g_nbcfgs.type == 1)
        fd = socket(AF_INET, SOCK_STREAM, 0);
    else
        fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (fd == -1)
    {
        perror("socket");
        exit(0);
    }

    // 2. 通信的套接字和本地的IP与端口绑定
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(g_nbcfgs.port);    // 大端

    if (g_nbcfgs.type == 1)
    {
        // Convert IPv4 and IPv6 addresses from text to binary form
        if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0) {
            printf("\nInvalid address/ Address not supported \n");
            return -1;
        }
        // Connecting to the server
        if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            printf("\nConnection Failed \n");
            return -1;
        }
    }
    else
    {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);  // 0.0.0.0
        ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
        if (ret == -1)
        {
            perror("bind");
            exit(0);
        }
        
        if (g_nbcfgs.type == 2)
        {
            // 3. 加入到多播组
            //使用struct ip_mreqn或者 struct ip_mreq 设置接收端组播属性都可以正常接收

            /*struct ip_mreqn opt;
            // 要加入到哪个多播组, 通过组播地址来区分
            inet_pton(AF_INET, GROUPCAST_IP, &opt.imr_multiaddr.s_addr);
            opt.imr_address.s_addr = htonl(INADDR_ANY);
            opt.imr_ifindex = if_nametoindex("ens33");
            setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &opt, sizeof(opt));
            */

            struct ip_mreq mreq; // 多播地址结构体
            inet_pton(AF_INET, GROUPCAST_IP, &mreq.imr_multiaddr.s_addr);
            mreq.imr_interface.s_addr = htonl(INADDR_ANY);
            ret = setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&mreq, sizeof(mreq));
        }
    }

    char buf[1024];
    char sendaddrbuf[64];
    socklen_t len = sizeof(struct sockaddr_in);
    struct sockaddr_in sendaddr;

    // 3. 通信
    while (1)
    {
        memset(buf, 0, sizeof(buf));
        if (g_nbcfgs.type == 1)
        {
            // Sending data to the server
            const char* hello = "Hello from client..\n";
            send(fd, hello, strlen(hello)+1, 0);
            //printf("Message sent to server\n");

            // Reading response from server (if any)
            int valread = recv(fd, buf, sizeof(buf), 0);
            printf("Server response: %s\n", buf);
        }
        // 接收广播消息
        else if (g_nbcfgs.type == 2)
        {
            // 阻塞等待数据达到
            recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr*)&sendaddr, &len);
            printf("sendaddr:%s, port:%d %d %s\n", inet_ntop(AF_INET, &sendaddr.sin_addr.s_addr, sendaddrbuf, sizeof(sendaddrbuf)), sendaddr.sin_port, ntohs(sendaddr.sin_port), buf);
            strcpy_s(buf, " from client..\n");
            sendto(fd, buf, strlen(buf) + 1, 0, (struct sockaddr*)&sendaddr, len);
        }
        else if (g_nbcfgs.type == 4)
        {
            recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr*)&sendaddr, &len);
            printf("sendaddr:%s, port:%d %d %s\n", inet_ntop(AF_INET, &sendaddr.sin_addr.s_addr, sendaddrbuf, sizeof(sendaddrbuf)), sendaddr.sin_port, ntohs(sendaddr.sin_port), buf);
        }
    }
    closesocket(fd);
     
#if defined(WIN32)
    WSACleanup();
#endif
    return 0;
}
