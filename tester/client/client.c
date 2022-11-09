#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

int	main(int argc, char **argv)
{
	int					client_socket;
	struct sockaddr_in	server_address; // 구조체는 netinet/in.h 참고
	int					ret;

	if (argc != 2) {
		printf("Usage: ./client [TEST FILE NAME]\n");
		return (0);
	}
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
	int		file_buf_len = 1024;
	char	file_buf[file_buf_len];
	char	recv_buf[2];
	int		buf_len = 1024;
	char	buf[buf_len];
	int		file_fd = open(argv[1], O_RDONLY);
	memset(file_buf, 0, file_buf_len);
	memset(recv_buf, 0, 2);
	memset(buf, 0, buf_len);
	if (file_fd == -1) {
		printf("fail: open()\n");
		close(client_socket);
		exit(EXIT_FAILURE);
	}
	while((ret = read(file_fd, file_buf, file_buf_len - 1)) != 0)
	{	
		if (ret == -1) {
			printf("fail: read()\n");
			close(client_socket);
			exit(EXIT_FAILURE);
		}
		file_buf[ret] = '\0';
		printf("========= request =========\n%s\n", file_buf);
		ret = send(client_socket, file_buf, strlen(file_buf), 0);
		if (ret == -1)
		{
			printf("fail: send()\n");
			close(client_socket);
			exit(EXIT_FAILURE);
		}
		while ((ret = recv(client_socket, recv_buf, 1, 0)) != 0) {
			if (ret == -1)
			{
				printf("fail(%d): recv()\n", errno);
				close(client_socket);
				exit(EXIT_FAILURE);
			}
			recv_buf[ret] = '\0';
			strlcat(buf, recv_buf, buf_len);
		}
		printf("========= response =========\n%s\n", buf);
	}
	close(client_socket);
	return (0);
}
