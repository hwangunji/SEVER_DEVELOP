#include <stdio.h>//��ó���� ���� �������
#include <stdlib.h>//��ó���� ���� �������
#include <unistd.h>//��ó���� ���� �������
#include <string.h>//��ó���� ���� �������
#include <sys/types.h>//socket ����,listenip �ּ� ��ȯ���� ���� �������
#include <sys/socket.h>//socket ����,listen,ip �ּ� ��ȯ���� ���� �������
#include <netinet/in.h>//ip �ּ� ��ȯ���� ���� �������
#include <arpa/inet.h>//bind,ip �ּ� ��ȯ���� ���� �������

int cli_number = 0;//Ŭ���̾�Ʈ �ο� ��
int cli_sockets[5];//Ŭ���̾�Ʈ�� �� ���Ͽ� �����ϱ� ���� ������ �迭

int main(int argc, char** argv){//�����Լ�
    
    int sd; //��������
    int ns; //Ŭ���̾�Ʈ ����
    struct sockaddr_in sin,cli;//������ Ŭ���̾�Ʈ�� ���ͳ� ���� ����ü
    int clientlen=sizeof(cli);//accept�� cli ũ��
    int rlen = 0;//recv���� ���ŵ� ������ ���� ��ŭ ����
    char buf[256];//�޼����� ����Ǵ� ��
    int i,j;//swich �ݺ����� ���� ������
 
    if (argc != 2) {//�߸��� �Է��� ���� �� �Է��� �������ϰ� ��Ʈ�ѹ� �ΰ����� �Ǿ���Ѵ�
        //��Ʈ��ȣ command�� ���� �� �ֵ���
        printf("��� �� %s�ڿ� ��Ʈ�ѹ��� ���̽ÿ�\n", argv[0]);//�����޼��� ���
        exit(1);//if�� �����
    }//if�� ��

    //��.���� ����(socket)
    sd = socket(PF_INET, SOCK_STREAM, 0);//���� �������ϱ���� ����(fopen�����)
    //�������� ���ͳ�,��Ź���� TCP���, ���� �̿� ���������� ��

    //���� ����ü ����
    memset(&sin, 0, sizeof(sin));//clear�� ���� �����
    sin.sin_family = AF_INET;//���� ����ü ����
    sin.sin_port = htons(atoi(argv[1]));//���� ����ü ����(��Ʈ�ѹ�) //��Ʈ��ȣ command�� ���� �� �ֵ���
    sin.sin_addr.s_addr = htonl(INADDR_ANY);//���� ����ü ���� ip
    
    // ��.bind
    bind(sd, (struct sockaddr*) & sin, sizeof(sin));//���Ͽ� �̸� �����ϱ�

    // ��.listen
    listen(sd,5);//Ŭ���̾�Ʈ ���� ��ٸ���

    while (1) {//���� �ݺ��Ѵ�
        //��.accept �����û �����ϱ�
        ns = accept(sd, (struct sockaddr*) & cli, &clientlen);//Ŭ���̾�Ʈ�� ��û�� �� ������ ���

        switch (fork())//ns�� ���� ������ fork�� �ڽ� ���μ��� ����(���� ���� ����)
        {//switch����
        case 0://�� �Ʒ��� child�� �� ��
            close(sd);//sd�� ns�� �����ؼ� ȥ���� ������ �ʵ��� �ϴ� �ݴ´�
            cli_sockets[cli_number++] = ns;// accept���� ���� Ŭ���̾�Ʈ ���� �迭�� ����,Ŭ���̾�Ʈ�� ���� ���̹Ƿ� �迭 ���
            // ���ο� ����ڰ� ������ ������ ���Ϲ�ȣ �ο�
            while ((rlen = recv(ns, buf, sizeof(buf), 0)) != 0){//Ŭ���̾�Ʈ�� ���� �޼����� ������ �� ����
                printf("%s", buf);//Ŭ���̾�Ʈ���� ���� �޼����� ���� �� ���� ����Ѵ�
                for (i = 0; i < cli_number; i++)//��� Ŭ���̾�Ʈ����
                    send(cli_sockets[i], buf, rlen, 0);//�޼����� ����(���� �޼�����)
            }//while��

            for (j = 0; j < cli_number; j++) {  /* Ŭ���̾�Ʈ ���� ���� �� �ش� Ŭ���̾�Ʈ ���� �迭���� ����  */
                if (ns == cli_sockets[j]) {//�ش� Ŭ���̾�Ʈ �϶�(����Ǵ�)
                    for (; j < cli_number - 1; j++)//cli_sockets[cli_number - ��]����
                        cli_sockets[j] = cli_sockets[j + 1];//�޽����� ���� Ŭ���̾�Ʈ�� ������ ����
                    break;//������ ����
                }//if��
            }//for��
            cli_number--;//Ŭ���̾�Ʈ ���� �Ǿ����� Ŭ���̾�Ʈ ���� ����           
        }//switch��
        close(ns);//parent�� �ٽ� while������ ���ư���
    }//while��

    return 0;//int�� main�Լ��� ��ȯ
}//main��