#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUFSIZE 100
#define NAMESIZE 20

void *send_message (void *arg);
void *recv_message (void *arg);
void error_handling (char *message);

char name[NAMESIZE] = "[Default]";
char message[BUFSIZE];

int main (int argc, char **argv)
{
	int sock;
	struct sockaddr_in serv_addr;
	pthread_t snd_thread, rcv_thread;
	void *thread_result;

	printf ("================================\n");
	printf ("TCP/IP Chatting Program - Client\n");
	printf ("================================\n\n");

	if (argc != 4)
	{
		printf ("Usa%s <IP> <port> <name>\n", argv[0]);
		exit (1);
	}

	sprintf (name, "[%s]", argv[3]);

	sock = socket (PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		error_handling ("socket() error");

	memset (&serv_addr, 0, sizeof (serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr (argv[1]);
	serv_addr.sin_port = htons (atoi (argv[2]));

	if (connect (sock, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) == -1)
		error_handling ("connect() error");

	pthread_create (&snd_thread, NULL, send_message, (void *) sock);
	pthread_create (&rcv_thread, NULL, recv_message, (void *) sock);

	pthread_join (snd_thread, &thread_result);
	pthread_join (rcv_thread, &thread_result);

	close (sock);
	return 0;
}

void *send_message (void *arg)
{
	int sock = (int) arg;
	char name_message[NAMESIZE + BUFSIZE];
	while (1)
	{
		fgets (message, BUFSIZE, stdin);
		if (!strcmp (message, "q\n"))
		{
			close (sock);
			exit (0);
		}
		sprintf (name_message, "%s %s", name, message);
		send (sock, name_message, strlen (name_message), 0);
	}
}

void *recv_message (void *arg)
{
	int sock = (int) arg;
	char name_message[NAMESIZE + BUFSIZE];
	int str_len;
	int f_read = 1;

	while (f_read)
	{
		str_len = recv (sock, name_message, NAMESIZE + BUFSIZE - 1, 0);
		if (str_len == -1)
			f_read = 0;
		else
		{
			name_message[str_len] = 0;
			fputs (name_message, stdout);
		}
	}
}

void error_handling (char *message)
{
	fputs (message, stderr);
	fputc ('\n', stderr);
	exit (1);
}
