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

// struct sockaddr {
//     sa_family_t sa_family;
//     char sa_data[14];
// };

// struct sockaddr_in {
//     sa_family_t sin_family;
//     unsigned short int sin_port;
//     struct in_addr sin_addr;
// };

struct addrinfo hints, *res;

int main(int argc, char *argv[]) {
    int total_count = 0;
    struct sockaddr_in sa;
    memset((char *)&sa, 0, sizeof(sa));

    char buf[MAX_LEN];

    int s = socket(AF_INET, SOCK_STREAM, 0);

    int yes = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    sa.sin_family = AF_INET; // IPv4
    sa.sin_port = htons(61001); // 待ち受けポート番号
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(s, (struct sockaddr*)&sa, sizeof(sa));

    listen(s, 1);
    printf("Start...\n");

    int connect_s = accept(s, NULL, NULL);

    while (1) {
        int count = read(connect_s, buf, MAX_LEN);
        if (count > 0) {
            write(1, buf, count);
            total_count += count;

            char result[MAX_LEN];
            int i = sprintf(result, "strlen: %d\n", count);
            write(connect_s, result, i);
            printf("Recv\n");
        } else {
            break;
        }
    }

    printf("Recv Complete\n");

    // char result[MAX_LEN];
    // int i = sprintf(result, "strlen: %d\n", total_count);
    // write(connect_s, result, i);

    // freeaddrinfo((struct sockaddr*)&sa);

    return 0;
}
