
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int	main(void)
{
	int					client_socket;
	struct sockaddr_in	server_address; // 구조체는 netinet/in.h 참고
	int					ret;

	// int socket(int domain, int type, int protocol);
	// 1. domain: Protocol family - define 종류는 sys/socket.h 내용과 주석 참고
	// 2. type: 데이터 전송방식, define 종류는 sys/socket.h 내용과 주석 참고
	// 3. protocol: domain과 type에 의해 결정되기 때문에 항상 0
	// return: 새 소켓의 fd, 오류는 -1
	client_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (client_socket == -1)
	{
		printf("fail: socket()\n");
		exit(EXIT_FAILURE);
	}

	// int connect(int socket, const struct sockaddr *address, socklen_t address_len);
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_len = sizeof(struct sockaddr_in);
	server_address.sin_family = PF_INET;
	server_address.sin_port = htons(4242);
	server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
	ret = connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address));
	if (ret == -1)
	{
		printf("fail: connect()\n");
		printf("errno: %d\n", errno);
		close(client_socket);
		exit(EXIT_FAILURE);
	}
	// send, recv
	// ssize_t send(int socket, const void *buffer, size_t length, int flags);
	const char	*str_buffer = "1234567890";
	int			scan_buf_len = 1024;
	char		*scan_buf = (char *)malloc(sizeof(char) * scan_buf_len);
	int			buf_len = 5;
	char		*buf = (char *)malloc(sizeof(char) * buf_len + 1);
	printf("Input >");
	while(scanf("%s", scan_buf) != EOF)
	{	
		ret = send(client_socket, scan_buf, strlen(scan_buf), 0);
		if (ret == -1)
		{
			printf("fail: send()\n");
			close(client_socket);
			exit(EXIT_FAILURE);
		}
		int received_len = 0;
		while (received_len < strlen(scan_buf))
		{
			ret = recv(client_socket, buf, buf_len, 0);
			received_len += ret;
			if (ret == -1)
			{
				printf("fail(%d): recv()\n", errno);
				close(client_socket);
				exit(EXIT_FAILURE);
			}
			buf[ret] = '\0';
			printf("client received(%d): %s\n", ret, buf);
		}
	}
	close(client_socket);
	return (0);
}
