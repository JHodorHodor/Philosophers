#define _POSIX_SOURCE 1

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <math.h>

int encode(unsigned short i, unsigned short j);
int validate(int c1, int c2, unsigned short ph, unsigned short phils);

void counter(pid_t producer);

unsigned short rounds;
int phils;

int main(int argc, char* argv[])
{
	unsigned short i,j;
	int c, milisec_delay;
	char buf1[10], buf2[10],buf3[10];
	pid_t producer;

	if (argc != 4 || ((rounds= atoi(argv[2]))<0) || ((phils= atoi(argv[1]))<2) || ((milisec_delay= atoi(argv[3]))<0)) {
		fprintf(stderr,"%s NB_OF_PHILS NB_OF_ROUNDS MILISECONDS_TURN_DELAY\n", argv[0]);
		exit(1);
	}

	int pipes[2*phils], opipe[2];
	pid_t children[phils];

	fprintf(stderr,"philosophers: %d\n", phils);
	fprintf(stderr,"rounds: %d\n", rounds);
	
	if (pipe(opipe)){
		fprintf(stderr,"Output pipe creation failed.\n");
		exit(1);
	}

	producer= fork();
	if (producer == -1) {
		fprintf(stderr,"Producer process fork failed.\n");
		exit(1);
	}
	if (producer) {
		dup2(opipe[0],0);
		close(opipe[1]);
		close(opipe[0]);
		counter(producer);
		// never returns
	}

/*	PRODUCER STARTS HERE */

	for (i=0; i<phils; i++) {
		if (pipe(&pipes[2*i])){
			fprintf(stderr,"Pipe creation failed.\n");
			exit(1);
		};
	}

	close(opipe[0]);

	sprintf(buf1, "%d", phils);

	sigset_t usrmask, oldmask;
	sigemptyset(&usrmask);
	sigaddset(&usrmask, SIGUSR1);

	sigprocmask(SIG_BLOCK, &usrmask, &oldmask);

	for (i=0; i< phils; i++){
		children[i]= fork();

		if (children[i]==-1){
			fprintf(stderr,"Fork failed.\n");
			exit(1);
		}

		if (children[i]==0){
			close(0);
			dup2(opipe[1], 1);
			close(opipe[1]);
			dup2(pipes[(2*i+2)%(2*phils)],4);
			dup2(pipes[2*i],3);
			for (j=0; j< 2*phils; j++) close(pipes[j]); 

			sprintf(buf2, "%d",i);
	
			if (!execl("./phil.x","phil.x", buf1, buf2, NULL)){
				fprintf(stderr,"Exec failed!\n");
				exit(1);
			}
		}
	}

	sigprocmask(SIG_BLOCK, &oldmask, NULL);

	close(opipe[1]);

	for (i=0; i<phils; i++){
		close(pipes[2*i]);
		write(pipes[2*i+1], &(children[(i+1)%phils]), sizeof(pid_t));
	}

	for (j=0; j< rounds; j++){
		usleep(milisec_delay*1000);
		for (i=0; i<phils; i++){
			c=encode(j,i);
			write(pipes[2*i+1],&j,sizeof(short));	//	time stamp
			write(pipes[2*i+1],&c,sizeof(int));	//	value
		}
	}
}

void counter(pid_t producer){
	int valid[phils], invalid[phils];
	int i,n, c1, c2, minv, maxv, total;
	char buf[10];
	unsigned short ph;

	for (i=0; i<phils; i++) 
		valid[i]=invalid[i]=0;

	fprintf(stderr,"Counter STARTED\n");

	while (n=read(0,buf,10)){
		if (n==-1) 
			if (errno==EINTR) continue;
			else{
				fprintf(stderr,"Read failed.\n");
				exit(0);
			}	

		if (n!=10) {
			fprintf(stderr,"Boundary error: %d\n",n);
			exit(0);
		}

		ph=*((unsigned short *) buf);
		c1=*((int*) (buf+2));
		c2=*((int*) (buf+6));
		if (ph!=ph%phils) {
			fprintf(stderr,"Invalid ID: %d\n",ph);
			exit(0);
		}
		else if (!validate(c1,c2,ph,phils)){
			fprintf(stderr,"Invalid pair: [%x,%x] from %d\n",c1,c2,ph);
			invalid[ph]++;
		} else {
			fprintf(stderr,"Valid pair: [%x,%x] from %d\n",c1,c2,ph);
			valid[ph]++;
		}
	}


	minv= rounds;
	maxv=0;
	total=0;
	for (i=0; i<phils; i++) {
		fprintf(stderr,"[%d] valid:%d 	invalid:%d\n", i,valid[i],invalid[i]);
		if (invalid[i]) exit(0); //	Fail
		if (minv > valid[i]) minv=valid[i];
		if (maxv < valid[i]) maxv=valid[i];
		total+=valid[i];
	}

	if (maxv>minv+1) exit(0); // Fail
	if (total != rounds*(phils/2)) exit(0); // Fail
	
	printf("OK");

	exit(0);
}
