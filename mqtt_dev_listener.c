#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <signal.h>
#include <pthread.h>

#include "mosquitto.h"

#define mqtt_port 1883

static int run = 1;

struct sigaction act;

void cleanup() {
	mosquitto_lib_cleanup();
	pthread_exit(NULL);
	printf("Cleanup done");
}

void sighandler(int signum, siginfo_t *info, void *ptr) {
	//signal(signum, SIG_IGN);
	printf("Received signal %d from process %lu\n", signum, (unsigned long)info->si_pid);
	run = 0;
}

void connect_callback(struct mosquitto *mosq, void *obj, int result) {
}

void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message) {
}

void message_send(struct mosquitto *mosq, char *message) {
	mosquitto_publish(mosq, NULL, "/input", strlen(message), message, 1, false);
}

void *mosquitto_loop_thread(void *arg) {
	struct mosquitto *mosq;
	mosq = (struct mosquitto *) arg;
	printf("Loop thread started\n");
	int rc = 0;
	while (run) {
		rc = mosquitto_loop(mosq, -1, 1);
		if (run && rc) {
			sleep(20);
			mosquitto_reconnect(mosq);
		}
	}
	printf("Loop thread stopped\n");
	pthread_exit(NULL);
}

void show_help_and_exit(char *argv[]) {
	printf("usage: %s <mqtt_host> <device>\n", argv[0]);
	printf("\t%s 127.0.0.1 /dev/input/by-id/*event*kbd\n", argv[0]);
	printf("\t%s 127.0.0.1 /dev/input/by-id/*event*mouse\n", argv[0]);
	printf("\t%s 127.0.0.1 /dev/input/by-id/*event*joystick\n", argv[0]);
	printf("list of devices: ls -l /dev/input/by-id/\n");
	exit(1);
}

int main(int argc, char *argv[]) {
	char clientid[24];
	struct mosquitto *mosq;
	int rc = 0;

	int fd;
	char name[256] = "Unknown";
	char *device = NULL;
	char *mqtt_host = NULL;
	struct stat;
	struct input_event ev;
	int rd, size = sizeof (struct input_event);

	if(argc < 3) {
		show_help_and_exit(argv);
	}

	// Termination handler
	printf("PID %lu\n", (unsigned long)getpid());
	memset(&act, 0, sizeof(act));
	act.sa_sigaction = sighandler;
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGTERM, &act, NULL);
	sigaction(SIGINT, &act, NULL);
	sigaction(SIGQUIT, &act, NULL);

	mqtt_host = argv[1];

	// Open Device
	device = argv[2];
	if ((fd = open (device, O_RDONLY)) == -1)
		printf ("%s is not a vaild device\n", device);

	// Print Device Name
	ioctl(fd, EVIOCGNAME (sizeof (name)), name);
	printf("Reading From : %s (%s)\n", device, name);
	
	// Start Mosquitto
	mosquitto_lib_init();
	mosq = mosquitto_new(clientid, true, NULL);
	if (mosq) {
		mosquitto_connect_callback_set(mosq, connect_callback);
		mosquitto_message_callback_set(mosq, message_callback);
		rc = mosquitto_connect(mosq, mqtt_host, mqtt_port, 60);
		if (rc) {
			printf("ERROR; return code from mosquitto_connect() is %d\n", rc);
			exit(-1);
		}
		
		// Thread init
		pthread_t thread;
		rc = pthread_create(&thread, NULL, mosquitto_loop_thread, (void *)mosq);
		if (rc) {
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		} else {
			while (run) {
				if ((rd = read(fd, &ev, size )) < size) {
					perror ("read()");
					run = 0;
				} else if (ev.type <= 2 && ev.type >= 0 && !(ev.code == 0 && ev.type ==0 && ev.value == 0)) {
					char str[] = "%i %i %i";
					char str2[100];
					sprintf(str2, str, ev.code, ev.type, ev.value);
					printf("%s\n", str2);
					message_send(mosq, str2);
				}
			}
		}

		// Cleanup Mosquitto
		mosquitto_destroy(mosq);
	}

	// Exit
	cleanup();

	printf("Bye");
	return rc;
}

