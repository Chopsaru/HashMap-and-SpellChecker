#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
    while (1)
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
	char* curWord = nextWord(file);

	while (curWord != NULL) {
		hashMapPut(map, curWord, 1);
		free(curWord);
		curWord = nextWord(file);
	}
}


int levenshtein(const char* s, const char* t)
{
	int ls = strlen(s), lt = strlen(t);
	int d[ls + 1][lt + 1];

	for (int i = 0; i <= ls; i++)
		for (int j = 0; j <= lt; j++)
			d[i][j] = -1;

	int dist(int i, int j) {
		if (d[i][j] >= 0) return d[i][j];

		int x;
		if (i == ls)
			x = lt - j;
		else if (j == lt)
			x = ls - i;
		else if (s[i] == t[j])
			x = dist(i + 1, j + 1);
		else {
			x = dist(i + 1, j + 1);

			int y;
			if ((y = dist(i, j + 1)) < x) x = y;
			if ((y = dist(i + 1, j)) < x) x = y;
			x++;
		}
		return d[i][j] = x;
	}
	return dist(0, 0);
}

/**
 * Checks the spelling of the word provded by the user. If the word is spelled incorrectly,
 * print the 5 closest words as determined by a metric like the Levenshtein distance.
 * Otherwise, indicate that the provded word is spelled correctly. Use dictionary.txt to
 * create the dictionary.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    HashMap* map = hashMapNew(1000);

    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);

    char inputBuffer[256];
    int quit = 0;
    while (!quit)
    {
        printf("Enter a word or \"quit\" to quit: ");
        scanf("%s", inputBuffer);

        // Implement the spell checker code here..

		//accomidate for user input string case
		tolower(inputBuffer);

		if (hashMapContainsKey(map, inputBuffer)) { //correct spelling
			printf("The inputted word %s is spelled correctly \n", inputBuffer);
		}
		else { // incorrect spelling
			printf("The inputted word %s is spelled incorrectly. \nDid you mean:\n");

			HashMap* suggestions = hashMapNew(5);
			int minDist = 1;

			while(hashMapSize(suggestions) < 5){
				for (int i = 0; i < hashMapCapacity(map); i++) {
					HashLink* cur = map->table[i];

					while (cur != NULL) {
						int wordDist = levenshtein(inputBuffer, cur->key);
						hashMapPut(map, cur->key, wordDist);
						if (wordDist = minDist) {
							hashMapPut(suggestions, cur->key, wordDist);
						}
						cur = cur->next;
					}
				}
				minDist++;
			}

			//print suggestions
			for (int i = 0; i < hashMapCapacity(suggestions); i++) {
				HashLink* cur = map->table[i];
				if (cur != NULL) {
					while (cur != NULL) {
						printf("%s \n ", cur->key);
						cur = cur->next;
					}
				}
			}
		}

        if (strcmp(inputBuffer, "quit") == 0)
        {
            quit = 1;
        }
    }

    hashMapDelete(map);
    return 0;
}
