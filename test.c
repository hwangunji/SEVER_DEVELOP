#include <stdio.h> //ǥ������¶��̺귯��
#include <stdlib.h> //ǥ������¶��̺귯��
#include <unistd.h> //fork�����̺귯��
#include <errno.h> //�����ڵ� ��ũ�� ����
#include <string.h> //���ڿ�ó�� ���̺귯��
#include <fcntl.h> //���ϰ��� ���̺귯��
#include <signal.h> //�ñ׳�ó�� ���̺귯��
#include <sys/types.h> //�ý��۰��� ���̺귯��
#include <sys/socket.h> //��Ʈ��ũ��� ���̺귯��
#include <netinet/in.h> //���ͳ��ּ�ü�� ��� ���̺귯��
#include <arpa/inet.h> //��Ŭ�����ϻ�� ���̺귯��
#include <sys/stat.h> // �������� ���̺귯��
#include <pthread.h> // ������ ���̺���
#define BUFSIZE 1024 // ���������� ����
#define LOG   44 //�α� ����
#define HOME /index.html //��Ʈ���� �� ��� ������
struct stat s; //�Ʒ����� ���� ũ�⸦ ���ϱ� ���ؼ� ����� 
int lol;
pthread_mutex_t m_lock;// ���ؽ� ���� ����
typedef struct num{//����ü ����
   int nn;
   char *buff;
   int num;
}num2;

struct {//����ü ����
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
   char *result; //�ڸ��� NNN,MMM��� ����
	char token[]="=&";//�ڸ� �� �ʿ� 
   int i; 
	int n1,n2;//���� NNN, MMM��� ���� 
   int sum=0;//int�� ���� sum���� 

   result=strtok(buf,token); // =������ ���ڿ� �ڸ���
	result=strtok(NULL,token); // =,&������ NNN �߶� ����
   n1=atoi(result); //result�� ���������� n1�� ���� 
   strtok(NULL,token); // &,=������ to �ڸ���
   result=strtok(NULL,token);// &,=������ MMM �߶� ����
	n2=atoi(result);//result�� ���������� n2�� ���� 

   i=-(n1-n2)+1;// ���ϴ� ���� ���� �Ǻ�
   int half=i/2;//i�� ������ 
   if(i&1){//���� �� %2��� &1����ؼ� Ȧ�� ���� ¦������ 
      sum=(n1+n2)*half;//cgi��� 
		sum+=n1+half;//cgi ���
   }
   else
      sum=(n1+n2)*half;//cgi ��� 
	   return sum;//��� ����� ���� 
   }
   
void log(char *s1, char *s2, int size){//�α��� ��
	int fpp;//�α׿� ���� ���ؼ� ���� 
	char logbuffer[200];//�α� ���� 
	sprintf(logbuffer,"%s %s %d\n",s1, s2,size); //s0=send/req, s1= ip ,s2= path/filename , size=ũ��,num=����    
	if((fpp= open("./log.txt",O_WRONLY | O_APPEND,0644)) >= 0) {// ������ ����
		write(fpp,logbuffer,strlen(logbuffer)); //������ ������ �α׿� �ۼ��Ѵ�
		close(fpp);//���� �ݱ�
	}
} 
   
   
void *web(void *n1){// ���� main�Լ��� �ִ� ������ �Լ��� ����
   
   struct num *n2 = (struct num *)n1;
   char logbuffer[BUFSIZE];
   int *fd = n2->nn;
   char *buff=n2->buff;
   char file_name[BUFSIZ];//���� �̸�
   int size;//���� ũ�⸦ ���ϱ� ���� ���� 
   int j, file_fd, buflen, len;
   int i, ret;
   char * fstr;
   char buffer[BUFSIZE+1];   

   ret=read(fd,buffer,BUFSIZE); //fd���� ��� �о��  
  
   
   if(ret == 0 || ret == -1) {//�б� �����ϸ�
      close(fd);//�α������� �ݾ��ش�.
      return 0;   }
   if(ret > 0 && ret < BUFSIZE) //ret�� 0���� ũ�� BUFSIZE���� ������  
      buffer[ret]=0;   //buffer[ret]�� 0�� �ȴ�.
      else buffer[0]=0;//���� �������� �ʴ´ٸ� buffer[0]=0�̵ȴ�. 
      for(i=4;i<BUFSIZE;i++) { //GET /images/05_08-over.gif �̷������� ������� 
         if(buffer[i] == ' ') { //������ Ȯ��
            buffer[i] = 0;//�����϶� 0
            break;//for�� ������
         }
      }
        printf("%s\n",buffer);//�α����

      // IP:19000 ���� ��û�� ���� �� �⺻������ index.html�� ������
      if( !strncmp(&buffer[0],"GET /\0",6)) //GET /\0�϶� //strncmp => ���ڿ����Լ� (���ҹ���1, ���ҹ���2, ���� ���ڿ� ����)  
         strcpy(buffer,"GET /index.html");   //strcpy => ���ڿ����� �Լ� //�ּ�:��Ʈ�� ���� �� index.html ���� 
 
      
      buflen=strlen(buffer);// buflen�� buffer���� ����
      fstr = (char *)0;// �ʱ�ȭ
      // �̹��� ������ ��û �� �̹����� ������
      for(i=0;extensions[i].ext != 0;i++) {// ����ü �� Ž��
         len = strlen(extensions[i].ext);// ��������//strcpy => ���ڿ����� �Լ� //�ּ�:��Ʈ�� ���� �� index.html ����
         if( !strncmp(&buffer[buflen-len], extensions[i].ext, len)) {//gif �̹��� ��û�� // ������ ���� ���������� ��
            fstr =extensions[i].filetype; //gif�����̸� image/gif�� 
             printf("%s   \n\n",fstr);//���
            break;//for�� ������
         }
      }

      strcpy(file_name,&buffer[5]); //strcpy => ���ڿ����� �Լ� //gif �̹��� ��û��
      file_fd = open(file_name,O_RDONLY); //get�� ����װ� ������ ��� ���� �����϶� 
      fstat(file_fd,&s);//�������� �ƴ��� Ȯ���ϱ� ���ؼ� 
      if(file_fd==-1){//Ŭ���̾�Ʈ�� ��û�� ������ �ƴ� �� 
         if(strstr(file_name,"&to")){ // ��û �Ķ���Ͱ��� to�� �ִٸ� cgi��û���� �Ǵ��Ͽ� cgi�� ������
            int n=cgi(file_name);//���ϳ����� cgi�� �ְ� cgi�� ���ϰ��� �޾� n�� ���
            sprintf(buffer,"HTTP/1.1 200 OK\nContent-Type: %s\n\n", fstr); /* Header + a blank line *///NNN ~ MMM ���� �����ִ� �������� Ŭ���̾�Ʈ���� ����

            write(fd,buffer,strlen(buffer));//socketfd�� ���۸� ���ش� //write => ���� ���� �Լ�
            sprintf(buffer, "<HTML><BODY><H1>%d</H1></BODY></HTML>\r\n",n);//NNN ~ MMM ���� �����ִ� �������� Ŭ���̾�Ʈ���� ����
            write(fd,buffer,strlen(buffer));//������ ������ fd�� ���ش�
             close(fd);//fd�ݴ´�.
         free(n1); //����  
         close(file_fd); //�ݰ�  
           return 0;         }
         else{
         // ��û�� ������ ���� �� NOT FOUND�� ������
         write(fd,"HTTP/1.1 404 Found",18); 
         sprintf(buffer, "<HTML><BODY><H1>NOT FOUND</H1></BODY></HTML>\r\n");   //��û�� ������ ���� �� NOT FOUND�����ֱ�   
         write(fd,buffer,strlen(buffer));//������ ������ fd�� ���ش�. 
         
       //���ϴݰ� ����	
        close(fd);
         free(n1);//����  
         close(file_fd);//(file_fd)���ϴݱ�
         return 0;//��ȯ  
      }
  
      }
      // ��û�� ������ ���� �� NOT FOUND�� ������
      else if(S_ISDIR(s.st_mode)){//Ŭ���̾�Ʈ�� ��û�� �����϶�
         write(fd,"HTTP/1.1 404 Found",18);   // Ŭ���̾�Ʈ�� ��û�� ������ ���� �� NOT FOUND�� ������
         sprintf(buffer, "<HTML><BODY><H1>NOT FOUND</H1></BODY></HTML>\r\n");//��û�� ������ ������ NOT FOUND   
         write(fd,buffer,strlen(buffer));//������ ������ fd�� ���ش�.
         close(fd);//���� �ݰ� ����
         free(n1);//�� ��  
         close(file_fd);//(file_fd)���ϴݱ�
         return 0;  //��ȯ   
      } 
      // ��û������ ���� �� ��û�� ������ ������
      sprintf(buffer,"HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", fstr);//200���� ����� ���� //200�� ��û�� ������ �ǹ� 
      size=s.st_size;
      sprintf(logbuffer,"%s %s %d\n",buff,file_name, size);//��û�� ������ �α� ���Ͽ� ����  
      write(fd,buffer,strlen(buffer));//������ ������ fd�� ���ش�.
     write(lol,logbuffer,strlen(logbuffer));//�α׳����� lol�� ���� 
      while ((ret = read(file_fd, buffer, BUFSIZE)) > 0 ) {
         write(fd,buffer,ret);
      }
      
   close(fd);//fd���ϴݱ�
   free(n1);//����
   close(file_fd);//(file_fd)���ϴݱ�
   return 0;//��ȯ
}


int main(int argc, char *argv[])//���� �Լ�
{
   pthread_t tid; // ������ ����
   num2 *n1; // Ŭ���̾�Ʈ ���� Ȯ�� �� ���
   int port; // ��Ʈ ����
   struct sockaddr_in sin,cli; // Ŭ���̾�Ʈ ����
   int sd,clientlen=sizeof(cli); // Ŭ���̾� ���� ������
   // �α�����
   lol = open("./log.txt", O_CREAT| O_WRONLY | O_TRUNC,0644); // log.txt ������ ������ �����Ͽ� �ۼ� ������ ����� ����
   if(chdir(argv[1]) == -1){ // ��û������ ��θ� ã�� ������ �� 
      printf("ERROR: Can't Change to directory %s\n",argv[1]); // �����α� �ۼ�
      exit(4); // ����
   }
   (void)signal(SIGCLD, SIG_IGN); // �ڽ����μ����� �ϳ��� ����Ǹ� �θ𿡰� ��ȣ����
   (void)signal(SIGHUP, SIG_IGN); // ����� �͹̳��� ���� ����
   if((sd=socket(AF_INET,SOCK_STREAM,0))<0){ // ���� ����
      perror("socket");//������ �����޼����˸�
   }
   port = atoi(argv[2]); //�Է¹��� ��Ʈ���� port�� ���� 
   memset((char*)&sin,'\0',sizeof(sin)); //�����ּҷ� �ʱ�ȭ
   sin.sin_family=AF_INET; //�ּ�ü�踦 ����
   sin.sin_port=htons(port); //��Ʈ ������ ����
   sin.sin_addr.s_addr=htons(INADDR_ANY); //��Ʈ������ ����
	
   if(bind(sd,(struct sockaddr *)&sin,sizeof(sin))<0){ // ���� ���ε�
      perror("bind");//������ �����޼����˸�
      return 0;
   }
   if(listen(sd,100)<0){ // Ŭ���̾�Ʈ ����
      perror("listen");//�ƴϸ� �����޼����˸�
      return 0;
   }
    
   while(1){ // ������ ���� �Լ�(���� main�Լ����� �����ϴ� �ڵ�)�� �� �۾��� �� ������� ����
      pthread_mutex_init(&m_lock, NULL); // ���ؽ� Ȱ��
      n1=(num2 *)malloc(sizeof(num2)); // �޸� �����Ҵ�
      if((n1->nn=accept(sd,(struct sockaddr *)&cli,&clientlen))<0){ // Ŭ���̾�Ʈ ����
         perror("accept");// �����޼����˸�
         exit(1);//���� 
      }
      
      n1->buff=inet_ntoa(cli.sin_addr); //Ŭ���̾�Ʈ IP�� ��Ʈ��ũ ����Ʈ ���� ����� 4����Ʈ ������ �ٲ㼭 ������ ��� 
      pthread_create(&tid,NULL,web,n1); // ������ ����
      pthread_detach(tid); // �����尡 ����Ǹ� �ڿ��ݳ�
      pthread_mutex_destroy(&m_lock); // ���ؽ� ����
   }
	close(lol);//�α����� �ݱ�
   return 0;

}//2016039069 �����ϼ�  ����  

