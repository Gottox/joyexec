#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <fcntl.h>
#include <string.h>
#include <libconfig.h>
#include <unistd.h>
#include <signal.h>
#include <libgen.h>



#include "common.h"

#define JS_EVENT_BUTTON         0x01    /* button pressed/released */
#define JS_EVENT_AXIS           0x02    /* joystick moved */
#define JS_EVENT_INIT           0x80    /* initial state of device */
#define MAX_AXIS		16
#define MIN_DELAY		100
#define THRESHOLD		(32767 / 2)

struct js_event {
	uint32_t time;     /* event timestamp in milliseconds */
	int16_t value;     /* value */
	uint8_t type;      /* event type */
	uint8_t number;    /* axis/button number */
};

enum Mode {
	UNKNOWN,
	DAEMON,
	DUMP,
	FOREGROUND
};

void die(const char *errstr, ...);
void loadconfig();
void init();
void cleanup();
int readev();
int axisvalue(int axis, int raw);
void daemonize();
void loop();

int mode = FOREGROUND;
int run = 1;
int fd;
config_t config;
char *configpath = ETCPREFIX"/joyexec.conf";
char *configdir = ETCPREFIX;
char *device = "/dev/input/js0";
int axisstate[MAX_AXIS] = { 0 };
config_setting_t *commands;

struct js_event ev = { 0 };

void
die(const char *errstr, ...) {
	va_list ap;

	va_start(ap, errstr);
	vfprintf(stderr, errstr, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

void
loadconfig() {
	config_init(&config);
	config_set_include_dir(&config, configdir);
	config_read_file(&config, configpath);
	
	const char *dev = device;
	config_lookup_string(&config, "joystick.device", &dev);
	commands = config_lookup(&config, "commands");
}

void
init() {
	loadconfig();
	fd = open(device, O_RDONLY);
}

void
cleanup() {
	config_destroy(&config);
}


int
readev() {
	int last;
	do {
		last = ev.time;
		if(sizeof(struct js_event) != read(fd, &ev, sizeof(struct js_event)))
			die("error reading event");
	} while(ev.time - last < MIN_DELAY);
	return ev.type;
}

int
axisvalue(int axis, int raw) {
	int value;
	if(abs(raw) < THRESHOLD)
		value = 0;
	else
		value = raw > 0 ? 1 : -1;
	
	if(value == axisstate[axis])
		return 0;

	axisstate[axis] = value;
	return value;
}

void
daemonize() {
	int i;

	if(getppid()==1) return; /* already a daemon */
	i=fork();
	if(i<0)
		exit(1); /* fork error */
	else if (i>0)
		exit(0); /* parent exits */
	

	/* child (daemon) continues */
	setsid(); /* obtain a new process group */

	/* first instance continues */
	signal(SIGCHLD,SIG_IGN); /* ignore child */
	signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
	signal(SIGTTOU,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
}

void
loop() {
	char key[64];
	int value;
	const char *exec = NULL;

	while(run) {
		readev();
		switch(ev.type) {
		case JS_EVENT_BUTTON:
			value = ev.value;
			snprintf(key, sizeof(key),
				"button%d", ev.number);
			break;
		case JS_EVENT_AXIS:
			value = axisvalue(ev.number, ev.value);
			snprintf(key, sizeof(key),
				"axis%d_%s",
				ev.number,
				ev.value < 0 ? "up" : "down");
			break;
		case JS_EVENT_INIT:
		default:
			continue;
		}
		if(value == 0)
			continue;
		else if(mode == DUMP) {
			printf("%s\n", key);
			continue;
		}
		else if(commands && config_setting_lookup_string(commands, key, &exec) == CONFIG_TRUE) {
			system(exec);
		}
	}
}

int
main(int argc, char *argv[]) {
	ARG {
	case 'f':
		mode = DAEMON;
		break;
	case 'd':
		mode = DUMP;
		break;
	case 'c':
		configpath = ARGVAL();
		configdir = dirname(configpath);
		break;
	case 'v':
		die("joyexec-"VERSION", \u00A9 2013 Enno Boland <eb@s01.de>\n");
	default:
	argerr:
		die("%s [-c config] [-f|-d]\n", argv[0]);
		return EXIT_SUCCESS;
	}
	if(argc != ARGC())
		goto argerr;

	init();

	switch(mode) {
	case DAEMON:
		daemonize();
		/* NO BREAK HERE */
	case DUMP:
	case FOREGROUND:
		loop();
		break;
	}

	cleanup();
	return EXIT_SUCCESS;
}
