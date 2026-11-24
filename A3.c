#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "A3.h"

void removeNewline(char* text)
{
    size_t length;

    if (text != NULL)
    {
        length = strlen(text);

        while (length > 0 &&
            (text[length - 1] == '\n' || text[length - 1] == '\r'))
        {
            text[length - 1] = '\0';
            length--;
        }
    }
}

char* duplicateName(const char* name)
{
    char* copy;
    size_t length;

    if (name == NULL)
    {
        return NULL;
    }
    length = strlen(name);
    copy = malloc((length + 1) * sizeof(char));
    if (copy != NULL)
    {
        if (strcpy_s(copy, length + 1, name) != 0)
        {
            free(copy);
            copy = NULL;
        }
    }
    return copy;
}

int insertSorted(linkedList** head, const char* name)
{
    linkedList* newNode;
    linkedList* current;
    linkedList* previous;

    //new node initialization
    if (head == NULL || name == NULL)
    {
        return 0;
    }
    newNode = malloc(sizeof(linkedList));
    if (newNode == NULL)
    {
        return 0;
    }
    newNode->name = duplicateName(name);
    if (newNode->name == NULL)
    {
        free(newNode);
        return 0;
    }
    newNode->next = NULL;

    //insertion at the beginning
    if (*head == NULL || strcmp(name, (*head)->name) < 0)
    {
        newNode->next = *head;
        *head = newNode;
        return 1;
    }
    
    //insertion at the middle or end
    previous = *head;
    current = (*head)->next;
    while (current != NULL && strcmp(current->name, name) < 0)
    {
        previous = current;
        current = current->next;
    }
    newNode->next = current;
    previous->next = newNode;
    return 1;
}

int hashName(const char* name)
{
    unsigned long hashValue;
    size_t i;
    hashValue = 0;
    for (i = 0; name[i] != '\0'; i++)
    {
        //17 is an arbitrary prime number that i chose for the hashing using polynomial rolling hashing
        hashValue = (hashValue * 17) + (unsigned char)name[i];
    }
    return (int)(hashValue % BUCKET_COUNT);
}

linkedList* searchLinkedList(char* searchName, linkedList* list, int* comparisonCount)
{
    linkedList* current;
    int comparison;
    if (comparisonCount == NULL)
    {
        return NULL;
    }
    *comparisonCount = 0;
    current = list;

    //loop through list until eaching either a matching name or the end of the list.
    while (current != NULL)
    {
        comparison = strcmp(searchName, current->name);
        (*comparisonCount)++;
        if (comparison == 0)
        {
            return current;
        }
        if (comparison < 0)
        {
            return NULL;
        }
        current = current->next;
    }
    return NULL;
}

void searchForNameTwice(char* searchName, linkedList* hashTable[], int comparisonCount[2], linkedList* allNames)
{
    linkedList* listResult;
    linkedList* hashResult;
    int bucket;

    listResult = searchLinkedList(searchName, allNames, &comparisonCount[0]);
    bucket = hashName(searchName);
    hashResult = searchLinkedList(searchName, hashTable[bucket], &comparisonCount[1]);

    if (listResult != NULL)
    {
        printf("\t%s was found in the linked list in %d comparisons\n", searchName, comparisonCount[0]);
    }
    else
    {
        printf("\t%s was NOT found in the linked list in %d comparisons\n", searchName, comparisonCount[0]);
    }

    if (hashResult != NULL)
    {
        printf("\t%s was found in the hash table bucket in %d comparisons\n", searchName, comparisonCount[1]);
    }
    else
    {
        printf("\t%s was NOT found in the hash table bucket in %d comparisons\n", searchName, comparisonCount[1]);
    }
}

void freeList(linkedList* head)
{
    linkedList* current;
    linkedList* nodeToDelete;
    current = head;
    while (current != NULL)
    {
        nodeToDelete = current;
        current = current->next;
        free(nodeToDelete->name);
        free(nodeToDelete);
    }
}

int main(void)
{
    FILE* inputFile;
    char inputName[MAX_NAME_LENGTH + 2];
    linkedList* allNames;
    linkedList* hashTable[BUCKET_COUNT];

    int nameCount;
    int i;
    int bucket;
    int comparisonCount[2];
    long totalListComparisons;
    long totalHashComparisons;
    int totalSearches;
    
    //inializing linked lists as null
    allNames = NULL;
    for (i = 0; i < BUCKET_COUNT; i++)
    {
        hashTable[i] = NULL;
    }

    nameCount = 0;
    totalListComparisons = 0;
    totalHashComparisons = 0;
    totalSearches = 0;

    //openning names.txt file to read and saving names to the linked list and hash table
    if (fopen_s(&inputFile, "names.txt", "r") != 0)
    {
        fprintf(stderr, "Error: could not open names.txt.\n");
        return EXIT_FAILURE;
    }
    //validating the names by name length
    while (fgets(inputName, sizeof(inputName), inputFile) != NULL)
    {
        removeNewline(inputName);
        if (strlen(inputName) == 0)
        {
            continue;
        }
        if (strlen(inputName) > MAX_NAME_LENGTH)
        {
            fprintf(stderr, "Error: a name is longer than %d characters.\n", MAX_NAME_LENGTH);
            fclose(inputFile);
            freeList(allNames);

            for (i = 0; i < BUCKET_COUNT; i++)
            {
                freeList(hashTable[i]);
            }
            return EXIT_FAILURE;
        }
        //validating the names by current amount vs max allowed
        if (nameCount >= MAX_NAMES)
        {
            fprintf(stderr, "Error: names.txt contains more than %d names.\n", MAX_NAMES);
            fclose(inputFile);
            freeList(allNames);
            for (i = 0; i < BUCKET_COUNT; i++)
            {
                freeList(hashTable[i]);
            }
            return EXIT_FAILURE;
        }
        //inserts names into linked list where sorting is done in its function
        if (!insertSorted(&allNames, inputName))
        {
            fprintf(stderr, "Error: could not insert name into linked list.\n");
            fclose(inputFile);
            freeList(allNames);
            for (i = 0; i < BUCKET_COUNT; i++)
            {
                freeList(hashTable[i]);
            }
            return EXIT_FAILURE;
        }
        //calculates the bucket number and inserts names into their proper buckets
        bucket = (hashName(inputName) + 1) % BUCKET_COUNT;
        if (!insertSorted(&hashTable[bucket], inputName))
        {
            fprintf(stderr, "Error: could not insert name into hash table.\n");
            fclose(inputFile);
            freeList(allNames);
            for (i = 0; i < BUCKET_COUNT; i++)
            {
                freeList(hashTable[i]);
            }
            return EXIT_FAILURE;
        }
        nameCount++;
    }
    //varifies the list of names is more than the minimum required
    fclose(inputFile);
    if (nameCount < MIN_NAMES)
    {
        fprintf(stderr, "Error: names.txt must contain at least %d names.\n", MIN_NAMES);
        freeList(allNames);
        for (i = 0; i < BUCKET_COUNT; i++)
        {
            freeList(hashTable[i]);
        }
        return EXIT_FAILURE;
    }
    //reads for a name to search from the console
    while (fgets(inputName, sizeof(inputName), stdin) != NULL)
    {
        removeNewline(inputName);
        if (strlen(inputName) == 0)
        {
            continue;
        }
        //searches for the given name in both the linked list and hash table and records their number of comparisons.
        searchForNameTwice(inputName, hashTable, comparisonCount, allNames);
        totalSearches++;
        totalListComparisons += comparisonCount[0];
        totalHashComparisons += comparisonCount[1];
    }
    //outputs the totle comparisons to find the name
    printf("\n\n\n");
    printf("Total number of searches: %d\n", totalSearches);
    printf("Total comparisons in linked list: %ld\n", totalListComparisons);
    printf("Total comparisons in hash table: %ld\n", totalHashComparisons);
    //frees both the linked list and hash table at the end of the program
    freeList(allNames);
    for (i = 0; i < BUCKET_COUNT; i++)
    {
        freeList(hashTable[i]);
    }
    return EXIT_SUCCESS;
}