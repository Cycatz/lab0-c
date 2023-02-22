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


static struct list_head *find_mid(struct list_head *head, size_t len)
{
    size_t mid = len / 2;
    struct list_head *entry = head;
    while (1) {
        if (mid-- == 0)
            break;
        entry = entry->next;
    }
    return entry;
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
    if (head == NULL || head->next == head)
        return false;

    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    size_t size = q_size(head);
    struct list_head *mid = find_mid(head->next, size);
    element_t *entry = list_entry(mid, element_t, list);

    delete_node(entry);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (head == NULL || head->next == head)
        return false;

    bool dup = false;
    element_t *entry, *safe, *ori = NULL;
    list_for_each_entry_safe (entry, safe, head, list) {
        if (ori && ori->value && entry->value &&
            strcmp(ori->value, entry->value) == 0) {
            delete_node(entry);
            dup = true;
        } else {
            if (dup)
                delete_node(ori);
            ori = entry;
            dup = false;
        }
    }
    if (dup)
        delete_node(ori);
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
void q_reverse(struct list_head *head)
{
    if (head == NULL)
        return;

    struct list_head *entry, *former = head, *safe;
    list_for_each (entry, head) {
        entry->prev = entry->next;
    }

    entry = head->next;
    safe = entry->next;
    while (1) {
        /* Suppress unreadVariable */
        (void) safe;
        if (entry == head)
            break;
        safe = entry->next;
        entry->next = former;
        former = entry;
        entry = safe;
    }

    /* Swap head's next and prev */
    struct list_head *tmp;
    tmp = head->next;
    head->next = head->prev;
    head->prev = tmp;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}


static struct list_head *merge_sort(struct list_head *head, size_t len)
{
    if (len == 0 || len == 1) {
        return head;
    }

    struct list_head *mid = find_mid(head, len);
    size_t l, r;

    l = len / 2;
    r = len - l;

    struct list_head *l_head = merge_sort(head, l);
    struct list_head *r_head = merge_sort(mid, r);

    struct list_head *l_cur = l_head;
    struct list_head *r_cur = r_head;


    LIST_HEAD(new);
    struct list_head *tmp = &new;

    while (l > 0 && r > 0) {
        element_t *l_ele, *r_ele;
        l_ele = list_entry(l_cur, element_t, list);
        r_ele = list_entry(r_cur, element_t, list);

        if (strcmp(l_ele->value, r_ele->value) <= 0) {
            tmp->next = l_cur;
            tmp = tmp->next;
            l_cur = l_cur->next;
            l--;
        } else {
            tmp->next = r_cur;
            tmp = tmp->next;
            r_cur = r_cur->next;
            r--;
        }
    }
    if (l)
        tmp->next = l_cur;

    if (r)
        tmp->next = r_cur;

    return new.next;
}


/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (head == NULL || head->next == head)
        return;

    size_t len = q_size(head);
    struct list_head *new = merge_sort(head->next, len);
    struct list_head *cur = new, *prev = head;

    while (len-- > 0) {
        cur->prev = prev;
        prev = cur;
        cur = cur->next;
    }

    head->next = new;
    head->prev = prev;
    prev->next = head;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    struct list_head *entry = head->prev, *pprev;
    char *max = NULL;
    size_t len = 0;

    pprev = entry->prev;

    while (1) {
        element_t *e;
        if (entry == head)
            break;
        e = list_entry(entry, element_t, list);

        /* keep */
        if (!max || strcmp(e->value, max) >= 0) {
            max = e->value;
            len++;
        } else { /* remove */
            list_del(entry);
        }
        entry = pprev;
        pprev = pprev->prev;
    }
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return len;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
