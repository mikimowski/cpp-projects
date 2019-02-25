#ifndef CITATION_GRAPH_H
#define CITATION_GRAPH_H

#include <vector>
#include <memory>
#include <map>
#include <set>

class PublicationNotFound : public std::exception {
    const char* what() const noexcept override {
        return "PublicationNotFound";
    }
};

class PublicationAlreadyCreated : public  std::exception {
    const char* what() const noexcept override {
        return "PublicationAlreadyCreated";
    }
};

class TriedToRemoveRoot : public std::exception {
    const char* what() const noexcept override {
        return "TriedToRemoveRoot";
    }
};

template <class Publication> class CitationGraph {
private:
    class Node : public std::enable_shared_from_this<Node> {
    private:
        // owner_less używa owner_before - nigdy nie rzuca wyjątku
        Publication publication;
        std::map<typename Publication::id_type, std::weak_ptr<Node>> *publications_map_ptr;
        typename std::map<typename Publication::id_type, std::weak_ptr<Node>>::iterator iterator_in_map;
        std::set<std::weak_ptr<Node>, std::owner_less<std::weak_ptr<Node>>> parents;
        std::set<std::shared_ptr<Node>> children;

    public:
        void set_iterator(typename std::map<typename Publication::id_type, std::weak_ptr<Node>>::iterator &it) {
            this->iterator_in_map = std::move(it);
        }

        /// Może rzucić - strong guarantee
        void add_parent(const std::weak_ptr<Node> &new_parent) {
            parents.insert(std::weak_ptr(new_parent));
        }

        /// Może rzucić wyjątek - strong guarantee
        void add_child(const std::shared_ptr<Node> &new_child) {
            children.insert(std::shared_ptr(new_child));
        }

        /// Nie rzuca wyjątku
        Publication& get_publication() noexcept {
            return this->publication;
        }

        /// Nie rzuca wyjątku, bo porównanie shared_ptr nie rzuca wyjątku
        void erase_child(const std::shared_ptr<Node> &to_delete) noexcept {
            children.erase(to_delete);
        }

        /// Nie rzuca wyjątku, bo porównanie weak_ptr nie rzuca wyjątku
        void erase_parent(const std::weak_ptr<Node> &to_delete) noexcept {
            parents.erase(to_delete);
        }

        /// Nie rzuca wyjątku, bo erase_child nie rzuca wyjątku
        void erase_from_parents() noexcept {
            std::shared_ptr<Node> this_shared_ptr = this->shared_from_this();

            for (auto &parent : parents)
                parent.lock()->erase_child(this_shared_ptr);
        }

        explicit Node(typename Publication::id_type const &id,
                std::shared_ptr<std::map<typename Publication::id_type, std::weak_ptr<Node>>> const &pub_map) :
                publication(Publication(id)),
                publications_map_ptr(pub_map.get()),
                iterator_in_map((*pub_map).end()) {}

        ~Node() {
            std::weak_ptr<Node> this_weak_ptr = this->weak_from_this();
            if (iterator_in_map != (*publications_map_ptr).end())
                (*publications_map_ptr).erase(iterator_in_map); // Nie rzuca wyjątku, bo comparator nie rzuca.

            for (auto &child : children)
                (*child).erase_parent(this_weak_ptr);
        }

        /// Może rzucić wyjątek, ale nic nie edytuje
        std::vector<typename Publication::id_type> get_children_id() {
            std::vector<typename Publication::id_type> children_id;

            for (auto &child_ptr : this->children)
                children_id.push_back((*child_ptr).get_publication().get_id());

            return children_id;
        }

        /// Może rzucić wyjątek, ale nic nie edytuje
        std::vector<typename Publication::id_type> get_parents_id() {
            std::vector<typename Publication::id_type> parent_id;

            for (auto &parent_ptr : this->parents)
                parent_id.push_back(parent_ptr.lock()->get_publication().get_id());

            return parent_id;
        }
    };

    // Dzięki temu że są to wskaźniki to konstruktor przenoszący staje się prosty
    std::shared_ptr<std::map<typename Publication::id_type, std::weak_ptr<Node>>> publications_map;
    std::shared_ptr<Node> root_ptr; // root będzie zniszczony najpierw, bo jest później zadeklarowany

public:
    // Sprawdza, czy publikacja o podanym id istnieje.
    bool exists(typename Publication::id_type const &id) const {
        auto it = (*publications_map).find(id);
        return it != (*publications_map).end() && !it->second.expired();
    }

private:
    void checkExistence(typename Publication::id_type id) const {
        if (!exists(id))
            throw PublicationNotFound();
    }

    void checkNoExistence(typename Publication::id_type const &id) const {
        if (exists(id))
            throw PublicationAlreadyCreated();
    }

    /**
     * Dwa warunki muszą być spełnione:
     * 1. Vector nie może byc pusty
     * 2. Każda publikacja podana w wektorze musi istnieć w grafie
     * Jeśli któryś z warunków nie jest spełniony wyjątek PublicationNotFound zostaje rzucony.
     */
    void checkParentsExistence(std::vector<typename Publication::id_type> const &parents_ids) const {
        if (parents_ids.empty())
            throw PublicationNotFound();

        for (auto &pub_id : parents_ids)
            if (!exists(pub_id))
                throw PublicationNotFound();
    }

    ///Sprawdza czy podane id jest id roota, jeśli tak to rzuca wyjątek TriedToRemoveRoot
    void checkRootDeletion(typename Publication::id_type id) const {
        if (id == get_root_id())
            throw TriedToRemoveRoot();
    }

public:
    // Tworzy nowy graf. Tworzy także węzeł publikacji o identyfikatorze stem_id
    explicit CitationGraph(typename Publication::id_type const &stem_id) {
        publications_map = std::make_shared<std::map<typename Publication::id_type, std::weak_ptr<Node>>>();
        root_ptr = std::make_shared<Node>(stem_id, publications_map);
        auto feedback = (*publications_map).insert({stem_id, std::weak_ptr(root_ptr)});
        (*root_ptr).set_iterator(feedback.first);
    }

    // Konstruktor przenoszący i przenoszący operator przypisania. Powinny być noexcept
    CitationGraph(CitationGraph<Publication> &&other) noexcept {
        std::swap(this->root_ptr, other.root_ptr);
        std::swap(this->publications_map, other.publications_map);
    }

    CitationGraph<Publication>& operator=(CitationGraph<Publication> &&other) noexcept {
        std::swap(this->root_ptr, other.root_ptr);
        std::swap(this->publications_map, other.publications_map);

        return *this;
    }

    // Zwraca identyfikator źródła. Metoda ta powinna być noexcept wtw, gdy metoda Publication::get_id jest noexcept.
    typename Publication::id_type get_root_id() const noexcept(noexcept(std::declval<Publication>().get_id())) {
        return (*root_ptr).get_publication().get_id();
    }

    // Zwraca listę identyfikatorów publikacji cytujących publikację o podanym identyfikatorze.
    // Zgłasza wyjątek PublicationNotFound, jeśli dana publikacja nie istnieje.
    std::vector<typename Publication::id_type> get_children(typename Publication::id_type const &id) const {
        checkExistence(id);

        std::shared_ptr node_ptr = (*publications_map).find(id)->second.lock();
        return (*node_ptr).get_children_id();
    }

    std::vector<typename Publication::id_type> get_parents(typename Publication::id_type const &id) const {
        checkExistence(id);

        std::shared_ptr node_ptr = (*publications_map).find(id)->second.lock();
        return (*node_ptr).get_parents_id();
    }

    // Zwraca referencję do obiektu reprezentującego publikację o podanym id. Zgłasza wyjątek PublicationNotFound,
    // jeśli żądana publikacja nie istnieje
    Publication& operator[](typename Publication::id_type const &id) const {
        checkExistence(id);

        std::shared_ptr<Node> node_ptr = (*publications_map).find(id)->second.lock();
        return node_ptr->get_publication();
    }

    // Tworzy węzeł reprezentujący nową publikację o identyfikatorze id cytującą
    // publikacje o podanym identyfikatorze parent_id lub podanych identyfikatorach
    // parent_ids. Zgłasza wyjątek PublicationAlreadyCreated, jeśli publikacja
    // o identyfikatorze id już istnieje. Zgłasza wyjątek PublicationNotFound, jeśli
    // któryś z wyspecyfikowanych poprzedników nie istnieje albo lista poprzedników jest pusta.
    void create(typename Publication::id_type const &id, typename Publication::id_type const &parent_id) {
        create(id, std::vector<typename Publication::id_type> {parent_id});
    }

    void create(typename Publication::id_type const &id, std::vector<typename Publication::id_type> const &parent_ids) {
        checkNoExistence(id);
        checkParentsExistence(parent_ids);

        std::shared_ptr<Node> new_node = std::make_shared<Node>(id, publications_map);

        for (auto &parent_id : parent_ids)   // Nawet jak rzuci wyjątek to ok - new_node po prostu zniknie
            (*new_node).add_parent((*publications_map).find(parent_id)->second);

        std::vector<std::shared_ptr<Node>> edited_parents;
        try {
            // Może rzucić wyjątek przy każdym szukaniu / wstawieniu, dlatego zapamiętujemy znalezione POINTERY,
            // które nie rzucą wyjątku gdy będziemy chcieli cofnąć wykonane zmiany - usunąć ze zbioru dzieci
            // NAJPIERW dodajemy do wektora (na wypadek błędu) - ewentualne usuwanie dziecka rodzica
            // które nie istnieje nie będzie problemem (brak efektu).
            for (auto &parent_id : parent_ids) {
                std::shared_ptr<Node> parent_ptr = (*publications_map).find(parent_id)->second.lock();
                edited_parents.push_back(parent_ptr);
                parent_ptr->add_child(new_node);
            }

            // Może rzucić wyjątek. Robimy to na koniec, ponieważ usuwanie potem znowu mogłoby rzucić wyjątek.
            auto feedback = (*publications_map).insert({id, new_node});
            (*new_node).set_iterator(feedback.first);
        } catch (...) {
            for (auto &parent : edited_parents)
                (*parent).erase_child(new_node);
            throw;
        }
    }

    // Dodaje nową krawędź w grafie cytowań. Zgłasza wyjątek PublicationNotFound,
    // jeśli któraś z podanych publikacji nie istnieje.
    void add_citation(typename Publication::id_type const &child_id, typename Publication::id_type const &parent_id) {
        checkExistence(child_id);
        checkExistence(parent_id);

        std::shared_ptr<Node> child_ptr((*publications_map).find(child_id)->second);
        std::shared_ptr<Node> parent_ptr((*publications_map).find(parent_id)->second);

        (*parent_ptr).add_child(child_ptr); // Pierwsza zmiana w strukturze
        try {
            (*child_ptr).add_parent(std::weak_ptr<Node>(parent_ptr));
        } catch (...) {
            (*parent_ptr).erase_child(child_ptr);
            throw;
        }
    }

    // Usuwa publikację o podanym identyfikatorze. Zgłasza wyjątek
    // PublicationNotFound, jeśli żądana publikacja nie istnieje. Zgłasza wyjątek
    // TriedToRemoveRoot przy próbie usunięcia pierwotnej publikacji.
    // W wypadku rozspójnienia grafu, zachowujemy tylko spójną składową zawierającą źródło.
    void remove(typename Publication::id_type const &id) {
        checkExistence(id);
        checkRootDeletion(id);

        auto to_remove = (*publications_map).find(id); // Może rzucić wyjątek, ale to ok.
        to_remove->second.lock()->erase_from_parents(); // Pierwsze zmiany w strukturze, ale tu nie rzuci już wyjątku.
    }
};


#endif //CITATION_GRAPH_H
