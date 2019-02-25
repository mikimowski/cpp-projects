#include "strsetconst.h"
#include "strset.h"
#include <iostream>

namespace {
#ifdef NDEBUG
    const bool debug = false;
#else
    const bool debug = true;
#endif
    using namespace std;
    enum class initiation_state {not_begun, ongoing, completed};
    initiation_state curr_initiation_state = initiation_state::not_begun;
    unsigned long id;

    struct DebugInfo {
        DebugInfo() {
            ios_base::Init init;
        }

        void inline print_init_invoked_log(const char * fun_name) {
            if (debug)
                cerr << fun_name << "(): strsetconst init invoked" << endl;
        }

        void inline print_init_finished_log(const char * fun_name) {
            if (debug)
                cerr << fun_name << "(): strsetconst init finished, its id = " << id << endl;
        }
    };

    DebugInfo& debug_info() {
        static auto ans = DebugInfo();
        return ans;
    }
}

namespace jnp1 {
    unsigned long strset42() {
        if (curr_initiation_state == initiation_state::not_begun) {
            debug_info().print_init_invoked_log(__func__);

            curr_initiation_state = initiation_state::ongoing;
            id = jnp1::strset_new();
            jnp1::strset_insert(id, "42");
            curr_initiation_state = initiation_state::completed;

            debug_info().print_init_finished_log(__func__);
        } else if (curr_initiation_state == initiation_state::ongoing) {
            // Zbiór podaje nie swoje ID, co umożliwia wstawienie do niego.
            // TYLKO gdy się tworzy - sam dla siebie wywoła funkcję insert,
            // ta wartość NIGDY nie jest zwracana do użytkownika.
            return id + 1;
        }

        return id;
    }
}
