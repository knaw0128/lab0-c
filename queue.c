#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

int min(int a, int b)
{
    return (a < b) ? a : b;
}

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *newHead =
        (struct list_head *) malloc(sizeof(struct list_head));
    if (!newHead) {
        return NULL;
    }
    newHead->prev = newHead;
    newHead->next = newHead;
    return newHead;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head) {
        return;
    }
    struct list_head *cursor;
    struct list_head *tmp;
    list_for_each_safe (cursor, tmp, head) {
        free(list_entry(cursor, element_t, list)->value);
        free(list_entry(cursor, element_t, list));
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    element_t *newNode = malloc(sizeof(element_t));
    if (!newNode) {
        return false;
    }
    newNode->value = (char *) malloc(sizeof(char) * strlen(s) + 1);
    if (!(newNode->value)) {
        free(newNode);
        return false;
    }
    memcpy(newNode->value, s, sizeof(char) * strlen(s) + 1);
    INIT_LIST_HEAD(&newNode->list);
    list_add(&newNode->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    element_t *newNode = malloc(sizeof(element_t));
    if (!newNode) {
        return false;
    }
    newNode->value = (char *) malloc(sizeof(char) * strlen(s) + 1);
    if (!(newNode->value)) {
        free(newNode);
        return false;
    }
    memcpy(newNode->value, s, sizeof(char) * strlen(s) + 1);
    INIT_LIST_HEAD(&newNode->list);
    list_add_tail(&newNode->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->next == head) {
        return NULL;
    }
    element_t *now = list_entry(head->next, element_t, list);
    if (sp) {
        memcpy(sp, now->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    struct list_head *newFirst = head->next->next;
    newFirst->prev = head;
    head->next = newFirst;
    return now;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->prev == head) {
        return NULL;
    }
    element_t *now = list_entry(head->prev, element_t, list);
    if (sp) {
        memcpy(sp, now->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    struct list_head *newLast = head->prev->prev;
    newLast->next = head;
    head->prev = newLast;
    return now;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || head->next == head) {
        return false;
    }
    int step = 0;
    struct list_head *fast = head->next;
    struct list_head *slow = head->next;

    while (fast != head) {
        if (step) {
            slow = slow->next;
        }
        fast = fast->next;
        step = 1 - step;
    }
    slow->prev->next = slow->next;
    slow->next->prev = slow->prev;
    free(list_entry(slow, element_t, list)->value);
    free(list_entry(slow, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head) {
        return false;
    }
    struct list_head *pre = NULL;
    struct list_head *safe;
    struct list_head *cursor;
    list_for_each_safe (cursor, safe, head) {
        bool duplicated = false;
        while (pre && cursor != head &&
               !strcmp(list_entry(pre, element_t, list)->value,
                       list_entry(cursor, element_t, list)->value)) {
            list_del(cursor);
            free(list_entry(cursor, element_t, list)->value);
            free(list_entry(cursor, element_t, list));
            cursor = safe;
            safe = safe->next;
            duplicated = true;
        }
        if (duplicated) {
            list_del(pre);
            free(list_entry(pre, element_t, list)->value);
            free(list_entry(pre, element_t, list));
        }
        if (cursor == head) {
            break;
        }
        pre = cursor;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || q_size(head) < 2) {
        return;
    }
    q_reverseK(head, 2);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    struct list_head *safeNext;
    struct list_head *now;
    list_for_each_safe (now, safeNext, head) {
        struct list_head *tmp = now->prev;
        now->prev = now->next;
        now->next = tmp;
    }
    struct list_head *tmpHead = head->prev;
    head->prev = head->next;
    head->next = tmpHead;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    int listSize = q_size(head);
    if (!head || listSize < 2 || k < 1) {
        return;
    } else if (k >= listSize) {
        q_reverse(head);
    }
    int idx = 0;
    struct list_head *cursor;
    struct list_head *front = head;
    struct list_head *back = head;
    while (idx < listSize) {
        cursor = front->next;
        for (int i = 0; i < min(k, listSize - idx); i++) {
            if (i == min(k, listSize - idx) - 1) {
                back = cursor->next;
            }
            struct list_head *tmp = cursor->prev;
            cursor->prev = cursor->next;
            cursor->next = tmp;
            cursor = cursor->prev;
        }
        front->next->next = back;
        back->prev->prev = front;
        struct list_head *tmpFront = front->next;
        front->next = back->prev;
        back->prev = tmpFront;
        front = back->prev;
        idx += k;
    }
}

struct list_head *MergeSort_merge(struct list_head *fir,
                                  struct list_head *sec,
                                  int firLen,
                                  int secLen)
{
    int firIdx = 0;
    int secIdx = 0;
    struct list_head *sortedHead = NULL;
    struct list_head *cursor;
    while (firIdx < firLen || secIdx < secLen) {
        struct list_head *now;
        if (firIdx == firLen) {
            now = sec;
            sec = sec->next;
            secIdx++;
        } else if (secIdx == secLen) {
            now = fir;
            fir = fir->next;
            firIdx++;
        } else {
            char *firStr = list_entry(fir, element_t, list)->value;
            char *secStr = list_entry(sec, element_t, list)->value;
            if (strcmp(firStr, secStr) < 0) {
                now = fir;
                fir = fir->next;
                firIdx++;
            } else {
                now = sec;
                sec = sec->next;
                secIdx++;
            }
        }

        if (!sortedHead) {
            INIT_LIST_HEAD(now);
            sortedHead = now;
            cursor = now;
        } else {
            cursor->next = now;
            now->prev = cursor;
            now->next = sortedHead;
            sortedHead->prev = now;
            cursor = now;
        }
    }
    return sortedHead;
}

struct list_head *MergeSort_sort(struct list_head *head, int size)
{
    if (size == 1) {
        INIT_LIST_HEAD(head);
        return head;
    }
    struct list_head *walk = head;
    int step = 1;
    while (step < size / 2) {
        walk = walk->next;
        step++;
    }
    walk = walk->next;
    struct list_head *front = MergeSort_sort(head, step);
    struct list_head *back = MergeSort_sort(walk, size - step);
    return MergeSort_merge(front, back, step, size - step);
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head) {
        return;
    }
    struct list_head *newFirst = MergeSort_sort(head->next, q_size(head));
    head->next = newFirst;
    head->prev = newFirst->prev;
    newFirst->prev->next = head;
    newFirst->prev = head;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head) {
        return 0;
    }
    struct list_head *safe;
    struct list_head *cursor;
    for (cursor = head->next, safe = cursor->next;
         safe != head && cursor != head; cursor = safe, safe = safe->next) {
        if (strcmp(list_entry(cursor, element_t, list)->value,
                   list_entry(safe, element_t, list)->value) < 0) {
            safe = cursor->prev;
            list_del(cursor);
            free(list_entry(cursor, element_t, list)->value);
            free(list_entry(cursor, element_t, list));
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return 0;
    } else if (list_is_singular(head)) {
        return list_entry(head->next, queue_contex_t, chain)->size;
    }

    struct list_head *newQueueHead =
        list_entry(head->next, queue_contex_t, chain)->q;
    struct list_head *contextNode;
    int ans = 0;
    list_for_each (contextNode, head) {
        queue_contex_t *now = list_entry(contextNode, queue_contex_t, chain);
        ans += now->size;
        if (!(now->q) || list_empty(now->q)) {
            now->q = NULL;
            continue;
        }
        now->q->prev->next = NULL;
        now->q = now->q->next;
    }
    INIT_LIST_HEAD(newQueueHead);

    while (1) {
        struct list_head *nextContext = NULL;
        struct list_head *cursor;
        list_for_each (cursor, head) {
            queue_contex_t *nowQueue =
                list_entry(cursor, queue_contex_t, chain);
            if (nowQueue->size == 0) {
                continue;
            } else if (!nextContext) {
                nextContext = cursor;
                continue;
            }
            queue_contex_t *preQueue =
                list_entry(nextContext, queue_contex_t, chain);
            element_t *nowNode = list_entry(nowQueue->q, element_t, list);
            element_t *preNode = list_entry(preQueue->q, element_t, list);
            if (strcmp(nowNode->value, preNode->value) < 0) {
                nextContext = cursor;
            }
        }
        if (!nextContext) {
            break;
        }
        queue_contex_t *nextQueue =
            list_entry(nextContext, queue_contex_t, chain);
        struct list_head *newQueueNode = nextQueue->q;
        nextQueue->size--;
        nextQueue->q = nextQueue->q->next;
        list_add_tail(newQueueNode, newQueueHead);
    }
    list_entry(head->next, queue_contex_t, chain)->q = newQueueHead;
    list_entry(head->next, queue_contex_t, chain)->size = ans;
    return ans;
}
