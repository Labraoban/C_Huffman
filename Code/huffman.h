/* Laban Melander 2017
 * dv16lmr@cs.umu.se
 */

#ifndef huffman_huffman_h
#define huffman_huffman_h

#include "dlist.h"
#include "prioqueue.h"
#include "table.h"
#include "bitset.h"

#define FREQ_ARR_SIZE 257
#define END_OF_ENCODING 256
#define BYTESIZE 8

struct Node;
typedef struct Node
{
	int value;
	int symbol;
	struct Node* parent;
	struct Node* left;
	struct Node* right;

} Node;


typedef struct HuffmanData
{
	Node* root;
	Table* table;

} HuffmanData;


/* huffman_calculateFrequency
 *
 * Input:  Path and filename to the file that the will ba analysed.
 * Output: Returns an int array of the length 257 containing information
 *         on how many times each character occur.
 *         NULL if the file could not be read
 */
int* huffman_calculateFrequency(const char* fileName);


/* huffman_create
 *
 * Input:  An int array with a length of 257 containing the frequency
 *         analysis required.
 * Output: HuffmanData struct containing a pointer to the root node of
 *         the tree and a table containing the paths to each char and
 *  	   the 257 char that is used as a terminator.
 */
HuffmanData* huffman_create(int frequency[FREQ_ARR_SIZE]);


/* huffman_createTree
 *
 * Input:  A prioqueue containing the nodes that will become the
 *         trees leafs.
 * Output: The root node of the created tree.
 */
Node* huffman_createTree(pqueue* queue);


/* huffman_getPathToRoot
 *
 * Input:  The node from which it finds the path to the root.
 *         A buffer that will store the string that represents
 *         the path to the root node.
 *         The size of the buffer
 * Output: A string made out of 1's and 0's representing the path
 *         to the root node is placed in the buffer.
 */
void huffman_getPathToRoot(Node* node, char* str, int maxStrSize);

/* huffman_encode
 *
 * Input:  HuffmanData struct containing the table that is needed
 *         to encode.
 *         The path and name of the file to be ecoded.
 *         The path and name of the file where the encoding will
 *  	   be stored.
 *
 * Output: Writes the encoded data to the output file.
 *		   Returns false if the inputfile could not be opened.
 */
bool huffman_encode(HuffmanData* data, char* inFileName, char* outFileName);


/* huffman_decode
 *
 * Input:  HuffmanData struct containing the root to the tree
 *         that is needed to decode the file.
 *         The path and name of the encoded file.
 *         The path and name to the output file.
 *
 * Output: The decoded file is stored in the output file.
 */
	bool huffman_decode(HuffmanData* data, char* encodedFileName, char* outFileName);


/* huffman_free
 *
 * Input:	Pointer to HuffmanData
 * Output:	None
 * Purpose:	Frees the tree and the table.
 */
void huffman_free(HuffmanData* hd);


/* huffman_freeTree
 *
 * Input:	Root node of the tree
 * Output:	None
 * Purpose:	Frees each node in the tree.
 */
void huffman_freeTree(Node* node);


/* huffman_getFileSize
 *
 * Input:  The path and name to the file
 *
 * Output: The size of the file in bytes.
 */
long huffman_getFileSize(char* filename);


/* huffman_help_reverseString
 *
 * Input:  String to be reversed.
 *
 * Output: The reversed string is stored in place.
 */
void huffman_help_reverseString(char* str);


/* huffman_help_appendBitset
 *
 * Input:  Two bitsets, the second bitset will be added at the end
 *         of the first.
 *
 * Output: The second bitset is added to the first.
 */
void huffman_help_appendBitset(bitset* target, bitset* source);


/* huffman_help_prioqueueCompare
 *
 * Comparson function for the prioqueue.
 */
int huffman_help_prioqueueCompare(VALUE v1, VALUE v2);


/* huffman_help_tableCompare
 *
 * Comparson function for the table.
 */
int huffman_help_tableCompare(KEY k1, KEY k2);


#endif