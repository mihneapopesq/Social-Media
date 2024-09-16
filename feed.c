#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "feed.h"
#include "users.h"
#include "posts.h"
#include "graph.h"

void feed(post_array **array, char *name, list_graph_t *graph, int feed_size)
{
	int num_posts = (*array)->num_posts;
	int id_user = get_user_id(name);
	for (int i = num_posts - 1; i >= 0 ; i--) {
		if (feed_size == -1)
			break;

		post_info *p_info = (*array)->posts[i]->root->info;
		int id = p_info->user_id;
		if (lg_has_edge(graph, id_user, id) || id_user == id) {
			printf("%s: %s\n", get_user_name(id), p_info->title);
			feed_size--;
		}
	}
}

void view_reposts(post_node *node, int user_id, char *name)
{
	post_info *info = node->info;
	if (info->user_id == user_id && info->title[0] == '\0')
		printf("Reposted: %s\n", name);
	for (int i = 0; i < node->children_cnt; i++)
		view_reposts(node->children[i], user_id, name);
}

void view_profile(post_array **array,  char *name)
{
	int user_id = get_user_id(name);
	int num_posts = (*array)->num_posts;
	for (int i = 0; i < num_posts; i++) {
		post_info *p_info = (*array)->posts[i]->root->info;
		if (p_info->user_id == user_id)
			printf("Posted: %s\n", p_info->title);
	}
	for (int i = 0; i < num_posts; i++) {
		post_node *root_node = (*array)->posts[i]->root;
		post_info *p_info = (*array)->posts[i]->root->info;
		view_reposts(root_node, user_id, p_info->title);
	}
}

void get_reposts(list_graph_t *graph, post_node *root, int id)
{
	post_info *info = root->info;
	if (lg_has_edge(graph, id, info->user_id))
		printf("%s\n", get_user_name(info->user_id));
	for (int i = 0; i < root->children_cnt; i++)
		get_reposts(graph, root->children[i], id);
}

void friends(post_array **array, list_graph_t *graph, char *name, int post_id)
{
	int num_posts = (*array)->num_posts;
	int wanted_pos;
	for (int i = 0; i < num_posts; i++) {
		post_info *info = (*array)->posts[i]->root->info;
		if (info->id == post_id) {
			wanted_pos = i;
			break;
		}
	}
	get_reposts(graph, (*array)->posts[wanted_pos]->root, get_user_id(name));
}

int belongs_to_list(linked_list_t *list, int node)
{
	if (!list || !list->head)
		return 0;
	ll_node_t *current = list->head;
	do {
		if (current->data && *(int *)current->data == node)
			return 1;
		current = current->next;
	} while (current);
	return 0;
}

linked_list_t *ll_copy(linked_list_t *list) {
	linked_list_t *new_list = ll_create(list->data_size);
	ll_node_t *current = list->head;
	while (current) {
		ll_add_nth_node(new_list, new_list->size, current->data);
		current = current->next;
	}
	return new_list;
}

linked_list_t *lg_get_neighbours(list_graph_t *graph, int node) {
	return graph->neighbors[node];
}

void create_RPX(linked_list_t **R, linked_list_t **P, linked_list_t **X)
{
	*R = ll_create(sizeof(int));
	*P = ll_create(sizeof(int));
	*X = ll_create(sizeof(int));
}

void free_RPX(linked_list_t **R, linked_list_t **P, linked_list_t **X)
{
	ll_free(&(*R));
	ll_free(&(*P));
	ll_free(&(*X));
}

void bron_kerbosch(linked_list_t *R, linked_list_t *P, linked_list_t *X,
				   list_graph_t *graph, linked_list_t **max_clique,
				    int start_node)
{
	if (!P->size && !X->size) {
		if (belongs_to_list(R, start_node)) {
			if (R->size > (*max_clique)->size) {
				ll_free(max_clique);
				*max_clique = ll_copy(R);
			}
		}
		return;
	}
	// Choosing a pivot to reduce the number of recursive calls
	ll_node_t *pivot_node = P->head ? P->head : X->head;
	int pivot = pivot_node ? *(int *)pivot_node->data : -1;
	linked_list_t *P_without_neighbors = ll_create(sizeof(int));
	ll_node_t *current = P->head;
	while (current) {
		int node = *(int *)current->data;
		if (!belongs_to_list(graph->neighbors[pivot], node))
			ll_add_nth_node(P_without_neighbors, P_without_neighbors->size,
							&node);
		current = current->next;
	}
	ll_node_t *P_node = P_without_neighbors->head;
	while (P_node) {
		int node = *(int *)P_node->data;
		linked_list_t *new_R = ll_copy(R);
		ll_add_nth_node(new_R, new_R->size, &node);
		linked_list_t *new_P = ll_create(sizeof(int));
		linked_list_t *new_X = ll_create(sizeof(int));
		linked_list_t *neighbors = lg_get_neighbours(graph, node);
		ll_node_t *current_P = P->head;
		while (current_P) {
			if (belongs_to_list(neighbors, *(int *)current_P->data))
				ll_add_nth_node(new_P, new_P->size, current_P->data);
			current_P = current_P->next;
		}
		for (ll_node_t *current_X = X->head; current_X;
		 current_X = current_X->next) {
			if (belongs_to_list(neighbors, *(int *)current_X->data))
				ll_add_nth_node(new_X, new_X->size, current_X->data);
		}
		bron_kerbosch(new_R, new_P, new_X, graph, max_clique, start_node);
		ll_node_t *removed_node = ll_remove_nth_node(P, 0);
		free(removed_node->data);
		free(removed_node);
		ll_add_nth_node(X, X->size, &node);
		free_RPX(&new_R, &new_P, &new_X);
		P_node = P_node->next;
	}
	ll_free(&P_without_neighbors);
}

void handle_cliques(list_graph_t *graph, int start_node,
					linked_list_t **max_clique)
{
	if (!graph || !max_clique)
		return;
	linked_list_t *R, *P, *X;
	create_RPX(&R, &P, &X);
	int i = 0;
	while (i < graph->nodes) {
		ll_add_nth_node(P, P->size, &i);
		i++;
	}
	bron_kerbosch(R, P, X, graph, max_clique, start_node);
	free_RPX(&R, &P, &X);
}

void handle_input_feed(char *input, list_graph_t *graph, post_array **array)
{
	char name1[MAX_NAME];
	char copy_commands[VAR_CHAR];
	char *commands = strdup(input);
	strcpy(copy_commands, input);
	char *cmd = strtok(commands, "\n ");
	if (!cmd)
		return;
	if (!strcmp(cmd, "feed")) {
		int feed_size;
		if (sscanf(copy_commands, "feed %s %d", name1, &feed_size) == 2)
			feed(array, name1, graph, feed_size);

	} else if (!strcmp(cmd, "view-profile")) {
		if (sscanf(copy_commands, "view-profile %s", name1) == 1)
			view_profile(array,  name1);

	} else if (!strcmp(cmd, "friends-repost")) {
		int post_id;
		if (sscanf(copy_commands, "friends-repost %s %d", name1, &post_id) == 2)
			friends(array, graph, name1, post_id);
	} else if (!strcmp(cmd, "common-group")) {
		if (sscanf(copy_commands, "common-group %s", name1) == 1) {
			linked_list_t *max_clique = ll_create(sizeof(int));
			handle_cliques(graph, get_user_id(name1), &max_clique);

			printf("The closest friend group of %s is:\n", name1);
			ll_node_t *node = max_clique->head;
			while (node) {
				int user_id = *(int *)node->data;
				printf("%s\n", get_user_name(user_id));
				node = node->next;
			}

			ll_free(&max_clique);
		}
	}
	free(commands);
}
