#include "citation_graph.h"

#include <cassert>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

class Publication {
public:
  typedef typename std::string id_type;
  Publication(id_type const &_id) : id(_id) {
  }
  id_type get_id() const noexcept {
    return id;
  }
private:
  id_type id;
};

int main() {
  CitationGraph<Publication> gen("Goto Considered Harmful");
  Publication::id_type const id1 = gen.get_root_id(); // Czy to jest noexcept?
  assert(gen.exists(id1));
  assert(gen.get_parents(id1).size() == 0);
  gen.create("A", id1);
  gen.create("B", id1);
  assert(gen.get_children(id1).size() == 2);
  gen.create("C", "A");
  gen.add_citation("C", "B");
  assert(gen.get_parents("C").size() == 2);
  assert(gen.get_children("A").size() == 1);
  std::vector<Publication::id_type> parents;
  parents.push_back("A");
  parents.push_back("B");
  gen.create("D", parents);
  assert(gen.get_parents("D").size() == parents.size());
  assert(gen.get_children("A").size() == 2);
  assert("D" == gen["D"].get_id());
  gen.remove("A");
  assert(!gen.exists("A"));
  assert(gen.exists("B"));
  assert(gen.exists("C"));
  assert(gen.exists("D"));
  gen.remove("B");
  assert(!gen.exists("A"));
  assert(!gen.exists("B"));
  assert(!gen.exists("C"));
  assert(!gen.exists("D"));
  try {
    gen["E"];
  }
  catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }
  try {
    gen.create("E", "Goto Considered Harmful");
    gen.create("E", "Goto Considered Harmful");
  }
  catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }
  try {
    gen.remove("Goto Considered Harmful");
  }
  catch (std::exception &e) {
    std::cout << e.what() << std::endl;
  }
}
