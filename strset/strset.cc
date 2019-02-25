#include "strset.h"
#include "strsetconst.h"
#include <iostream>
#include <unordered_map>
#include <set>
#include <string>
#include <cstring>

namespace {
    using namespace std;
    unsigned long id_counter = 0;
    enum class Equality_relation : int {smaller = -1, equal = 0, bigger = 1};

    // "Construct On First Use Idiom"
    unordered_map<unsigned long, set<string>>& sets() {
        static auto* ans = new unordered_map<unsigned long, set<string>>();
        return *ans;
    }

    int parse_equality_relation_into_int(Equality_relation relation) {
        switch (relation) {
            case Equality_relation::smaller:
                return -1;
            case Equality_relation::equal:
                return 0;
            case Equality_relation::bigger:
                return 1;
            default:
                return -2;
        }
    }

    /**
     * @brief Porównuje dwa przedziały leksykograficznie.
     *
     * @param start1 - początek pierwszego przedziału.
     * @param end1 - koniec pierwszego przedziału.
     * @param start2 - początek drugiego przedziału.
     * @param end2 - koniec drugiego przedziału.
     * @return 1 Jeśli pierwszy przedział jest mniejszy leksykograficznie od drugiego,
     * 0 jeśli przedziały są sobie równe leksykograficznie.
     * -1 jeśli pierwszy przedział jest większy leksykograficznie od drugiego.
     */
    Equality_relation lexicographical_compare(set<string>::iterator first1, set<string>::iterator end1,
                                                     set<string>::iterator first2, set<string>::iterator end2) {
        while (first1 != end1 && first2 != end2) {
            if (*first1 < *first2)
                return Equality_relation::smaller;
            if (*first1 > *first2)
                return Equality_relation::bigger;
            first1++;
            first2++;
        }

        if (first1 == end1 && first2 == end2)
            return Equality_relation::equal;
        if (first1 == end1)
            return Equality_relation::smaller;
        return Equality_relation::bigger;
    }

#ifdef NDEBUG
    const bool debug = false;
#else
    const bool debug = true;
#endif

    struct DebugInfo {
        DebugInfo() {
            ios_base::Init init;
        }

        void inline print_function_call_info(const char * fun_name) {
            if (debug)
                cerr << fun_name << "()" << endl;
        }

        void inline print_function_call_info(const char * fun_name, const unsigned long & set_id) {
            if (debug)
                cerr << fun_name << "(" << set_id << ")" << endl;
        }

        void inline print_function_call_info(const char * fun_name, const unsigned long & set_id,
                const char * value) {
            if (debug)
                cerr << fun_name << "(" << set_id << ", '" << value << "')" << endl;
        }

        void inline print_function_call_info(const char * fun_name, const unsigned long & set1_id,
                const unsigned long & set2_id) {
            if (debug)
                cerr << fun_name << "(" << set1_id << ", " << set2_id << ")" << endl;
        }

        void inline print_attempt_to_modify_set42(const char * fun_name) {
            if (debug)
                cerr << fun_name << " attempt to modify Set 42 - NO action taken" << endl;
        }

        void inline print_new_set_created_log(const char * fun_name, const unsigned long & set_id) {
            if (debug)
                cerr << fun_name << ": set " << set_id << " created" << endl;
        }

        void inline print_element_inserted_log(const char * fun_name, const unsigned long & set_id,
                const char * value) {
            if (debug)
                cerr << fun_name << ": set " << set_id << ", element '" << value << "' inserted" << endl;
        }

        void inline print_element_already_present_log(const char * fun_name, const unsigned long & set_id, const string & value) {
            if (debug)
                cerr << fun_name << " set: " << set_id << " element '" << value << "' was already present" << endl;
        }

        void inline print_set_not_exists_log(const char * fun_name, const unsigned long & set_id) {
            if (debug)
                cerr << fun_name << ": set " << set_id << " doesn't exists" << endl;
        }

        void inline print_set_deleted_log(const char * fun_name, const unsigned long & set_id) {
            if (debug)
                cerr << fun_name << ": " << set_id << " deleted" << endl;
        }

        void inline print_set_cleared_log(const char * fun_name, const unsigned long & set_id) {
            if (debug)
                cerr << fun_name << ": set " << set_id << " cleared" << endl;
        }

        void inline print_set_element_removed(const char * fun_name, const unsigned long & set_id,
                const string & value) {
            if (debug)
                cerr << fun_name << ": set " << set_id << " element '" << value << "' was removed" << endl;
        }

        void print_set_contains_nelements_log(const char * fun_name, const unsigned long & set_id,
                                              const size_t & num_elements) {
            if (debug)
                cerr << fun_name << " set: " << set_id << " contains " << num_elements << " element(s)" << endl;
        }

        void inline print_set_contains_log(const char * fun_name, const unsigned long & set_id, const string & value) {
            if (debug)
                cerr << fun_name << " set: " << set_id << " contains '" << value << "'" << endl;
        }

        void inline print_set_not_contain(const char * fun_name, const unsigned long & set_id, const string & value) {
            if (debug)
                cerr << fun_name << " set: " << set_id << " doesn't contain '" << value << "'" << endl;
        }

        void inline print_null_value_given(const char * fun_name, const unsigned long & set_id) {
            if (debug)
                cerr << fun_name << "(" << set_id << ", NULL): invalid value (NULL) - NO action taken" << endl;
        }

        void inline print_comparing_result_log(const char * fun_name, const unsigned long & set1_id,
                const unsigned long & set2_id, int result) {
            if (debug)
                cerr << fun_name << ": result of comparing " << set1_id << " to "
                     << set2_id << " is " << to_string(result) << endl;
        }
    };

    // "Construct On First Use Idiom"
    DebugInfo& debug_log() {
        static auto ans = DebugInfo();
        return ans;
    }
}

namespace jnp1 {

    unsigned long strset_new() {
        debug_log().print_function_call_info(__func__);
        sets()[id_counter] = set<string>();
        debug_log().print_new_set_created_log(__func__, id_counter);

        return id_counter++;
    }

    void strset_delete(unsigned long id) {
        debug_log().print_function_call_info(__func__, id);
        auto it = sets().find(id);

        if (it != sets().end()) {
            if (id != strset42()) {
                sets().erase(it);
                debug_log().print_set_deleted_log(__func__, id);
            } else {
                debug_log().print_attempt_to_modify_set42(__func__);
            }
        } else {
            debug_log().print_set_not_exists_log(__func__, id);
        }
    }

    size_t strset_size(unsigned long id) {
        debug_log().print_function_call_info(__func__, id);
        size_t number_of_elements = 0;

        if (sets().find(id) != sets().end()) {
            number_of_elements = sets()[id].size();
            debug_log().print_set_contains_nelements_log(__func__, id, number_of_elements);
        } else {
            debug_log().print_set_not_exists_log(__func__, id);
        }

        return number_of_elements;
    }

    void strset_insert(unsigned long id, const char *value) {
        if (value != nullptr) {
            debug_log().print_function_call_info(__func__, id, value);
            auto it = sets().find(id);

            if (it != sets().end()) {
                if (id != strset42()) {
                    if (it->second.insert(string(value)).second)
                        debug_log().print_element_inserted_log(__func__, id, value);
                    else
                        debug_log().print_element_already_present_log(__func__, id, value);
                } else {
                    debug_log().print_attempt_to_modify_set42(__func__);
                }
            } else {
                debug_log().print_set_not_exists_log(__func__, id);
            }
        } else {
            debug_log().print_null_value_given(__func__, id);
        }
    }

    void strset_remove(unsigned long id, const char *value) {
        if (value != nullptr) {
            debug_log().print_function_call_info(__func__, id, value);
            auto it = sets().find(id);

            if (it != sets().end()) {
                if (id != strset42()) {
                    if (it->second.erase(value) > 0) // Element was erased.
                        debug_log().print_set_element_removed(__func__, id, value);
                    else
                        debug_log().print_set_not_contain(__func__, id, value);
                } else {
                    debug_log().print_attempt_to_modify_set42(__func__);
                }
            } else {
                debug_log().print_set_not_exists_log(__func__, id);
            }
        } else {
            debug_log().print_null_value_given(__func__, id);
        }
    }

    int strset_test(unsigned long id, const char *value) {
        if (value != nullptr) {
            debug_log().print_function_call_info(__func__, id, value);
            auto it = sets().find(id);

            if (it != sets().end()) {
                if (it->second.find(value) != it->second.end()) {
                    debug_log().print_set_contains_log(__func__, id, value);
                    return 1;
                } else {
                    debug_log().print_set_not_contain(__func__, id, value);
                }
            } else {
                debug_log().print_set_not_exists_log(__func__, id);
            }
        } else {
            debug_log().print_null_value_given(__func__, id);
        }

        return 0;
    }

    void strset_clear(unsigned long id) {
        debug_log().print_function_call_info(__func__, id);
        auto it = sets().find(id);

        if (it != sets().end()) {
            if (id != strset42()) {
                it->second.clear();
                debug_log().print_set_cleared_log(__func__, id);
            } else {
                debug_log().print_attempt_to_modify_set42(__func__);
            }
        } else {
            debug_log().print_set_not_exists_log(__func__, id);
        }
    }

    int strset_comp(unsigned long id1, unsigned long id2) {
        debug_log().print_function_call_info(__func__, id1, id2);

        Equality_relation relation;
        auto it1 = sets().find(id1), it2 = sets().find(id2);

        if (it1 == sets().end())
            debug_log().print_set_not_exists_log(__func__, id1);
        if (it2 == sets().end() && id1 != id2)
            debug_log().print_set_not_exists_log(__func__, id1);

        if (it1 == it2) // Oba nie istnieją == zbiory puste.
            relation = Equality_relation::equal;
        else if (it1 == sets().end())
            relation = Equality_relation::smaller;
        else if (it2 == sets().end())
            relation = Equality_relation::bigger;
        else
            relation = lexicographical_compare(it1->second.begin(), it1->second.end(),
                    it2->second.begin(), it2->second.end());

        int result_int = parse_equality_relation_into_int(relation);
        debug_log().print_comparing_result_log(__func__, id1, id2, result_int);

        return result_int;
    }
}
