#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

static void delete_node(element_t *e)
{
    list_del(&e->list);
    if (e->value)
        free(e->value);
    free(e);
}

/* Create an empty queue */
struct list_head *q_new()
{
    element_t *q = malloc(sizeof(element_t));
    if (q == NULL)
        return NULL;

    q->value = NULL;
    INIT_LIST_HEAD(&q->list);

    return &q->list;
}


/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (l == NULL)
        return;

    element_t *cur, *next;
    list_for_each_entry_safe (cur, next, l, list) {
        delete_node(cur);
    }
    /* free the head */
    cur = list_entry(l, element_t, list);
    free(cur);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (head == NULL || s == NULL)
        return false;

    element_t *node = malloc(sizeof(element_t));
    if (node == NULL)
        return false;

    size_t len = strlen(s);
    node->value = malloc(len + 1);
    if (node->value == NULL) {
        free(node);
        return false;
    }

    strncpy(node->value, s, len);
    node->value[len] = '\0';

    list_add(&node->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (head == NULL || s == NULL)
        return false;

    element_t *node = malloc(sizeof(element_t));
    if (node == NULL)
        return false;

    size_t len = strlen(s);
    node->value = malloc(len + 1);
    if (node->value == NULL) {
        free(node);
        return false;
    }

    strncpy(node->value, s, len);
    node->value[len] = '\0';

    list_add_tail(&node->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || head->next == head)
        return NULL;

    struct list_head *first = head->next;
    list_del(first);

    element_t *entry = list_entry(first, element_t, list);
    size_t slen, dlen;

    if (!entry->value || bufsize == 0)
        return entry;

    slen = strlen(entry->value);
    dlen = slen < (bufsize - 1) ? slen : (bufsize - 1);
    strncpy(sp, entry->value, dlen);
    sp[dlen] = '\0';

    return entry;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || head->next == NULL)
        return NULL;

    struct list_head *last = head->prev;
    list_del(last);

    element_t *entry = list_entry(last, element_t, list);
    size_t slen, dlen;

    if (!entry->value || bufsize == 0)
        return entry;

    slen = strlen(entry->value);
    dlen = slen < (bufsize - 1) ? slen : (bufsize - 1);
    strncpy(sp, entry->value, dlen);
    sp[dlen] = '\0';

    return entry;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    int len = 0;
    const struct list_head *node;
    if (!head)
        return 0;
    list_for_each (node, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (head == NULL)
        return;

    struct list_head *former, *latter;
    list_for_each (former, head) {
        latter = former->next;
        if (latter != head) {
            latter->prev = former->prev;
            former->prev->next = latter;

            former->next = latter->next;
            latter->next->prev = former;

            latter->next = former;
            former->prev = latter;
        }
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head) {}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
