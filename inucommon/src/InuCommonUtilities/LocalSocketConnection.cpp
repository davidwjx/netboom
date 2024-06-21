#include "OSUtilities.h"
#include "LocalSocketConnection.h"

#ifdef BOOST_ASIO_HAS_LOCAL_SOCKETS

#include "boost/lexical_cast.hpp"
#include <boost/bind.hpp>
#include "Logger.h"

//safe socket close http://stackoverflow.com/a/12730776/536086
int getSO_ERROR(int fd) {
   int err = 1;
   socklen_t len = sizeof err;
   if (-1 == getsockopt(fd, SOL_SOCKET, SO_ERROR, (char *)&err, &len))
   {
      //FatalError("getSO_ERROR");
   }
   if (err)
   {
      errno = err;              // set errno to the socket SO_ERROR
   }
   return err;
}

void InuCommon::SafeCloseSocket(int iSocket)
{      // *not* the Windows closesocket()
   if (iSocket >= 0) {
      getSO_ERROR(iSocket); // first clear any errors, which can cause close to fail
      if (shutdown(iSocket, SHUT_RDWR) < 0) // secondly, terminate the 'reliable' delivery
      {
         if (errno != ENOTCONN && errno != EINVAL) // SGI causes EINVAL
         {
            //Perror("shutdown");
         }
      }
      if (close(iSocket) < 0) // finally call close()
      {
         //Perror("close");
      }
   }
}

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

using namespace InuCommon;

CLocalSocketClientConnector::CLocalSocketClientConnector(std::string iPath)
{
    ::memset(&mAddress,0,sizeof(mAddress));
    mAddress.sun_family = AF_UNIX;
    InuCommon::COsUtilities::memcpy(&mAddress.sun_path[1],sizeof(mAddress.sun_path) -1,iPath.c_str(),iPath.size()); //path should look like this: \0Somename	__android_log_print(ANDROID_LOG_ERROR,  "Common", "CLocalSocketClientConnector::CLocalSocketClientConnector");
    mAddressLength = sizeof(mAddress.sun_family) + strlen(&mAddress.sun_path[1]) + 2;  //check strlen from position 1, because position 0 is \0


    //CLogger::SetSeverity("Common", CLogger::eInfo);

    mSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (mSocket == -1)
    {
        throw errno;
        //InuCommon::CLogger::Write("CLocalSocketClientConnector socket creation failed",  InuCommon::CLogger::eError, INUSTREAMS_L);
    }

    std::string address(&mAddress.sun_path[1]);
    InuCommon::CLogger::Write("Try to connect : " + address, InuCommon::CLogger::eInfo, "Common");

    int errorCode = connect(mSocket, (struct sockaddr *) &mAddress, mAddressLength);
    if (errorCode != 0)
    {
        InuCommon::CLogger::Write("connect return error : " + std::to_string(errorCode), InuCommon::CLogger::eError, "Common");

        //InuCommon::CLogger::Write("CLocalSocketClientConnector connect failed: " + std::to_string(errorCode) + " " + std::string(strerror(errorCode)),  InuCommon::CLogger::eError, "Common");

        //log connection error, can't connect to service
        // InuCommon::CLogger::Write(std::string("CMemoryAshmemMappedArea connect() error: ") + std::string(strerror(errno)), InuCommon::CLogger::eError, INUSERVICE_LOG_PREFIX);
        SafeCloseSocket(mSocket);
        mSocket = -1;
        throw errno;
    }
}

CLocalSocketClientConnector::~CLocalSocketClientConnector()
{
    Stop();
}

void CLocalSocketClientConnector::Stop()
{
    SafeCloseSocket(mSocket);
    mSocket = -1;
    InuCommon::CLogger::Write("stop CLocalSocketClientConnector " , InuCommon::CLogger::eInfo, "Common");
    std::string address(&mAddress.sun_path[1]);
    InuCommon::CLogger::Write("Was bind to : " + address, InuCommon::CLogger::eInfo, "Common");


}

size_t CLocalSocketClientConnector::Write(std::vector<unsigned char>& writeBuffer) const
{

    //__android_log_print(ANDROID_LOG_ERROR,  "Common", "CLocalSocketServerConnector:: Write size: %d", writeBuffer.size());

    if (mSocket == -1)
    {
        return -1;
    }

    size_t size = 0;
    try
    {
        size = send(mSocket, writeBuffer.data(), writeBuffer.size(), MSG_NOSIGNAL); //MSG_NOSIGNAL means to prevent OS sending a signal on socket connection break
    }
    catch (...)
    {
        return -1;
    }


    if(size != writeBuffer.size())
    {
        InuCommon::CLogger::Write("CLocalSocketClientConnector write error ", InuCommon::CLogger::eInfo, "Common");

        return -1;
    }
    
    return size;
  //  __android_log_print(ANDROID_LOG_ERROR,  "Common", "Write OK");
}

CLocalSocketServerConnector::CLocalSocketServerConnector(std::string iPath): mLastAcceptedSocket(-1), mSocket(-1)
{
    ::memset(&mAddress,0,sizeof(mAddress));
    mAddress.sun_family = AF_UNIX;
    InuCommon::COsUtilities::memcpy(&mAddress.sun_path[1],sizeof(mAddress.sun_path) -1,iPath.c_str(),iPath.size()); //path should look like this: \0Somename	__android_log_print(ANDROID_LOG_ERROR,  "Common", "CLocalSocketClientConnector::CLocalSocketClientConnector");
    mAddressLength = sizeof(mAddress.sun_family) + strlen(&mAddress.sun_path[1]) + 2; //check strlen from position 1, because position 0 is \0

    Start();
}

CLocalSocketServerConnector::~CLocalSocketServerConnector()
{
    Stop();
}

/// \brief    Use unsynchronized accept
/// 
///  callback function which call when new data arrived from client
void CLocalSocketServerConnector::accept_handler(const boost::system::error_code& error)
{
    mAcceptRecived = true;
}

/// \brief    Use for exit form tcp accept
///
///  close the Tcp socket at application exit
///  call from destructor of CIpcServerTCP
void CLocalSocketServerConnector::StopListening()
{
    Stop();
}

int CLocalSocketServerConnector::AcceptConnection()
{
//	__android_log_print(ANDROID_LOG_ERROR,  "Common", "CLocalSocketServerConnector::AcceptConnection");
    sockaddr_un addr;
    socklen_t addrLen = sizeof(addr);
    mLastAcceptedSocket = accept(mSocket, (struct sockaddr *) &addr, &addrLen);

    if (mLastAcceptedSocket == -1)
    {
        InuCommon::CLogger::Write("CLocalSocketServerConnector accept failed: " + std::to_string(errno) + " " + std::string(strerror(errno)),  InuCommon::CLogger::eError, "Common");
    }
    return mLastAcceptedSocket;

}

int CLocalSocketServerConnector::Read(std::vector<unsigned char>& readResult)
{
    return Read(readResult, mLastAcceptedSocket);
}

int CLocalSocketServerConnector::Read(std::vector<unsigned char>& readResult, int iSocket)
{
    int totalLength = 0;
    try
    {
        static const int CHUNK_SIZE(4096); //one page is enough for IPCMessage
        readResult.resize(CHUNK_SIZE) ;
        size_t pos =0;
        int len = recv(iSocket, &readResult[pos], CHUNK_SIZE, 0);
 
        totalLength += len;

        if (len < 0 && errno == EINTR) // 10
        {
            InuCommon::CLogger::Write("CLocalSocketConnector recv failed [1] : " + std::to_string(len) + " "  + std::to_string(errno) + " " + std::string(strerror(errno)),  InuCommon::CLogger::eError, "Common");
        }
        else if (len < 0)
        {
            InuCommon::CLogger::Write("CLocalSocketConnector recv failed [2] : " + std::to_string(len) + " "  + std::to_string(errno) + " " + std::string(strerror(errno)),  InuCommon::CLogger::eError, "Common");
            throw errno;
        }
        else if (len == 0)
        {
            InuCommon::CLogger::Write("CLocalSocketConnector recv failed [3] : " + std::to_string(len) + " "  + std::to_string(errno) + " " + std::string(strerror(errno)),  InuCommon::CLogger::eError, "Common");
        }
    }
    catch (...)
    {
        return -1;
    }
    
//    __android_log_print(ANDROID_LOG_ERROR,  "Common", "CLocalSocketServerConnector:: read ok %d %d", totalLength, errno);

    return (int)totalLength;
}

void CLocalSocketServerConnector::Start()
{
    if (mSocket != -1)
    {
        return;
    }
    mSocket = socket(AF_UNIX, SOCK_STREAM, 0);

    int errorCode = bind(mSocket, (struct sockaddr *) &mAddress, mAddressLength);
    if (errorCode != 0)
    {
        InuCommon::CLogger::Write("CLocalSocketServerConnector bind failed: " + std::to_string(errorCode) + " " + std::string(strerror(errorCode)),  InuCommon::CLogger::eError, "Common");

        SafeCloseSocket(mSocket);
        throw errorCode;
    }
    else
    {
        std::string address(&mAddress.sun_path[1]);
        InuCommon::CLogger::Write("CLocalSocketServerConnector bind to : " + address, InuCommon::CLogger::eInfo, "Common");
    }
 
    errorCode = listen(mSocket, 20);
    if (errorCode != 0) //handle up to 20 connections
    {
        InuCommon::CLogger::Write("CLocalSocketServerConnector listen failed: " + std::to_string(errorCode) + " " + std::string(strerror(errorCode)),  InuCommon::CLogger::eError, "Common");

        SafeCloseSocket(mSocket);
        throw errorCode;
    }
}

void CLocalSocketServerConnector::Stop()
{
    SafeCloseSocket(mLastAcceptedSocket);
    SafeCloseSocket(mSocket);
    mLastAcceptedSocket = -1;
    mSocket = -1;
    InuCommon::CLogger::Write("stop CLocalSocketClientConnector ", InuCommon::CLogger::eInfo, "Common");

}

bool CLocalSocketServerConnector::IsAlive()
{
    int error = 0;
    socklen_t len = sizeof(error);
    int retval = getsockopt(mLastAcceptedSocket, SOL_SOCKET, SO_ERROR, &error, &len);

    return (retval !=0 && error != 0);
}

#endif
