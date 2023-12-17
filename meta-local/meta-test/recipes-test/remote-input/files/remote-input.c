// https://kernel.googlesource.com/pub/scm/linux/kernel/git/penberg/linux/+/v2.6.37-rc4/Documentation/input/multi-touch-protocol.txt

// TODO.1: Use common struct data for remote-input and input to comunicate
/*
 * struct data {
 * 	int screen_weight;
 * 	int screen_height;
 * 	int mode;
 * 	int x1;
 * 	int y1;
 * 	int x2;
 * 	int y2;
 * }
 *
 */

// TODO.2: Customizable screen width and height

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <sys/time.h>
#include <limits.h>

#define DISPLAY_WIDTH 1920
#define DISPLAY_HEIGHT 1080
#define ABS_RANGE_MAX SHRT_MAX
#define MT_PRESSURE 50
#define MT_MAJOR 5

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

void print_help()
{
}

int getAbsMax(const char *devicePath, int axis) {

	// devicePath e.g.: /sys/class/input/eventX/device/absinfo_0
	
	char absinfoPath[256];
	sprintf(absinfoPath, "%s/absinfo_%d/max", devicePath, axis);

	int fd = open(absinfoPath, O_RDONLY);
	if (fd == -1) {
		printf("[WARN] Error opening absmax file, use default ABS_MAX: %d", ABS_RANGE_MAX);
		return ABS_MAX;
	}

	int absmax;
	if (read(fd, &absmax, sizeof(int)) == sizeof(int)) {
		close(fd);
		return absmax;
	} else {
		printf("[WARN] Error reading absmax file, use default ABS_MAX: %d", ABS_RANGE_MAX);
		close(fd);
		return -1;
	}
}

void setup_abs(int uinput_fd, unsigned channel, int min, int max)
{
	if (ioctl(uinput_fd, UI_SET_ABSBIT, channel))
		perror("UI_SET_ABSBIT in setup_abs");

	struct uinput_abs_setup abs_lim = 
	{
		.code = channel,
		.absinfo = { .minimum = min, .maximum = max },
	};

	if (ioctl(uinput_fd, UI_ABS_SETUP, &abs_lim))
		perror("UI_ABS_SETUP in setup_abs");
}

void emit(int fd, int type, int code, int val) {
	struct input_event ev;

	ev.type = type;
	ev.code = code;
	ev.value = val;
	gettimeofday(&ev.time, NULL);
	if (write(fd, &ev, sizeof(ev)) == -1) {
		printf("[WARN] Error writing uinput event\n");
	}
}

int open_uinput()
{
	int uinput_fd = open("/dev/uinput", O_WRONLY);
	if (-1 == uinput_fd) {
		perror("Error opening /dev/uinput");
		exit(1);
	}
	return uinput_fd;
}

void close_uinput(int uinput_fd)
{
	close(uinput_fd);
}

void setup_uinput_device(const int uinput_fd, struct uinput_setup *usetup)
{
	// setup support event type
	ioctl(uinput_fd, UI_SET_EVBIT, EV_KEY);
	ioctl(uinput_fd, UI_SET_KEYBIT, BTN_TOUCH);

	ioctl(uinput_fd, UI_SET_EVBIT, EV_ABS);
	setup_abs(uinput_fd, ABS_X, 0, ABS_RANGE_MAX);
	setup_abs(uinput_fd, ABS_Y, 0, ABS_RANGE_MAX);
	setup_abs(uinput_fd, ABS_MT_SLOT, 0, 16);
	setup_abs(uinput_fd, ABS_MT_POSITION_X, 0, ABS_RANGE_MAX);
	setup_abs(uinput_fd, ABS_MT_POSITION_Y, 0, ABS_RANGE_MAX);
	setup_abs(uinput_fd, ABS_MT_PRESSURE, 0, MT_PRESSURE*2);
	setup_abs(uinput_fd, ABS_MT_TOUCH_MAJOR, 0, MT_MAJOR*2);

	// create uinput device
	usetup->id.bustype = BUS_VIRTUAL;
	usetup->id.vendor = 0x1;
	usetup->id.product = 0x1;
	ioctl(uinput_fd, UI_DEV_SETUP, usetup);
	ioctl(uinput_fd, UI_DEV_CREATE);
}

void close_uinput_device(int uinput_fd)
{
	// close uinput device
	ioctl(uinput_fd, UI_DEV_DESTROY);
	printf("Device is destroied\n");
}

int pipe_open(const char *pipe_name)
{
	debug("Waiting write process to open file ...\n");
	int fd = open(pipe_name, O_RDONLY);
	if (fd == -1) {
		perror("PIPE_FILE: open");
		exit(1);
	}
	debug("Done.\n");
	return fd;
}

void data_parse(char *str, int *mode, int *x1, int *y1, int *x2, int *y2)
{
	char argc = 0;
	char argv[5][256] = {0};
	char *p;

	debug("str: %s\n", str);
	p = strtok(str, " ");
	strcpy(argv[argc], p);
	debug("    data %d: %s\n", argc, argv[argc]);
	while ( (p = strtok(NULL, " ")) != NULL ) {
		argc++;
		strcpy(argv[argc], p);
		debug("    data %d: %s\n", argc, argv[argc]);
	}
	debug("argc: %d\n", argc);

	switch ( argc ) {
		case 3:
			if ( strcmp(argv[0], "touchscreen") == 0 && strcmp(argv[1], "tap") == 0 ) {
				*x1 = atoi(argv[2]) * ABS_RANGE_MAX / DISPLAY_WIDTH;
				*y1 = atoi(argv[3]) * ABS_RANGE_MAX / DISPLAY_HEIGHT;
				*mode = 1;
				debug("mode: tap, x: %d, y: %d\n", *x1, *y1);
				return;
			}
			break;
		case 4:
			if ( strcmp(argv[0], "swipe") == 0 ) {
				*x1 = atoi(argv[1]) * ABS_RANGE_MAX / DISPLAY_WIDTH;
				*y1 = atoi(argv[2]) * ABS_RANGE_MAX / DISPLAY_HEIGHT;
				*x2 = atoi(argv[3]) * ABS_RANGE_MAX / DISPLAY_WIDTH;
				*y2 = atoi(argv[4]) * ABS_RANGE_MAX / DISPLAY_HEIGHT;
				*mode = 2;
				debug("mode: swipe, x1: %d, y1: %d, x2: %d, y2: %d\n", *x1, *y1, *x2, *y2);
				return;
			}
			break;
		default:
			break;
	}

	printf("[WARN] Invalied data: %s\n", str);
	*mode = 0;
}


void tap(int uinput_fd, int x, int y)
{
	emit(uinput_fd, EV_ABS, ABS_MT_SLOT, 0);
	emit(uinput_fd, EV_ABS, ABS_MT_TRACKING_ID, 1);
	emit(uinput_fd, EV_ABS, ABS_MT_POSITION_X, x);
	emit(uinput_fd, EV_ABS, ABS_MT_POSITION_Y, y);
	emit(uinput_fd, EV_ABS, ABS_PRESSURE, MT_PRESSURE);
	emit(uinput_fd, EV_ABS, ABS_MT_TOUCH_MAJOR, MT_MAJOR);
	emit(uinput_fd, EV_SYN, SYN_REPORT, 0);
	usleep(100000);
	emit(uinput_fd, EV_ABS, ABS_MT_TRACKING_ID, -1);
	emit(uinput_fd, EV_SYN, SYN_REPORT, 0);
}

void swipe(int uinput_fd, int x1, int y1, int x2, int y2)
{
	emit(uinput_fd, EV_ABS, ABS_MT_SLOT, 0);
	emit(uinput_fd, EV_ABS, ABS_MT_TRACKING_ID, 1);
	emit(uinput_fd, EV_ABS, ABS_MT_POSITION_X, x1);
	emit(uinput_fd, EV_ABS, ABS_MT_POSITION_Y, y1);
	emit(uinput_fd, EV_ABS, ABS_MT_PRESSURE, MT_PRESSURE);
	emit(uinput_fd, EV_ABS, ABS_MT_TOUCH_MAJOR, MT_MAJOR);
	emit(uinput_fd, EV_SYN, SYN_REPORT, 0);
	usleep(100000);
	emit(uinput_fd, EV_ABS, ABS_MT_POSITION_X, (4*x1+x2)/5);
	emit(uinput_fd, EV_ABS, ABS_MT_POSITION_Y, (4*y1+y2)/5);
	emit(uinput_fd, EV_SYN, SYN_REPORT, 0);
	usleep(100000);
	emit(uinput_fd, EV_ABS, ABS_MT_POSITION_X, (3*x1+2*x2)/5);
	emit(uinput_fd, EV_ABS, ABS_MT_POSITION_Y, (3*y1+2*y2)/5);
	emit(uinput_fd, EV_SYN, SYN_REPORT, 0);
	usleep(100000);
	emit(uinput_fd, EV_ABS, ABS_MT_POSITION_X, (2*x1+3*x2)/5);
	emit(uinput_fd, EV_ABS, ABS_MT_POSITION_Y, (2*y1+3*y2)/5);
	emit(uinput_fd, EV_SYN, SYN_REPORT, 0);
	usleep(100000);
	emit(uinput_fd, EV_ABS, ABS_MT_POSITION_X, (x1+4*x2)/5);
	emit(uinput_fd, EV_ABS, ABS_MT_POSITION_Y, (y1+4*y2)/5);
	emit(uinput_fd, EV_SYN, SYN_REPORT, 0);
	usleep(100000);
	emit(uinput_fd, EV_ABS, ABS_MT_TRACKING_ID, -1);
	emit(uinput_fd, EV_SYN, SYN_REPORT, 0);
}

int main(int argc, char *argv[])
{
	printf("version: 0.11\n");

	char *env = getenv("DEBUG");
        if ( env != NULL && strcmp( env, "true") == 0 ) {
                DEBUG=1;
                debug("DEBUG: true\n");
        }

	char *pipe_name;
	if (( pipe_name = getenv("PIPE_FILE")) == NULL ) {
	        pipe_name = DEFAULT_PIPE;
    	}
	debug("PIPE_FILE: %s\n", pipe_name);


	int uinput_fd;
	struct uinput_setup usetup;

	uinput_fd = open_uinput();
	setup_uinput_device(uinput_fd, &usetup);

	char buf[256] = {0};
	int mode, x1, y1, x2, y2;
	while (1) {
		int pipe_fd = pipe_open(pipe_name);
		int readbyte = read(pipe_fd, buf, sizeof(buf));
		if ( readbyte == -1 ) {
			printf("[WARN] Failed to read data from pipe\n");
			continue;
		}
		debug("    readbyte: %d\n", readbyte);
		debug("    data: %s\n", buf);

		data_parse(buf, &mode, &x1, &y1, &x2, &y2);

		switch ( mode ) {
			case 0:
				continue;
			case 1:
				tap(uinput_fd, x1, y1);
				break;
			case 2:
				swipe(uinput_fd, x1, y1, x2, y2);
				break;
			default:
				printf("[WARN] Unkown internal failer: mode = %d\n", mode);
				break;
		}

		close(pipe_fd);
	}

	// Never reach here
	close_uinput_device(uinput_fd);
	close_uinput(uinput_fd);

	return 0;
}

