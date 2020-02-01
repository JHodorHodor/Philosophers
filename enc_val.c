

int encode(unsigned short round, unsigned short pipe){
	int r;
	r= (int) round | ((int) pipe)<<16;
	return r;
};

void decode(int i, unsigned short * round, unsigned short *pipe){
	(*round)= (unsigned short) i;
	(*pipe)= (unsigned short) (i>>16);
	return;
}

int validate(int c1, int c2, unsigned short ph, unsigned short phils){
	unsigned short round1,round2,pipe1,pipe2;

	decode(c1,&round1, &pipe1);
	decode(c2,&round2, &pipe2);

	return ((round1==round2) && ((pipe1+1)%phils==pipe2) && (pipe1==ph));
}
