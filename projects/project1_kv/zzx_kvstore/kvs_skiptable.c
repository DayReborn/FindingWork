


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_LEVEL 6

typedef struct Node {
    int key;
    int value;
    struct Node** forward;
} Node;

typedef struct SkipList {
    int level;
    Node* header;
} SkipList;

Node* createNode(int level, int key, int value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->key = key;
    newNode->value = value;
    newNode->forward = (Node**)malloc((level + 1) * sizeof(Node*));
    
    return newNode;
}

SkipList* createSkipList() {
    SkipList* skipList = (SkipList*)malloc(sizeof(SkipList));
    skipList->level = 0;
    
    skipList->header = createNode(MAX_LEVEL, -1, -1);
    
    for (int i = 0; i <= MAX_LEVEL; ++i) {
        skipList->header->forward[i] = NULL;
    }
    
   return skipList; 
}

int randomLevel() {
   int level = 0;
   while (rand() < RAND_MAX / 2 && level < MAX_LEVEL)
      level++;
   return level;
}

bool insert(SkipList* skipList, int key, int value) {
   Node* update[MAX_LEVEL + 1];
   Node* current = skipList->header;

   for (int i = skipList->level; i >= 0; --i) {
      while (current->forward[i] != NULL && current->forward[i]->key < key)
         current = current->forward[i];
      update[i] = current;
   }

   current = current->forward[0];

   if (current == NULL || current->key != key) {
      int level = randomLevel();

      if (level > skipList->level) {
         for (int i = skipList->level + 1; i <= level; ++i)
            update[i] = skipList->header;
         skipList->level = level;
      }

      Node* newNode = createNode(level, key, value);

      for (int i = 0; i <= level; ++i) {
         newNode->forward[i] = update[i]->forward[i];
         update[i]->forward[i] = newNode;
      }
      
      printf("Inserted key %d\n", key);
      
      return true;
   } else {
       printf("Key %d already exists\n", key);
       return false;
   }
}

void display(SkipList* skipList) {
    printf("Skip List:\n");
    
    for (int i = 0; i <= skipList->level; ++i) {
        Node* node = skipList->header->forward[i];
        printf("Level %d: ", i);
        
        while (node != NULL) {
            printf("%d ", node->key);
            node = node->forward[i];
        }
        
        printf("\n");
    }
}

bool search(SkipList* skipList, int key) {
    Node* current = skipList->header;

    for (int i = skipList->level; i >= 0; --i) {
        while (current->forward[i] != NULL && current->forward[i]->key < key)
            current = current->forward[i];
    }

    current = current -> forward[0];

    if(current && current -> key == key){
        printf("Key %d found with value %d\n", key, current->value);
        return true;
    }else{
        printf("Key %d not found\n", key);
        return false;
    }
}

int main() {
    SkipList* skipList = createSkipList();
    
    insert(skipList, 3, 30);
    insert(skipList, 6, 60);
    insert(skipList, 2, 20);
    insert(skipList, 4, 40);

    display(skipList);

    search(skipList, 3);
    search(skipList, 7);

   return 0;
}




