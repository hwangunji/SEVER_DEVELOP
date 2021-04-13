#include <stdio.h>//전처리를 위한 헤더파일
#include <stdlib.h>//전처리를 위한 헤더파일
#include <unistd.h>//전처리를 위한 헤더파일
#include <string.h>//전처리를 위한 헤더파일
#include <sys/types.h>//socket 생성,listenip 주소 뱐환등을 위한 헤더파일
#include <sys/socket.h>//socket 생성,listen,ip 주소 뱐환등을 위한 헤더파일
#include <netinet/in.h>//ip 주소 뱐환등을 위한 헤더파일
#include <arpa/inet.h>//bind,ip 주소 뱐환등을 위한 헤더파일

int cli_number = 0;//클라이언트 인원 수
int cli_sockets[5];//클라이언트를 각 소켓에 저장하기 위한 정수형 배열

int main(int argc, char** argv){//메인함수
    
    int sd; //서버소켓
    int ns; //클라이언트 소켓
    struct sockaddr_in sin,cli;//서버와 클라이언트의 인터넷 소켓 구조체
    int clientlen=sizeof(cli);//accept시 cli 크기
    int rlen = 0;//recv사용시 수신된 데이터 길이 만큼 리턴
    char buf[256];//메세지가 저장되는 곳
    int i,j;//swich 반복문을 위한 변수들
 
    if (argc != 2) {//잘못된 입력을 했을 때 입력은 실행파일과 포트넘버 두가지가 되어야한다
        //포트번호 command로 받을 수 있도록
        printf("사용 시 %s뒤에 포트넘버를 붙이시오\n", argv[0]);//정정메세지 출력
        exit(1);//if문 벗어난다
    }//if문 끝

    //일.소켓 생성(socket)
    sd = socket(PF_INET, SOCK_STREAM, 0);//서버 소켓파일기술자 생성(fopen과비슷)
    //도메인은 인터넷,통신방식은 TCP사용, 소켓 이용 프로토콜은 영

    //소켓 구조체 생성
    memset(&sin, 0, sizeof(sin));//clear한 상태 만들기
    sin.sin_family = AF_INET;//소켓 구조체 생성
    sin.sin_port = htons(atoi(argv[1]));//소켓 구조체 생성(포트넘버) //포트번호 command로 받을 수 있도록
    sin.sin_addr.s_addr = htonl(INADDR_ANY);//소켓 구조체 생성 ip
    
    // 이.bind 
    bind(sd, (struct sockaddr*) & sin, sizeof(sin));//소켓에 이름 지정하기

    // 삼.listen
    listen(sd,5);//클라이언트 연결 기다리기

    while (1) {//무한 반복한다
        //사.accept 연결요청 수락하기
        ns = accept(sd, (struct sockaddr*) & cli, &clientlen);//클라이언트가 요청이 올 때까지 대기

        switch (fork())//ns를 받은 순간에 fork로 자식 프로세스 생성(동시 동작 서버)
        {//switch시작
        case 0://이 아래는 child가 할 일
            close(sd);//sd가 ns를 공유해서 혼선이 생기지 않도록 일단 닫는다
            cli_sockets[cli_number++] = ns;// accept리턴 값을 클라이언트 소켓 배열에 저장,클라이언트가 여러 개이므로 배열 사용
            // 새로운 사용자가 입장할 때마다 소켓번호 부여
            while ((rlen = recv(ns, buf, sizeof(buf), 0)) != 0){//클라이언트가 보낸 메세지가 존재할 때 까지
                printf("%s", buf);//클라이언트에서 받은 메세지를 받을 때 마다 출력한다
                for (i = 0; i < cli_number; i++)//모든 클라이언트에게
                    send(cli_sockets[i], buf, rlen, 0);//메세지를 보내(같은 메세지임)
            }//while끝

            for (j = 0; j < cli_number; j++) {  /* 클라이언트 연결 종료 시 해당 클라이언트 수를 배열에서 제거  */
                if (ns == cli_sockets[j]) {//해당 클라이어트 일때(종료되는)
                    for (; j < cli_number - 1; j++)//cli_sockets[cli_number - 일]까지
                        cli_sockets[j] = cli_sockets[j + 1];//메시지를 보낸 클라이언트의 소켓을 정렬
                    break;//끝나면 나감
                }//if끝
            }//for끝
            cli_number--;//클라이언트 종료 되었으니 클라이언트 개수 감소           
        }//switch끝
        close(ns);//parent는 다시 while문으로 돌아간다
    }//while끝

    return 0;//int형 main함수의 반환
}//main끝
