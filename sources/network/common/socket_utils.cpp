#include "tacopie/network/socket_utils.hpp"
#include <tacopie/network/tcp_server.hpp>
#include <tacopie/utils/error.hpp>
#include <tacopie/utils/logger.hpp>


#ifdef H_OS_WINDOWS
#include <Winsock2.h>
#include <Ws2tcpip.h>


#define SOCKET_ERRNO(error) WSA##error
#define socket_close closesocket
#define socket_errno WSAGetLastError()
#define socket_strerror gai_strerror

#else
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/uio.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <net/if.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#define socket_errno errno
#endif /* _WIN32 */
namespace tacopie {
    
    int socket_utils::socket_set_nobio(fd_t fd) {
#ifndef H_OS_WINDOWS
        int ret = fcntl(fd, F_GETFL, 0);
        if(ret >= 0) {
            long flags = ret | O_NONBLOCK;
            ret = fcntl(fd, F_SETFL, flags);
        }
        
#else
        static const int noblock = 1;
        int ret = ioctlsocket(fd, FIONBIO, (u_long*)&noblock);
        
#endif
        return ret;
    }
    
    int socket_utils::socket_set_bio(fd_t fd)
    {
#ifndef H_OS_WINDOWS
        int ret = fcntl(fd, F_GETFL, 0);
        if(ret >= 0) {
            long flags = ret & (~O_NONBLOCK);
            ret = fcntl(fd, F_SETFL, flags);
        }
        
#else
        static const int noblock = 0;
        int ret = ioctlsocket(fd, FIONBIO, (u_long*)&noblock);
        
#endif
        return ret;
    }
    
    
    int socket_utils::socket_error(fd_t  sock) {
        int error = 0;
        socklen_t len = sizeof(error);
        if (0 != getsockopt(sock, SOL_SOCKET, SO_ERROR, (char*)&error, &len))
        {
			error = socket_errno; 
		}
        return error;
    }
    
};
