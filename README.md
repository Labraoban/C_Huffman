## Important
Several files not written by me are needed to compile this program.
I will push them as soon as I've verified that it's okay to push them.


## Compilation using gcc:
gcc -o huffman std=c99 -Wall main.c huffman.c dlist.c list_2cell.c table.c prioqueue.c bitset.c -lm

## Usage:
huffman [OPTION] [FILE0] [FILE1] [FILE2]
Options:
-encode encodes FILE1 according to frequence analysis done on FILE0. Stores the result in FILE2
-decode decodes FILE1 according to frequence analysis done on FILE0. Stores the result in FILE2

Example:
./huffman -encode frequency.txt infile.txt outfile.txt
./huffman -decode frequency.txt outfile.txt decoded.txt


