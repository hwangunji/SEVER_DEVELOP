#include <stdio.h> //표준입출력라이브러리
#include <stdlib.h> //표준입출력라이브러리
#include <unistd.h> //fork사용라이브러리
#include <errno.h> //오류코드 매크로 정의
#include <string.h> //문자열처리 라이브러리
#include <fcntl.h> //파일관련 라이브러리
#include <signal.h> //시그널처리 라이브러리
#include <sys/types.h> //시스템관련 라이브러리
#include <sys/socket.h> //네트워크통신 라이브러리
#include <netinet/in.h> //인터넷주소체계 사용 라이브러리
#include <arpa/inet.h> //버클리소켓사용 라이브러리
#include <sys/stat.h> // 파일정보 라이브러리
#include <pthread.h> // 쓰레드 라이블러리
#define BUFSIZE 1024 // 버프사이즈 정의
#define LOG   44 //로그 정의
#define HOME /index.html //포트접속 시 띄울 페이지
struct stat s; //아래에서 파일 크기를 구하기 위해서 사용함 
int lol;
pthread_mutex_t m_lock;// 뮤텍스 정보 저장
typedef struct num{//구조체 선언
   int nn;
   char *buff;
   int num;
}num2;

struct {//구조체 선언
   char *ext;
   char *filetype;
} extensions [] = {
   {"gif", "image/gif" },  //gif 
	{"jpg", "image/jpg"},    //jpg
	{"jpeg","image/jpeg"},   //jpeg
	{"png", "image/png" },  //png
	{"htm", "text/html" },  //htm
	{"html","text/html" },  //html
	{0,0} };//NULL

int cgi(char *buf){
   char *result; //자르고 NNN,MMM담는 변수
	char token[]="=&";//자를 때 필요 
   int i; 
	int n1,n2;//각각 NNN, MMM담는 변수 
   int sum=0;//int형 변수 sum선언 

   result=strtok(buf,token); // =이전의 문자열 자르기
	result=strtok(NULL,token); // =,&사이의 NNN 잘라서 저장
   n1=atoi(result); //result를 정수형으로 n1에 저장 
   strtok(NULL,token); // &,=사이의 to 자르기
   result=strtok(NULL,token);// &,=사이의 MMM 잘라서 저장
	n2=atoi(result);//result를 정수형으로 n2에 저장 

   i=-(n1-n2)+1;// 더하는 원소 갯수 판별
   int half=i/2;//i를 반절로 
   if(i&1){//변경 후 %2대신 &1사용해서 홀수 인지 짝수인지 
      sum=(n1+n2)*half;//cgi계산 
		sum+=n1+half;//cgi 계산
   }
   else
      sum=(n1+n2)*half;//cgi 계산 
	   return sum;//계산 결과를 리턴 
   }
   
void log(char *s1, char *s2, int size){//로그작 성
	int fpp;//로그에 쓰기 위해서 선언 
	char logbuffer[200];//로그 버퍼 
	sprintf(logbuffer,"%s %s %d\n",s1, s2,size); //s0=send/req, s1= ip ,s2= path/filename , size=크기,num=숫자    
	if((fpp= open("./log.txt",O_WRONLY | O_APPEND,0644)) >= 0) {// 파일을 연다
		write(fpp,logbuffer,strlen(logbuffer)); //버퍼의 내용을 로그에 작성한다
		close(fpp);//파일 닫기
	}
} 
   
   
void *web(void *n1){// 기존 main함수에 있던 동작을 함수로 만듦
   
   struct num *n2 = (struct num *)n1;
   char logbuffer[BUFSIZE];
   int *fd = n2->nn;
   char *buff=n2->buff;
   char file_name[BUFSIZ];//파일 이름
   int size;//파일 크기를 구하기 위한 변수 
   int j, file_fd, buflen, len;
   int i, ret;
   char * fstr;
   char buffer[BUFSIZE+1];   

   ret=read(fd,buffer,BUFSIZE); //fd에서 계속 읽어옴  
  
   
   if(ret == 0 || ret == -1) {//읽기 실패하면
      close(fd);//로그파일을 닫아준다.
      return 0;   }
   if(ret > 0 && ret < BUFSIZE) //ret이 0보다 크고 BUFSIZE보다 작으면  
      buffer[ret]=0;   //buffer[ret]은 0이 된다.
      else buffer[0]=0;//위를 만족하지 않는다면 buffer[0]=0이된다. 
      for(i=4;i<BUFSIZE;i++) { //GET /images/05_08-over.gif 이런식으로 만들어줌 
         if(buffer[i] == ' ') { //공백을 확인
            buffer[i] = 0;//공백일때 0
            break;//for문 나가기
         }
      }
        printf("%s\n",buffer);//로그출력

      // IP:19000 으로 요청을 했을 때 기본파일인 index.html을 돌려줌
      if( !strncmp(&buffer[0],"GET /\0",6)) //GET /\0일때 //strncmp => 문자열비교함수 (비교할문자1, 비교할문자2, 비교할 문자열 길이)  
         strcpy(buffer,"GET /index.html");   //strcpy => 문자열복사 함수 //주소:포트로 접속 시 index.html 응답 
 
      
      buflen=strlen(buffer);// buflen에 buffer길이 저장
      fstr = (char *)0;// 초기화
      // 이미지 파일을 요청 시 이미지를 돌려줌
      for(i=0;extensions[i].ext != 0;i++) {// 구조체 내 탐색
         len = strlen(extensions[i].ext);// 길이지정//strcpy => 문자열복사 함수 //주소:포트로 접속 시 index.html 응답
         if( !strncmp(&buffer[buflen-len], extensions[i].ext, len)) {//gif 이미지 요청시 // 지정한 문자 갯수까지만 비교
            fstr =extensions[i].filetype; //gif형식이면 image/gif로 
             printf("%s   \n\n",fstr);//출력
            break;//for문 나가기
         }
      }

      strcpy(file_name,&buffer[5]); //strcpy => 문자열복사 함수 //gif 이미지 요청시
      file_fd = open(file_name,O_RDONLY); //get을 떼어네고 파일을 열어봄 없는 파일일때 
      fstat(file_fd,&s);//폴더인지 아닌지 확인하기 위해서 
      if(file_fd==-1){//클라이언트의 요청이 파일이 아닐 시 
         if(strstr(file_name,"&to")){ // 요청 파라메터값에 to가 있다면 cgi요청으로 판단하여 cgi를 돌려줌
            int n=cgi(file_name);//파일내용을 cgi에 넣고 cgi의 리턴값을 받아 n에 담기
            sprintf(buffer,"HTTP/1.1 200 OK\nContent-Type: %s\n\n", fstr); /* Header + a blank line *///NNN ~ MMM 합을 보여주는 페이지를 클라이언트에게 응답

            write(fd,buffer,strlen(buffer));//socketfd에 버퍼를 써준다 //write => 파일 쓰기 함수
            sprintf(buffer, "<HTML><BODY><H1>%d</H1></BODY></HTML>\r\n",n);//NNN ~ MMM 합을 보여주는 페이지를 클라이언트에게 응답
            write(fd,buffer,strlen(buffer));//버퍼의 내용을 fd에 써준다
             close(fd);//fd닫는다.
         free(n1); //해제  
         close(file_fd); //닫고  
           return 0;         }
         else{
         // 요청한 파일이 없을 시 NOT FOUND를 돌려줌
         write(fd,"HTTP/1.1 404 Found",18); 
         sprintf(buffer, "<HTML><BODY><H1>NOT FOUND</H1></BODY></HTML>\r\n");   //요청한 파일이 없을 시 NOT FOUND보여주기   
         write(fd,buffer,strlen(buffer));//버퍼의 내용을 fd에 써준다. 
         
       //파일닫고 종료	
        close(fd);
         free(n1);//해제  
         close(file_fd);//(file_fd)파일닫기
         return 0;//반환  
      }
  
      }
      // 요청한 파일이 없을 시 NOT FOUND를 돌려줌
      else if(S_ISDIR(s.st_mode)){//클라이언트의 요청이 파일일때
         write(fd,"HTTP/1.1 404 Found",18);   // 클라이언트가 요청한 파일이 없을 시 NOT FOUND를 돌려줌
         sprintf(buffer, "<HTML><BODY><H1>NOT FOUND</H1></BODY></HTML>\r\n");//요청한 파일이 없을때 NOT FOUND   
         write(fd,buffer,strlen(buffer));//버퍼의 내용을 fd에 써준다.
         close(fd);//파일 닫고 종료
         free(n1);//해 제  
         close(file_fd);//(file_fd)파일닫기
         return 0;  //반환   
      } 
      // 요청파일이 있을 시 요청한 파일을 돌려줌
      sprintf(buffer,"HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", fstr);//200으로 헤더를 설정 //200은 요청에 성공을 의미 
      size=s.st_size;
      sprintf(logbuffer,"%s %s %d\n",buff,file_name, size);//요청한 파일의 로그 파일에 저장  
      write(fd,buffer,strlen(buffer));//버퍼의 내용을 fd에 써준다.
     write(lol,logbuffer,strlen(logbuffer));//로그내용을 lol에 쓴다 
      while ((ret = read(file_fd, buffer, BUFSIZE)) > 0 ) {
         write(fd,buffer,ret);
      }
      
   close(fd);//fd파일닫기
   free(n1);//해제
   close(file_fd);//(file_fd)파일닫기
   return 0;//반환
}


int main(int argc, char *argv[])//메인 함수
{
   pthread_t tid; // 쓰레드 변수
   num2 *n1; // 클라이언트 접속 확인 시 사용
   int port; // 포트 정보
   struct sockaddr_in sin,cli; // 클라이언트 정보
   int sd,clientlen=sizeof(cli); // 클라이언 정보 사이즈
   // 로그파일
   lol = open("./log.txt", O_CREAT| O_WRONLY | O_TRUNC,0644); // log.txt 파일이 없으면 생성하여 작성 있으면 지우고 생성
   if(chdir(argv[1]) == -1){ // 요청파일의 경로를 찾지 못했을 시 
      printf("ERROR: Can't Change to directory %s\n",argv[1]); // 에러로그 작성
      exit(4); // 나감
   }
   (void)signal(SIGCLD, SIG_IGN); // 자식프로세스중 하나라도 종료되면 부모에게 신호전달
   (void)signal(SIGHUP, SIG_IGN); // 사용자 터미널의 단절 보고
   if((sd=socket(AF_INET,SOCK_STREAM,0))<0){ // 소켓 생성
      perror("socket");//없으면 ㅇ류메세지알림
   }
   port = atoi(argv[2]); //입력받은 포트값을 port에 저장 
   memset((char*)&sin,'\0',sizeof(sin)); //소켓주소로 초기화
   sin.sin_family=AF_INET; //주소체계를 저장
   sin.sin_port=htons(port); //포트 정보를 저장
   sin.sin_addr.s_addr=htons(INADDR_ANY); //포트정보를 저장
	
   if(bind(sd,(struct sockaddr *)&sin,sizeof(sin))<0){ // 소켓 바인딩
      perror("bind");//없으면 오류메세지알림
      return 0;
   }
   if(listen(sd,100)<0){ // 클라이언트 리슨
      perror("listen");//아니면 오류메세지알림
      return 0;
   }
    
   while(1){ // 위에서 만든 함수(기존 main함수에서 동작하던 코드)를 한 작업당 한 쓰레드로 수행
      pthread_mutex_init(&m_lock, NULL); // 뮤텍스 활성
      n1=(num2 *)malloc(sizeof(num2)); // 메모리 동적할당
      if((n1->nn=accept(sd,(struct sockaddr *)&cli,&clientlen))<0){ // 클라이언트 연결
         perror("accept");// 오류메세지알림
         exit(1);//나감 
      }
      
      n1->buff=inet_ntoa(cli.sin_addr); //클라이언트 IP를 네트워크 바이트 정렬 방식의 4바이트 정수로 바꿔서 변수에 담기 
      pthread_create(&tid,NULL,web,n1); // 쓰레드 만듦
      pthread_detach(tid); // 쓰레드가 종료되면 자원반납
      pthread_mutex_destroy(&m_lock); // 뮤텍스 삭제
   }
	close(lol);//로그파일 닫기
   return 0;

}//2016039069 최종완성  파일  

