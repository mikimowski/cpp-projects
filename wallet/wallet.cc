#include "wallet.h"
#include <string>
#include <algorithm>
#include <regex>
#include <cmath>
#include <climits>

namespace {
    using namespace std;
    constexpr int64_t INITIAL_WALLET_VALUE = 0;
    constexpr int64_t MIN_WALLET_VALUE = 0;
    // Przy odejmowaniu w1 - w2 / w1-=w2, stan portfela w2 po operacji jest 2 razy większy niż przed.
    constexpr int64_t STARTING_NUMBER_OF_B_IN_UNITS_IN_CIRCULATION = (int64_t)(21 * 1e6 * 1e8);
    constexpr int MAX_PRECISION = 8;
    constexpr int64_t UNITS_CONVERTER = (int64_t)1e8;
    constexpr int DATE_LENGTH = 11;
    constexpr int64_t EMPTY_WALLET_VALUE = 0;
    const regex STRING_CONSTRUCTOR_REGEX(R"(^\s*([1-9]{1}\d{0,7}|0)((\.|\,)(\d{1,8}))?\s*$)");

    string valueToString(int64_t value) {
        int64_t integer_part = value / UNITS_CONVERTER;
        int64_t fractional_part = value % UNITS_CONVERTER;
        string ans = to_string(integer_part);

        int cnt_zeros = 0;
        if (fractional_part != 0) {
            cnt_zeros = MAX_PRECISION - to_string(fractional_part).length();
            ans += ',';
        }

        while (cnt_zeros--)
            ans += '0';

        if (fractional_part != 0) {
            ans += to_string(fractional_part);
            ans.erase(ans.find_last_not_of('0') + 1);
        }

        return ans;
    }
}

/*** INITIALIZATION ***/
int64_t Wallet::B_units_in_circulation = STARTING_NUMBER_OF_B_IN_UNITS_IN_CIRCULATION;

/*** EMPTY ***/
/**
 * @return Pusta instancja klasy Wallet.
 */
const Wallet& Empty() {
    static Wallet emptyWallet{};
    return emptyWallet;
}

/*** CONSTRUCTORS ***/
Wallet::Operation::Operation(int64_t wallet_balance) : wallet_balance(wallet_balance) {
    chrono::time_point time_pt = chrono::system_clock::now();
    this->time_creation = chrono::duration_cast<chrono::milliseconds>(time_pt.time_since_epoch());

    char char_date[DATE_LENGTH];
    time_t t = chrono::system_clock::to_time_t(time_pt);
    strftime(char_date, DATE_LENGTH, "%Y-%m-%d", localtime(&t));
    this->date = string(char_date);
}

Wallet::Wallet() : value(getFromCirculation(INITIAL_WALLET_VALUE)) {
    this->addOperation();
}

Wallet::Wallet(int n) {
    minWalletValueValidation(n);
    multiplicationValidation(n, UNITS_CONVERTER);
    this->value = getFromCirculation(((int64_t) n) * UNITS_CONVERTER);
    this->addOperation();
}

Wallet::Wallet(const char* str) {
    int64_t integer_part = 0;
    int64_t fractional_part = 0;
    cmatch groups;

    if (!regex_match(str, groups, STRING_CONSTRUCTOR_REGEX))
        throw invalid_argument("Podany napis nie reprezentuje liczby naturalnej zgodnej z założeniami.");

    integer_part = stoi(groups[1], nullptr);
    integer_part *= UNITS_CONVERTER;

    if (groups[4] != "") {
        fractional_part = stoi(groups[4]);
        fractional_part *= UNITS_CONVERTER / (int64_t)pow(10, groups[4].length());
    }

    this->value = getFromCirculation(integer_part + fractional_part);
    this->addOperation();
}

Wallet::Wallet(const string& str) : Wallet(str.c_str()) {}

Wallet::Wallet(Wallet &&w2) : value(exchange(w2.value, 0)), operations(move(w2.operations)) {
    this->addOperation();
}

Wallet::Wallet(Wallet&& w1, Wallet&& w2) : value(w1.getUnits() + w2.getUnits()),
                                        operations(vector<Operation>(w1.operations.size() + w2.operations.size())) {
    merge(w1.operations.begin(), w1.operations.end(), w2.operations.begin(), w2.operations.end(), this->operations.begin());
    this->addOperation();
    w1.dropOut(); w2.dropOut();
}

Wallet Wallet::fromBinary(const char* str) {
    int val = 0;
    try {
        val = stoi(str, nullptr, 2);
    } catch (invalid_argument &e) {
        throw invalid_argument("Podany napis nie reprezentuje liczby w zapisie binarnym");
    } catch (out_of_range &e) {
        throw out_of_range("Podany napis reprezentuje zbyt dużą liczbę");
    }

    return Wallet(val);
}

Wallet Wallet::fromBinary(const string& str) {
    return fromBinary(str.c_str());
}

/*** DESTRUCTOR ***/
Wallet::~Wallet() {
    B_units_in_circulation += this->value;
}

/*** STREAMS ***/
ostream& operator<<(ostream& os, const Wallet::Operation& operation) {
    os << "Wallet balance is " << valueToString(operation.wallet_balance)
        << " B after operation made at day " << operation.date;

    return os;
}

ostream& operator<<(ostream& os, const Wallet& w) {
    os << "Wallet[" << valueToString(w.value) << " B]";

    return os;
}

/*** COMPARISON OPERATORS ***/
bool Wallet::Operation::operator<(const Wallet::Operation& rhs) const {
    return this->time_creation < rhs.time_creation;
}

bool Wallet::Operation::operator==(const Wallet::Operation& rhs) const {
    return this->time_creation == rhs.time_creation;
}

bool operator==(const Wallet& lhs, const Wallet& rhs) {
    return lhs.value == rhs.value;
}

bool operator<(const Wallet& lhs, const Wallet& rhs) {
    return lhs.value < rhs.value;
}

/*** OPERATORS ***/
Wallet& Wallet::operator=(Wallet&& rhs) {
    if (this != &rhs) {
        this->operations = move(rhs.operations);
        addToCirculation(this->getAndSet(rhs.value));
        rhs.value = 0;
    }

    return *this;
}

Wallet& Wallet::operator+=(Wallet&& rhs) {
    this->updateValue(rhs.getAndSet(EMPTY_WALLET_VALUE));

    return *this;
}

Wallet& Wallet::operator+=(Wallet& rhs) {
    if (this != &rhs)
        return *this += move(rhs);
    return *this;
}

Wallet operator+(Wallet&& w1, Wallet&& w2) {
    Wallet newWallet;
    newWallet.getAndSet(Wallet::getFromCirculation(w1.returnAllB() + w2.returnAllB()));

    return newWallet;
}

Wallet operator+(Wallet&& w1, Wallet& w2) {
    return move(w1 += move(w2));
}

Wallet& Wallet::operator-=(Wallet&& rhs) {
    minWalletValueValidation(this->value - rhs.value);

    this->updateValue(-addToCirculation(rhs.value));
    rhs.updateValue(getFromCirculation(0));

    return *this;
}

Wallet& Wallet::operator-=(Wallet& rhs) {
    return *this -= move(rhs);
}

Wallet operator-(Wallet&& w1, Wallet&& w2) {
    Wallet::minWalletValueValidation(w1.value - w2.value);
    Wallet::multiplicationValidation(w2.value, 2);

    Wallet newWallet;
    newWallet.getAndSet(w1.value - w2.value);
    w1.getAndSet(w1.value - w2.value);

    return newWallet;
}

Wallet operator-(Wallet&& w1, Wallet& w2) {
    return move(w1) - move(w2);
}


Wallet& Wallet::operator*=(int64_t n) {
    naturalNumberValidation(n);
    multiplicationValidation(this->value, n);

    if (n == 0)
        this->returnAllB();
    else
        this->updateValue(getFromCirculation(this->value * (n - 1)));

    return *this;
}

Wallet Wallet::operator*(int64_t n) const {
    naturalNumberValidation(n);
    multiplicationValidation(this->value, n);

    Wallet newWallet;
    newWallet.value = getFromCirculation(this->value * n);

    return newWallet;
}

Wallet operator*(int64_t n, const Wallet &w) {
    return w * n;
}


/*** GETTERS ***/
int64_t Wallet::getUnits() const {
    return this->value;
}

size_t Wallet::opSize() const {
    return this->operations.size();
}

int64_t Wallet::Operation::getUnits() const {
    return this->wallet_balance;
}

const Wallet::Operation & Wallet::operator[](int64_t k) const {
    if (k < 0)
        throw out_of_range("Numer k-tej operacji musi być liczbą naturalną");
    if (this->operations.size() <= (size_t)k)
        throw out_of_range("Próba pobrania k-tej operacji, podczas gdy na portfelu wykonanu mniej niż k operacji.");

    return this->operations[k];
}

/*** OTHERS ***/
void Wallet::addOperation() {
    this->operations.emplace_back(this->value);
}

int64_t Wallet::getAndSet(int64_t newValue) {
    int64_t res = this->value;
    this->value = newValue;
    this->addOperation();

    return res;
}

void Wallet::updateValue(int64_t delta) {
    this->value += delta;
    this->addOperation();
}

void Wallet::dropOut() {
    addToCirculation(this->value);
    this->value = 0;
    this->operations = vector<Operation>();
}

int64_t Wallet::getFromCirculation(int64_t value) {
    if (B_units_in_circulation < value)
        throw noBInCirculation();
    if (value < 0)
        throw minValueOverdrawn();

    B_units_in_circulation -= value;

    return value;
}

int64_t Wallet::addToCirculation(int64_t value) {
    B_units_in_circulation += value;

    return value;
}

int64_t Wallet::returnAllB() {
    int64_t returned = this->value;
    B_units_in_circulation += this->value;
    this->value = 0;
    this->addOperation();

    return returned;
}

void Wallet::multiplicationValidation(int64_t a, int64_t b) {
    if (a != 0 && b != 0 && a >= INT64_MAX / b)
        throw noBInCirculation();
}

void Wallet::minWalletValueValidation(int64_t value) {
    if (value < MIN_WALLET_VALUE)
        throw minValueOverdrawn();
}

void Wallet::naturalNumberValidation(int64_t value) {
    if (value < 0)
        throw invalid_argument("Podana wartość musi być liczbą naturalną");
}
