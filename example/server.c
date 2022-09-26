

// 1. socket() API가 종료점을 나타내는 소켓 설명자를 리턴합니다. 이 명령문은 또한 이 소켓에 TCP 전송(SOCK_STREAM)을 사용하는 AE_INET6(Internet Protocol version 6) 주소 계열이 사용됨을 식별합니다.
// 2. setsockopt() API는 필수 대기 시간이 만료되기 전에 서버가 다시 시작되면 애플리케이션이 로컬 주소를 다시 사용할 수 있도록 합니다.
// 3. ioctl() API가 소켓을 비차단으로 설정합니다. 수신 연결에 대한 모든 소켓은 청취 소켓으로부터 해당 상태를 상속하므로 이들 또한 비차단입니다.
// 4. 소켓 설명자가 작성된 후에는 bind() API가 소켓의 고유 이름을 가져옵니다.
// 5. listen() API 호출은 서버가 수신 클라이언트 연결을 승인할 수 있도록 합니다.
// 6. poll() API는 프로세스가 이벤트가 발생하기를 기다리고, 이벤트가 발생하면 프로세스를 웨이크업할 수 있도록 합니다. poll() API는 다음 값 중 하나를 리턴합니다.
	// `0`
	// 프로세스가 제한시간을 초과했음을 표시합니다. 이 예제에서는 제한시간이 3분(밀리초 단위)으로 설정되어 있습니다.
	// `-1`
	// 프로세스가 실패했음을 표시합니다.
	// `1`
	// 하나의 설명자만 처리할 준비가 되었음을 표시하며, 이 설명자는 청취 소켓인 경우에만 처리됩니다.
	// 1보다 큰 값
	// 복수 설명자가 처리되기를 기다리고 있음을 표시합니다. poll() API는 청취 소켓의 큐에 있는 모든 설명자에 대한 동시 연결을 허용합니다.
// 7. accept() 및 recv() API는 EWOULDBLOCK이 리턴될 때 완료됩니다.
// 8. send() API가 이 데이터를 클라이언트에 다시 송신합니다.
// 9. close() API가 모든 열린 소켓 설명자를 닫습니다.

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
	int					server_socket;
	int					client_socket;
	struct sockaddr_in	server_address; // 구조체는 netinet/in.h 참고
	struct sockaddr_in	client_address;
	socklen_t			address_len;
	int					ret;

	// int socket(int domain, int type, int protocol);
	// 1. domain: Protocol family - define 종류는 sys/socket.h 내용과 주석 참고
	// 2. type: 데이터 전송방식, define 종류는 sys/socket.h 내용과 주석 참고
	// 3. protocol: domain과 type에 의해 결정되기 때문에 항상 0
	// return: 새 소켓의 fd, 오류는 -1
	server_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (server_socket == -1)
	{
		printf("fail: socket()\n");
		exit(EXIT_FAILURE);
	}
	// int bind(int socket, const struct sockaddr *address, socklen_t address_len);
	// 1. socket: 
	// 2. *address: 
	// 3. address_len: 
	// return: 성공은 0, 오류는 -1

	/*
	struct sockaddr_in {
		__uint8_t       sin_len;
		sa_family_t     sin_family;
		in_port_t       sin_port;
		struct  in_addr sin_addr;
		char            sin_zero[8];
	};

	struct in_addr {
		in_addr_t s_addr;
	};
	*/
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_len = sizeof(struct sockaddr_in);
	server_address.sin_family = PF_INET;
	server_address.sin_port = htons(4242);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);// ip address(0.0.0.0). 컴퓨터의 랜카드 중 사용가능한 랜카드의 IP주소
	ret = bind(server_socket, (const struct sockaddr *)&server_address, sizeof(server_address));
	if (ret == -1)
	{
		printf("fail(%d): bind()\n", errno);
		close(server_socket);
		exit(EXIT_FAILURE);
	}
	printf("len: %u | family: %u | port: %u | addr: %u\n", server_address.sin_len, server_address.sin_family, server_address.sin_port, server_address.sin_addr.s_addr);
	
	// int listen(int socket, int backlog);
	// backlog: 대기할 큐의 사이즈
	// return: 성공은 0, 오류는 -1
	ret = listen(server_socket, 2);
	if (ret == -1)
	{
		printf("fail: listen()\n");
		close(server_socket);
		exit(EXIT_FAILURE);
	}

	// int accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len);
	while (1)
	{
		address_len = sizeof(client_address);
		client_socket = accept(server_socket, (struct sockaddr *)&client_address, &address_len); // 클라이언트가 접속할 때까지 block된 상태로 대기
		printf("(accept) client_socket: %d\n", client_socket);
		if (ret == -1)
		{
			printf("fail: accept()\n");
			close(server_socket);
			exit(EXIT_FAILURE);
		}
		// send, recv
		// ssize_t recv(int socket, void *buffer, size_t length, int flags);
		int		buf_len = 5;
		char	*buf = (char *)malloc(sizeof(char) * buf_len + 1);
		while((ret = recv(client_socket, buf, buf_len, 0)) != 0)
		{
			if (ret == -1)
			{
				printf("fail(%d): recv()\n", errno);
				close(client_socket);
				exit(EXIT_FAILURE);
			}
			buf[ret] = '\0';
			printf("server received(%d): %s\n", ret, buf);
			ret = send(client_socket, buf, ret, 0);
			if (ret == -1)
			{
				printf("fail: send()\n");
				close(client_socket);
				exit(EXIT_FAILURE);
			}
		}
		close(client_socket);
	}
	close(server_socket);
	return (0);
}
