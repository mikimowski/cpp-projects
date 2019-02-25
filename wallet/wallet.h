#ifndef _WALLET_H
#define _WALLET_H

#include <vector>
#include <chrono>
#include <boost/operators.hpp>
#include <iostream>

class Wallet: boost::ordered_field_operators<Wallet> {
public:
    class Operation: boost::ordered_field_operators<Operation> {
        std::chrono::milliseconds time_creation{};
        std::string date;
        // W "jednostkach" - 1B = 1e8
        int64_t wallet_balance{};

    public:
        explicit Operation(int64_t wallet_balance);
        Operation() = default;
        ~Operation() = default;

        friend std::ostream& operator<<(std::ostream &os, const Operation &operation);

        bool operator==(const Operation &rhs) const;
        bool operator<(const Operation &rhs) const;

        int64_t getUnits() const;

        template <class T> Operation(T) = delete;
    };

    class minValueOverdrawn: public std::exception {
        const char* what() const noexcept override {
            return "Żądana operacja spowodowałaby że wartość portfela byłaby mniejsza od minimalnej dopuszczalnej."
                   " Anulowanie operacji...";
        }
    };

    class noBInCirculation: public std::exception {
        const char* what() const noexcept override {
            return "Żądana operacja spowodowałaby że ilość monet w obiegu przekroczyłaby maksymalną liczbę."
                   " Anulowanie operacji...";
        }
    };


private:
    /**
     * Bajtkomonety są trzymane jako int64_t
     * Pierwsze 8 najmniej znaczących cyfr reprezentuje część dziesiętną Bajtkomonet przetrzymywanych
     * w portfelu.
     * Pozostałe cyfry reprezentują część całkowitą.
     * Przykład:
     * value = 23'400'123'000
     * oznacza że obecna wartość portfela wynosi: 234,00123B
     */
    static int64_t B_units_in_circulation; // W "jednostkach" - 1B = 1e8
    int64_t value; // W "jednostkach" - 1B = 1e8
    std::vector<Operation> operations;

    /**
     * Tworzy i dodaje nową operacje do listy operacji danego portfela, dla obecnej jego wartości.
     */
    void addOperation();

    /**
     * NOTE: To prywatna metoda pomocniczna, nie można modyfikować portfela z zewnątrz!
     * Zwraca wartość portfela, po czym ustawia jego wartość na podaną.
     * Funkcja wiąże się z dodaniem operacji do historii operacji portfela.
     * Nie ma wpływu na ilość monety w obiegu.
     * @param newValue - nowa wartość portfela.
     * @return Wartość portfela przed zmianą wartości
     */
    int64_t getAndSet(int64_t newValue);

    /**
     * NOTE: To prywatna metoda pomocniczna, nie można modyfikować portfela z zewnątrz!
     * Aktualizuje wartość portfela o podaną wartość, w szczgólności gdy delta jest ujemna, wartośc
     * portfela zmniejsza się.
     * Funkcja zakłada, że podana delta jest poprawna, czyli na skutek operacji nowa wartość portfela
     * będzie zgodna z przyjętymi założeniami.
     * Procedura wiąże się z dodaniem operacji do historii operacji portfela.
     * Nie ma wpływu na ilość monet w obiegu.
     * @param delta - Wartość o którą należy zaktualizować wartość portfela.
     */
    void updateValue(int64_t delta);

    /**
     * Porzuca swoje atrybuty (pomocniczna funkcja m.in. dla move assignment i move constructor),
     * ustawiają je na wartości domyślne.
     */
    void dropOut();

    /**
     * Pobiera podaną DODATNIĄ ilość z obiegu, jeżeli podana kwota przewyższa kwotę w obiegu funkcja rzuca wyjątek
     * noBInCirculation.
     * Jeżeli podana wartość jest ujemna, rzuca wyjątek minValueOverdrawn.
     * @param value - kwota do pobrania
     * @return Pobrana kwota, jeśli udało się pobrać (wyjątek jeśli nie).
     */
     static int64_t getFromCirculation(int64_t value);

     /**
     * Dodaje podaną wartość do obiegu.
     * Używane np. gdy portfel zostaje nadpisany(=), wtedy jego wartość wraca do obiegu.
     * @return Dodana kwota
     */
     static int64_t addToCirculation(int64_t value);

     /**
     * Zwraca wszystkie pieniądze z danego portfela do obiegu, zeruje stan portfela
     * oraz dodaje operacje do jego historii.
     * @return Oddana kwota
     */
     int64_t returnAllB();

    /**
     * Sprawdza czy podane wartości pomnożone przez siebie nie przekroczą wartości INT64_MAX
     * Jeśli przekroczą to rzuca wyjątek informujący o zbyt dużej wartości.
     */
    static void multiplicationValidation(int64_t a, int64_t b);

    /**
    * Sprawdzaa czy podana wartość jest poprawna - większa niż MIN_WALLET_VALUE
    * Jeśli nie to procedura rzuca wyjątek.
    */
    static void minWalletValueValidation(int64_t value);

    /**
     * Sprawdza czy podana wartość jest liczbą naturalną.
     */
    static void naturalNumberValidation(int64_t value);

public:
    Wallet();
    Wallet(int n);
    explicit Wallet(const char *str);
    explicit Wallet(const std::string &str);
    Wallet(Wallet &&w2); // Move constructor.
    Wallet(Wallet &&w1, Wallet &&w2);
    static Wallet fromBinary(const char *);
    static Wallet fromBinary(const std::string& str);
    ~Wallet();

    friend bool operator==(const Wallet &lhs, const Wallet &rhs);
    friend bool operator<(const Wallet &lhs, const Wallet &rhs);

    Wallet& operator=(Wallet &&rhs); // Move assignment operator.

    Wallet& operator+=(Wallet &&rhs);
    Wallet& operator+=(Wallet &rhs);
    friend Wallet operator+(Wallet &&w1, Wallet &&w2);
    friend Wallet operator+(Wallet &&w1, Wallet &w2);

    Wallet& operator-=(Wallet &&rhs);
    Wallet& operator-=(Wallet &rhs);
    friend Wallet operator-(Wallet &&w1, Wallet &&w2);
    friend Wallet operator-(Wallet &&w1, Wallet &w2);

    Wallet& operator*=(int64_t n);
    Wallet operator*(int64_t n) const;
    friend Wallet operator*(int64_t n, const Wallet &w);

    int64_t getUnits() const;
    size_t opSize() const;
    const Operation& operator[](int64_t k) const;
    friend std::ostream& operator<<(std::ostream &os, const Wallet &w);

    // Copy constructor
    Wallet(const Wallet &w) = delete;
    Wallet & operator=(const Wallet &w) = delete;
    template <class T> Wallet(T) = delete;
};

const Wallet & Empty();

#endif //_WALLET_H
