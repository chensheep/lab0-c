#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *lh = malloc(sizeof(struct list_head));

    if (!lh) {
        return NULL;
    }

    INIT_LIST_HEAD(lh);

    return lh;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    element_t *e = NULL, *is = NULL;
    if (!head)
        return;

    list_for_each_entry_safe(e, is, head, list) {
        list_del(&e->list);
        q_release_element(e);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    element_t *e = malloc(sizeof(element_t));
    if (!e) {
        return false;
    }

    int sl = strlen(s);
    char *c = malloc((sl + 1) * sizeof(char));
    if (!c) {
        free(e);
        return false;
    }
    e->value = c;
    strncpy(e->value, s, sl + 1);
    list_add(&e->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    element_t *e = malloc(sizeof(element_t));
    if (!e) {
        return false;
    }

    int sl = strlen(s);
    char *c = malloc((sl + 1) * sizeof(char));
    if (!c) {
        free(e);
        return false;
    }
    e->value = c;
    strncpy(e->value, s, sl + 1);
    list_add_tail(&e->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head)
        return NULL;

    if (list_empty(head))
        return NULL;

    element_t *first;
    first = list_first_entry(head, element_t, list);

    if (!first)
        return NULL;

    size_t vallen = strlen(first->value);
    if (vallen >= bufsize - 1) {
        strncpy(sp, first->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    } else {
        strncpy(sp, first->value, vallen);
        sp[vallen] = '\0';
    }
    list_del(&first->list);

    return first;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head)
        return NULL;

    if (list_empty(head)) {
        return NULL;
    }

    element_t *last;
    last = list_last_entry(head, element_t, list);

    strncpy(sp, last->value, bufsize - 1);
    sp[bufsize - 1] = '\0';
    list_del(&last->list);

    return last;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    if (list_empty(head))
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each(li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list
    if (!head)
        return false;

    if (list_empty(head)) {
        return false;
    }


    element_t *entry_f = list_entry((head)->next, typeof(*entry_f), list);
    element_t *entry_s = list_entry((head)->next, typeof(*entry_s), list);

    while (entry_f->list.next != (head) && entry_f->list.next->next != (head)) {
        entry_f = list_entry(entry_f->list.next->next, typeof(*entry_f), list);
        entry_s = list_entry(entry_s->list.next, typeof(*entry_s), list);
    }

    if (entry_f->list.next != head)
        entry_s = list_entry(entry_s->list.next, typeof(*entry_s), list);

    list_del(&entry_s->list);
    q_release_element(entry_s);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;

    if (list_is_singular(head)) {
        return true;
    }

    struct list_head *l1 = head->next;
    struct list_head *l2 = head->next->next;
    bool isDup = false;

    while (l1 != head && l2 != head) {
        element_t *l1entry = list_entry(l1, typeof(*l1entry), list);
        element_t *l2entry = list_entry(l2, typeof(*l2entry), list);
        if (strcmp(l1entry->value, l2entry->value) == 0) {
            struct list_head *l2next = l2->next;
            list_del(l2);
            q_release_element(l2entry);
            l2 = l2next;
            isDup = true;
        } else {
            if (isDup) {
                l1 = l1->next;
                l2 = l2->next;
                list_del(l1->prev);
                q_release_element(l1entry);
                isDup = false;
            } else {
                l1 = l1->next;
                l2 = l2->next;
            }
        }
    }

    if (isDup) {
        element_t *l1entry = list_entry(l1, typeof(*l1entry), list);
        list_del(l1);
        q_release_element(l1entry);
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_is_singular(head))
        return;

    struct list_head *l1 = head->next;
    struct list_head *l2 = head->next->next;
    for (; l1 != head && l2 != head; l1 = l1->next, l2 = l1->next) {
        l1->next = l2->next;
        l2->prev = l1->prev;

        l1->next->prev = l1;
        l2->prev->next = l2;

        l1->prev = l2;
        l2->next = l1;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    struct list_head *l = head->next;
    struct list_head *r = head->prev;

    while (l != r && l->next != r) {
        struct list_head *lp = l->prev;
        struct list_head *ln = l->next;
        struct list_head *rp = r->prev;
        struct list_head *rn = r->next;
        l->next = r->next;
        l->prev = r->prev;
        lp->next = r;
        ln->prev = r;
        r->next = ln;
        r->prev = lp;
        rp->next = l;
        rn->prev = l;
        l = ln;
        r = rp;
    };

    if (l->next == r) {
        struct list_head *lp = l->prev;
        l->prev->next = r;
        l->prev = r;
        l->next = r->next;
        r->next->prev = l;
        r->prev = lp;
        r->next = l;
    }
}

static void reverse_internal(struct list_head *left, struct list_head *right)
{
    struct list_head *l = left;
    struct list_head *r = right;

    while (l != r && l->next != r) {
        struct list_head *lp = l->prev;
        struct list_head *ln = l->next;
        struct list_head *rp = r->prev;
        struct list_head *rn = r->next;
        l->next = r->next;
        l->prev = r->prev;
        lp->next = r;
        ln->prev = r;
        r->next = ln;
        r->prev = lp;
        rp->next = l;
        rn->prev = l;
        l = ln;
        r = rp;
    };

    if (l->next == r) {
        struct list_head *lp = l->prev;
        l->prev->next = r;
        l->prev = r;
        l->next = r->next;
        r->next->prev = l;
        r->prev = lp;
        r->next = l;
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head)
        return;

    struct list_head *cur = head->next;
    struct list_head *left = NULL;
    struct list_head *safe = NULL;
    int count = 0;
    for (; cur != head; cur = safe) {
        safe = cur->next;
        if (count == 0)
            left = cur;
        count++;
        if (count == k) {
            reverse_internal(left, cur);
            count = 0;
            left = NULL;
        }
    }
}

struct list_head *mergesort_list(struct list_head *head, bool descend)
{
    if (!head || list_is_singular(head))
        return head;

    size_t size = q_size(head);

    LIST_HEAD(r);
    INIT_LIST_HEAD(&r);
    LIST_HEAD(l);
    INIT_LIST_HEAD(&l);

    int mid = size / 2 - 1;
    int i = 0;
    struct list_head *node;
    list_for_each(node, head) {
        if (mid == i)
            break;
        i++;
    }

    list_cut_position(&l, head, node);
    list_splice_init(head, &r);

    struct list_head *sort_l = mergesort_list(&l, descend);
    struct list_head *sort_r = mergesort_list(&r, descend);

    while (list_empty(sort_l) == 0 && list_empty(sort_r) == 0) {
        element_t *l_entry = list_first_entry(sort_l, element_t, list);
        element_t *r_entry = list_first_entry(sort_r, element_t, list);
        if (!descend) {
            if (strcmp(l_entry->value, r_entry->value) <= 0) {
                list_move_tail(&l_entry->list, head);
            } else {
                list_move_tail(&r_entry->list, head);
            }
        } else {
            if (strcmp(l_entry->value, r_entry->value) >= 0) {
                list_move_tail(&l_entry->list, head);
            } else {
                list_move_tail(&r_entry->list, head);
            }
        }
    }

    if (list_empty(sort_l) == 0) {
        list_splice_tail(sort_l, head);
    }
    if (list_empty(sort_r) == 0) {
        list_splice_tail(sort_r, head);
    }

    return head;
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return;

    mergesort_list(head, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_is_singular(head))
        return 0;

    int count = 0;
    struct list_head *cur = head->prev->prev;
    struct list_head *safe;
    const element_t *last = list_entry(head->prev, element_t, list);
    char *largval = strdup(last->value);
    for (; cur != head; cur = safe) {
        safe = cur->prev;
        element_t *e = list_entry(cur, element_t, list);
        if (strcmp(largval, e->value) <= 0) {
            list_del(cur);
            q_release_element(e);
            count++;
        } else {
            free(largval);
            largval = strdup(e->value);
        }
    }


    if (largval != NULL)
        free(largval);

    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_is_singular(head))
        return 0;

    int count = 0;
    struct list_head *cur = head->prev->prev;
    struct list_head *safe;
    const element_t *last = list_entry(head->prev, element_t, list);
    char *largval = strdup(last->value);
    for (; cur != head; cur = safe) {
        safe = cur->prev;
        element_t *e = list_entry(cur, element_t, list);
        if (strcmp(largval, e->value) >= 0) {
            list_del(cur);
            q_release_element(e);
            count++;
        } else {
            free(largval);
            largval = strdup(e->value);
        }
    }


    if (largval != NULL)
        free(largval);

    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;

    if (list_is_singular(head)) {
        queue_contex_t *first;
        first = list_first_entry(head, queue_contex_t, chain);
        return q_size(first->q);
    }

    queue_contex_t *first;
    first = list_first_entry(head, queue_contex_t, chain);

    queue_contex_t *e = NULL, *is = NULL;

    list_for_each_entry_safe(e, is, head, chain) {
        if (e != first) {
            list_splice_tail(e->q, first->q);
            INIT_LIST_HEAD(e->q);
        }
    }

    q_sort(first->q, descend);

    return q_size(first->q);
}
