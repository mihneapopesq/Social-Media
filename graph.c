#include <stdio.h>
#include "graph.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "users.h"

/*
 * Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca
 * parametru. Pozitiile din lista se indexeaza de la 0 (i.e. primul nod din
 * lista se afla pe pozitia n=0). Daca n >= nr_noduri - 1, se returneaza
 * ultimul nod.
 */
ll_node_t *get_nth_node(linked_list_t *list, unsigned int n)
{
	if (!list)
		return NULL;

	unsigned int len = list->size - 1;
	unsigned int i;
	ll_node_t *node = list->head;

	n = MIN(n, len);

	for (i = 0; i < n; ++i)
		node = node->next;

	return node;
}

linked_list_t *ll_create(unsigned int data_size)
{
	linked_list_t *ll;

	ll = malloc(sizeof(*ll));

	ll->head = NULL;
	ll->data_size = data_size;
	ll->size = 0;

	return ll;
}

/*
 * Pe baza datelor trimise prin pointerul new_data, se creeaza un nou nod care e
 * adaugat pe pozitia n a listei reprezentata de pointerul list. Pozitiile din
 * lista sunt indexate incepand cu 0 (i.e. primul nod din lista se afla pe
 * pozitia n=0). Daca n >= nr_noduri, noul nod se adauga la finalul listei. Daca
 * n < 0, eroare.
 */
void ll_add_nth_node(linked_list_t *list, unsigned int n, const void *new_data)
{
	ll_node_t *prev, *curr;
	ll_node_t *new_node;

	if (!list)
		return;

	/* n >= list->size inseamna adaugarea unui nou nod la finalul listei. */
	if (n > list->size)
		n = list->size;

	curr = list->head;
	prev = NULL;
	while (n > 0) {
		prev = curr;
		curr = curr->next;
		--n;
	}

	new_node = malloc(sizeof(*new_node));
	new_node->data = malloc(list->data_size);
	memcpy(new_node->data, new_data, list->data_size);

	new_node->next = curr;
	if (!prev) {
		/* Adica n == 0. */
		list->head = new_node;
	} else {
		prev->next = new_node;
	}

	list->size++;
}

/*
 * Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca
 * parametru. Pozitiile din lista se indexeaza de la 0 (i.e. primul nod din
 * lista se afla pe pozitia n=0). Daca n >= nr_noduri - 1, se elimina nodul de
 * la finalul listei. Daca n < 0, eroare. Functia intoarce un pointer spre acest
 * nod proaspat eliminat din lista. Este responsabilitatea apelantului sa
 * elibereze memoria acestui nod.
 */
ll_node_t *ll_remove_nth_node(linked_list_t *list, unsigned int n)
{
	ll_node_t *prev, *curr;

	if (!list || !list->head)
		return NULL;

	/* n >= list->size - 1 inseamna eliminarea nodului de la finalul listei. */
	if (n > list->size - 1)
		n = list->size - 1;

	curr = list->head;
	prev = NULL;
	while (n > 0) {
		prev = curr;
		curr = curr->next;
		--n;
	}

	if (!prev) {
		/* Adica n == 0. */
		list->head = curr->next;
	} else {
		prev->next = curr->next;
	}

	list->size--;
	return curr;
}

/*
 * Functia intoarce numarul de noduri din lista al carei pointer este trimis ca
 * parametru.
 */
unsigned int ll_get_size(linked_list_t *list)
{
	if (!list)
		return -1;

	return list->size;
}

/*
 * Procedura elibereaza memoria folosita de toate nodurile din lista, iar la
 * sfarsit, elibereaza memoria folosita de structura lista si actualizeaza la
 * NULL valoarea pointerului la care pointeaza argumentul (argumentul este un
 * pointer la un pointer).
 */
void ll_free(linked_list_t **pp_list)
{
	ll_node_t *curr;

	if (!pp_list || !*pp_list)
		return;

	while (ll_get_size(*pp_list) > 0) {
		curr = ll_remove_nth_node(*pp_list, 0);
		free(curr->data);
		curr->data = NULL;
		free(curr);
		curr = NULL;
	}

	free(*pp_list);
	*pp_list = NULL;
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza int-uri. Functia afiseaza toate valorile int stocate in nodurile
 * din lista inlantuita separate printr-un spatiu.
 */
void ll_print_int(linked_list_t *list)
{
	ll_node_t *curr;

	if (!list)
		return;

	curr = list->head;
	while (!curr) {
		printf("%d ", *((int *)curr->data));
		curr = curr->next;
	}

	printf("\n");
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza string-uri. Functia afiseaza toate string-urile stocate in
 * nodurile din lista inlantuita, separate printr-un spatiu.
 */
void ll_print_string(linked_list_t *list)
{
	ll_node_t *curr;

	if (!list)
		return;

	curr = list->head;
	while (!curr) {
		printf("%s ", (char *)curr->data);
		curr = curr->next;
	}

	printf("\n");
}

/* Graful este ORIENTAT */

/*
 * Initializeaza graful cu numarul de noduri primit ca parametru si aloca
 * memorie pentru lista de adiacenta a grafului.
 */
list_graph_t *lg_create(int nodes)
{
	list_graph_t *graph = malloc(sizeof(*graph));
	graph->nodes = nodes;
	graph->neighbors = malloc(nodes * sizeof(*graph->neighbors));
	for (int i = 0; i < nodes; i++)
		graph->neighbors[i] = ll_create(sizeof(int));

	return graph;
}

/* Adauga o muchie intre nodurile primite ca parametri. */
void lg_add_edge(list_graph_t *graph, int src, int dest)
{
	if (src < graph->nodes && dest < graph->nodes)
		ll_add_nth_node(graph->neighbors[src],
						ll_get_size(graph->neighbors[src]), &dest);
}

/* Returneaza 1 daca exista muchie intre cele doua noduri, 0 in caz contrar */
int lg_has_edge(list_graph_t *graph, int src, int dest)
{
	if (src >= graph->nodes)
		return 0;
	ll_node_t *node = graph->neighbors[src]->head;
	while (node) {
		if (*((int *)node->data) == dest)
			return 1;
		node = node->next;
	}
	return 0;
}

/* Elimina muchia dintre nodurile primite ca parametri */
void lg_remove_edge(list_graph_t *graph, int src, int dest)
{
	if (src < graph->nodes) {
		ll_node_t *node = graph->neighbors[src]->head;
		int i = 0;
		while (node) {
			if (*((int *)node->data) == dest) {
				ll_remove_nth_node(graph->neighbors[src], i);
				free(node->data);
				free(node);
				break;
			}
			node = node->next;
			i++;
		}
	}
}

/* Elibereaza memoria folosita de lista de adiacenta a grafului */
void lg_free(list_graph_t *graph)
{
	for (int i = 0 ; i < graph->nodes; i++)
		ll_free(&graph->neighbors[i]);

	free(graph->neighbors);
	free(graph);
}

/* Printeaza lista de adiacenta a grafului */
void lg_print_graph(list_graph_t *graph)
{
	for (int i = 0; i < graph->nodes; i++) {
		printf("%d: ", i);
		ll_print_int(graph->neighbors[i]);
	}
}

queue_t *q_create(unsigned int data_size, unsigned int max_size)
{
	queue_t *q = calloc(1, sizeof(*q));

	q->data_size = data_size;
	q->max_size = max_size;

	q->buff = malloc(max_size * sizeof(*q->buff));

	return q;
}

/*
 * Functia intoarce numarul de elemente din coada al carei pointer este trimis
 * ca parametru.
 */
unsigned int q_get_size(queue_t *q)
{
	return !q ? 0 : q->size;
}

/*
 * Intoarce 1 daca coada este goala si 0 in caz contrar.
 */
unsigned int q_is_empty(queue_t *q)
{
	return !q ? 1 : !q->size;
}

/*
 * Functia intoarce primul element din coada, fara sa il elimine.
 */
void *q_front(queue_t *q)
{
	if (!q || !q->size)
		return NULL;
	return q->buff[q->read_idx];
}

/*
 * Functia scoate un element din coada. Se va intoarce 1 daca operatia s-a
 * efectuat cu succes (exista cel putin un element pentru a fi eliminat) si
 * 0 in caz contrar.
 */
int q_dequeue(queue_t *q)
{
	if (!q || !q->size)
		return 0;

	free(q->buff[q->read_idx]);

	q->read_idx = (q->read_idx + 1) % q->max_size;
	--q->size;
	return 1;
}

/*
 * Functia introduce un nou element in coada. Se va intoarce 1 daca
 * operatia s-a efectuat cu succes (nu s-a atins dimensiunea maxima)
 * si 0 in caz contrar.
 */
int q_enqueue(queue_t *q, void *new_data)
{
	void *data;
	if (!q || q->size == q->max_size)
		return 0;

	data = malloc(q->data_size);
	memcpy(data, new_data, q->data_size);

	q->buff[q->write_idx] = data;
	q->write_idx = (q->write_idx + 1) % q->max_size;
	++q->size;

	return 1;
}

/*
 * Functia elimina toate elementele din coada primita ca parametru.
 */

void q_clear(queue_t *q)
{
	unsigned int i;
	if (!q || !q->size)
		return;

	for (i = q->read_idx; i != q->write_idx; i = (i + 1) % q->max_size)
		free(q->buff[i]);

	q->read_idx = 0;
	q->write_idx = 0;
	q->size = 0;
}

/*
 * Functia elibereaza toata memoria ocupata de coada.
 */
void q_free(queue_t *q)
{
	if (!q)
		return;

	q_clear(q);
	free(q->buff);
	free(q);
}

int bfs_list_graph(list_graph_t *lg, int src, int dest)
{
	queue_t *q = q_create(sizeof(int), lg->nodes);
	q_enqueue(q, &src);
	int *dist = calloc(lg->nodes, sizeof(int));

	for (int i = 0; i < lg->nodes; i++)
		dist[i] = -1;

	dist[src] = 0;

	while (!q_is_empty(q)) {
		int x = *(int *)(q_front(q));
		q_dequeue(q);

		// Ne plimbam prin vecinii lui x
		ll_node_t *aux = lg->neighbors[x]->head;

		while (aux) {
			int neighbor = *(int *)aux->data;
			if (dist[neighbor] == -1) {
				dist[neighbor] = dist[x] + 1;
				q_enqueue(q, &neighbor);
				if (neighbor == dest) {
					int result = dist[dest];
					free(dist);
					q_free(q);
					return result;
				}
			}
			aux = aux->next;
		}
	}

	q_free(q);
	free(dist);
	return -1; // Returnam -1 daca destinatia nu este accesibila
}

int number_of_friends(list_graph_t *graph, int src)
{
	return ll_get_size(graph->neighbors[src]);
}

void suggestions(list_graph_t *lg, int src, char *name)
{
	int x = src;
	ll_node_t *aux = lg->neighbors[x]->head;

	int *index = malloc(lg->nodes * sizeof(int));
	int cnt = 0;
	while (aux) {
		int neighbor = *(int *)aux->data;

		ll_node_t *aux2 = lg->neighbors[neighbor]->head;
		while (aux2) {
			if (lg_has_edge(lg, src, *(int *)aux2->data) == 0 &&
			    lg_has_edge(lg, *(int *)aux2->data, src) == 0) {
				index[cnt++] = *(int *)aux2->data;
			}
			aux2 = aux2->next;
		}

		aux = aux->next;
	}

	for (int i = 0; i < cnt - 1; i++) {
		for (int j = i + 1; j < cnt; j++) {
			if (index[i] > index[j]) {
				int aux = index[i];
				index[i] = index[j];
				index[j] = aux;
			}
		}
	}

	for (int i = 1; i < cnt; i++) {
		if (index[i] == index[i - 1]) {
			for (int j = i; j < cnt - 1; j++)
				index[j] = index[j + 1];

			--cnt;
			--i;
		}
	}

	if (cnt - 1 == 0) {
		printf("There are no suggestions for %s\n", name);
		free(index);
		return;
	}

	printf("Suggestions for %s:\n", name);
	for (int i = 0; i < cnt; i++) {
		if (index[i] != src)
			printf("%s\n", get_user_name(index[i]));
	}
	free(index);
}

void common_friends(list_graph_t *lg, int src, int dest)
{
	int x = src;
	ll_node_t *aux = lg->neighbors[x]->head;

	int *index = malloc(lg->nodes * sizeof(int));
	int cnt = 0;
	while (aux) {
		if (lg_has_edge(lg, dest, *(int *)aux->data) == 1 &&
			lg_has_edge(lg, *(int *)aux->data, dest) == 1)
			index[cnt++] = *(int *)aux->data;
		aux = aux->next;
	}

	for (int i = 0; i < cnt - 1; i++) {
		for (int j = i + 1; j < cnt; j++) {
			if (index[i] > index[j]) {
				int aux = index[i];
				index[i] = index[j];
				index[j] = aux;
			}
		}
	}

	for (int i = 1; i < cnt; i++) {
		if (index[i] == index[i - 1]) {
			for (int j = i; j < cnt - 1; j++)
				index[j] = index[j + 1];
			--cnt;
			--i;
		}
	}
	if (cnt == 0) {
		printf("No common friends for %s and %s\n", get_user_name(src),
			   get_user_name(dest));
		free(index);
		return;
	}

	printf("The common friends between %s and %s are:\n",
		   get_user_name(src), get_user_name(dest));
	for (int i = 0; i < cnt; i++) {
		if (index[i] != src)
			printf("%s\n", get_user_name(index[i]));
	}
	free(index);
}
