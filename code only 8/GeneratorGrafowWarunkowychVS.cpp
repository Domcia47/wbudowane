#include <iostream>
#include <fstream>
#include <vector>
#include <set>
class Generator {
private:
    int l_wierzcholkow;
    int l_HC;
    int l_PP;
    int suma_PE;
    int l_CL;
public:
    Generator() :suma_PE(0) {
        wczytaj_dane();
    }

    void wczytaj_dane() {
        std::cout << "Witaj! Program generuje losowe grafy reprezentujace system wbudowany." << std::endl;
        std::cout << "Podaj liczbe zadan - wierzcholkow: " << std::endl;
        std::cin >> l_wierzcholkow;
        while (suma_PE <= 0) {
            std::cout << "Podaj liczbe jednostek uniwersalnych: " << std::endl;
            std::cin >> l_PP;
            std::cout << "Podaj liczbe jednostek wyspecjalizowanych: " << std::endl;
            std::cin >> l_HC;
            suma_PE = l_HC + l_PP;
            if (suma_PE <= 0) {
                std::cout << "Liczba jednostek obliczeniowych musi byc wieksza niz 0 " << std::endl;
            }
        }
        std::cout << "Podaj liczbe szyn komunikacyjnych: " << std::endl;
        std::cin >> l_CL;
    }
    void generuj_plik() {
        std::ofstream file("graf.txt");

        if (!file.is_open()) {
            std::cerr << "Nie udalo sie otworzyc pliku wyjsciowego" << std::endl;
        }

        generuj_zadania(file);
        generuj_jednostki(file);
        generuj_czasy(file);
        generuj_koszty(file);
        generuj_szyny(file);

        file << std::endl;
        std::cout << "Wygenerowany graf zadan znajdziesz w pliku graf.txt w katalogu //GeneratorGrafowWarunkowychVS/GeneratorGrafowWarunkowychVS" << std::endl;
        file.close();
    }

    void generuj_zadania(std::ofstream& file) {
        int ile_poloczono = 0;
        int ile_zostalo = 0;
        int ile_dzieci;
        int licznik = 0;
        //TASKS
        file << "@ tasks " << l_wierzcholkow << std::endl;

        //Wypełniam wektor wierzchołkami
        std::vector<int> wierzcholki(l_wierzcholkow);
        for (int i = 0; i < l_wierzcholkow; ++i) {
            wierzcholki[i] = i;
        }

        bool czy_jest_rodzicem[200];
        for (int i = 0; i < l_wierzcholkow;i++) {
            czy_jest_rodzicem[i] = false;
        }
        srand(time(0));

        std::set<int> losowy;


        for (int i = 0; i < l_wierzcholkow - 2;i++) {
            //obliczam ile zostało wierzchołków bez rodzica
            ile_zostalo = l_wierzcholkow - ile_poloczono;
            if (ile_zostalo == 0) ile_dzieci == 0;
            if (ile_zostalo <= 4) {
                ile_dzieci = rand() % 2;
            }
            else ile_dzieci = rand() % (ile_zostalo / 2);
            if (ile_dzieci > 0) {
                czy_jest_rodzicem[i] = 1;
            }

            file << "T" << i << " " << ile_dzieci << " ";
            ile_poloczono += ile_dzieci;

            for (int j = 0; j < ile_dzieci; j++) {
                int ile_przejsc = 0;
                while (czy_jest_rodzicem[wierzcholki[licznik]] || i == licznik) {
                    licznik++;
                    ile_przejsc++;
                    licznik = licznik % l_wierzcholkow;
                    if (ile_przejsc == l_wierzcholkow) break;
                }
                file << wierzcholki[licznik++];
                licznik %= l_wierzcholkow;
                int losowaLiczba = rand() % 100;
                //krawędź zwykła z prawdopodbieństwem 75%
                if (losowaLiczba > 25) file << "(" << rand() % 101 << ")" << " ";
                //krawędź warunkowa
                else {
                    file << "c" << "(";
                    losowaLiczba = rand() % 100;
                    if (losowaLiczba < 33) {
                        file << "min " << rand() % 100;
                    }
                    else if (losowaLiczba > 33 && losowaLiczba < 66) {
                        file << "ex(";
                        losowaLiczba = rand() % 100;
                        if (losowaLiczba < 50) file << "<";
                        else file << ">";
                        file << (rand() % (l_wierzcholkow / 2)) + 1 << ")";

                    }
                    else {
                        //tworzę zbiór losowych zadań do warunku executed
                        int max_warunek = 1 + (rand() % 4);
                        for (int i = 0; i < max_warunek; i++) {
                            int losowa = rand() % l_wierzcholkow;
                            losowy.insert(losowa);
                        }
                        file << "ex(";
                        losowaLiczba = rand() % (l_wierzcholkow / 2);
                        auto it = losowy.begin();
                        while (it != losowy.end()) {
                            file << *it;
                            if (++it != losowy.end()) {
                                file << ", ";
                            }
                        }
                        losowy.clear();
                        file << ")";
                    }
                    file << ", " << rand() % 101 << ") ";
                }
            }
            file << std::endl;
        }
        ile_zostalo = l_wierzcholkow - ile_poloczono;
        if (ile_zostalo > 0) {
            file << "T" << l_wierzcholkow - 2 << " " << ile_zostalo - 1 << " ";
            licznik %= (l_wierzcholkow);
            for (int i = 0; i < ile_zostalo - 1;i++) {
                int ile_przejsc = 0;
                while (czy_jest_rodzicem[wierzcholki[licznik]] || l_wierzcholkow - 2 == licznik) {
                    ++licznik;
                    licznik = licznik % (l_wierzcholkow);
                    ile_przejsc++;
                    if (ile_przejsc == l_wierzcholkow) break;
                }
                file << wierzcholki[licznik];

                file << "(" << rand() % 101 << ")" << " ";


                licznik++;
                licznik %= (l_wierzcholkow);
            }
        }
        else file << "T" << l_wierzcholkow - 2 << " " << 0 << " ";
        file << std::endl << "T" << l_wierzcholkow - 1 << " " << 0 << " ";
        file << std::endl;
    }

    void generuj_jednostki(std::ofstream& file) {
        file << "@ proc " << suma_PE << std::endl;

        //jednostki uniwersalne
        for (int i = 0; i < l_PP; i++) {
            //losowanie liczb z przedziału 100-600 podzielnych przez 5 lub 10
            int losowyCzynnik = rand() % 2 == 0 ? 5 : 10;
            int losowaLiczba = (rand() % 51 + 10) * losowyCzynnik;

            file << losowaLiczba << " " << 0 << " " << 1 << std::endl;
        }
        //jednostki sepcjalizowane
        for (int i = 0; i < l_HC; i++) {
            //losowanie liczb z przedziału 1000-4000 podzielnych przez 5 lub 10
            int losowyCzynnik = rand() % 2 == 0 ? 5 : 10;
            int losowaLiczba = (rand() % 301 + 100) * losowyCzynnik;

            file << losowaLiczba << " " << 0 << " " << 0 << std::endl;
        }
    }

    void generuj_czasy(std::ofstream& file) {
        file << "@ time " << std::endl;

        for (int j = 0; j < l_wierzcholkow; j++) {

            //jednostki uniwersalne - czas zadań
            for (int i = 0; i < l_PP; i++) {
                file << rand() % 901 + 100 << " ";
            }
            //jednostki sepcjalizowane - koszt zadań
            for (int i = 0; i < l_HC; i++) {
                file << rand() % 101 << " ";
            }
            file << std::endl;
        }
    }

    void generuj_koszty(std::ofstream& file) {
        file << "@ costs " << std::endl;

        for (int j = 0; j < l_wierzcholkow; j++) {
            file << "T" << j << " ";
            //jednostki uniwersalne - koszt zadań
            for (int i = 0; i < l_PP; i++) {
                file << rand() % 100 + 1 << " ";
            }
            //jednostki sepcjalizowane - koszt zadań
            for (int i = 0; i < l_HC; i++) {
                file << rand() % 901 + 100 << " ";
            }
            file << std::endl;
        }
    }
    void generuj_szyny(std::ofstream& file) {
        bool czy_ma_szyne[200];
        for (int i = 0; i < l_wierzcholkow;i++) {
            czy_ma_szyne[i] = false;
        }
        file << "@ comm " << l_CL << std::endl;
        for (int i = 0; i < l_CL; i++) {
            file << "CHAN" << i << " " << rand() % 301 << " ";
            int ile_podpietych = 0;


            if (i == l_CL - 1) {
                for (int k = 0;k < suma_PE;k++) {
                    int losowa = rand() % 2;
                    if (!czy_ma_szyne[k]) {
                        file << 1 << " ";
                        ile_podpietych++;
                    }
                    else if (ile_podpietych < 2 && k >= suma_PE - 2) {
                        file << 1 << " ";
                        ile_podpietych++;
                    }
                    else {
                        file << losowa << " ";
                        if (losowa == 1) {
                            ile_podpietych++;
                        }
                    }
                }
                file << std::endl;
            }
            else {
                for (int j = 0; j < suma_PE; j++) {
                    int losowa = rand() % 2;

                    if (ile_podpietych < 2 && j >= suma_PE - 2) {
                        file << 1 << " ";
                        ile_podpietych++;
                        czy_ma_szyne[j] = 1;
                    }
                    else {
                        file << losowa << " ";
                        if (losowa == 1) {
                            ile_podpietych++;
                            czy_ma_szyne[j] = 1;
                        }
                    }
                }
                file << std::endl;
            }
        }
    }
};

int main() {
    Generator* g = new Generator();
    g->generuj_plik();
}