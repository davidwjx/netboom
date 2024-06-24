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

#define GROUPCAST_IP "224.0.1.0"

int main()
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
    // 1. ����ͨ�ŵ��׽���
    SOCKET fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (fd == -1)
    {
        perror("socket");
        exit(0);
    }

    // 2. ͨ�ŵ��׽��ֺͱ��ص�IP��˿ڰ�
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);    // ���
    addr.sin_addr.s_addr = htonl(INADDR_ANY);  // 0.0.0.0
    ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret == -1)
    {
        perror("bind");
        exit(0);
    }

    // 3. ���뵽�ಥ��
#if 0 //ʹ��struct ip_mreqn���� struct ip_mreq ���ý��ն��鲥���Զ�������������
    struct ip_mreqn opt;
    // Ҫ���뵽�ĸ��ಥ��, ͨ���鲥��ַ������
    inet_pton(AF_INET, GROUPCAST_IP, &opt.imr_multiaddr.s_addr);
    opt.imr_address.s_addr = htonl(INADDR_ANY);
    opt.imr_ifindex = if_nametoindex("ens33");
    setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &opt, sizeof(opt));
#else
    struct ip_mreq mreq; // �ಥ��ַ�ṹ��
    inet_pton(AF_INET, GROUPCAST_IP, &mreq.imr_multiaddr.s_addr);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    ret = setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*) & mreq, sizeof(mreq));
#endif

    char buf[1024];
    char sendaddrbuf[64];
    socklen_t len = sizeof(struct sockaddr_in);
    struct sockaddr_in sendaddr;

    // 3. ͨ��
    while (1)
    {
        // ���չ㲥��Ϣ
        memset(buf, 0, sizeof(buf));
        // �����ȴ����ݴﵽ
        recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr*)&sendaddr, &len);
        printf("sendaddr:%s, port:%d %x\n", inet_ntop(AF_INET, &sendaddr.sin_addr.s_addr, sendaddrbuf, sizeof(sendaddrbuf)), sendaddr.sin_port, 9999);
        printf("���յ����鲥��Ϣ: %s\n", buf);
        sendto(fd, buf, strlen(buf) + 1, 0, (struct sockaddr*)&sendaddr, len);
    }
    closesocket(fd);

#if defined(WIN32)
    WSACleanup();
#endif
    return 0;
}
