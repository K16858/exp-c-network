#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define MAX_LINE_LEN 1024
#define MAX_DATA 50000

struct date{
    int y;
    int m;
    int d;
};

struct profile{
    int id;
    char name[72];
    struct date birth;
    char address[72];
    char *disc;
};

struct profile profile_data_store[MAX_DATA];
struct addrinfo hints, *res;

int profile_data_nitems = 0;

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
        if(fgets(line,MAX_LINE_LEN+1,stram)==NULL){
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

int get_message(char *line, int socket) {
    while(1) {
        memset(line, 0, MAX_LINE_LEN);
        if (recv(socket, line, MAX_LINE_LEN, 0)==0) {
            close(socket);
            return 0;
        }
        subst(line,'\n','\0');
        if(line[0]=='\0'){
            send(socket, "InputError", 10, 0);
            return 1;       
        }
        else{
            return 1;
            break;
        }
    }
}

int split(char *str, char *ret[], char sep, int max){
    int count = 0;
    char *p=str;

    while(*str!='\0'){
        if(*str==sep && count<max){
            *str = '\0';
            p = str + 1;
            count++;
        }

        ret[count] = p;
        str++;
    }

    return count;
}

int start_server() {
    struct sockaddr_in sa;
    memset((char *)&sa, 0, sizeof(sa));

    int s = socket(AF_INET, SOCK_STREAM, 0);

    int yes = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    sa.sin_family = AF_INET; // IPv4
    sa.sin_port = htons(61001); // 待ち受けポート番号
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(s, (struct sockaddr*)&sa, sizeof(sa));

    listen(s, 1);
    printf("Start...\n");

    return s;
}

int recv_connection(int socket) {
    char buf[MAX_LINE_LEN+1];

    int connect_s = accept(socket, NULL, NULL);
    int count = recv(connect_s, buf, MAX_LINE_LEN, 0);
    if (count > 0) {
        printf("Connected client\n");
        send(connect_s, "Connected. This is a Server", 28, 0);
        return connect_s;
    }

    return 0;
}

void new_profile(int number,char *line){
    int c1, c2;
    char *ret[1000],*date[1000];
    char sep1 = ',';
    char sep2 = '-';

    c1 = split(line,ret,sep1,10);
    c2 = split(ret[2],date,sep2,10);
    if(c1!=4||c2!=2){
        fprintf(stderr,"CSV ERROR\n");
        return;
    }
    profile_data_nitems++;
    profile_data_store[number].id = atoi(ret[0]);
    strncpy(profile_data_store[number].name,ret[1],72);
    strncpy(profile_data_store[number].address,ret[3],72);
    profile_data_store[number].disc = (char *)malloc(sizeof(char) * (strlen(ret[4])+1));
    strcpy(profile_data_store[number].disc,ret[4]);

    profile_data_store[number].birth.y = atoi(date[0]);
    profile_data_store[number].birth.m = atoi(date[1]);
    profile_data_store[number].birth.d = atoi(date[2]);
}

void print_profile(int start,int nitems){
    int count = 0;
    if(nitems==0){
        nitems = profile_data_nitems;
    }
    else if(nitems<0){
        nitems *= -1;
        start = profile_data_nitems - nitems;
        if(start<0){
            start = 0;
        }
        nitems = profile_data_nitems;
    }

    while(nitems!=count&&start<profile_data_nitems){
        printf("Id    : %d\n",profile_data_store[start].id);
        printf("Name  : %s\n",profile_data_store[start].name);
        printf("Birth : %d-",profile_data_store[start].birth.y);
        printf("%02d-",profile_data_store[start].birth.m);
        printf("%02d\n",profile_data_store[start].birth.d);
        printf("Addr. : %s\n",profile_data_store[start].address);
        printf("Comm  : %s\n\n",profile_data_store[start].disc);

        count++;
        start++;
    }
}

void swap(int p1, int p2){
    struct profile d;
    d = profile_data_store[p1];
    profile_data_store[p1] = profile_data_store[p2];
    profile_data_store[p2] = d;
}

void quick_sort(int left, int right, int column){
    if(left<right){
        int mid = (left + right)/2;
        int x = profile_data_store[mid].id;
        int i=left, j=right;
        char birth1[100] = "";
        char birth2[100] = "";
        char birthM[100] = "";
        sprintf(birth1,"%d-%02d-%02d",profile_data_store[i].birth.y,profile_data_store[i].birth.m,profile_data_store[i].birth.d);
        sprintf(birth2,"%d-%02d-%02d",profile_data_store[j].birth.y,profile_data_store[j].birth.m,profile_data_store[j].birth.d);
        sprintf(birthM,"%d-%02d-%02d",profile_data_store[mid].birth.y,profile_data_store[mid].birth.m,profile_data_store[mid].birth.d);
        while(i<=j){
            switch(column){
                case 1:
                    while(profile_data_store[i].id<x){
                        i++;
                    }
                    while(profile_data_store[j].id>x){
                        j--;
                    }
                    if(i<=j){
                        swap(i++,j--);
                    }
                    break;
                case 2:
                    while(strcmp(profile_data_store[i].name,profile_data_store[mid].name)<0){
                        i++;
                    }
                    while(strcmp(profile_data_store[j].name,profile_data_store[mid].name)>0){
                        j--;
                    }
                    if(i<=j){
                        swap(i++,j--);
                    }
                    break;
                case 3:
                    while(strcmp(birth1,birthM)<0){
                        i++;
                        sprintf(birth1,"%d-%02d-%02d",profile_data_store[i].birth.y,profile_data_store[i].birth.m,profile_data_store[i].birth.d);
                    }
                    while(strcmp(birth2,birthM)>0){
                        j--;
                        sprintf(birth2,"%d-%02d-%02d",profile_data_store[j].birth.y,profile_data_store[j].birth.m,profile_data_store[j].birth.d);
                    }
                    if(i<=j){
                        swap(i++,j--);
                    }
                    break;
                case 4:
                    while(strcmp(profile_data_store[i].address,profile_data_store[mid].address)<0){
                        i++;
                    }
                    while(strcmp(profile_data_store[j].address,profile_data_store[mid].address)>0){
                        j--;
                    }
                    if(i<=j){
                        swap(i++,j--);
                    }
                    break;
                case 5:
                    while(strcmp(profile_data_store[i].disc,profile_data_store[mid].disc)<0){
                        i++;
                    }
                    while(strcmp(profile_data_store[j].disc,profile_data_store[mid].disc)>0){
                        j--;
                    }
                    if(i<=j){
                        swap(i++,j--);
                    }
                    break;
            }
        }
        quick_sort(left,j,column);
        quick_sort(i,right,column);
    }
}

void cmd_quit(void){
    exit(0);
}

void cmd_check(void){
    printf("%d profile(s).\n",profile_data_nitems);
}

void cmd_print(int nitems){
    print_profile(0,nitems);
}

void cmd_read(char *file){
    FILE *fpr;
    fpr = fopen(file,"r");
    char line[MAX_LINE_LEN + 1];

    if(fpr==NULL){
        fprintf(stderr,"No such file %s\n",file);
    }
    else{
        while(fgets(line,1025,fpr)!=NULL){
            subst(line,'\n','\0');
            new_profile(profile_data_nitems,line);
            line[0] = '\0';
        }
        fclose(fpr);
    }
}

void cmd_write(char *file){
    FILE *fpw;
    fpw = fopen(file,"w");
    int n = 0;
    while(n!=profile_data_nitems){
        fprintf(fpw,"%d,",profile_data_store[n].id);
        fprintf(fpw,"%s,",profile_data_store[n].name);
        fprintf(fpw,"%d-",profile_data_store[n].birth.y);
        fprintf(fpw,"%02d-",profile_data_store[n].birth.m);
        fprintf(fpw,"%02d,",profile_data_store[n].birth.d);
        fprintf(fpw,"%s,",profile_data_store[n].address);
        fprintf(fpw,"%s\n",profile_data_store[n].disc);

        n++;
    }
    fclose(fpw);
}

int search(char *word, int result[5000]){
    int i = 0;
    int count = 0;
    for(i=0;i<profile_data_nitems;i++){
        char birth[100] = "";
        sprintf(birth,"%d-%02d-%02d",profile_data_store[i].birth.y,profile_data_store[i].birth.m,profile_data_store[i].birth.d);
        if(profile_data_store[i].id==atoi(word)||
        strcmp(birth,word)==0||
        strcmp(profile_data_store[i].address,word)==0||
        strcmp(profile_data_store[i].name,word)==0||
        strcmp(profile_data_store[i].disc,word)==0){
        result[count] = i; 
        count++;
        }
    }

    return count;
}

void cmd_find(char *word){
    int result[5000];
    int count;
    count = search(word,result);
    int i = 0;
    for(i=0;i<count;i++){
        print_profile(result[i],1);
    }
}

void cmd_sort(int column){
    if(column>0&&column<6){
        quick_sort(0,profile_data_nitems-1,column);
    }
    else{
        fprintf(stderr,"%%S: Invalid column number %d: ignore\n",column);
    }
}

void cmd_help(void){
    fprintf(stdout,"Manual:Command List\n");
    fprintf(stdout,"--------------------------------------------------------\n");
    fprintf(stdout,"%%Q : Exit the program. Argument is nothing\n");
    fprintf(stdout,"%%C : Display number of data. Argument is nothing\n");
    fprintf(stdout,"%%P : Display data. Argument is integer\n");
    fprintf(stdout,"%%F : Explore and display data. Argument is word\n");
    fprintf(stdout,"%%R : Read from CSV file. Argument is file name\n");
    fprintf(stdout,"%%W : Export to CSV file. Argument is file name\n");
    fprintf(stdout,"%%S : Sort data by column. Argument is column number\n");
    fprintf(stdout,"%%H : Display command manual. Argument is nothing\n");
    fprintf(stdout,"%%D : Search and delete data. Argument is word\n");
    fprintf(stdout,"--------------------------------------------------------\n");
}

void cmd_delete(char *word){
    int delete_number = -1;
    int result[5000];
    int count;
    count = search(word,result);
    int i = 0;
    for(i=0;i<count;i++){
        print_profile(result[i],1);
        delete_number = result[i];
    }
    if(count>1){
        while(1){
            char ans[MAX_LINE_LEN] = "";
            fprintf(stdout,"Which data do you want to delete ?:1-%d\n",count);
            get_line(ans,stdin);
            int ans_number = atoi(ans);
            if(ans_number>0&&ans_number<count+1){
                delete_number = result[ans_number-1];
                break;
            }
            else{
                fprintf(stdout,"Input correct number\n");               
            }
        }
    }
    if(delete_number==-1){
        fprintf(stdout,"No Data\n");
    }
    else{
        fprintf(stdout,"delete this data ?(Y/n)\n");
        char ans[MAX_LINE_LEN] = "";
        get_line(ans,stdin);
        if(strcmp(ans,"y")==0||strcmp(ans,"Y")==0){
            for(int j=delete_number;j<profile_data_nitems;j++){
                profile_data_store[j] = profile_data_store[j+1];
            }
            profile_data_nitems--;
            fprintf(stdout,"delete data\n");
        }
    }
}

void exec_command(char *cmd, char *param1, char *param2, char *param_str, int socket){
    int number;
    int nitems;
    if(strcmp(cmd,"Q")==0){
        // cmd_quit();
        send(socket, "!Q", 4, 0);
    }
    else if(strcmp(cmd,"C")==0){
        cmd_check();
    }
    else if(strcmp(cmd,"P")==0){
        nitems = atoi(param1);
        cmd_print(nitems);
    }
    else if(strcmp(cmd,"R")==0){
        cmd_read(param1);
    }
    else if(strcmp(cmd,"W")==0){
        cmd_write(param1);
    }
    else if(strcmp(cmd,"F")==0){
        cmd_find(param_str);  
    }
    else if(strcmp(cmd,"S")==0){
        number = atoi(param1);
        cmd_sort(number);    
    }
    else if(strcmp(cmd,"H")==0){
        cmd_help();
    }
    else if(strcmp(cmd,"D")==0){
        cmd_delete(param_str);
    }
    else{
        fprintf(stderr, "Invalid command %s: ignored.\n", cmd);
    }
}

void parse_line(char *line, int socket){
    char *cmd, *param1, *param2;
    char *param_str;
    char *ret[100];
    int n;
    if(line[0]=='%'){
        n = split(&line[1],ret,' ',2);
        cmd = ret[0];
        param1 = ret[1];
        param2 = ret[2];
        if(n>1){
            sprintf(param_str,"%s %s",ret[1],ret[2]);
        }
        else{
            param_str = ret[1];
        }

        exec_command(cmd, param1, param2, param_str, socket);
    }
    else{
        new_profile(profile_data_nitems,line);
    }
}

int main(void){
    char line[MAX_LINE_LEN + 1];

    int socket = start_server();
    int connected_socket = recv_connection(socket);

    while (get_message(line, connected_socket)){
        parse_line(line, connected_socket);
    }

    return 0;
}
