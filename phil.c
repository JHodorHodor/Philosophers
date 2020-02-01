#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <signal.h>

unsigned short phils, my_num;
pid_t next;
bool started = false;

void send_packet(){

	char buf3[6];
	char buf4[6];
	short id3, id4;
	int data3, data4;
	int to_read;

	bool read3 = true;
	bool read4 = true;

	do {

		int r3 = -1;
		int r4 = -1;

		if(read3) {
			to_read = 6;
			while(r3 = read(3, buf3 + 6 - to_read, to_read)){
				if(r3 == -1){
					if (errno == EINTR) continue;
					else{
						fprintf(stderr,"Read failed.\n");
						exit(0);
					}
				}
				if(r3 == 0) break;
				if(r3 != to_read) {
					to_read -= r3;
					continue;
				}

				id3 = *(short*)(buf3);
				data3 = *(int*)(buf3 + 2);
				break;
			}
		}
		if(read4) {
			to_read = 6;
			while(r4 = read(4, buf4 + 6 - to_read, to_read)){
				if(r4 == -1){
					if (errno == EINTR) continue;
					else{
						fprintf(stderr,"Read failed.\n");
						exit(0);
					}
				}
				if(r4 == 0) break;
				if(r4 != to_read) {
					to_read -= r4;
					continue;
				}

				id4 = *(short*)(buf4);
				data4 = *(int*)(buf4 + 2);
				break;
			}
		}
		
		if(r3 == 0 || r4 == 0) {
			fprintf(stderr,"%d DIE!!!\n",my_num);
			close(3); 
			close(4);
			if(my_num == phils - 1 && phils % 2 == 0) kill(next, SIGUSR1);
			else kill(next, SIGUSR2);
			exit(0);
		}

		if(id3 < id4) { read3 = true; read4 = false; }
		if(id3 > id4) { read4 = true; read3 = false; }

	} while(id3 != id4);

	char buf[10];

	*(short*)buf = my_num;
	*(int*)(buf + 2) = data3;
	*(int*)(buf + 6) = data4;

	write(1, buf, 10);

	kill(next, SIGUSR1);

}

void sigusr1_handler(int sig_number){

	if(!started){
		started = true;
		if(my_num == 0){
			fprintf(stderr,"ALL READY!!!\n");
			send_packet();
		} else {
			kill(next, SIGUSR1);
		}
		return;
	} else {
		if(phils % 2 != 0 || my_num != 0)
			kill(next, SIGUSR2);
		else 
			send_packet();
	}

}

void sigusr2_handler(int sig_number){
	if(phils % 2 != 0 || my_num != 0)
		send_packet();
	else 
		kill(next, SIGUSR2);

}

int main(int argc, char* argv[]){

	sigset_t emask;
	sigemptyset(&emask);

	sigset_t sigusr_mask;
	sigemptyset(&sigusr_mask);
	sigaddset(&sigusr_mask, SIGUSR1);
	sigaddset(&sigusr_mask, SIGUSR2);
	sigprocmask(SIG_BLOCK, &sigusr_mask, NULL);

	struct sigaction act1;
	act1.sa_handler = sigusr1_handler;
	act1.sa_flags = 0;
	act1.sa_mask = sigusr_mask;
	sigaction(SIGUSR1, &act1, NULL);

	struct sigaction act2;
	act2.sa_handler = sigusr2_handler;
	act2.sa_flags = 0;
	act2.sa_mask = sigusr_mask;
	sigaction(SIGUSR2, &act2, NULL);

	phils = atoi(argv[1]);
	my_num = atoi(argv[2]);

	if(read(3, &next, sizeof(pid_t)) < 0) exit(1);

	fprintf(stderr,"%d is READY\n", my_num);

	if(my_num == 0)
		kill(next, SIGUSR1);
	
	while(1)
		sigsuspend(&emask);

	return 0;
}

//./frame.x 66 981 0