/* Laban Melander 2017
 * dv16lmr@cs.umu.se
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <memory.h>

#include "huffman.h"
#include "list_2cell.h"
#include "bitset.h"


/* huffman_calculateFrequency
*
* Input:  Path and filename to the file that the will ba analysed.
* Output: Returns an int array of the length 257 containing information
*         on how many times each character occur.
*         NULL if the file could not be read
*/
int* huffman_calculateFrequency(const char* fileName)
{
	FILE* file = fopen(fileName, "rb");

	if (file == NULL)
		return NULL;

	fseek(file, 0L, SEEK_END);
	long inlen = ftell(file);
	rewind(file);

	int* frequencyArray = calloc(FREQ_ARR_SIZE, sizeof(int));

	for (int i = 0; i < inlen; ++i)
	{
		unsigned char c;
		fread(&c, 1, 1, file);
		frequencyArray[c]++;
	}

	fclose(file);

	return frequencyArray;
}

/* huffman_create
*
* Input:  An int array with a length of 257 containing the frequency
*         analysis required.
* Output: HuffmanData struct containing a pointer to the root node of
*         the tree and a table containing the paths to each char and
*  	   the 257 char that is used as a terminator.
*/
HuffmanData* huffman_create(int frequency[FREQ_ARR_SIZE])
{
	pqueue* pq = pqueue_empty(huffman_help_prioqueueCompare);
	list* l = list_empty();

	for (int i = 0; i < FREQ_ARR_SIZE; ++i)
	{
		Node* node = malloc(sizeof(Node));
		node->symbol = i;
		node->value = frequency[i];
		node->parent = NULL;
		node->left = NULL;
		node->right = NULL;

		pqueue_insert(pq, node);
		list_insert(l, node, list_end(l));
	}

	Node* root = huffman_createTree(pq);
	pqueue_free(pq);

	list_position p = list_first(l);


	char* buffer = calloc(FREQ_ARR_SIZE, sizeof(char));

	Table* table = table_create(huffman_help_tableCompare);
	table_setKeyMemHandler(table, free);


	for (unsigned int i = 0; i < FREQ_ARR_SIZE; ++i)
	{
		huffman_getPathToRoot(list_inspect(l, p), buffer, FREQ_ARR_SIZE);
		huffman_help_reverseString(buffer);

		bitset* bset = bitset_empty();
		int bitIndex = 0;

		char* current = buffer;

		while (*current != '\0')
		{
			bitset_setBitValue(bset, bitIndex, *current == '0' ? false : true);
			bitIndex++;
			current++;
		}

		//printf("%3d, %c,  %s\n", i, i, buffer);


		int *k = malloc(sizeof(int));
		*k = i;

		table_insert(table, (KEY)k, bset);

		p = list_next(l, p);

	}

	free(buffer);
	list_free(l);
	HuffmanData* hdata = malloc(sizeof(HuffmanData));
	hdata->root = root;
	hdata->table = table;
	return hdata;
}

/* huffman_createTree
*
* Input:  A prioqueue containing the nodes that will become the
*         trees leafs.
* Output: The root node of the created tree.
*/
Node* huffman_createTree(pqueue* pq)
{
	while (!pqueue_isEmpty(pq))
	{
		Node* n1 = pqueue_inspect_first(pq);
		pqueue_delete_first(pq);

		if (pqueue_isEmpty(pq))
			return n1;

		Node* n2 = pqueue_inspect_first(pq);
		pqueue_delete_first(pq);

		Node* parent = (Node*)malloc(sizeof(Node));

		parent->symbol = -1;
		parent->parent = NULL;
		parent->left = n1;
		parent->right = n2;
		parent->value = n1->value + n2->value;

		n1->parent = parent;
		n2->parent = parent;

		pqueue_insert(pq, parent);
	}
	return NULL;
}

/* huffman_getPathToRoot
*
* Input:  The node from which it finds the path to the root.
*         A buffer that will store the string that represents
*         the path to the root node.
*         The size of the buffer
* Output: A string made out of 1's and 0's representing the path
*         to the root node is placed in the buffer.
*/
void huffman_getPathToRoot(Node* node, char* str, int maxStrSize)
{
	if (node->parent == NULL)
	{
		*str = '\0';
		return;
	}

	Node* parent = node->parent;

	if (parent->left == node)
	{
		*str = '0';
		huffman_getPathToRoot(parent, ++str, maxStrSize);
		return;
	}
	if (parent->right == node)
	{
		*str = '1';
		huffman_getPathToRoot(parent, ++str, maxStrSize);
		return;
	}
}

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
bool huffman_encode(HuffmanData* data, char* inFileName, char* outFileName)
{

	FILE* in = fopen(inFileName, "rb");

	if (in == NULL)
	{
		printf("Error: Could not open file: %s\n", inFileName);
		return false;
	}

	FILE* out = fopen(outFileName, "w");

	fseek(in, 0L, SEEK_END);
	long flen = ftell(in);
	rewind(in);

	bitset* completebitset = bitset_empty();

	for (long long i = 0; i < flen; ++i)
	{
		unsigned char currentSymol;
		fread(&currentSymol, 1, 1, in);

		int key = (int)currentSymol;
		bitset* bitPath = table_lookup(data->table, &key);
		huffman_help_appendBitset(completebitset, bitPath);
	}

	fclose(in);

	// Appends END_OF_ENCODING bit-pattern at the end
	int c = END_OF_ENCODING;
	bitset* bitPath = table_lookup(data->table, &c);
	huffman_help_appendBitset(completebitset, bitPath);

	unsigned long long bitIndex = 0;;
	unsigned long long totalBits = ceil((float)bitset_size(completebitset) / 8.0f) * 8;

	while (bitIndex < totalBits)
	{
		unsigned char currentByte = '\0';

		for (int i = 0; i < BYTESIZE; ++i)
		{
			bool bit = bitset_memberOf(completebitset, bitIndex);
			currentByte |= bit << (7 - i);
			bitIndex++;
		}
		fputc(currentByte, out);
	}

	bitset_free(completebitset);
	fclose(out);

	return true;
};

/* huffman_decode
*
* Input:  HuffmanData struct containing the root to the tree
*         that is needed to decode the file.
*         The path and name of the encoded file.
*         The path and name to the output file.
*
* Output: The decoded file is stored in the output file.
*/
bool huffman_decode(HuffmanData* data, char* encodedFileName, char* outFileName)
{
	FILE* encoded = fopen(encodedFileName, "rb");

	if (encoded == NULL)
	{
		printf("Error: Could not open file: %s\n", encodedFileName);
		return false;
	}

	fseek(encoded, 0L, SEEK_END);
	long fileSize = ftell(encoded);
	rewind(encoded);

	FILE* out = fopen(outFileName, "w");

	Node* currentNode = data->root;

	for (long i = 0; i < fileSize; ++i)
	{
		unsigned char currentSymbol;
		fread(&currentSymbol, 1, 1, encoded);

		for (short j = 0; j < BYTESIZE; ++j)
		{
			if (currentNode->symbol == END_OF_ENCODING)
			{
				fclose(out);
				fclose(encoded);
				return true;
			}

			if (currentNode->symbol >= 0)
			{
				putc(currentNode->symbol, out);
				currentNode = data->root;
			}

			bool bitValue = ((currentSymbol << j) & 0x80);
			currentNode = (bitValue ? currentNode->right : currentNode->left);
		}
	}

	fclose(out);
	fclose(encoded);

	printf("Error: Terminating character with value 256 (END_OF_ENCODING) not found\n");
	return false;
}

/* huffman_free
*
* Input:	Pointer to HuffmanData
* Output:	None
* Purpose:	Frees the tree and the table.
*/
void huffman_free(HuffmanData* hd)
{
	huffman_freeTree(hd->root);

	for (int i = 0; i < FREQ_ARR_SIZE; i++)
	{
		bitset_free((bitset*)table_lookup(hd->table, &i));
	}

	table_free(hd->table);
	free(hd);
}

/* huffman_freeTree
*
* Input:	Root node of the tree
* Output:	None
* Purpose:	Frees each node in the tree.
*/
void huffman_freeTree(Node* node)
{
	if (node->left != NULL)
		huffman_freeTree(node->left);
	if (node->right != NULL)
		huffman_freeTree(node->right);

	free(node);
};

/* huffman_getFileSize
*
* Input:  The path and name to the file
*
* Output: The size of the file in bytes.
*/
long huffman_getFileSize(char* filename)
{
	FILE* file = fopen(filename, "rb");
	if (file == NULL)
		return -1;

	fseek(file, 0L, SEEK_END);

	long size = ftell(file);

	fclose(file);

	return size;
}

/* huffman_help_reverseString
*
* Input:  String to be reversed.
*
* Output: The reversed string is stored in place.
*/
void huffman_help_reverseString(char* str)
{
	int length = strlen(str) - 1;

	for (int i = 0; i < length - i; ++i)
	{
		unsigned char c = str[i];
		str[i] = str[length - i];
		str[length - i] = c;
	}
};

/* huffman_help_appendBitset
*
* Input:  Two bitsets, the second bitset will be added at the end
*         of the first.
*
* Output: The second bitset is added to the first.
*/
void huffman_help_appendBitset(bitset* target, bitset* source)
{
	long targetSize = bitset_size(target);
	long sourceSize = bitset_size(source);

	for (long i = 0; i < sourceSize; ++i)
	{
		bitset_setBitValue(target, targetSize + i, bitset_memberOf(source, i));
	}
};

/* huffman_help_prioqueueCompare
*
* Comparson function for the prioqueue.
*/
int huffman_help_prioqueueCompare(VALUE v1, VALUE v2)
{
	int i1 = *(int*)v1;
	int i2 = *(int*)v2;
	if (i1 < i2)
		return 1;
	return 0;
}

/* huffman_help_tableCompare
*
* Comparson function for the table.
*/
int huffman_help_tableCompare(KEY k1, KEY k2)
{
	int i1 = *(int*)k1;
	int i2 = *(int*)k2;

	if (i1 > i2)
		return 1;
	else if (i1 < i2)
		return -1;
	return 0;
};