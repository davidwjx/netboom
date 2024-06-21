#define WIN32_LEAN_AND_MEAN

#include "OSUtilities.h"
#include "TCPConnection.h"
#include "boost/lexical_cast.hpp"

#include <boost/bind.hpp>
#include <boost/thread/locks.hpp>
#include <assert.h>

#define NUM_OF_INTERATIONS 5
#define MESSAGE_SIZE_OFFSET 4
#define MESSAGE_SIZE_LEN	4
#define MIN_PACKAGE_HEADER_LEN (MESSAGE_SIZE_OFFSET + MESSAGE_SIZE_LEN)
#define IPC_MESSAGE_SIZE 92
#define CHUNK_SIZE	4096
#define MAX_BUFFER_SIZE	1080 * 1920 * 4 * 2

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

using namespace InuCommon;

static inline size_t recevie_sized_data(boost::asio::ip::tcp::socket& sock, char *buffer,
		size_t size, boost::system::error_code &error)
{
    size_t readLen = 0;

    do {
        size_t ret = sock.read_some(boost::asio::buffer(&buffer[readLen], size - readLen), error);
	if (!error)
            readLen += ret;
	else if (boost::asio::error::try_again != error && boost::asio::error::interrupted != error)
            break;
    } while (readLen < size);

    return readLen;
}

static uint32_t ReadPackage(boost::asio::ip::tcp::socket& sock, boost::system::error_code &error, std::vector<unsigned char>& readResult, unsigned long& senderIP)
{
    uint32_t messageSize = 0;
    char buf[MIN_PACKAGE_HEADER_LEN];

    // get peer ip
    senderIP = sock.remote_endpoint().address().to_v4().to_ulong();

    // try to read 8 bytes to get message size
    size_t readLen = recevie_sized_data(sock, buf, MIN_PACKAGE_HEADER_LEN, error);
    if (error)
        return 0;

    // get the size of message, allocate buffer for the message
    // and copy the 8 bytes to message buffer
    messageSize = *reinterpret_cast<uint32_t*>(buf + MESSAGE_SIZE_OFFSET);
    if (messageSize < readLen)
        messageSize = uint32_t(readLen);
    readResult.resize(messageSize);
    InuCommon::COsUtilities::memcpy(&readResult[0], readLen, &buf[0], readLen);

    // now read the rest of the message into the message buffer directly
    if (messageSize > readLen)
        recevie_sized_data(sock, (char *)&readResult[readLen], messageSize - readLen, error);

    return messageSize;
}


//
//
//	CTcpClientConnector
//
////////////////////////////////////////////////////////////////////////

CTcpClientConnector::CTcpClientConnector(std::string ip, std::string port) : mIp(ip), mPort(port),
    m_socket(m_io_service)
{
}

CTcpClientConnector::~CTcpClientConnector()
{
    m_socket.close();
}

void CTcpClientConnector::Write(unsigned long ip, std::vector<unsigned char>& writeBuffer) const
{
    Write(boost::asio::ip::address_v4(ip).to_string(),writeBuffer);
}

void CTcpClientConnector::Close()
{
    m_socket.close();
}

void CTcpClientConnector::Write(const std::string& ip, std::vector<unsigned char>& writeBuffer) const
{
    boost::system::error_code error = boost::asio::error::host_not_found;
    boost::asio::write(m_socket, boost::asio::buffer(writeBuffer), boost::asio::transfer_all(), error );

    if(error)
    {
        throw boost::system::system_error(error);
    }
}

int CTcpClientConnector::Read(std::vector<unsigned char>& readResult, unsigned long& senderIP)
{
    int totalLength = -1;
    boost::system::error_code error = boost::asio::error::host_not_found;

    try
    {
        totalLength = (int)ReadPackage(m_socket, error, readResult, senderIP);
        if (error)
            throw boost::system::system_error(error);
    }
    catch (std::exception&)
    {
        totalLength = -1;
    }

    //printf("client read %d\n", totalLength);
    return totalLength;
}

void CTcpClientConnector::Connect(const std::string& ip) const
{
    if (m_socket.is_open())
    {
        return;
    }

    boost::asio::ip::tcp::resolver resolver(m_io_service);
    boost::asio::ip::tcp::resolver::query query(ip, mPort);
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    boost::asio::ip::tcp::resolver::iterator start = endpoint_iterator;
    boost::asio::ip::tcp::resolver::iterator end;    
    boost::system::error_code error = boost::asio::error::host_not_found;
    
    //iterate over all the endpoints and check if any of them is available 
    endpoint_iterator = start;
    while(error && endpoint_iterator != end)
    {
        m_socket.close();
        m_socket.connect(*endpoint_iterator++, error);
    }

    if(error)
    {
        throw boost::system::system_error(error);
    }
}

//
//
//	CTcpServerConnector
//
////////////////////////////////////////////////////////////////////////

CTcpServerConnector::CTcpServerConnector(std::string port) : m_port(port),
m_endpoint(boost::asio::ip::tcp::v4(), boost::lexical_cast<unsigned short>(port)),
m_acceptor(m_io_service, m_endpoint),
m_socket(m_io_service)
{

}

CTcpServerConnector::~CTcpServerConnector()
{
    Stop();
}

///  accept handler - accept a new socket, build client pair and add it to clients map
//	 register this accept handler again - for the next time io_service.run will be called	

void CTcpServerConnector::accept_handler(const boost::system::error_code& error)
{
    if (!error)
    {
        //uint32_t senderIP = m_socket.remote_endpoint().address().to_v4().to_ulong();
        //TcpLockPair tcpLock;
        //tcpLock.tcpConnector = std::make_shared<CTcpClientConnection>( m_io_service, m_socket);
        //tcpLock.lock = std::make_shared<boost::shared_mutex>();
        //mTcpClientsMap.insert( std::pair<uint32_t, TcpLockPair>( senderIP, tcpLock) ); //create a new tcp connector for this address
    }
    else
    {
        std::string msg = error.message();
    }
}

//	open the listening socket
//	register the accept call back
//	wait for the first connection

void CTcpServerConnector::StartListening()
{
    const boost::system::error_code error;
    m_socket.close();
    m_acceptor.async_accept(m_socket, boost::bind(&CTcpServerConnector::accept_handler, this, error));
    m_io_service.reset();
    m_io_service.run();
    mIsListening = true;
}

//  close the listening socket 
void CTcpServerConnector::StopListening()
{
    mIsListening = false;
    boost::system::error_code error;

    m_socket.close();
    m_acceptor.cancel(error);
    m_acceptor.close(error);
    boost::asio::io_service io_service;

    // How to stop io_service according to StackOverflow
    std::unique_ptr<boost::asio::io_service::work> work(new boost::asio::io_service::work(io_service));
    work.reset(); // Allow run() to exit.

    assert(!error);
}

// block Write 
int CTcpServerConnector::Write(unsigned long processID, std::vector<unsigned char>& writeBuffer)
{
    int numWritten = 0;
    boost::unique_lock<boost::shared_mutex> wlock(m_write_lock);

    boost::system::error_code error = boost::asio::error::host_not_found;
    boost::asio::write( m_socket, boost::asio::buffer(writeBuffer), boost::asio::transfer_all(), error);
    
    if (!error)
    {
        numWritten = (int)writeBuffer.size();
    }

    //auto foundIterator = mTcpClientsMap.find(processID);
    //if (foundIterator != mTcpClientsMap.end())
    //{
    //	boost::unique_lock <boost::shared_mutex> wLock( *(foundIterator->second.lock));
    //	numWritten = foundIterator->second.tcpConnector->Write(processID, writeBuffer); //create a new tcp connector for this address
    //}

    //printf("server: write %d\n", numWritten);

    return numWritten;
}

// block Read a complete ipc message 
CTcpServerConnector::EErrorCode CTcpServerConnector::Read(std::vector<unsigned char>& readResult, unsigned long& senderIP, unsigned long timeout)
{
    boost::system::error_code error = boost::asio::error::host_not_found;

    try
    {
        int to(timeout);
        ::setsockopt(m_socket.native_handle(), SOL_SOCKET, SO_RCVTIMEO, (const char*)&to, sizeof to);
 
        (void)ReadPackage(m_socket, error, readResult, senderIP);
        if (error) {
            if (error == boost::asio::error::timed_out)
            {
                return eTimeout;
            }
            mIsListening = false;
            return eFailure;
        }
    }
    catch (std::exception&)
    {
        mIsListening = false;
        return eException;
    }

    //printf("server read %d\n", (int)readResult.size());

    return eOk;
}

void CTcpServerConnector::Stop()
{
    StopListening();
    if (m_socket.is_open())
    {
        m_socket.cancel();
    }
}


//
//
//	CTcpClientConnection
//
////////////////////////////////////////////////////////////////////////


int CTcpClientConnection::Write(unsigned long ip, std::vector<unsigned char>& writeBuffer) const
{
    return Write(boost::asio::ip::address_v4(ip).to_string(), writeBuffer);
}

int CTcpClientConnection::Write(const std::string& ip, std::vector<unsigned char>& writeBuffer) const
{
    Connect(ip);
    boost::system::error_code error = boost::asio::error::host_not_found;
    boost::asio::write(m_socket, boost::asio::buffer(writeBuffer), boost::asio::transfer_all(), error);

    if (error)
    {
        throw boost::system::system_error(error);
    }

    //m_socket.close();
    
    return 0;
}

void CTcpClientConnection::Connect(const std::string& ip) const
{
    boost::asio::ip::tcp::resolver resolver(m_io_service);
    boost::asio::ip::tcp::resolver::query query(ip, m_port);
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    boost::asio::ip::tcp::resolver::iterator start = endpoint_iterator;
    boost::asio::ip::tcp::resolver::iterator end;
    boost::system::error_code error = boost::asio::error::host_not_found;

    int numOfInterations = 0;

    do
    {
        //iterate over all the endpoints and check if any of them is available 
        endpoint_iterator = start;
        while (error && endpoint_iterator != end)
        {
            m_socket.close();
            m_socket.connect(*endpoint_iterator++, error);
            numOfInterations++;
        }

    } while ((error || endpoint_iterator == end) && (numOfInterations < NUM_OF_INTERATIONS)); //loop until connection succeeds

    if (error) // 6
    {
        throw boost::system::system_error(error);
    }
}

