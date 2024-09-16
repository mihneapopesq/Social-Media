# Social-Media

The Social Media platform is designed to facilitate user interactions through posts, reposts, likes, and social connections. It provides various functionalities to manage and explore content and connections, enabling users to engage with their network in meaningful ways.

Key features include:

- **Post Creation and Management:** Users can create new posts, manage reposts, and delete posts as needed. Each post is associated with a user and can be liked by other users.
- **User Interaction:** Users can like posts, view posts from their friends, and explore profiles to see all their posts and reposts.
- **Social Networking:** The platform supports friend connections and identifies common friends between users. It also allows users to see which friends have reposted their content.
- **Advanced Features:** Includes algorithms for finding cliques in the social network, suggesting friends, and analyzing user engagement through metrics such as like ratios.

The system leverages various data structures and algorithms to manage and analyze posts and friendships efficiently.

## Friendship Network

* To determine the distance between two nodes, we use the BFS algorithm.
* To store information about nodes, we use a generic data structure.
* For subtasks like suggestions, common friends, and the most popular, we use an auxiliary vector to keep track of adjacent nodes to the source node.

## Posts and Reposts

* Create - The code increments the post counters, allocates or reallocates memory for the posts array, creates and initializes a new post and its root node, sets the post information, and returns the pointer to the updated posts array.

* Repost - This function allows a user to repost an existing post. It takes as parameters the original post and the user making the repost. The function creates a new repost post by copying the information from the original post and setting the reposting user as the author. The repost post is then added to the posts array, and the updated array is returned.

* Common - This function takes two posts and returns true if they have a common author and false otherwise. It compares the author users of the two posts and returns the result of the check.

* Like - This function allows a user to like a post. It takes as parameters the post and the user liking it. The function searches for the post in the posts array and adds the user to the list of users who liked that post.

* Ratio - This function calculates the ratio between the number of likes and the total number of posts by a user. It takes as parameters the user and the posts array. The function iterates through the posts array and counts how many posts were made by the user and how many likes those posts received. It then calculates the ratio and returns it.

* Delete - This function allows a user to delete a post. It takes as parameters the post and the posts array. The function searches for the post in the posts array and removes it.

* Get_reposts - This function takes a post and returns an array with all the reposts of that post. It iterates through the posts array and checks if each post is a repost of the given post. If so, the post is added to the reposts array, which is returned at the end.

## Social Media

* Feed - Iterates through posts from end to start and displays only those made by a friend of the original user, or by the user themselves. It displays a number of posts equal to the minimum of `feed_size` and the total number of posts.

* View profile - Recursively traverses all trees in the posts vector, meaning all existing posts and reposts. It displays only the posts and reposts corresponding to the given user.

* Friends that reposted - Searches for the user's post in the posts vector and then traverses the corresponding tree, displaying only those users who have reposted but are also friends with the user.

* Clique - The `bron_kerbosch` function receives the clique R for expansion, and X is a linked list of nodes that have already been processed and should not be reintroduced into the current clique. `graph` is the graphical structure representing the connections between nodes, `max_clique` is a pointer to the largest clique found so far, and `start_node` is the starting node for finding the maximum clique.

The algorithm begins by checking an essential termination condition. If lists P and X are empty, it means R is a maximum clique, as there are no additional nodes in P to extend R and none in X to restrict the extension of R. In this case, if R contains the start node and its size is larger than that of the previously found maximum clique, `max_clique` is updated to be a copy of R.

For each node in `P_without_neighbors`, the algorithm creates a new clique `new_R` by adding the current node to R. It also creates new lists `new_P` and `new_X`, which contain the neighbors of the current node in P and X, respectively. Then, the `bron_kerbosch` function is called recursively with the new lists to continue the clique expansion process.
