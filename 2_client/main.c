#include<stdio.h>       // perror(), printf(),
#include<string.h>      // memset(), strlen(), strcmp(), bzero(), bcopy()
#include<unistd.h>      // chdir(), gethostname(), access(), close()
#include<stdlib.h>      // exit()
//#include<sys/types.h>
//#include<sys/socket.h>
//#include<netinet/in.h>
#include<netdb.h> // socket(), connect(), htons(), recv(), send()

int main()
{
    struct sockaddr_in kaddr;
    struct hostent *hp;
    int sid, num = 0;
    char buffer[512] = {0};

    sid = socket(AF_INET, SOCK_STREAM,0);
    if (sid == -1) exit(1);
    printf("Клиент\n-----------------\nСокет создан\n");

    bzero((void *)&kaddr, sizeof(kaddr));

    gethostname(buffer, 256);
    hp=gethostbyname(buffer);
    bcopy(hp -> h_addr, (struct sockaddr *)&kaddr.sin_addr, (size_t)hp -> h_length);
    memset(buffer,0,strlen(buffer));

    kaddr.sin_port = htons(8080);
    kaddr.sin_family = AF_INET;

    int i;
    for(i = 0; i<10; ++i){
        if(connect(sid, (struct sockaddr *)&kaddr, sizeof(kaddr)) != 0) sleep(1);
        else break;
    }
    if(i == 10){
        perror("ошибка соединения с гнездом сервера\n");
        close(sid);
        exit(1);
    }

    printf("Подключение к серверу успешно\n");
    printf("Жду сообщения\n");

    while((recv(sid, buffer, sizeof(buffer), 0) != -1 && strcmp(buffer,"exit") != 0)){
        buffer[strlen(buffer)-1] = 0;
        printf("%s\n",buffer);
        if(!access(buffer, 1)){
            send(sid, buffer, sizeof(buffer), 0);
            ++num;
        } else send(sid, "", 1, 0);
        memset(buffer,0,strlen(buffer));
    }

    if(num == 0) send(sid, "\nНет файлов с правами на выполнение\n", 100, 0);

    close(sid);	//закрываем дескриптор гнезда
}
