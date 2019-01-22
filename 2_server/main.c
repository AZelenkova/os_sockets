#include<stdio.h>       // perror(), printf(),
#include<string.h>      // memset(), strlen(), strcmp(), bzero(), bcopy()
#include<unistd.h>      // chdir(), gethostname(), access(), close()
#include<stdlib.h>      // exit()
//#include<sys/types.h>
//#include<sys/socket.h>
//#include<netinet/in.h>
#include<netdb.h> // socket(), connect(), htons(), recv(), send()

// find . -type f -printf "\n%AD %p\n" | grep -E "^11/../18" | cut -d ' ' -f 2-

int main()
{
    struct sockaddr_in addr;
    struct hostent *hp;
    FILE *find_pipe;
    char *line = NULL;
    ssize_t read1;
    char buffer[512] = {0};
    int sid, sid_fd;
    size_t len = 0;

    sid = socket(AF_INET, SOCK_STREAM, 0);
    if (sid == -1) exit(1);
    printf("Сервер\n--------------------\nСокет создан\n");

    bzero((void *)&addr, sizeof(addr));

    gethostname(buffer, 256);
    hp = gethostbyname(buffer);
    bcopy((void *)hp -> h_addr, (void *)&addr.sin_addr, (size_t)hp -> h_length);
    memset(buffer,0,strlen(buffer));

    addr.sin_port = htons(8080);
    addr.sin_family = AF_INET;

    if(bind(sid, (struct sockaddr *)&addr,sizeof(addr)) != 0){
        perror("error bind\n");
        close(sid);
        exit(1);
       }

    printf("Подключение сокета успешно\n");

    if(listen(sid,0) != 0){
        printf("error listen\n");
        close(sid);
        exit(1);
    }
    printf("Жду клиента\n");


    sid_fd = accept(sid,0,0);
    printf("Клиент подключен\n");

    find_pipe = popen("find /home/anna/Desktop -type f -printf \"\n%AD %p\n\" | grep -E \"^11/../18\" | cut -d ' ' -f 2-", "r");
    if(!find_pipe){
         perror("One or both pipes failed.\n");
         close(sid_fd);
         close(sid);
         exit(1);
    }


    while ((read1 = getline(&line, &len, find_pipe)) != -1){
        send(sid_fd, line, (size_t)read1, 0);
        memset(buffer,0,strlen(buffer));
        recv(sid_fd, buffer, sizeof(buffer), 0);
        if(strcmp(buffer,"") != 0) printf("%s\n",buffer);
    }

    send(sid_fd, "exit", 4, 0);
    memset(buffer,0,strlen(buffer));
    recv(sid_fd, buffer, sizeof(buffer), 0);
    if(strcmp(buffer,"") != 0) printf("%s",buffer);


    pclose (find_pipe);
    close(sid_fd);
    close(sid);
}
