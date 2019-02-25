#include "strset.h"

#include <cassert>

using jnp1::strset_new;
using jnp1::strset_insert;
using jnp1::strset_delete;
using jnp1::strset_test;

namespace {
    unsigned long test() {
        unsigned long id = ::jnp1::strset_new();
        ::jnp1::strset_insert(id, "testowy");
        return id;
    }

    unsigned long id = test();
}

int main() {
    ::jnp1::strset_insert(id, nullptr);
    assert(::jnp1::strset_test(id, "testowy"));
    assert(!::jnp1::strset_test(id, "nietestowy"));
    ::jnp1::strset_delete(id);
}
