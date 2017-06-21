/* Laban Melander 2017
 * dv16lmr@cs.umu.se
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <memory.h>

#include "huffman.h"

#define OPTION int

#define ERROR -1
#define ENCODE 0
#define DECODE 1

OPTION ui_getOption(char* option)
{
	if (strcmp(option, "-encode") == 0)
		return 0;
	if (strcmp(option, "-decode") == 0)
		return 1;
	return -1;
}

void ui_printGuide()
{
	printf("USAGE:\n");
	printf("huffman [OPTION] [FILE0] [FILE1] [FILE2]\n");
	printf("Options:\n");
	printf("-encode encodes FILE1 according to frequence analysis done on FILE0. Stores the result in FILE2\n");
	printf("-decode decodes FILE1 according to frequence analysis done on FILE0. Stores the result in FILE2\n");
}

int main(int argc, char** argv)
{
	HuffmanData* huffmanData = NULL;

	if (argc != 5)
	{
		ui_printGuide();
		return 0;
	}

	OPTION option = ui_getOption(argv[1]);

	if (option == ERROR)
	{
		ui_printGuide();
		return 0;
	}

	int* characterFrequency = huffman_calculateFrequency(argv[2]);

	if (characterFrequency == NULL)
	{
		free(characterFrequency);
		printf("ERROR: Failed to preform a frequencyanalysis on file: %s\n", argv[2]);
		return 0;
	}

	huffmanData = huffman_create(characterFrequency);
	free(characterFrequency);

	if (option == ENCODE)
	{
		if (!huffman_encode(huffmanData, argv[3], argv[4]))
		{
			huffman_free(huffmanData);
			return 0;
		}

		printf("%ld bytes read from %s\n", huffman_getFileSize(argv[3]), argv[3]);
		printf("%ld bytes used in encoded form\n", huffman_getFileSize(argv[4]));
	}

	if (option == DECODE)
	{
		if (!huffman_decode(huffmanData, argv[3], argv[4]))
		{
			huffman_free(huffmanData);
			return 0;
		}

		printf("File decoded succesfully.\n");
	}

	huffman_free(huffmanData);
	return 0;
}