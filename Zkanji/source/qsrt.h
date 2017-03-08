//---------------------------------------------------------------------------

#ifndef qsrtH
#define qsrtH
//---------------------------------------------------------------------------

// Search and sort algorithms, not just quicksort.

template<typename T>
int logsearch(T *buff, int min, int max, int searched, int(*getsortval)(T *a))
{
    int cnt = max + 1;
    int val = searched;
    while (min <= max)
    {
        int mid = (min + max) / 2;
        val = getsortval(buff + mid);
        if (searched < val)
            max = mid - 1;
        else if (searched > val)
            min = mid + 1;
        else
        {
            min = mid;
            break;
        }
    }

    if (min >= 0 && min < cnt && searched == val)
        return min;
    return -min - 1;
}

template<typename T, typename K>
int logsearch(T *buff, int min, int max, K searched, int(*getsortval)(K a, T *b))
{
    int cnt = max + 1;
    int d = -1;
    while (min <= max)
    {
        int mid = (min + max) / 2;
        d = getsortval(searched, buff + mid);
        if (d < 0)
            max = mid - 1;
        else if (d > 0)
            min = mid + 1;
        else
        {
            min = mid;
            break;
        }
    }

    if (min >= 0 && min < cnt && d == 0)
        return min;
    return -min - 1;
}

template<typename T>
void quicksort(T* buff, int left, int right, int(*getsortval)(T *a))
{
    if (right <= left)
        return;

    int i, j;
    int p;
    T t;

    do
    {
        i = left;
        j = right;
        p = getsortval(buff + ((left + right) / 2));
        do
        {
            while (getsortval(buff + i) < p)
                ++i;
            while (getsortval(buff + j) > p)
                --j;
            if (i <= j)
            {
                if (i != j)
                {
                    t = buff[i];
                    buff[i] = buff[j];
                    buff[j] = t;
                }
                ++i;
                --j;
            }
        }
        while (i <= j);
        if (left < j)
            quicksort<T>(buff, left, j, getsortval);
        left = i;
    }
    while (i < right);

}

template<typename T>
void quicksort(T* buff, int left, int right, int(*getsortval)(T *a, T *b))
{
    if (right <= left)
        return;

    int i, j;
    T t, p;

    do
    {
        i = left;
        j = right;
        p = buff[(left + right) / 2];
        do
        {
            while (getsortval(buff + i, &p) < 0)
                ++i;
            while (getsortval(buff + j, &p) > 0)
                --j;
            if (i <= j)
            {
                if (i != j)
                {
                    t = buff[i];
                    buff[i] = buff[j];
                    buff[j] = t;
                }
                ++i;
                --j;
            }
        }
        while (i <= j);
        if (left < j)
            quicksort<T>(buff, left, j, getsortval);
        left = i;
    }
    while (i < right);

}

template<typename T, typename C, typename D>
bool quicksortthread(T* buff, int left, int right, D data, int(*qsortcmp)(D, C *, C *, bool &))
{
    if (right <= left)
        return true;

/*
    int l = left, r = right, pivot = right;
    bool terminated = false;

    T tmp;
    while (!terminated && l < r)
    {
        while (!terminated && l < r && qsortcmp(data, (C)buff[l], (C)buff[pivot], terminated) < 0)
            l++;
        while (!terminated && r > l && qsortcmp(data, (C)buff[r], (C)buff[pivot], terminated) >= 0)
            r--;

        if (!terminated && l < r)
        {
            tmp = buff[l];
            buff[l] = buff[r];
            buff[r] = tmp;
            if (r == pivot)
                pivot = l;
            else if (l == pivot)
                pivot = r;
        }
    }
    if (terminated || l >= right && r < right)
        return !terminated;

    tmp = buff[right];
    buff[right] = buff[r];
    buff[r] = tmp;

    if (!terminated && r - 1 > left)
        terminated = !quicksortthread(buff, left, r - 1, data, qsortcmp);
    if (!terminated && r + 1 < right)
        terminated = !quicksortthread(buff, r + 1, right, data, qsortcmp);

    return !terminated;
*/
    int i, j;
    T t, p;
    bool terminated = false;

    do
    {
        i = left;
        j = right;
        p = buff[(left + right) / 2];
        do
        {
            while (!terminated && qsortcmp(data, (C*)buff + i, &p, terminated) < 0)
                ++i;
            while (!terminated && qsortcmp(data, (C*)buff + j, &p, terminated) > 0)
                --j;
            if (!terminated && i <= j)
            {
                if (i != j)
                {
                    t = buff[i];
                    buff[i] = buff[j];
                    buff[j] = t;
                }
                ++i;
                --j;
            }
        }
        while (!terminated && i <= j);
        if (!terminated && left < j)
            quicksortthread<T, C, D>(buff, left, j, data, qsortcmp);
        left = i;
    }
    while (!terminated && i < right);

    return !terminated;
}

// T must be an item in a linked list with a member "next" which points to the next item, and has no member "prev". (Or fix prev after the call.)
template<typename T>
void _m_merge(T *a1, T *a2, T *b1, T *b2, int (*compare)(T* a, T* b), T* &first, T* &last); // Merges two sorted sequences [a1, a2] and [b1, b2], into the resulting sequence [first, last].
template<typename T>
void _m_sequence(T *list, int (*compare)(T* a, T* b), T* &last, int &len); // Returns a naturally sorted sequence [list, last] with a length of len.
template<typename T>
void _mergesort_next(T* &list, T* &last, int &len, int (*compare)(T* a, T* b)); // Merges the sequence [list, last] with a sequence [last->next, ...] if the next sequence is the same length or longer. Otherwise recursively calls itself with the next sequence.

template<typename T>
void mergesort(T* &list, int (*compare)(T* a, T* b))
{
    if (!list || list->next == NULL)
        return;

    T *last;
    int len;
    _m_sequence(list, compare, last, len);

    if (!last->next)
        return;

    do
    {
        _mergesort_next(list, last, len, compare);
    }
    while (last->next);
}

template<typename T>
void _mergesort_next(T* &list, T* &last, int &len, int (*compare)(T* a, T* b))
{
    T *nextlast;
    int nextlen;
    _m_sequence(last->next, compare, nextlast, nextlen);

    while (nextlast->next && nextlen < len)
        _mergesort_next(last->next, nextlast, nextlen, compare);

    _m_merge(list, last, last->next, nextlast, compare, list, last);
    len += nextlen;
}

template<typename T>
void _m_sequence(T *list, int (*compare)(T* a, T* b), T* &last, int &len)
{
    len = 1;
    last = list;
    while (last->next && compare(last, last->next) <= 0)
       ++len, last = last->next;
}

template<typename T>
void _m_merge(T *a1, T *a2, T *b1, T *b2, int (*compare)(T* a, T* b), T* &first, T* &last)
{
    if (!a1)
    {
        first = b1;
        last = b2;
        return;
    }
    else if (!b1)
    {
        first = a1;
        last = a2;
        return;
    }


    T *curr;
    last = b2->next;

    first = curr = (a1 == a2 ? b1 /* always a2 > b1 */
                    : compare(a1, b1) <= 0 ? a1 : b1);
    if (curr == a1)
        a1 = a1 == a2 ? NULL : a1->next;
    else
        b1 = b1 == b2 ? NULL : b1->next;

    while (a1 && b1)
    {
        curr = curr->next = compare(a1, b1) <= 0 ? a1 : b1;

        if (curr == a1)
            a1 = a1 == a2 ? NULL : a1->next;
        else
            b1 = b1 == b2 ? NULL : b1->next;
    }

    if (a1 != NULL)
    {
        curr->next = a1;
        a2->next = last;
        last = a2;
    }
    else
    {
        curr->next = b1;
        b2->next = last;
        last = b2;
    }
}



#endif
