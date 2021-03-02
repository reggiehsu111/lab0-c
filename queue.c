#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (q) {
        q->head = NULL;
        q->tail = NULL;
        q->size = 0;
    }
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (q) {
        list_ele_t *tmp = q->head;
        while (tmp) {
            q->head = q->head->next;
            // After update the queue header, free the list element
            free(tmp->value);
            free(tmp);
            tmp = q->head;
        }
        /* Free queue structure */
        free(q);
    }
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (q) {
        list_ele_t *newh;
        char *news;
        size_t slength = strlen(s) + 1;
        /* Don't forget to allocate space for the string and copy it */
        /* What if either call to malloc returns NULL? */
        newh = malloc(sizeof(list_ele_t));
        news = malloc(slength);
        if (!newh || !news) {
            free(newh);
            free(news);
            return false;
        }
        newh->next = q->head;
        strncpy(news, s, slength);
        q->head = newh;
        q->head->value = news;
        q->size++;
        if (q->size == 1) {
            q->tail = q->head;
            q->tail->next = NULL;
        }
        return true;
    } else {
        return false;
    }
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (q) {
        list_ele_t *newt;
        size_t slength = strlen(s) + 1;
        char *news;
        newt = malloc(sizeof(list_ele_t));
        news = malloc(slength);
        if (!newt || !news) {
            free(newt);
            free(news);
            return false;
        }
        strncpy(news, s, slength);
        newt->value = news;
        if (q->tail)
            q->tail->next = newt;
        else
            q->head = newt;
        q->tail = newt;
        q->size++;
        q->tail->next = NULL;
        return true;
    } else {
        return false;
    }
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (q) {
        if (q->head) {
            if (sp) {
                size_t val_len = strlen(q->head->value);
                size_t realcpy =
                    (bufsize - 1) > val_len ? val_len : (bufsize - 1);
                // snprintf( sp, bufsize, "%s", q->head->value);
                memcpy(sp, q->head->value, realcpy);
                *(sp + realcpy) = '\0';
            }
            list_ele_t *tmp_head = q->head;
            q->head = q->head->next;
            free(tmp_head->value);
            free(tmp_head);
            q->size--;
            if (q->size == 0)
                q->tail = NULL;
            return true;
        }
    }
    return false;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    if (q) {
        return q->size;
    } else {
        return 0;
    }
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (q) {
        if (q->head) {
            q->tail = q->head;
            list_ele_t *prev = NULL, *curr = q->head, *next = q->head->next;
            while (next) {
                curr->next = prev;
                prev = curr;
                curr = next;
                next = curr->next;
            }
            q->head = curr;
            curr->next = prev;
        }
    }
}

list_ele_t *merge(list_ele_t *left, list_ele_t *right)
{
    if (!left)
        return right;
    if (!right)
        return left;
    /*
    if (strcmp(left->value, right->value) < 0) {
        left->next = merge(left->next, right);
        return left;
    } else {
        right->next = merge(right->next, left);
        return right;
    }
    */
    list_ele_t *head = strcmp(left->value, right->value) < 0 ? left : right;
    list_ele_t *tmp;
    while (left && right) {
        if (strcmp(left->value, right->value) < 0) {
            tmp = left->next;
            if (tmp)
                left->next = strcmp(left->next->value, right->value) < 0
                                 ? left->next
                                 : right;
            else
                left->next = right;
            left = tmp;
        } else {
            tmp = right->next;
            if (tmp)
                right->next = strcmp(right->next->value, left->value) <= 0
                                  ? right->next
                                  : left;
            else
                right->next = left;
            right = tmp;
        }
    }
    return head;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
list_ele_t *mergeSort(list_ele_t *head)
{
    if (!head || !head->next)
        return head;
    // split
    list_ele_t *singlePtr = head, *doublePtr = head->next;
    while (doublePtr && doublePtr->next) {
        singlePtr = singlePtr->next;
        doublePtr = doublePtr->next->next;
    }
    // printf("head: %s, single: %s, double:%s\n", head->value,
    // singlePtr->value, doublePtr->value);
    doublePtr = singlePtr->next;
    singlePtr->next = NULL;

    // Recursive mergeSort
    head = mergeSort(head);
    doublePtr = mergeSort(doublePtr);

    // Merge and return head
    return merge(head, doublePtr);
}

void q_sort(queue_t *q)
{
    if (q) {
        if (q->head && q->size > 1) {
            // sort
            q->head = mergeSort(q->head);
            while (q->tail->next) {
                q->tail = q->tail->next;
            }
        }
    }
}
