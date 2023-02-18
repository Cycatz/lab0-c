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
    if (head == NULL || head->next == head)
        return false;

    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    size_t pos;
    pos = q_size(head) / 2 + 1;

    element_t *entry = NULL;
    list_for_each_entry (entry, head, list) {
        if (--pos == 0) {
            delete_node(entry);
            break;
        }
    }
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

static struct list_head *merge_sort(struct list_head *head, size_t len)
{
    if (len == 0 || len == 1) {
        struct list_head *new = q_new();
        q_insert_tail(new, list_entry(head, element_t, list)->value);
        return new;
    }

    struct list_head *mid = find_mid(head, len);
    size_t l, r;

    l = len / 2;
    r = len - l;

    struct list_head *l_head = merge_sort(head, l);
    struct list_head *r_head = merge_sort(mid, r);

    /* merge */
    struct list_head *lcur = l_head->next, *rcur = r_head->next;
    element_t *l_ele, *r_ele;

    struct list_head *new = q_new();

    while (l > 0 && r > 0) {
        l_ele = list_entry(lcur, element_t, list);
        r_ele = list_entry(rcur, element_t, list);

        if (strcmp(l_ele->value, r_ele->value) <= 0) {
            q_insert_tail(new, l_ele->value);
            l--;
        } else {
            q_insert_tail(new, r_ele->value);
            r--;
        }
    }

    while (l && r-- > 0) {
        r_ele = list_entry(rcur, element_t, list);
        q_insert_tail(new, r_ele->value);
        rcur = rcur->next;
    }
    while (r && l-- > 0) {
        l_ele = list_entry(lcur, element_t, list);
        q_insert_tail(new, l_ele->value);
        lcur = lcur->next;
    }

    q_free(l_head);
    q_free(r_head);

    return new;
}


/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (head == NULL || head->next == head)
        return;

    size_t len = q_size(head);
    struct list_head *new = merge_sort(head, len);

    head->next = new->next;
    head->prev = new->prev;
    new->next->prev = head;
    new->prev->next = head;

    q_free(new);
}

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
