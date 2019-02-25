/*
  Assumptions / Convention:
  element_label = {T, D, R, C, E}
  element_tag = e.g. E5, R1 etc.
  element_type: e.g. 1uF/6,3V etc.

  unordered_map<char, string_map> elements_labels; Maps: {elements_labels} -> {{elements_tags} -> {elements_types}}
  unordered_map<string, unordered_map<char, set<string>>> elements_types; Maps: {elements_types} ->
  {{element_label} -> {elements_tags_of_given_type_and_label}}
  map<int, int> cnt_nodes_plugs; {nodes_id} -> {#terminals_plugged_in}
*/

#include <iostream>
#include <map>
#include <set>
#include <unordered_map>
#include <sstream>
#include <regex>
#include <list>

using namespace std;

namespace circuit_structures {

    class cmp_by_string_length {
    public:
        bool operator()(const string &a, const string &b) const {
            if (a.length() < b.length())
                return true;
            else if (a.length() > b.length())
                return false;
            return a.compare(b) < 0;
        }
    };

    using string_map = map<string, string, cmp_by_string_length>;
    using mapping_structures = tuple<unordered_map<char, string_map>,
            unordered_map<string, unordered_map<char, set<string, cmp_by_string_length>>>,
            map<int, int>>;
}


namespace reader {
    using namespace circuit_structures;
    const string EMPTY_STRING = "";

    class wrong_input_exception : public exception {
    private:
        string message; /**< Error message to be printed. */

    public:
        /**
        * @brief Constructor for wrong_input_exception, concatenates given strings in proper manner
        * in order to create error message.
        *
        * @param[in] incorrect_input - sequence of characters which turned out to be incorrect.
        * @param[in] line_number - number of the line.
        */
        wrong_input_exception(const string &incorrect_input, const int line_number) {
            this->message = "Error in line " + to_string(line_number) + ": " + incorrect_input;
        }

        const char *what() const noexcept {
            return this->message.c_str();
        }
    };

    /**
    * @brief Determines whether given string matches given regular expression.
    *
    * @param[in] s - sequence to check for match.
    * @param[in] regex_temple - regular expression which is to be matched.
    * @return True if s matches @p regex_temple, false otherwise.
    */
    bool matches_template(const string &s, const regex &regex_temple) {
        return regex_match(s, regex_temple);
    }

    /**
    * @brief Determines whether given string matches any of regular expressions in the given list.
    *
    * @param[in] s - sequence to check for match.
    * @param[in] regex_list - list of regular expressions which are to be matched.
    * @return True if @par s matches any regular expression in the given list.
    */
    bool matches_any_template(const string &s, list<regex> &regex_list) {
        for (const auto &reg : regex_list)
            if (matches_template(s, reg))
                return true;
        return false;
    }

    /**
    * @brief Creates list of regular expressions that matches correct input.
    *
    * @return List of created regular expressions.
    */
    list<regex> create_regex_for_elements_data() {
        list <regex> l;
        const regex transistor_regex(
                R"(\s*[T](0|[1-9]{1}\d{0,8})\s+([A-Z]|\d)([A-Za-z0-9]|[,\-\/])*(\s+(0|[1-9]{1}\d{0,8})){3}\s*)");
        const regex other_elements_regex(
                R"(\s*[DRCE](0|[1-9]{1}\d{0,8})\s+([A-Z]|\d)([A-Za-z0-9]|[,\-\/])*(\s+(0|[1-9]{1}\d{0,8})){2}\s*)");

        l.push_back(transistor_regex);
        l.push_back(other_elements_regex);

        return l;
    }

    /**
    * @brief Increases (+1) counter of terminals plugged in the given node.
    *
    * @param[in, out] cnt_nodes_plugs - structure containing counters for every node in the circuit.
    * @param[in] node_id - id of node whose counter is to be increased.
    */
    void update_nodes_plugs(map<int, int> &cnt_nodes_plugs, int node_id) {
        cnt_nodes_plugs[node_id]++;
    }

    /**
    * @brief Determines whether element with given tag has been already added to dataset.
    *
    * @param[in, out] elements_labels - Structure containing mapping:
    * {elements_labels} -> {{elements_tags} -> {elements_types}}
    * @param[in] element_tag - tag of element to be checked.
    * @return True if similar element has been already added to data, false otherwise.
    */
    bool is_repetition(unordered_map<char, string_map> &elements_labels, const string &element_tag) {
        char element_label = element_tag[0];
        return elements_labels[element_label].count(element_tag) > 0;
    }

    /**
    * @brief Determines whether read data meets given requirements:
    * Element's tag is not repetition = elements in circuit must be unique.
    * Element's terminals have to be plugged into at least two different nodes == nodes_connected must
    * contain at least two elements.
    *
    * @param elements_labels - Structure containing mapping:
    * {elements_labels} -> {{elements_tags} -> {elements_types}}
    * @param element_tag - tag of element to be checked.
    * @param nodes_connected - set of nodes to which are connected element's terminals.
    *
    * @return True if data meet requirements, false otherwise.
    */
    bool is_correct_data(unordered_map<char, string_map> &elements_labels, const string &element_tag,
                         set<int> &nodes_connected) {
        return !is_repetition(elements_labels, element_tag) && nodes_connected.size() > 1;
    }

    /**
    * @brief Inserts new element to the mapping structure.
    *
    * @param[in, out] elements_types - structure containing mapping:
    * {elements_types} -> {{element_label} -> {elements_tags_of_given_type_and_label}}
    * @param[in] element_type - type of given element.
    * @param[in] element_tag - tag of given element.
    */
    void insert_new_element_type_mapping(
            unordered_map<string, unordered_map<char, set<string, cmp_by_string_length>>> &elements_types,
            const string &element_type, const string &element_tag) {
        char element_label = element_tag[0];
        elements_types[element_type][element_label].insert(element_tag);
    }

    /**
    * @brief Inserts new element to the mapping structue.
    *
    * @param[in, out] elements_labels - Structure containing mapping:
    * {elements_labels} -> {{elements_tags} -> {elements_types}}
    * @param[in] element_type - type of given element.
    * @param[in] element_tag - tag of given element.
    */
    void insert_new_element_label_mapping(unordered_map<char, string_map> &elements_labels,
                                          const string &element_type, const string &element_tag) {
        char element_label = element_tag[0];
        elements_labels[element_label][element_tag] = element_type;
    }

    /**
    * @brief Parses correct element description and adds necessary data to mapping structures.
    * Assumption: element_description is correct description of some element: template-wise.
    * Although element might be repetition, or list of nodes might contain repetitions, therefore
    * it might still be incorrect.
    *
    * @throws wrong_input_exception if line contains input inconsistent with presumptions.
    * @param[in, out] circuit_data - Tuple containing mapping structures which contain loaded data.
    * @param[in] element_description - line describing new elemnt to be added.
    * @param[in] cnt_line - parsed lines counter.
    */
    void parse_element_description(mapping_structures &circuit_data, string &element_description, int cnt_line) {
        string element_tag, element_type;
        set<int> nodes_connected;
        int node_id;
        istringstream iss; iss.clear();
        iss.str(element_description);

        iss >> element_tag >> element_type;
        while (iss >> node_id)
            nodes_connected.insert(node_id);

        if (is_correct_data(get<0>(circuit_data), element_tag, nodes_connected)) {
            insert_new_element_type_mapping(get<1>(circuit_data), element_type, element_tag);
            insert_new_element_label_mapping(get<0>(circuit_data), element_type, element_tag);

            for (int node_id : nodes_connected)
                update_nodes_plugs(get<2>(circuit_data), node_id);
        } else {
            throw wrong_input_exception(element_description, cnt_line);
        }
    }

    /**
    * @brief String is considered "white" when it's empty or composed only of white characters.
    *
    * @throws wrong_input_exception if line contains input inconsistent with presumptions.
    * @param[in] line - line to be parsed.
    * @param[in] cnt_line - parsed lines counter.
    * @return EMPTY_STRING if line was incorrect, empty or "white".
    * Otherwise string following given template: ELEMENT_TOKEN ELEMENT_TYPE LIST_OF_NODES_CONNECTED_TO_IT'S_TERMINALS
    */
    string parse_line(const string &line, int cnt_line,
                      list<regex> &elements_data_regex, const regex &empty_string_regex) {
        if (matches_any_template(line, elements_data_regex))
            return line;
        else if (matches_template(line, empty_string_regex))
            return EMPTY_STRING;

        throw wrong_input_exception(line, cnt_line);
    }

    /**
    * @brief Returns tuple composed of three mapping structures containing read data: (A, B, C):
    *
    * A: unordered_map<char, string_map> elements_labels - u_map where key is element's label and value is string_map
    * which contains mapping where keys are elements_tags and values are elements_types.
    * Mapping: {elements_labels} -> {{elements_tags} -> {elements_types}}
    *
    * B: unordered_map<string, unordered_map<char, set<string>>> - u_map where key is element type and value is set containing
    * element's tags of this element type. Mapping: {elements_types} ->
    * {{element_label} > {elements_tags_of_given_type_and_label}}.
    *
    * C: map<int, int> - u_map where key is node's number and value is number of terminals
    * plugged to that node. Mapping: {nodes_id} -> {#terminals_plugged_in}.
    *
    * @return Tuple composed of three mapping structures containing read data: (A, B, C):
    */
    mapping_structures read_data() {
        mapping_structures circuit_data;
        list<regex> elements_data_regex = create_regex_for_elements_data();
        const regex empty_string_regex("^$");

        string line, element_description;
        int cnt_line = 0;
        while (getline(cin, line)) {
            cnt_line++;

            try {
                element_description = parse_line(line, cnt_line, elements_data_regex, empty_string_regex);
                if (!element_description.empty())
                    parse_element_description(circuit_data, element_description, cnt_line);
            } catch (wrong_input_exception &e) {
                cerr << e.what() << endl;
            }
        }

        return circuit_data;
    }
} // End of namespace reader.

namespace writer {
    using namespace circuit_structures;

    enum Element {transistor = 'T', diode = 'D', resistor = 'R', condensator = 'C', power_source = 'E'};

    using type_map = unordered_map<string, unordered_map<char, set<string, cmp_by_string_length>>>;

    /**
    * @brief Lists all tags in given set in order.
    *
    * @param tags[in] - set of element tags.
    */
    void list_tags_from_set(set<string, cmp_by_string_length> &tags) {
        bool first_tag = true;

        for (const string &tag : tags) {
            if (first_tag) {
                cout << tag;
                first_tag = false;
            } else {
                cout << ", " << tag;
            }
        }
    }

    /**
    * @brief Lists all tags of given type and label and adds the type at the end o line.
    *
    * @param element[in] - enum corresponding to particular element label,
    * @param type[in] - type of elements we want to list,
    * @param element_types[in,out] - mapping of element types to sets of tag with given type.
    */
    void list_tags_of_type_and_label(Element element, const string &type, type_map &element_types) {
        auto iterator = element_types[type].find(element);

        if (iterator != element_types[type].end()) {
            list_tags_from_set(element_types[type][element]);
            cout << ": " << type << endl;
            element_types[type].erase(element);
        }
    }

    /**
    * @brief Lists all elements of given label.
    *
    * @param element_label[in] - enum corresponding to particular element label,
    * @param data[in,out] - tuple containing data concerning the circuit.
    */
    void list_element_items(Element element_label, mapping_structures &data) {
        string_map elements_of_type = get<0>(data)[element_label];

        for (pair<string, string> element : elements_of_type)
            list_tags_of_type_and_label(element_label, element.second, get<1>(data));
    }

    /**
    * @brief Adds node number 0 to the map of nodes in the circuit if it is not already there.
    *
    * @param nodes[in,out] - map of nodes in the circuit with number of elements connected to them.
    */
    void add_zero_to_nodes(map<int, int> &nodes) {
        auto iterator = nodes.find(0);

        if (iterator == nodes.end())
            nodes.insert(pair<int, int>(0, 0));
    }

    /**
    * @brief Lists all elements that are in the circuit.
    *
    * Lists all elements that are in the circuit dividing them by tags and types.
    * Elements are listed in this order: transistors, diodes, resistors, condensators, power sources.
    * Then within those categories are grouped by element types (each line is one element type)
    * and sorted by numbers in tags.
    *
    * @param data[in,out] - tuple containing data concerning the circuit.
    */
    void list_all_items(mapping_structures &data) {
        Element elements[5] = {transistor, diode, resistor, condensator, power_source};

        for (Element element : elements)
            list_element_items(element, data);
    }

    /**
    * @brief Lists nodes in the circuit that are connected to one or less element.
    *
    * @param data[in,out] - tuple containing data concerning the circuit.
    */
    void list_warnings(mapping_structures &data) {
        bool no_warnings = true;

        add_zero_to_nodes(get<2>(data));

        for (pair<int, int> element : get<2>(data)) {
            if (element.second < 2) {
                if (no_warnings) {
                    cerr << "Warning, unconnected node(s): " << element.first;
                    no_warnings = false;
                } else {
                    cerr << ", " << element.first;
                }
            }
        }

        if (!no_warnings)
            cerr << endl;
    }
} // End of the namespace writer.



int main() {
    circuit_structures::mapping_structures data;

    data = reader::read_data();
    writer::list_all_items(data);
    writer::list_warnings(data);

    return 0;
}