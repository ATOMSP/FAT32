main.exe : main.c src/Iodisk.c src/sfat.c
	gcc main.c src/Iodisk.c src/sfat.c -I src/ -o main.exe



