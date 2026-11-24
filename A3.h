#define BUCKET_COUNT 127
#define MAX_NAME_LENGTH 30
#define MIN_NAMES 2000
#define MAX_NAMES 10000

typedef struct linkedList
{
    char* name;
    struct linkedList* next;
} linkedList;

void removeNewline(char* text);
char* duplicateName(const char* name);
int insertSorted(linkedList** head, const char* name);
int hashName(const char* name);
linkedList* searchLinkedList(char* searchName, linkedList* list, int* comparisonCount);
void searchForNameTwice(char* searchName, linkedList* hashTable[], int comparisonCount[2], linkedList* allNames);
void freeList(linkedList* head);