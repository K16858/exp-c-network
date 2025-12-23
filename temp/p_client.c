#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define MAX_LEN 100

int subst(char *str, char c1, char c2){
    int c=0;
    while(*str!='\0'){
        if(*str==c1){
            *str=c2;
            c++;
        }
        str++;
    }

    return c;
}

void get_line(char *line, FILE *stram){
    while(1){
        line[0] = '\0';
        if(fgets(line,MAX_LEN+1,stram)==NULL){
            fprintf(stderr,"Input ERROR\n");
        }
        subst(line,'\n','\0');
        if(line[0]=='\0'){
            fprintf(stderr,"Input ERROR\n");           
        }
        else{
            break;
        }
    }
}

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

    if(send(s, msg, 20, 0) < 0) {
        printf("Error\n");
        return 1;
    } else {
        printf("Send\n");
    }

    int count = recv(s, buf, MAX_LEN, 0);
    if (count > 0) {
        printf("Recv: %s\n", buf);
    } else {
        printf("Faild");
    }

    char line[MAX_LEN + 1];
    while (1){
        get_line(line,stdin);
        int count = send(s, line, MAX_LEN, 0);
        recv(s, line, count, 0);
        if (strncmp("%Q", line, 2)!=0) {
            printf("%s\n", line);
        } else {
            printf("Close Server\n");
            break;
        }
    }

    // freeaddrinfo(&hints);
    freeaddrinfo(res);

    return 0;
}
