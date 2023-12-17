#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>

#define DEFAULT_PIPE "/tmp/remote-input"


// No use NDEBUG for control during runtime
static int DEBUG = 0;


void debug(const char * format, ...)
{
	if ( DEBUG == 1 ) {
		va_list args;
		va_start(args, format);
		printf("[debug] "); vprintf(format, args);
		va_end(args);
	}
}

void print_help(void)
{
	printf("Options:\n");
	printf("    touchscreen tap <x> <y>\n");
	printf("    swipe <x1> <y1> <x2> <y2>\n");

	printf("Environment:\n");
	printf("    PIPE_FILE: Specify the pipe filename used for communication with remote-input tool (default: /tmp/remote-input).\n");
}

int pipe_open(const char *pipe_name)
{
	debug("Waiting read process to open file ...\n");
	int fd = open(pipe_name, O_WRONLY);
	if (fd == -1) {
		perror("PIPE_FILE: ");
		exit(1);
	}
	debug("Done.\n");
	return fd;
}

int main(int argc, char *argv[])
{
	printf("version: 0.2\n");

	char *env = getenv("DEBUG");
	if ( env != NULL && strcmp( env, "true") == 0 ) {
		DEBUG=1;
		debug("DEBUG: true\n");
	}

	char *pipe_name;
	if (( pipe_name = getenv("PIPE_FILE")) == NULL ) {
		pipe_name = DEFAULT_PIPE;
		debug("PIPE_FILE: %s\n", pipe_name);
	}

	char buf[256] = {0};
	switch ( argc ) {
		case 5:
			if ( strcmp(argv[1], "touchscreen") == 0 && strcmp(argv[2], "tap") == 0 )
				sprintf(buf, "%s %s %s %s", argv[1], argv[2], argv[3], argv[4]);
			break;
		case 6:
			if ( strcmp(argv[1], "swipe") == 0 )
				sprintf(buf, "%s %s %s %s %s", argv[1], argv[2], argv[3], argv[4], argv[5]);
			break;
		default:
			break;
	}

	if ( buf[0] == 0 ) {
		printf("Invalied args\n");
		print_help();
		exit(1);
	}
	else {
		debug("buf: %s\n", buf);
	}

	int fd = pipe_open(pipe_name);
	int writebyte =  write(fd, buf, strlen(buf) + 1);
	if ( writebyte == -1 ) {
		perror("Failed: Write message to pipe");
		exit(1);
	}
	debug("writebyte: %d\n", writebyte);

	close(fd);
}

