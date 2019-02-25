// Ten przykład celowo łamie reguły dobrego stylu kodowania.

#include "tst.h"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;
using namespace Detail;

void
example1()
{
    TST<> empty_tree{};

    try {
        empty_tree.value();
    } catch (const std::logic_error&) {
        std::cout << "logic_error: value()\n";
    }

    try {
        empty_tree.word();
    } catch (const std::logic_error&) {
        std::cout << "logic_error: word()\n";
    }

    try {
        empty_tree.left();
    } catch (const std::logic_error&) {
        std::cout << "logic_error: left()\n";
    }

    try {
        empty_tree.center();
    } catch (const std::logic_error&) {
        std::cout << "logic_error: center()\n";
    }

    try {
        empty_tree.right();
    } catch (const std::logic_error&) {
        std::cout << "logic_error: right()\n";
    }

    std::cout << "End of example 1.\n";
}

TST<>
make_empty_TST()
{
    return TST<>{};
}

void
example2()
{
    TST<> tst{};
    TST<> tst_copy{ tst };
    TST<> tst_move{ std::move(make_empty_TST()) };
    assert(tst.empty() && tst_copy.empty() && tst_move.empty());
    std::cout << "End of example 2.\n";
}

void
example3()
{
    TST<> tst{ std::string{ "catamorphism" } };
    assert(tst.exist(std::string{ "catamorphism" }));
    assert(!tst.exist(std::string{ "cat" }));
    auto tst2{ tst + std::string{ "cat" } };
    assert(tst2.exist(std::string{ "cat" }));
    std::cout << "End of example 3.\n";
}

void
example4()
{
    TST<> tst{ std::string{ "cat" } };
    assert(tst.value() == 'c');
    assert(tst.center().value() == 'a');
    assert(tst.center().center().value() == 't');
    assert(tst.word() == false);
    assert(tst.center().word() == false);
    assert(tst.center().center().word() == true);
    assert(tst.size() == 3);
    assert((tst + std::string{ "catalog" }).size() == 7);
    assert((tst + std::string{ "dog" }).size() == 6);
    std::cout << "End of example 4.\n";
}

void
example5()
{
    TST<> tst{ std::string{ "category" } };
    assert((tst + std::string{ "functor" } + std::string{ "theory" })
    .prefix(std::string{ "catamorphism" }) == std::string{ "cat" });
    std::cout << "End of example 5.\n";
}

void
example6() {
    TST<> tst{ std::string("cute") };
    TST<> tst2 = tst + std::string("cup") + std::string("at") + std::string("as")
                 + std::string("he") + std::string("us") + std::string("i");
    assert(tst2.prefix(std::string{ "cur" }) == std::string("cu"));
    assert(tst2.size() == 13);
    assert(tst2.center().center().center().word());
    assert(!tst2.center().center().word());
    assert(tst2.center().center().left().word());
    assert(tst2.left().center().word());
    assert(!tst2.empty());
    assert(tst2.exist(std::string{"i"}));
    assert(tst2.left().center().right().empty());
    assert(!tst2.exist(std::string{""}));
    assert(tst2.prefix(std::string("")) == std::string{""});
    assert(tst2.prefix(std::string("u")) == std::string("u"));
    assert(tst2.exist(std::string("us")));
    assert(tst2.exist(std::string("cup")));
    assert(!tst2.exist(std::string("cus")));
    assert(tst2.exist(std::string("at")));
    assert(tst2.exist(std::string("as")));
    assert(tst2.exist(std::string("he")));
    assert(tst2.exist(std::string("i")));
    assert(tst2.exist(std::string("cute")));
    assert(!tst2.exist(std::string("cuter")));
    assert(tst2.prefix(std::string("cuter")) == std::string("cute"));
    assert(tst2.prefix(std::string("atr")) == std::string("at"));
    assert(tst2.prefix(std::string("he")) == std::string("he"));

    std::cout << "End of example 6.\n";
}

int main()
{
    example1();
    example2();
    example3();
    example4();
    example5();
    example6();
}
