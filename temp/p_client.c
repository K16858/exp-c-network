#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define MAX_LEN 100

// struct addrinfo {
//     int              ai_flags;
//     int              ai_family;
//     int              ai_socktype;
//     int              ai_protocol;
//     socklen_t        ai_addrlen;
//     struct sockaddr *ai_addr;
//     char            *ai_canonname;
//     struct addrinfo *ai_next;
// };

struct addrinfo hints, *res;

int main(void) {
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM; /* TCP */
    hints.ai_family = AF_INET; /* IPv4 */

    char buf[MAX_LEN];
    char *msg = "TEST";
    int stdo = 1;

    if (getaddrinfo("localhost", "61001", &hints, &res)) {
        printf("Error\n");
        return 1;
    } else {
        printf("GetAddrInfo\n");
    }

    int s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if(connect(s, res->ai_addr, res->ai_addrlen)) {
        printf("Error\n");
        return 1;        
    } else {
        printf("Connected\n");
    }

    if(write(s, msg, 20) < 0) {
        printf("Error\n");
        return 1;
    } else {
        printf("Send\n");
    }

    while (1) {
        int count = read(s, buf, MAX_LEN);
        if (count > 0) {
            write(1, buf, count);
        } else {
            break;
        }
    }

    // freeaddrinfo(&hints);
    freeaddrinfo(res);

    return 0;
}
