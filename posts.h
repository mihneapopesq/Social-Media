#ifndef POSTS_H
#define POSTS_H

typedef struct post_info {
	// informatiile unei postari
	int id;
	char title[281];
	int user_id;
	int *who_liked;
	int n_likes;

} post_info;

// node posts t
// un nod are si un
typedef struct post_node {
	//nodul unui arbore de postari
	struct post_node **children;
	int parent_id;
	int children_cnt;
	void *info;
} post_node;

typedef struct events {
	//arborul de postari
	post_node *root;
	int num_reposts;
	size_t data_size;
} events;

typedef struct post_array {
	//vectorul de arbori de postari
	events **posts;
	int num_posts;
	int total_posts;
} post_array;

void free_post_tree(post_node **root);

void free_post_array(post_array **lg);

/**
 * Function that handles the calling of every command from task 2
 *
 * Please add any necessary parameters to the functions
*/
void handle_input_posts(char *input, post_array **array);

#endif // POSTS_H
