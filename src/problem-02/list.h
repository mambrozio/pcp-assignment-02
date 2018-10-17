#if !defined(list_h)
#define list_h

#include <stdbool.h>

#include "allocation.h"

typedef const char* (*StringifyFunction)(void*);

typedef struct List List;
typedef struct ListNode ListNode;
typedef void* ListValue;

extern List* list_new(void);
extern void list_free(List*);

extern size_t list_size(List*);
extern ListNode* list_first(List*);
extern ListNode* list_last(List*);

extern void list_append(List*, ListValue);
extern ListValue list_pop_first(List*);
extern bool list_contains(List*, ListValue);

extern ListValue list_value(ListNode*);
extern ListNode* list_next(ListNode*);
extern ListNode* list_previous(ListNode*);

extern void list_dump(List*, StringifyFunction);

#endif
