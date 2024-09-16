#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "users.h"
#include "posts.h"
#include "graph.h"

post_array **create_array(post_array **lg, int id1, char *title)
{
	(*lg)->num_posts++;
	(*lg)->total_posts++;

	if ((*lg)->num_posts == 1)
		(*lg)->posts = malloc(sizeof(events *));
	else
		(*lg)->posts = realloc((*lg)->posts,
							   (*lg)->num_posts * sizeof(events *));
	(*lg)->posts[(*lg)->num_posts - 1] = malloc(sizeof(events));
	(*lg)->posts[(*lg)->num_posts - 1]->root = NULL;
	(*lg)->posts[(*lg)->num_posts - 1]->data_size = 0;
	(*lg)->posts[(*lg)->num_posts - 1]->num_reposts = 0;
	(*lg)->posts[(*lg)->num_posts - 1]->root =
	(post_node *)malloc(sizeof(post_node));
	post_node *root_node = (*lg)->posts[(*lg)->num_posts - 1]->root;
	root_node->parent_id = -1;
	root_node->info = malloc(sizeof(post_info));
	post_info *info = (post_info *)root_node->info;
	info->n_likes = 0;
	info->user_id = id1;
	info->id = (*lg)->total_posts;
	root_node->children = malloc(MAX_POSTS * sizeof(post_node *));
	root_node->children_cnt = 0;
	info->who_liked = calloc(MAX_POSTS, sizeof(int));
	strncpy(info->title, title, MAX_LENGTH);
	info->title[MAX_LENGTH] = '\0';

	return lg;
}

void repost_2params(post_array **lg, char *name, int post_id)
{
	post_node *parent_node = NULL;
	// Finds the original post or the repost by ID
	for (int i = 0; i < (*lg)->num_posts; i++) {
		post_node *node = (*lg)->posts[i]->root;
		post_info *info = (post_info *)(node->info);
		if (info->id == post_id) {
			parent_node = node;
			(*lg)->posts[i]->num_reposts++;
			break;
		}
	}
	if (!parent_node) {
		printf("Post with ID %d not found.\n", post_id);
		return;
	}
	int new_post_id = ++((*lg)->total_posts);
	post_node *create = (post_node *)malloc(sizeof(post_node));
	memset(create, 0, sizeof(post_node));
	create->children = (post_node **)malloc(MAX_POSTS * sizeof(post_node *));
	// Initialize the children list to NULL
	memset(create->children, 0, MAX_POSTS * sizeof(post_node *));
	create->info = (post_info *)malloc(sizeof(post_info));

	memset(create->info, 0, sizeof(post_info));
	post_info *info = (post_info *)(create->info);
	info->id = new_post_id;
	info->n_likes = 0;
	info->user_id = get_user_id(name);
	info->who_liked = (int *)calloc(MAX_POSTS, sizeof(int));
	create->parent_id = post_id;
	// Add the new node to the parent node
	parent_node->children[parent_node->children_cnt++] = create;
	printf("Created repost #%d for %s\n", new_post_id, name);
}

post_node *find_post_node(post_node *node, int post_id)
{
	if (((post_info *)node->info)->id == post_id)
		return node;
	for (int i = 0; i < node->children_cnt; ++i) {
		post_node *found = find_post_node(node->children[i], post_id);
		if (found)
			return found;
	}
	return NULL;
}

post_node *find_post(post_array *array, int post_id) {
	for (int i = 0; i < array->num_posts; ++i) {
		post_node *found = find_post_node(array->posts[i]->root, post_id);
		if (found)
			return found;
	}
	return NULL;
}

void repost_3params(post_array **lg, char *name, int post_id, int repost_id)
{
	post_node *target_node = NULL;
	// Finds original post or repost by post_id/repost_id
	target_node = find_post(*lg, repost_id ? repost_id : post_id);
	if (!target_node) {
		printf("Repost with ID %d not found under post ID %d.\n",
			   repost_id, post_id);
		return;
	}
	// Create new repost
	post_info *new_info = malloc(sizeof(post_info));
	new_info->id = ++(*lg)->total_posts;
	new_info->user_id = get_user_id(name);
	new_info->n_likes = 0;
	new_info->who_liked = NULL; // Initial no likes
	new_info->title[0] = '\0'; // Title is NULL
	post_node *new_node = malloc(sizeof(post_node));
	new_node->info = new_info;
	new_node->parent_id = target_node->parent_id;
	new_node->children = NULL;
	new_node->children_cnt = 0;
	int target_children = target_node->children_cnt + 1;
	target_node->children = realloc(target_node->children,
									(target_children) * sizeof(post_node *));
	target_node->children[target_node->children_cnt++] = new_node;
	printf("Created repost #%d for %s\n", new_info->id, name);
}

void display_reposts(post_node *node, int level)
{
	post_info *info = (post_info *)node->info;
	// Verify if there s a title
	if (info->title[0] != '\0')
		printf("%s - Post by %s\n", info->title, get_user_name(info->user_id));
	else
		printf("Repost #%d by %s\n", info->id, get_user_name(info->user_id));
	for (int i = 0; i < node->children_cnt; i++)
		display_reposts(node->children[i], level + 1);
}

void getrepost(post_array **lg, int post_id)
{
	// Find specified post by post_id or post_array
	for (int i = 0; i < (*lg)->num_posts; i++) {
		post_node *root = (*lg)->posts[i]->root;
		if (((post_info *)root->info)->id == post_id) {
			// Display the post and all of its reposts
			display_reposts(root, 0);
			return;
		}
	}
	printf("Post with ID %d not found.\n", post_id);
}

void display_reposts_from_node(post_node *node)
{
	post_info *info = (post_info *)node->info;
	printf("Repost #%d by %s\n", info->id, get_user_name(info->user_id));
	for (int i = 0; i < node->children_cnt; i++)
		display_reposts_from_node(node->children[i]);
}

post_node *find_repostnode(post_node *root, int post_id)
{
	post_info *p_info = (post_info *)(root->info);
	if (p_info->id == post_id)
		return root;
	for (int i = 0; i < root->children_cnt; i++) {
		if (!root->children[i])
			continue;
		post_node *found = find_repostnode(root->children[i], post_id);
		if (found)
			return found;
	}
	return NULL;
}

void getrepost_with_repostid(post_array **lg, int post_id, int repost_id)
{
	post_node *repost_node_found = NULL;
	// Search for the post
	int poz;
	for (int i = 0; i < (*lg)->num_posts; i++) {
		post_node *current_node = (*lg)->posts[i]->root;
		if (((post_info *)current_node->info)->id == post_id) {
			poz = i;
			break;
		}
	}
	repost_node_found = find_repostnode((*lg)->posts[poz]->root, repost_id);
	display_reposts_from_node(repost_node_found);
}

int has_liked(post_info *info, char *name)
{
	for (int i = 0; i < info->n_likes; ++i) {
		if (strcmp(get_user_name(info->who_liked[i]), name) == 0)
			return 1;
	}
	return 0;
}

void add_like(post_info *info, char *name)
{
	info->n_likes++;
	info->who_liked = realloc(info->who_liked, info->n_likes * sizeof(char *));
	info->who_liked[info->n_likes - 1] = get_user_id(name);
}

void remove_like(post_info *info, char *name)
{
	int index = -1;
	for (int i = 0; i < info->n_likes; ++i) {
		if (strcmp(get_user_name(info->who_liked[i]), name) == 0) {
			index = i;
			break;
		}
	}
	if (index != -1) {
		for (int i = index; i < info->n_likes - 1; ++i)
			info->who_liked[i] = info->who_liked[i + 1];
		info->n_likes--;
		info->who_liked = realloc(info->who_liked,
								  info->n_likes * sizeof(char *));
	}
}

void like(post_array **array, char *name, int post_id)
{
	post_node *post = find_post(*array, post_id);
	if (!post) {
		printf("Post with ID %d not found.\n", post_id);
		return;
	}
	post_info *info = (post_info *)post->info;
	if (has_liked(info, name)) {
		remove_like(info, name);
		printf("User %s unliked post %s\n", name, info->title);
	} else {
		add_like(info, name);
		printf("User %s liked post %s\n", name, info->title);
	}
}

void like_3params(post_array **array, char *name, int post_id, int repost_id)
{
	int target_id = repost_id ? repost_id : post_id;
	post_node *post = find_post(*array, target_id);
	if (!post) {
		printf("Post with ID %d not found.\n", target_id);
		return;
	}

	post_info *info = (post_info *)post->info;
	post_info *original_info = NULL;
	if (repost_id) {
		post_node *original_post = find_post(*array, post_id);
		if (original_post)
			original_info = (post_info *)original_post->info;
	}
	if (has_liked(info, name)) {
		remove_like(info, name);
		if (original_info)
			printf("User %s unliked repost %s\n", name, original_info->title);
	} else {
		add_like(info, name);
		if (original_info)
			printf("User %s liked repost %s\n", name, original_info->title);
	}
}

void ratio(post_array **lg, int post_id)
{
	post_node *post_node_found = NULL;
	post_node *top_ratio_node = NULL;
	int max_likes = -1;
	// Search for the node of the original post
	for (int i = 0; i < (*lg)->num_posts; i++) {
		post_node *current_node = (*lg)->posts[i]->root;
		if (((post_info *)current_node->info)->id == post_id) {
			post_node_found = current_node;
			break;
		}
	}
	if (!post_node_found) {
		printf("Post with ID %d not found.\n", post_id);
		return;
	}
	int original_likes = ((post_info *)post_node_found->info)->n_likes;
	// See if reposts have more likes than the original post
	for (int i = 0; i < post_node_found->children_cnt; i++) {
		post_node *child = post_node_found->children[i];
		int child_likes = ((post_info *)child->info)->n_likes;
		if (child_likes > max_likes && child_likes > original_likes) {
			top_ratio_node = child;
			max_likes = child_likes;
		}
	}
	// Determinam rezultatul final
	if (top_ratio_node)
		printf("Post %d got ratio'd by repost %d\n", post_id,
			   ((post_info *)top_ratio_node->info)->id);
	else
		printf("The original post is the highest rated\n");
}

void get_likes2(post_array **array, int post_id)
{
	post_node *post = find_post(*array, post_id);
	if (!post) {
		printf("Post with ID %d not found.\n", post_id);
		return;
	}
	post_info *info = (post_info *)post->info;
	printf("Post %s has %d likes\n", info->title, info->n_likes);
}

void get_likes3(post_array **array, int post_id, int repost_id)
{
	post_node *post = find_post(*array, repost_id);
	if (!post) {
		printf("Repost with ID %d not found.\n", repost_id);
		return;
	}
	post_info *info = (post_info *)post->info;
	post_node *original_post = find_post(*array, post_id);
	if (!original_post) {
		printf("Original post with ID %d not found.\n", post_id);
		return;
	}
	printf("Repost #%d has %d likes\n", repost_id, info->n_likes);
}

void delete_post_node(post_node *node)
{
	for (int i = 0; i < node->children_cnt; ++i)
		delete_post_node(node->children[i]);
	post_info *info = (post_info *)node->info;
	free(info->who_liked);
	free(info);
	free(node->children);
	free(node);
}

void delete2(post_array **array, int post_id)
{
	post_node *post = find_post(*array, post_id);
	if (!post) {
		printf("Post with ID %d not found.\n", post_id);
		return;
	}
	post_info *info = (post_info *)post->info;
	printf("Deleted post %s\n", info->title);
	// Delete the post and all its reposts recursively
	delete_post_node(post);
	// Remove the post from the post array
	for (int i = 0; i < (*array)->num_posts; ++i) {
		if ((*array)->posts[i]->root->info == info) {
			free((*array)->posts[i]);
			for (int j = i; j < (*array)->num_posts - 1; ++j)
				(*array)->posts[j] = (*array)->posts[j + 1];
			(*array)->num_posts--;
			break;
		}
	}
}

void delete3(post_array **array, int post_id, int repost_id)
{
	post_node *repost = find_post(*array, repost_id);
	if (!repost) {
		printf("Repost with ID %d not found.\n", repost_id);
		return;
	}
	post_node *find_post_title = find_post(*array, post_id);
	const char *title = ((post_info *)(find_post_title->info))->title;
	printf("Deleted repost #%d of post %s\n", repost_id, title);
	// Delete the repost and all its reposts recursively
	delete_post_node(repost);
	// Find the parent post node to update its children list
	post_node *parent_post = find_post(*array, post_id);
	if (parent_post) {
		for (int i = 0; i < parent_post->children_cnt; ++i) {
			if (parent_post->children[i] == repost) {
				for (int j = i; j < parent_post->children_cnt - 1; ++j)
					parent_post->children[j] = parent_post->children[j + 1];
				parent_post->children_cnt--;
				parent_post->children = realloc(parent_post->children,
												parent_post->children_cnt
												* sizeof(post_node *));
				break;
			}
		}
	}
}

void dfs_mark_parents(post_node *node, post_node **parents)
{
	for (int i = 0; i < node->children_cnt; ++i) {
		parents[((post_info *)node->children[i]->info)->id] = node;
		dfs_mark_parents(node->children[i], parents);
	}
}

// Find out the LCA (Least Common Ancestor)
post_node *find_lca(post_node *root, int id1, int id2)
{
	post_node *parents[MAX_POSTS] = {NULL};
	dfs_mark_parents(root, parents);
	// Mark all the parents of node with the id1
	int visited[MAX_POSTS] = {0};
	while (id1 != -1) {
		visited[id1] = 1;
		id1 = parents[id1] ? ((post_info *)parents[id1]->info)->id : -1;
	}
	// Find the first common node in the way of the node with id2
	while (id2 != -1) {
		if (visited[id2])
			return find_post_node(root, id2);
		id2 = parents[id2] ? ((post_info *)parents[id2]->info)->id : -1;
	}
	return NULL;
}

void common_group(post_array **array, int post_id,
				  int repost_id1, int repost_id2)
{
	post_node *post = find_post(*array, post_id);
	if (!post) {
		printf("Post with ID %d not found.\n", post_id);
		return;
	}
	post_node *lca_node = find_lca(post, repost_id1, repost_id2);
	if (lca_node)
		printf("The first common repost of %d and %d is %d\n",
			   repost_id1, repost_id2, ((post_info *)lca_node->info)->id);
	else
		printf("No common repost found for reposts %d and %d.\n",
			   repost_id1, repost_id2);
}

void free_post_node(post_node *node)
{
	if (!node)
		return;
	for (int i = 0; i < node->children_cnt; ++i)
		free_post_node(node->children[i]);
	if (node->info) {
		post_info *info = (post_info *)node->info;
		free(info->who_liked);
		free(info);
	}
	free(node->children);
	free(node);
}

void free_post_array(post_array **lg)
{
	if (!lg || !(*lg))
		return;
	for (int i = 0; i < (*lg)->num_posts; ++i) {
		events *event = (*lg)->posts[i];
		if (event) {
			free_post_node(event->root);
			free(event);
		}
	}
	free((*lg)->posts);
	free(*lg);
	*lg = NULL;
}

void handle_input_posts(char *input, post_array **array)
{
	char name1[MAX_NAME], title[MAX_LENGTH];
	char dummy[MAX_LENGTH];
	char copy_commands[VAR_CHAR];
	int post_id, repost_id, num_params;
	char *commands = strdup(input);
	strcpy(copy_commands, input);
	char *cmd = strtok(commands, "\n ");
	if (!cmd)
		return;
	if (!strcmp(cmd, "create")) {
		if (sscanf(copy_commands, "%s %s \"%[^\"]\"",
				   dummy, name1, title) != 3) {
			printf("Wrong parameters\n");
			return;
		}
		size_t len = strlen(title);
		memmove(title + 1, title, len + 1);
		title[0] = '"';
		title[len + 1] = '"';
		title[len + 2] = '\0';
		int id1;
		id1 = get_user_id(name1);
		array = create_array(array, id1, title);
		printf("Created %s for %s\n", title, name1);
	} else if (!strcmp(cmd, "repost")) {
		num_params = sscanf(copy_commands, "repost %s %d %d",
							name1, &post_id, &repost_id);
		if (num_params == 2)
			repost_2params(array, name1, post_id);
		else if (num_params == 3)
			repost_3params(array, name1, post_id, repost_id);
	} else if (!strcmp(cmd, "common-repost")) {
		int post_id;
		int repost_id1, repost_id2;
		if (sscanf(copy_commands, "common-repost %d %d %d", &post_id,
				   &repost_id1, &repost_id2) != 3) {
			printf("Wrong parameters\n");
			return;
		}
		common_group(array, post_id, repost_id1, repost_id2);
	} else if (!strcmp(cmd, "like")) {
		num_params = sscanf(copy_commands, "like %s %d %d",
							name1, &post_id, &repost_id);
		if (num_params == 2)
			like(array, name1, post_id);
		else if (num_params == 3)
			like_3params(array, name1, post_id, repost_id);
	} else if (!strcmp(cmd, "ratio")) {
		int post_id;
		if (sscanf(copy_commands, "ratio %d", &post_id) != 1) {
			printf("Wrong parameters");
			return;
		}
		ratio(array, post_id);
	} else if (!strcmp(cmd, "delete")) {
		num_params = sscanf(copy_commands, "delete %d %d",
							&post_id, &repost_id);
		if (num_params == 1)
			delete2(array, post_id);
		else if (num_params == 2)
			delete3(array, post_id, repost_id);
	} else if (!strcmp(cmd, "get-likes")) {
		num_params = sscanf(copy_commands, "get-likes %d %d",
							&post_id, &repost_id);
		if (num_params == 1)
			get_likes2(array, post_id);
		else if (num_params == 2)
			get_likes3(array, post_id, repost_id);
	} else if (!strcmp(cmd, "get-reposts")) {
		num_params = sscanf(copy_commands, "get-reposts %d %d",
							&post_id, &repost_id);
		if (num_params == 1)
			getrepost(array, post_id);
		else if (num_params == 2)
			getrepost_with_repostid(array, post_id, repost_id);
	}
	free(commands);
}
