#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#include "STDM.h"

int maxtime = 0;
int averageR = 0;
int num_data = 0;

int main(int argc, char const *argv[]) {
	// check for correct usage
	if (argc != 2) {
		fprintf(stderr, "usage: ./STDM [inputfile.txt]\n");
		exit(1);
	}

	// open file
	FILE *file;
	file = fopen(argv[1], "r");
	if (file == NULL) {
		fprintf(stderr, "error: could not open '%s' for writing\n", argv[1]);
        exit(1);
	}

	// create data list
	struct source_list *sources = malloc(sizeof(struct source_list));
	list_init(sources);

	// parse file line-by-line
	int i = 0;
	char buffer[LINE_LEN];
	while (fgets(buffer, sizeof(buffer), file)) {		
		struct data_object *data = malloc(sizeof(struct data_object));
		data->priority = i;
		data->slots_used = 0;
		memset(&data->address[0], 0, ADDR_MAX);
		memset(&data->timeslot[0], 0, TIME_MAX);

		char *name = strtok(buffer, ":");
		data->name = malloc(sizeof(name));
		strcpy(data->name, name);

		// fill in relevant information
		int j = 1;
		char *start;
		while((start = strtok(NULL, " ,")) != NULL) {
			int tstart = atoi(start);
			data->timeslot[tstart] = j;

			strtok(NULL, " ,");
			strtok(NULL, " ,");

			if (tstart > maxtime) maxtime = tstart;
			data->slots_used++;
			j++;
		}
			
		// append data to list
		list_append(sources, data);
		num_data += data->slots_used;
		i++;
	}

	// calculate slots per frame
	int addr_bits = ceil(log(sources->num_members)/log(2));
	if (addr_bits <= 1) addr_bits = 2;

	char empty_addr[ADDR_MAX] = { 0 };
	memset(&empty_addr[0], '-', sizeof(char)*addr_bits);

	// assign binary addresses
	struct source_data *elem = list_first(sources);
	while(elem != NULL) {
		struct data_object *data = elem->obj;
		char address[ADDR_MAX] = { 0 };
		int i, j = 0;
		for (i = addr_bits - 1; i >= 0; i--, j++) {
		    int k = data->priority >> i;
		    if (k & 1) address[j] = '1';
		    else address[j] = '0';
		}

		strcpy(data->address, address);
		elem = list_next(sources, elem);
	}

	// calculate average transmission rate, prevent O R
	averageR = round((float)num_data/(maxtime+1));
	if (averageR == 0) averageR = 1;

	// calculate and print output link rate
	int frame_size = averageR*addr_bits + 2;
	printf("output link rate: %dR+%d\n\n", averageR, frame_size);

	// create list to store backlog subframes
	struct source_list *backlog = malloc(sizeof(struct source_list));
	list_init(backlog);
	
	// run algorithm
	int num_transmit = 0;
	int empty, time = 0;
	while (num_transmit < num_data) {
		empty = 0;
		int logflag = 0;
		struct data_object *data;

		// print frame start flag
		printf("| * |");
		for (i = 0; i < averageR; i++) {
			if (backlog->num_members) {
				// get and print subframe data
				elem = list_first(backlog);
				struct backlog_obj *backlog_data = elem->obj;		
				printf(" %s | %c%d |", backlog_data->address, 
						backlog_data->name[6], backlog_data->frame_no);
				logflag++;
				num_transmit++;

				// unlink subframe from backlog
				list_unlink(backlog, elem);

			} else {
				// get data for first subframe
				elem = list_first(sources);
				data = elem->obj;
				while (!data->timeslot[time] && elem != NULL) {				
					elem = list_next(sources, elem);
					if (elem == NULL) empty = 1;
					else data = elem->obj;
				}

				if (empty) {
					// nothing to transmit
					printf(" %s | -- |", empty_addr);
				} else {
					// print subframe data
					printf(" %s | %c%d |", data->address, 
							data->name[6], data->timeslot[time]);
					data->timeslot[time] = 0;
					num_transmit++;
				}
			}
		}
		
		// continue searching data for subframes to add to backlog
		for (elem = list_first(sources); elem != NULL; elem = list_next(sources, elem)) {
			data = elem->obj;
			if (data->timeslot[time]) {
				struct backlog_obj *backlog_data = malloc(sizeof(backlog_obj));
				backlog_data->name = malloc(sizeof(data->name));
				strcpy(backlog_data->name, data->name);
				strcpy(backlog_data->address, data->address);
				backlog_data->frame_no = data->timeslot[time];

				// add subframe data to backlog list
				list_append(backlog, backlog_data);
				data->timeslot[time] = 0;
			}
		}

		// print end flag and timing information
		printf(" * |");
		printf("\t\t(frame start time: %d, end time: %d)\n", time, time+1);

		time++;
	}

	return 0;
}
