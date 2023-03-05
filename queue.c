#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// clang-format off
#include "queue.h"
#include "custom.h"
#include "random.h"
// clang-format on

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

static void delete_node(element_t *e)
{
    list_del(&e->list);
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

    node->value = strdup(s);
    if (node->value == NULL) {
        free(node);
        return false;
    }

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

    node->value = strdup(s);
    if (node->value == NULL) {
        free(node);
        return false;
    }

    list_add_tail(&node->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || list_empty(head))
        return NULL;

    struct list_head *first = head->next;
    list_del(first);

    element_t *entry = list_entry(first, element_t, list);
    size_t slen, dlen;

    if (!entry->value || !sp || bufsize == 0)
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

    if (!entry->value || !sp || bufsize == 0)
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
    if (head == NULL || list_empty(head))
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
    if (head == NULL || list_empty(head))
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

    if (k <= 1 || head == NULL || list_empty(head))
        return;

    LIST_HEAD(last_head);
    LIST_HEAD(rcur_head);
    struct list_head *last = &last_head, *rcur = &rcur_head;
    struct list_head *cur, *next, *first = NULL;

    int cnt = 0;

    size_t size = q_size(head);
    size_t maximum = (size / k) * k;

    cur = head->next;
    next = cur->next;
    while (maximum-- > 0) {
        /* Insert to head */
        cur->next = rcur;
        cur->prev = rcur->prev;
        rcur->prev = cur;
        rcur = rcur->prev;

        if (++cnt == k) {
            struct list_head *last_new = rcur->prev->prev;
            rcur->prev = last;
            last->next = rcur;
            last = last_new;
            if (first == NULL)
                first = rcur;

            /* Reset rcur  */
            rcur = &rcur_head;
            INIT_LIST_HEAD(rcur);

            /* Reset cnt */
            cnt = 0;
        }
        cur = next;
        next = next->next;
    }

    /* Directly connect to the rest elements*/
    if (size != maximum) {
        last->next = cur;
        cur->prev = last;
        last = head->prev;
    }

    /* Occur when q_size < k */
    if (first == NULL)
        first = head->next;

    /* Connect to head  */
    head->prev = last;
    last->next = head;
    head->next = first;
    first->prev = head;
}

static struct list_head *merge_sort(struct list_head *head, size_t len)
{
    if (len <= 1) {
        return head;
    }

    struct list_head *mid = find_mid(head, len);
    size_t l = len / 2, r = len - l;

    struct list_head *l_cur = merge_sort(head, l);
    struct list_head *r_cur = merge_sort(mid, r);

    LIST_HEAD(new);
    struct list_head *tmp = &new;

    while (l > 0 && r > 0) {
        element_t *l_ele, *r_ele;
        l_ele = list_entry(l_cur, element_t, list);
        r_ele = list_entry(r_cur, element_t, list);

        if (strcmp(l_ele->value, r_ele->value) <= 0) {
            tmp->next = l_cur;
            l_cur = l_cur->next;
            l--;
        } else {
            tmp->next = r_cur;
            r_cur = r_cur->next;
            r--;
        }
        tmp = tmp->next;
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
    if (head == NULL || list_empty(head))
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

static struct list_head *merge(struct list_head *a, struct list_head *b)
{
    // cppcheck-suppress unassignedVariable
    struct list_head *head, **tail = &head;
    while (1) {
        if (strcmp(list_entry(a, element_t, list)->value,
                   list_entry(b, element_t, list)->value) <= 0) {
            *tail = a;
            tail = &a->next;
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        } else {
            *tail = b;
            tail = &b->next;
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        }
    }
    return head;
}

static void merge_final(struct list_head *head,
                        struct list_head *a,
                        struct list_head *b)
{
    struct list_head *tail = head;

    while (1) {
        if (strcmp(list_entry(a, element_t, list)->value,
                   list_entry(b, element_t, list)->value) <= 0) {
            tail->next = a;
            a->prev = tail;
            tail = a;
            a = a->next;
            if (!a) {
                break;
            }
        } else {
            tail->next = b;
            b->prev = tail;
            tail = b;
            b = b->next;
            if (!b) {
                b = a;
                break;
            }
        }
    }
    tail->next = b;
    do {
        b->prev = tail;
        tail = b;
        b = b->next;
    } while (b);
    tail->next = head;
    head->prev = tail;
}


#ifdef LIST_SORT_DETAILS
/* Only support single linked list  */
static void observe(struct list_head *head)
{
    printf("[ ");
    struct list_head *n = head;

    while (1) {
        if (n == NULL)
            break;

        element_t *e = list_entry(n, element_t, list);
        printf("%s ", e->value);
        n = n->next;
    }
    printf("]");
}
#endif

void q_list_sort(struct list_head *head)
{
    /* Now we can experiment with the cloned queue */
    struct list_head *list = head->next, *pending = NULL;
    size_t count = 0;

    if (list == head->prev)
        return;

    /* Convert to a null-terminated single linked-list */
    head->prev->next = NULL;

    do {
        size_t bits;
        struct list_head **tail = &pending;
        for (bits = count; bits & 1; bits >>= 1)
            tail = &(*tail)->prev;
        if (bits) {
            struct list_head *a = *tail, *b = a->prev;
            a = merge(b, a);
            /* Restore the old prev link */
            a->prev = b->prev;
            *tail = a;
        }

        list->prev = pending;
        pending = list;
        list = list->next;
        pending->next = NULL;
        count++;

#ifdef LIST_SORT_DETAILS
        printf("=== Count: %zu ===\n", count);
        printf("List: ");
        observe(list);
        printf("\n");

        struct list_head *o = pending;
        printf("Pending");
        while (o) {
            printf(" -> ");
            observe(o);
            o = o->prev;
        }
        printf("\n");
#endif

    } while (list);

    list = pending;
    pending = pending->prev;

    for (;;) {
        struct list_head *next = pending->prev;
        if (!next)
            break;

        list = merge(pending, list);
        pending = next;
    }
    merge_final(head, pending, list);
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

        if (!max || strcmp(e->value, max) >= 0) {
            max = e->value;
            len++;
        } else {
            delete_node(e);
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
    LIST_HEAD(mq);
    struct list_head *mqc = &mq;
    struct list_head *q_entry, *entry, *safe;

    /* Merge elements in each queue into one queue */
    list_for_each (q_entry, head) {
        queue_contex_t *q_ctx = list_entry(q_entry, queue_contex_t, chain);
        struct list_head *q_head = q_ctx->q;
        list_for_each_safe (entry, safe, q_head) {
            mqc->next = entry;
            mqc = mqc->next;
            list_del(entry);
        }
    }

    /* Replace the head pointer */
    struct list_head *first_queue =
        list_entry(head->next, queue_contex_t, chain)->q;
    first_queue->next = mq.next;
    first_queue->prev = mqc;
    mq.next->prev = first_queue;
    mqc->next = first_queue;


    q_sort(first_queue);
    return q_size(first_queue);
}

void q_shuffle(struct list_head *head)
{
    size_t rand;
    size_t len = q_size(head);
    size_t idx = 0;
    struct list_head *ent = head->next;
    LIST_HEAD(tmp);


    while (idx < len) {
        size_t tidx = 0;
        struct list_head *target = ent;

        randombytes((uint8_t *) &rand, sizeof(rand));
        rand = rand % (len - idx);

        while (tidx++ != rand) {
            target = target->next;
        }

        /* Swap two elements */
        if (ent->next == target) {
            /* Avoid node->next = node */
            target->prev = ent->prev;
            ent->prev->next = target;

            ent->next = target->next;
            target->next->prev = ent;

            target->next = ent;
            ent->prev = target;
        } else {
            tmp.next = ent->next;
            tmp.prev = ent->prev;

            ent->next = target->next;
            ent->prev = target->prev;

            target->next = tmp.next;
            target->prev = tmp.prev;

            ent->next->prev = ent;
            ent->prev->next = ent;

            target->next->prev = target;
            target->prev->next = target;
        }
        ent = target->next;
        idx++;
    }
}
