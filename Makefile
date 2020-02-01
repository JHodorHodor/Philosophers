
all: frame.x

frame.x: frame.c phil.x enc_val.o
	gcc -o frame.x frame.c enc_val.o

phil.x: phil.c
	gcc -o phil.x phil.c

enc_val.o: enc_val.c
	gcc -c $< -o $@

clean:
	rm -f frame.x phil.x enc_val.o
