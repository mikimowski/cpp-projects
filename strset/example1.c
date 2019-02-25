#include "strset.h"
#include "strsetconst.h"

#include <assert.h>
#include <stdio.h>

int main() {
    unsigned long s1, s2, s3;

    s1 = strset_new();
    strset_insert(s1, "foo");
    assert(strset_test(s1, "foo"));
    assert(!strset_test(s1, "bar"));
    strset_insert(s1, "bar");
    assert(strset_test(s1, "bar"));
    assert(strset_size(s1) == 2);
    strset_insert(s1, "bar");
    assert(strset_size(s1) == 2);
    strset_remove(s1, "foo");
    assert(!strset_test(s1, "foo"));
    assert(strset_test(s1, "bar"));
    assert(strset_size(s1) == 1);

    strset_delete(s1);
    strset_insert(s1, "whatever");
    assert(strset_size(s1) == 0);
    assert(!strset_test(s1, "whatever"));

    s2 = strset_new();
    s3 = strset_new();
    strset_insert(s2, "Ania");
    strset_insert(s2, "Alek");
    strset_insert(s2, "Maria");
    strset_insert(s2, "Fiona");
    strset_insert(s3, "Ania");
    strset_insert(s3, "Maria");
    assert(strset_comp(s2, s3) == -1);
    assert(strset_comp(s3, s2) == 1);
    strset_remove(s2, "Alek");
    strset_remove(s2, "Fiona");
    strset_remove(s2, "Olek");
    assert(strset_comp(s3, s2) == 0);
    strset_clear(s3);
    assert(strset_comp(s2, s3) == 1);
    strset_clear(s2);
    assert(strset_comp(s2, s3) == 0);
    strset_delete(s2);
    strset_delete(s3);
    assert(strset_comp(s2, s3) == 0);

    assert(strset_size(strset42()) == 1);
    strset_delete(strset42());
    strset_insert(strset42(), "66");
    assert(strset_size(strset42()) == 1);
    strset_insert(strset42(), "24");
    strset_insert(strset42(), "42");
    assert(!strset_test(strset42(), "24"));
    assert(strset_test(strset42(), "42"));
    strset_remove(strset42(), "42");
    assert(strset_size(strset42()) == 1);
    strset_clear(strset42());
    assert(strset_size(strset42()) == 1);
    assert(strset_comp(strset42(), strset42()) == 0);
    assert(strset_comp(strset42(), 666) == 1);
    assert(strset_comp(666, strset42()) == -1);

    return 0;
}
