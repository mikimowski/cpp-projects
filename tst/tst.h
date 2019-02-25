#ifndef TST_H
#define TST_H

#include <memory>

namespace Detail {
    constexpr char kEmptyData = '\0';

    template<typename Iter, typename Acc, typename Functor>
    Acc fold(Iter first, Iter last, Acc acc, Functor functor) {
        return first == last ? acc : fold(std::next(first), last, functor(acc, *first), functor);
    }
}

template<typename C = char>
class TST {
    const std::shared_ptr<const TST> left_node;
    const std::shared_ptr<const TST> center_node;
    const std::shared_ptr<const TST> right_node;
    const C data = '\0';
    const bool end_of_word = false;

    static bool is_end_of_string(C c) {
        return c == '\0';
    }

    static const auto& empty_node() {
        static auto res = std::make_shared<const TST<C>>();
        return res;
    }

    bool go_left(C c) const {
        return c < data;
    }

    bool go_right(C c) const {
        return c > data;
    }

    bool go_center(C c) const {
        return c == data;
    }

    TST(std::shared_ptr<const TST> left, std::shared_ptr<const TST> center, std::shared_ptr<const TST> right, C data, bool end_of_word)
    : left_node(std::move(left))
    , center_node(std::move(center))
    , right_node(std::move(right))
    , data(data)
    , end_of_word(end_of_word)
    {}

    size_t best_prefix_length(const C* str) const {
        return is_end_of_string(str[0]) || this->empty() ? 0
        : (go_center(str[0]) ? (1 + center_node->best_prefix_length(str + 1))
        : (go_left(str[0]) ? left_node->best_prefix_length(str)
        : right_node->best_prefix_length(str)));
    }


public:
    TST() = default;

    TST(const std::basic_string<C>& str)
    : TST(&*(str.begin()))
    {}

    TST(const C* str)
    : left_node(empty_node())
    , center_node(is_end_of_string(str[0]) || is_end_of_string(str[1]) ? empty_node()
                                                                       : std::make_shared<TST<C>>(str + 1))
    , right_node(empty_node())
    , data(str[0])
    , end_of_word(is_end_of_string(str[0]) ? false : is_end_of_string(str[1])) // In case of giving empty string to the constructor
    {}

    TST operator+(const std::basic_string<C>& str) const {
        return *(this) + &*(str.begin());
    }

    TST operator+(const C* str) const {
        return empty() ? TST(str)
        : is_end_of_string(str[0]) ? *this
        : go_left(str[0]) ? TST(std::make_shared<TST>(left() + str), center_node, right_node, data, end_of_word)
        : go_right(str[0]) ? TST(left_node, center_node, std::make_shared<TST>(right() + str), data, end_of_word)
        : TST(left_node, std::make_shared<TST>(center() + (str + 1)), right_node, data, is_end_of_string(str[1]));
    }

    /**
     * @return Data stored in the node
     */
    C value() const {
        return this->empty() ? throw std::logic_error("logic_error: value()") : this->data;
    }

    /**
     * @return True if the node is the end of some word, false otherwise.
     */
    bool word() const {
        return this->empty() ? throw std::logic_error("logic_error: word()") : this->end_of_word;
    }

    /**
     * @return Returns left child of the node
     */
    TST left() const {
        return this->empty() ? throw std::logic_error("logic_error: left()") : *(this->left_node);
    }

    /**
     * @return Returns center child of the node
     */
    TST center() const {
        return this->empty() ? throw std::logic_error("logic_error: center()") : *(this->center_node);
    }

    /**
     * @return Returns right child of the node
     */
    TST right() const {
        return this->empty() ? throw std::logic_error("logic_error: right()") : *(this->right_node);
    }

   /**
    * Node is empty iff it's data is equal Detail::kEmptyData
    * @return True if the node is empty, false otherwise
    */
    bool empty() const {
        return data == Detail::kEmptyData;
    }

    /**
     * Checks whether given string is in the tree
     * @param str - sought string
     * @return True if word is in the tree, false otherwise
     */
    bool exist(const C* str) const {
        return is_end_of_string(str[0]) || this->empty() ? false
        : go_center(str[0]) && is_end_of_string(str[1]) ? end_of_word
        : go_left(str[0]) ? left_node->exist(str)
        : go_right(str[0]) ? right_node->exist(str)
        : center_node->exist(str + 1);
    }

    /**
     * Checks whether given string is in the tree
     * @param str - sought string
     * @return True if word is in the tree, false otherwise
     */
    bool exist(const std::basic_string<C>& str) const {
        return exist(&*(str.begin()));
    }

    /**
     * Searches for the longest common prefix of word str and words in the tree.
     * For example if tst contains "category", "functor" and "theory" then tst.prefix("catamorphism") will return "cat"
    */
    std::basic_string<C> prefix(const C* str) const {
        return this->prefix(std::basic_string(str));
    }

    /**
     * Searches for the longest common prefix of word str and words in the tree.
     * For example if tst contains "category", "functor" and "theory" then tst.prefix("catamorphism") will return "cat"
    */
    std::basic_string<C> prefix(const std::basic_string<C>& str) const {
        return str.substr(0, this->best_prefix_length(&*(str.begin())));
    }

    template<typename Acc, typename Functor>
    Acc fold(Acc acc, Functor functor) const {
        return empty() ? acc :
               functor(left().fold(center().fold(right().fold(acc, functor), functor), functor), value());
    }

    /**
     * @return Number of nodes in the subtree
     */
    size_t size() const {
        return this->fold(size_t(0), [] (size_t acc, C) {
            return acc + 1;
        });
    }
};

#endif // TST_H
