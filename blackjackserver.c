#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <time.h>

typedef struct user {
	char id[30];
	int fd;
	int turn;
	int game;
	int win;
	int deck[11];
	int realnumber[11];
	int sum;
	int login;
	int stand_flag;
	int money;
	int state; //1 is burst
}user;
user player[3];	
int allcard[52];
int start_flag = 0;

void drawcard(int);
void initialize();
void firstturn();
void printall();

main(int argc, char *argv[])
{
	int serv_sock, clnt_sock,  fd;
	FILE *fp;
	struct sockaddr_in serv_adr, clnt_adr;
	struct timeval timeout;
	fd_set reads, cpy_reads;
	socklen_t adr_sz;
	int turn = 0;
	int fd_max, str_len, fd_num, i, j;
	int port = atoi(argv[1]);
	int a = 0, signal = 0;
	char command[32];
	time_t thetime;
	int temp;

	if(argc != 2){
		perror("please <./command portnum>\n");
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1){
		perror("bind() error");
		exit(1);
	}

	if(listen(serv_sock, 5) == -1){
		perror("listen() error");
		exit(1);
	}

	FD_ZERO(&reads);
	FD_SET(serv_sock, &reads);
	fd_max = serv_sock;


	while(1){
		cpy_reads = reads;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		if((fd_num = select(fd_max+1, &cpy_reads, 0, 0, &timeout)) == -1)
			break;	

		if(fd_num == 0)
			continue;

		for(i=0; i<fd_max+1; i++)
		{
			if(FD_ISSET(i, &cpy_reads))
			{
				if(i == serv_sock) // connection request
				{
					adr_sz=sizeof(clnt_adr);
					clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_adr, &adr_sz);
					FD_SET(clnt_sock, &reads);
					if(fd_max < clnt_sock)
						fd_max = clnt_sock;
					printf("connected client: %d\n", clnt_sock - 3);
					player[clnt_sock - 4].login = 1;
					//로그인시 파일을 열어보는 함수
					if(player[0].login == 1 && player[1].login == 1) {
						signal = 2;
						printf("gamestart\n");
						write(4, (int*)&signal, sizeof(int));
						write(5, (int*)&signal, sizeof(int));
						initialize();
						firstturn();
						printall();
					}
					else {
						signal = 1;//클라를 멈추는 신호
						printf("notenough");
						write(clnt_sock, (int*)&signal, sizeof(int));
					}

				}

				else 	
				{	
					str_len = read(i, command, 32);
					fprintf(stdout, "cleint : %d, message %s recieved\n", i - 3, command);
					if(str_len == 0) // close request
					{
						FD_CLR(i, &reads);
						close(i);
						player[i-4].login = 0;
						printf("closed client: %d\n",i - 3);
					}
					else
						if(strcmp(command, "hit") == 0){
							drawcard(i-4);
							player[i-4].turn++;
							printf("\n");
							for(j = 0; j < player[i - 4].turn; j++) {
								player[i-4].sum += player[i-4].realnumber[j];
								if(player[i-4].sum > 22) {
									player[i -4].state = 1;
									printf("player%d is burst!\n", i-3);
									player[i - 4].stand_flag = 1;
								}
							}
							printall();
						}
						else if(strcmp(command, "stand") == 0){
							player[i-4].stand_flag = 1;
							printf("player1 stand : %d, player stand : %d\n", player[0].stand_flag, player[1].stand_flag);

						}

					if(player[0].stand_flag == 1 && player[1].stand_flag == 1) {
						printf("dealer's turn\n");
						while(player[2].sum < 17) {
							drawcard(2);
							player[2].sum += player[2].realnumber[player[2].turn];
							player[2].turn++;
						}
						if(player[0].sum - player[2].sum > 0) {
							printf("player1 is win\n");
							player[0].win++;
						}
						else
							printf("player1 is lose\n");
						if(player[1].sum - player[2].sum > 0) {
							printf("player2 is win\n");
							player[1].win++;
						}
						else
							printf("player2 is lose\n");
						printf("new game\n");
						initialize();
						firstturn();
						player[0].game++;
						player[1].game++;
						printall();
					}
				}
			}
		}
	}


	close(serv_sock);
}

void initialize()
{
	int i, j;

	for(i = 0; i < 52; i++) {
		allcard[i] = 0;
	}
	for(i=0; i < 3; i++){
		player[i].turn = 1;
		player[i].sum = 0;
		for(j = 0; j < 11; j++) {
			player[i].deck[j] = 0;
			player[i].realnumber[j] = 0;
		}
		player[i].login = 0;
		player[i].stand_flag = 0;
		player[i].state = 0;
	}
}

void firstturn()
{
	int i, j, k, card;

	srand(time(NULL));

	for(k = 0; k < 2; k++) {
		for(j = 0; j < 2; j++) {
			while(1) {
				card = (rand() % 52) +1;
				if(allcard[card] == 0){
					printf("%d\n", card);
					allcard[card] = 1;
					player[k].deck[j] = card;
					card = card % 13;
					if(card == 0 || card > 10) {
						player[k].realnumber[j] = 10;
						break;
					}
					else {
						player[k].realnumber[j] = card;
						break;
					}
				}
			}
		}
	}

	while(1) {
		card = (rand() % 52) +1;
		if(allcard[card] == 0){
			printf("%d\n", card);
			allcard[card] = 1;
			player[2].deck[0] = card;
			card = card % 13;
			if(card == 0 || card > 10) {
				player[2].realnumber[2] = 10;
				break;
			}
			else {
				player[2].realnumber[2] = card;
				break;
			}
		}
	}
	player[0].turn++;
	player[1].turn++;

}
void drawcard(int id)
{
	int i, card;

	srand(time(NULL));
	while(1) {
		card = (rand() % 52) +1;
		if(allcard[card] == 0){
			printf("%d\n", card);
			allcard[card] = 1;
			player[id].deck[player[id].turn] = card;
			card = card % 13;
			if(card == 0 || card > 10) {
				player[id].realnumber[player[id].turn] = 10;
				break;
			}
			else {
				player[id].realnumber[player[id].turn] = card;
				break;
			}
		}
	}
}

void printall()
{
	int i, j;
	for(i = 4 ; i <= 5; i++) {
		write(i, (int*)&player[0].money, sizeof(int));
		usleep(5);
		write(i, (int*)&player[0].turn, sizeof(int));
		usleep(5);
		for(j = 0; j < player[0].turn; j++) {
			write(i, (int*)&player[0].deck[j], sizeof(int));
			usleep(5);
		}
		write(i, (int*)&player[1].money, sizeof(int));
		usleep(5);
		write(i, (int*)&player[1].turn, sizeof(int));
		usleep(5);
		for(j = 0; j < player[1].turn; j++) {
			write(i, (int*)&player[1].deck[j], sizeof(int));
			usleep(5);
		}
		write(i, (int*)&player[2].turn, sizeof(int));
		usleep(5);
		for(j = 0; j < player[2].turn; j++) {
			write(i, (int*)&player[2].deck[j], sizeof(int));
			usleep(5);
		}
	}
}	
