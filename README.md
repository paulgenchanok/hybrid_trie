# Hybrid Trie

An attempt to implement a somewhat memory / CPU efficient autocomplete program that can actually autocomplete an email. Still a work in progress.

## Initial methods

There are two extremes in implementing tries. The first extreme is that each node has a static array of pointers to the next node. The index of each pointer in that array represents a node associated with an ASCII character. The data structure for a node looks something like this:

```
typedef struct _trie_node {

    struct _trie_node * p_children[ALPHABET_SIZE];
    float               popularity;
    uint8_t             character;

} trie_node_t;
```
Where `ALPHABET_SIZE` is a constant representing the number of ASCII characters accepted. `p_children` is the static array of pointers. A node is accessed in O(1) time by taking the offset of the requested character. For example, assume this build out is supporting only alphabetical words in the range `[a-zA-Z]`. `ALPHABET_SIZE` would be `57` since `z = 122` in ASCII and `A = 65` in ASCII. If you're searching if a node has a character `char` among children you'd calculate the array offset with `offset = char - ASCII_PRINT_START` where `ASCII_PRINT_START = 57`.  

If the poitner at `offset` is `NULL` then that's not a valid follow-on character. Again, this should be fast. But it comes at a huge memory cost.

On a 64-bit machine the `p_children` array is guaranteed to be `8 * ALPHABET_SIZE` bytes in size. Per node. If a node is leaf, that is it only holds one character as the end of a word and has no valid children, it has `8 * ALPHABET_SIZE` bytes of overhead because it has `ALPHABET_SIZE` pointers pointing to `NULL`.  

For example, I loaded this type of trie with a test dictionary of 30,000 unique words and support for ASCII characters 33 through 126. So it could store an email in theory. The result was about 6.7 million pointers to NULL. That's not memory efficient at all.

The second extreme is to replace the the static array with a dynamic array. `p_children` is instead a dynamic array that is reallocated as necessary when a new element is added. The data structure for a node looks something like this:

```
typedef struct _trie_node_csearch {

    struct _trie_node_csearch * p_children;
    uint8_t                     array_size;
    uint8_t                     num_children;
    float                       popularity;
    uint8_t                     character;

} tnode_csearch_t;
```
Extraneous NULL pointers can be completely eliminated with this implementation by reallocing the `p_children` to include space for one additional element whenever a new character needs to be added. The node itself has to be two bytes larger to hold some metadata about the dynamic pointer array, but is still a significant memory improvement over many many pointers to NULL.  

There are two straightforward costs to this approach: memory allocation is expensive, and searching through the `p_children` array is expensive. Saying you're searchign for a character `char`. `p_children` is searched linearly to find a node that has a character matching `char`. If no nodes in `p_children` match then `char` is not a valid. The more nodes in `p_children` the more expensive searches are likely to be. This could be mitigated, somewhat, if `p_children` was organized as a binary search tree, or other data structure, but those have associated overhead costs as well.  

A significant benefit to this approach is that it natively supports the entire ASCII table.

## Hybrid Trie

A hybrid trie combines both. The first few "levels" of the trie (e.g. the first N characters of any word being looked up) are stored in a static children arrays. Lookups are performed in O(1) time based off the ASCII character's offset. After N characters the trie transitions to dynamically allocated children arrays where node lookups are done sequentially through a child array. I implemented this with three different types of nodes. It's possible with only two types of nodes. But then there's some additional pointer casting to work through. The node structures look something like:
```
typedef struct _htrie_depth_node {

    void    * p_children[ALPHABET_SIZE]; // htrie_dnode_t or htrie_tnode_t
    uint8_t   depth; 
    uint8_t   character;
    float     popularity;

} htrie_dnode_t;

typedef struct _htrie_cnode {

    struct _htrie_cnode * p_children;
    uint8_t               num_children;
    uint8_t               array_size;
    uint8_t               character;
    float                 popularity;

} htrie_cnode_t;


typedef struct _htrie_transition_node {

    htrie_cnode_t * p_children[ALPHABET_SIZE];
    uint8_t         depth;
    uint8_t         character;
    float           popularity;


} htrie_tnode_t;
```
A hybrid trie starts with "depth nodes" in which children are stored in static arrays. Each depth node tracks its depth with the root node starting with zero. At a predetermined depth the children of depth nodes become transition nodes (`htrie_tnode_t`). Children are also stored in a static array here but point to nodes that dynamically allocated children arrays.  

The tradeoff goal of a hybrid trie is that we accept some NULL pointers for the first few characters of a word, where trie nodes are more likely to have a large number of valid children, and then transition to dynamically allocated children arrays when nodes are more likely to have fewer valid children.  

## Some data

Some data to justify a hybrid trie. I built out a trie with dynamically allocated children and captured for each level (i.e. depth) how many nodes there are, how many children each node has, and the average / variance thereof. The table is below. I used a wordlist with [370,108 unique english words](words_alpha.txt) to build the trie.

|Level|Avg num children|Variance num children|Num nodes|
|-----|----------------|---------------------|---------|
|0    |26              |                     |1        |
|1    |21.53846154     |15.85846154          |26       |
|2    |8.569642857     |69.41911257          |560      |
|3    |5.144613461     |26.6589492           |4799     |
|4    |2.697152578     |8.082574928          |24689    |
|5    |1.674590779     |2.978625713          |66590    |
|6    |1.269740205     |1.440996183          |111511   |
|7    |1.069150364     |0.859939323          |141590   |
|8    |0.935810967     |0.602703628          |151381   |
|9    |0.820836628     |0.499196364          |141664   |
|10   |0.777491121     |0.436738319          |116283   |
|11   |0.731785552     |0.399843685          |90409    |
|12   |0.682829504     |0.368422956          |66160    |
|13   |0.643925978     |0.348559896          |45176    |
|14   |0.600309385     |0.327745788          |29090    |
|15   |0.566626582     |0.3165863            |17463    |
|16   |0.543304699     |0.306771168          |9895     |
|17   |0.498511905     |0.288369878          |5376     |
|18   |0.495149254     |0.28217131           |2680     |
|19   |0.465712133     |0.268619836          |1327     |
|20   |0.449838188     |0.260850865          |618      |
|21   |0.45323741      |0.292029193          |278      |
|22   |0.428571429     |0.262857143          |126      |
|23   |0.462962963     |0.253319357          |54       |
|24   |0.52            |0.26                 |25       |
|25   |0.615384615     |0.256410256          |13       |
|26   |1               |0                    |8        |
|27   |0.625           |0.267857143          |8        |
|28   |0.6             |0.3                  |5        |
|29   |0.333333333     |0.333333333          |3        |
|30   |1               |                     |1        |
|31   |0               |                     |1        |

A hybrid trie depth of 3 or 4 would be most appropriate for a dataset this size. After the third or fourth level the average number of children per node decreases drastically although there is still a high variance at the fourth level.




