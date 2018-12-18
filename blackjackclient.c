#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void game(int);
void printcard();
void print_state();

typedef struct user {
	int money;
	int turn;
	int deck[11];
}user;
user player[3];

main(int argc, char *argv[])
{
	char *host = argv[1];
	int fd;
	int port = atoi(argv[2]);
	char command[32], id[10];
	char signal;
	int a = 0;
	int turn = 0;
	int batting_money = 0;
	FILE *fp;

	if(argc != 3) {
		printf("Please <./command ip port>\n");
		exit(1);
	}
	if((fd = connect_to_server(host, port)) == -1) {
		perror("not connect to server\n");
		exit(1);
	}
	read(fd, (int*)&signal, 32);
	if(signal == 1) {
		printf("please wait\n");
		while(1) {
			read(fd, (int*)&signal, sizeof(int));
			printf("signal : %d\n", signal);
			if(signal == 2)
				break;
		}
	}
	while(1) {
	//	printf("batiing! : ");
	//	scanf("%d", &batting_money);
		printf("game start!\n");
		game(fd);
		print_state();
		while(1) {
			printf("turn : %d>Do you want card?? (hit / stand) ??", turn);
			scanf("%s", command);
			write(fd, command, 32);
			if(strcmp(command, "hit") == 0) {
				game(fd);
				print_state();
			}
			if(strcmp(command, "stand") == 0) {
				printf("stop\n");
				/*while(1) {
					game(fd);
					print_state;
				}*/
				break;
			}
		}
	}
	close(fd);
}

void game(int FD)
{	
	int i = 0;
	read(FD, (int*)&player[0].money, sizeof(int));
	usleep(5);
	read(FD, (int*)&player[0].turn, sizeof(int));
	usleep(5);
	for(i = 0; i < player[0].turn; i++) {
		read(FD, (int*)&player[0].deck[i], sizeof(int));
		usleep(5);
	}
	read(FD, (int*)&player[1].money, sizeof(int));
	usleep(5);
	read(FD, (int*)&player[1].turn, sizeof(int));
	usleep(5);
	for(i = 0; i < player[1].turn; i++) {
		read(FD, (int*)&player[1].deck[i], sizeof(int));
		usleep(5);
	}
	read(FD, (int*)&player[2].turn, sizeof(int));
	usleep(5);
	for(i = 0; i < player[2].turn; i++) {
		read(FD, (int*)&player[2].deck[i], sizeof(int));
		usleep(5);
	}

}

void print_state()
{
	int i;
	
	printf("player1 money : %d\n", player[0].money);
	printf("player1 card : ");
	for(i = 0; i < player[0].turn; i++) 
		printcard(player[0].deck[i]);
	printf("\nplayer2 money : %d\n", player[1].money);
	printf("player2 card : ");
	for(i = 0; i < player[1].turn; i++)		
		printcard(player[1].deck[i]);
	printf("\ndealer card : ");
	for(i = 0; i < player[2].turn; i++)
		printcard(player[2].deck[i]);
	printf("\n");

}
void printcard(int card_num)
{
	int shape;
	int num, temp;

	shape = (card_num-1) / 13;
	num = card_num % 13;
	switch(shape){
		case 0 : 
			printf("♥");
			break;
		case 1 :
			printf("◆");
			break;
		case 2 :
			printf("♠");
			break;
		case 3 :
			printf("♣");
			break;
	}
	switch(num) {
		case 0 :
			printf("K");
			break;
		case 12 :
			printf("Q");
			break;
		case 11 :
			printf("J");
			break;
		default :
			printf("%d", num);
			break;
	}	
}

