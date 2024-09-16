#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "friends.h"
#include "users.h"
#include "graph.h"
#include "errno.h"

void handle_input_friends(char *input, list_graph_t *graph)
{
	char name1[MAX_NAME], name2[MAX_NAME], dummy[MAX_NAME];
	char copy_commands[VAR_CHAR];

	char *commands = strdup(input);
	strcpy(copy_commands, input);

	if (sscanf(commands, "%s %s %s", dummy, name1, name2) == 3)
		dummy[0] = 'a';

	char *cmd = strtok(commands, "\n ");
	int id1, id2;
	if (!cmd)
		return;

	if (!strcmp(cmd, "add")) {
		printf("Added connection %s - %s\n", name1, name2);
		id1 = get_user_id(name1);
		id2 = get_user_id(name2);
		lg_add_edge(graph, id1, id2);
		lg_add_edge(graph, id2, id1);
	} else if (!strcmp(cmd, "remove")) {
		id1 = get_user_id(name1);
		id2 = get_user_id(name2);
		lg_remove_edge(graph, id1, id2);
		lg_remove_edge(graph, id2, id1);
		printf("Removed connection %s - %s\n", name1, name2);
	} else if (!strcmp(cmd, "suggestions")) {
		if (sscanf(copy_commands, "%s %s", dummy, name1) == 2) {
			int id1 = get_user_id(name1);
			suggestions(graph, id1, name1);
		}
	} else if (!strcmp(cmd, "distance")) {
		id1 = get_user_id(name1);
		id2 = get_user_id(name2);
		int dist = bfs_list_graph(graph, id1, id2);
		if (dist == -1)
			printf("There is no way to get from %s to %s\n", name1, name2);
		else
			printf("The distance between %s - %s is %d\n", name1, name2, dist);
	} else if (!strcmp(cmd, "common")) {
		id1 = get_user_id(name1);
		id2 = get_user_id(name2);
		common_friends(graph, id1, id2);
	} else if (!strcmp(cmd, "friends")) {
		if (sscanf(copy_commands, "%s %s", dummy, name1) == 2) {
			int id1 = get_user_id(name1);
			int nf = number_of_friends(graph, id1);
			printf("%s has %d friends\n", name1, nf);
		}
	} else if (!strcmp(cmd, "popular")) {
		int max;
		if (sscanf(copy_commands, "%s %s", dummy, name1) == 2) {
			int id1 = get_user_id(name1);
			int nf_input = number_of_friends(graph, id1);
			max = nf_input;
			ll_node_t *aux = graph->neighbors[id1]->head;
			int nf, id_max, id;
			id_max = id1;
			while (aux) {
				int neighbor = *(int *)aux->data;
				id = get_user_id(get_user_name(neighbor));
				nf = number_of_friends(graph, id);
				if (nf > max) {
					max = nf;
					id_max = id;
				}
				aux = aux->next;
			}
			if (id_max == id1)
				printf("%s is the most popular\n", name1);
			else
				printf("%s is the most popular friend of %s\n",
					   get_user_name(id_max), name1);
		}
	}
	free(commands);
}
