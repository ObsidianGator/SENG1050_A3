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
        printf("\t%s was found in the linked list in %d comparisons\n",
            searchName,
            comparisonCount[0]);
    }
    else
    {
        printf("\t%s was NOT found in the linked list in %d comparisons\n",
            searchName,
            comparisonCount[0]);
    }

    if (hashResult != NULL)
    {
        printf("\t%s was found in the hash table bucket in %d comparisons\n",
            searchName,
            comparisonCount[1]);
    }
    else
    {
        printf("\t%s was NOT found in the hash table bucket in %d comparisons\n",
            searchName,
            comparisonCount[1]);
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

int main(void) {

}