#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <queue>
#include <random>
#include "include/Example.h"
#include <algorithm>

using namespace std;

random_device rd;
mt19937 rng(rd());

class Obiect {
private:
    string nume;
    int bonusEnergie;
    char simbolHarta;

public:
    explicit Obiect(string idNume = "Necunoscut", int bonus = 0, char simbol = '?')
        : nume(std::move(idNume)), bonusEnergie(bonus), simbolHarta(simbol) {}

    [[nodiscard]] const string& preiaNume() const { return nume; }
    [[nodiscard]] int preiaBonus() const { return bonusEnergie; }
    [[nodiscard]] char preiaSimbol() const { return simbolHarta; }

    friend ostream& operator<<(ostream& os, const Obiect& obj);
};

ostream& operator<<(ostream& os, const Obiect& obj) {
    os << "[" << obj.nume << " | Bonus: " << obj.bonusEnergie << ", Simbol: " << obj.simbolHarta << "]";
    return os;
}

class Inventar {
private:
    vector<Obiect> elemente;
    int capacitateMaxima;

public:
    explicit Inventar(int cap = 5) : capacitateMaxima(cap) {}

    bool adauga(const Obiect& obj) {
        if ((int)elemente.size() < capacitateMaxima) {
            elemente.push_back(obj);
            return true;
        }
        return false;
    }

    void afiseazaInventar() const {
        if (elemente.empty()) {
            cout << "Inventarul este gol.\n";
            return;
        }
        cout << "Inventar (" << elemente.size() << "/" << capacitateMaxima << "):\n";
        for (size_t i = 0; i < elemente.size(); i++) {
            cout << "  " << i + 1 << ". " << elemente[i] << "\n";
        }
    }

    int consumaPrimulObiect() {
        if (elemente.empty()) return 0;
        int bonus = elemente[0].preiaBonus();
        elemente.erase(elemente.begin());
        return bonus;
    }

    [[nodiscard]] bool contineObiectCuSimbol(char simbol) const {
        return std::ranges::any_of(elemente, [simbol](const auto& ob) {
            return ob.preiaSimbol() == simbol;
        });
    }

    void eliminaObiectCuSimbol(char simbol) {
        for (auto it = elemente.begin(); it != elemente.end(); ++it) {
            if (it->preiaSimbol() == simbol) {
                elemente.erase(it);
                return;
            }
        }
    }

    friend ostream& operator<<(ostream& os, const Inventar& inv);
};

ostream& operator<<(ostream& os, const Inventar& inv) {
    os << "Inventar capacitate: " << inv.capacitateMaxima << ", iteme curent: " << inv.elemente.size();
    return os;
}

class Harta {
private:
    vector<vector<char>> grila;
    vector<vector<bool>> grilaVizibilitate;
    int dimensiune;

    [[nodiscard]] bool verificaDrumBFS(int startX, int startY, int endX, int endY) const {
        vector<vector<bool>> vizitat(dimensiune, vector<bool>(dimensiune, false));
        queue<pair<int, int>> coadaBFS;

        coadaBFS.emplace(startX, startY);
        vizitat[startX][startY] = true;

        const int dx[] = {-1, 1, 0, 0};
        const int dy[] = {0, 0, -1, 1};

        while (!coadaBFS.empty()) {
            auto [cx, cy] = coadaBFS.front();
            coadaBFS.pop();

            if (cx == endX && cy == endY) return true;

            for (int i = 0; i < 4; i++) {
                int nx = cx + dx[i];
                int ny = cy + dy[i];

                if (nx >= 0 && nx < dimensiune && ny >= 0 && ny < dimensiune) {
                    if (!vizitat[nx][ny] && grila[nx][ny] != '#') {
                        vizitat[nx][ny] = true;
                        coadaBFS.emplace(nx, ny);
                    }
                }
            }
        }
        return false;
    }

public:
    ~Harta() {
        grila.clear();
        grilaVizibilitate.clear();
    }

    explicit Harta(int dim = 15) : dimensiune(dim) {
        bool hartaBuna = false;

        while (!hartaBuna) {
            grila.assign(dimensiune, vector<char>(dimensiune, '.'));
            grilaVizibilitate.assign(dimensiune, vector<bool>(dimensiune, false));

            for(int i = 0; i < dimensiune; i++) {
                grila[0][i] = '#'; grila[dimensiune-1][i] = '#';
                grila[i][0] = '#'; grila[i][dimensiune-1] = '#';
            }
            for(int i = 1; i < dimensiune - 1; i++) {
                for(int j = 1; j < dimensiune - 1; j++) {
                    if (uniform_int_distribution<int> dist(0, 99); dist(rng) < 20) {
                        grila[i][j] = '#';
                    }
                }
            }
            grila[1][1] = '.'; grila[1][2] = '.'; grila[2][1] = '.';
            grila[dimensiune-2][dimensiune-2] = '.';
            grila[dimensiune-2][dimensiune-3] = '.';

            hartaBuna = verificaDrumBFS(1, 1, dimensiune-2, dimensiune-2);
        }
    }

    Harta(const Harta& other) = default;

    Harta& operator=(const Harta& other) {
        if (this != &other) {
            dimensiune = other.dimensiune;
            grila = other.grila;
            grilaVizibilitate = other.grilaVizibilitate;
        }
        return *this;
    }

    [[nodiscard]] int preiaDimensiunea() const { return dimensiune; }

    [[nodiscard]] bool esteZid(int x, int y) const {
        if (x < 0 || x >= dimensiune || y < 0 || y >= dimensiune) return true;
        return grila[x][y] == '#';
    }

    [[nodiscard]] char preiaCelula(int x, int y) const {
        if (x < 0 || x >= dimensiune || y < 0 || y >= dimensiune) return '#';
        return grila[x][y];
    }

    void seteazaEntitate(int x, int y, char c) {
        if (x >= 0 && x < dimensiune && y >= 0 && y < dimensiune) grila[x][y] = c;
    }

    void curataEntitate(int x, int y) {
        if (x >= 0 && x < dimensiune && y >= 0 && y < dimensiune) {
            if(grila[x][y] == 'J' || grila[x][y] == 'V' || grila[x][y] == 'F') grila[x][y] = '.';
        }
    }

    void calculeazaCampVizual(int centruX, int centruY, int raza) {
        grilaVizibilitate.assign(dimensiune, vector<bool>(dimensiune, false));

        for (int i = 0; i < dimensiune; i++) {
            for (int j = 0; j < dimensiune; j++) {
                if (pow(i - centruX, 2) + pow(j - centruY, 2) <= pow(raza, 2)) {
                    grilaVizibilitate[i][j] = true;
                }
            }
        }
    }

    friend ostream& operator<<(ostream& os, const Harta& h);
};

ostream& operator<<(ostream& os, const Harta& h) {
    os << "\n   == RADAR: TEREN ==\n";
    for (int i = 0; i < h.dimensiune; i++) {
        for (int j = 0; j < h.dimensiune; j++) {
            if (h.grilaVizibilitate[i][j]) {
                os << h.grila[i][j] << " ";
            } else {
                os << "? ";
            }
        }
        os << "\n";
    }
    return os;
}

class Jucator {
private:
    int x, y;
    int energie;
    Inventar rucsac;
    bool areCheie;

public:
    explicit Jucator(int startX = 1, int startY = 1, int startEnergie = 60)
        : x(startX), y(startY), energie(startEnergie), rucsac(3), areCheie(false) {}

    [[nodiscard]] int preiaX() const { return x; }
    [[nodiscard]] int preiaY() const { return y; }
    [[nodiscard]] int preiaEnergie() const { return energie; }

    void incarcaEnergie(int prada) {
        energie += prada;
    }

    void scadeEnergie(int val) {
        energie -= val;
        if (energie < 0) energie = 0;
    }

    bool adunaObiect(const Obiect& obj) {
        if (obj.preiaSimbol() == 'K') {
            areCheie = true;
            cout << "\n[!] Ai gasit cheia. Acum poti iesi din labirint.\n";
            return true;
        }
        return rucsac.adauga(obj);
    }

    [[nodiscard]] bool posedaCheie() const { return areCheie; }

    void beaPotiuneDinInventar() {
        int val = rucsac.consumaPrimulObiect();
        if (val > 0) {
            cout << "\n[!] Ai aplicat un obiect si ai castigat " << val << " baterie !\n";
            energie += val;
        } else {
            cout << "\n[!] Nimic in inventar.\n";
        }
    }

    void folosesteTeleportor() {
        if (rucsac.contineObiectCuSimbol('P')) {
            rucsac.eliminaObiectCuSimbol('P');
            cout << "\n[!] Teleportare activata! Esti proiectat aleatoriu pe harta.\n";
            return;
        }
        cout << "\n[!] Nu ai niciun teleportor in inventar.\n";
    }

    void seteazaPozitia(int nx, int ny) { x = nx; y = ny; }

    void afiseazaStatusInventar() const { rucsac.afiseazaInventar(); }

    void muta(char directie, const Harta& harta) {
        int nouX = x, nouY = y;

        if (directie == 'w') nouX--;
        else if (directie == 's') nouX++;
        else if (directie == 'a') nouY--;
        else if (directie == 'd') nouY++;

        if (!harta.esteZid(nouX, nouY) && energie > 0) {
            x = nouX;
            y = nouY;
            energie--;
        } else if (harta.esteZid(nouX, nouY)) {
            cout << "\n[Lovire] BAM! Te-ai izbit cu capul de un pietroi antic. Mai multa grija!\n";
        }
    }

    friend ostream& operator<<(ostream& os, const Jucator& j);
};

ostream& operator<<(ostream& os, const Jucator& j) {
    os << "---------------------------------\n";
    os << "EXPLORATOR 'J' => (X: " << j.x << ", Y: " << j.y << ") | Baterie Lanterna: " << j.energie << "%\n";
    os << "Cheie: " << (j.areCheie ? "Da" : "Nu") << "\n";
    os << j.rucsac;
    return os;
}

class VanatorAI {
protected:
    int x, y;

    static int distantaManhattan(int x1, int y1, int x2, int y2) {
        return abs(x1 - x2) + abs(y1 - y2);
    }

public:
    explicit VanatorAI(int startX = 5, int startY = 5) : x(startX), y(startY) {}

    virtual ~VanatorAI() = default;

    [[nodiscard]] int preiaX() const { return x; }
    [[nodiscard]] int preiaY() const { return y; }
    void reseteazaPozitie(int tx, int ty) { x = tx; y = ty; }

    virtual void muta(const Jucator& jucator, const Harta& harta) {
        int tintX = jucator.preiaX();
        int tintY = jucator.preiaY();
        int celMaiBunX = x;
        int celMaiBunY = y;
        int minDist = 9999;

        const int dx[] = {-1, 1, 0, 0};
        const int dy[] = {0, 0, -1, 1};

        for (int i = 0; i < 4; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];

            if (!harta.esteZid(nx, ny)) {
                int dist = distantaManhattan(nx, ny, tintX, tintY);
                if (dist < minDist) {
                    minDist = dist;
                    celMaiBunX = nx;
                    celMaiBunY = ny;
                }
            }
        }

        x = celMaiBunX;
        y = celMaiBunY;
    }

    [[nodiscard]] virtual char preiaSimbol() const { return 'V'; }

    friend ostream& operator<<(ostream& os, const VanatorAI& v);
};

ostream& operator<<(ostream& os, const VanatorAI& v) {
    os << "Spectru Inamic '" << v.preiaSimbol() << "' => (X: " << v.x << ", Y: " << v.y << ")";
    return os;
}

class Fantoma : public VanatorAI {
public:
    explicit Fantoma(int startX = 5, int startY = 5) : VanatorAI(startX, startY) {}

    void muta(const Jucator& jucator, const Harta& harta) override {
        (void)jucator;
        const int dx[] = {-1, 1, 0, 0};
        const int dy[] = {0, 0, -1, 1};
        vector<pair<int,int>> mutariPosibile;

        for (int i = 0; i < 4; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            if (!harta.esteZid(nx, ny)) {
                mutariPosibile.emplace_back(nx, ny);
            }
        }

        if (!mutariPosibile.empty()) {
            uniform_int_distribution<size_t> dist(0, mutariPosibile.size() - 1);
            size_t alegere = dist(rng);
            x = mutariPosibile[alegere].first;
            y = mutariPosibile[alegere].second;
        }
    }

    [[nodiscard]] char preiaSimbol() const override { return 'F'; }
};

class MotorJoc {
private:
    Harta harta;
    Jucator jucator;
    vector<VanatorAI*> inamici;
    int xDestinatie, yDestinatie;
    vector<Obiect> obiectePeHarta;
    vector<pair<int, int>> coordObiecte;
    vector<pair<int, int>> pozitiiCapcane;
    vector<int> dauneCapcane;
    int scor;
    bool cheiePlasata;

    void genereazaLoot() {
        int maxLoot = 5;
        int dim = harta.preiaDimensiunea();
        uniform_int_distribution<int> xyDist(0, dim - 1);
        uniform_int_distribution<int> tipDist(0, 2);
        uniform_int_distribution<int> damageDist(5, 12);
        while ((int)obiectePeHarta.size() < maxLoot) {
            int bx = xyDist(rng);
            int by = xyDist(rng);
            bool ocupat = false;
            for (const auto& coord : coordObiecte) {
                if (coord.first == bx && coord.second == by) {
                    ocupat = true;
                    break;
                }
            }
            if (!harta.esteZid(bx, by) && !(bx == 1 && by == 1) && !ocupat) {
                Obiect ob;
                int tip = tipDist(rng);
                if (tip == 0) {
                    ob = Obiect("Baterie Duracell", 18, 'B');
                } else if (tip == 1) {
                    ob = Obiect("Teleportor", 0, 'P');
                } else {
                    ob = Obiect("Elixir Vital", 30, 'E');
                }
                obiectePeHarta.push_back(ob);
                coordObiecte.emplace_back(bx, by);
                harta.seteazaEntitate(bx, by, ob.preiaSimbol());
            }
        }

        if (!cheiePlasata) {
            bool plasat = false;
            while (!plasat) {
                int kx = xyDist(rng);
                int ky = xyDist(rng);
                if (!harta.esteZid(kx, ky) && !(kx == 1 && ky == 1) && !(kx == xDestinatie && ky == yDestinatie)) {
                    harta.seteazaEntitate(kx, ky, 'K');
                    plasat = true;
                    cheiePlasata = true;
                }
            }
        }

        int numarCapcane = 4;
        for (int i = 0; i < numarCapcane; i++) {
            int cx = xyDist(rng);
            int cy = xyDist(rng);
            if (!harta.esteZid(cx, cy) && !(cx == 1 && cy == 1) && !(cx == xDestinatie && cy == yDestinatie)) {
                bool suprapus = false;
                for (const auto& p : pozitiiCapcane) {
                    if (p.first == cx && p.second == cy) { suprapus = true; break; }
                }
                for (const auto& coord : coordObiecte) {
                    if (coord.first == cx && coord.second == cy) { suprapus = true; break; }
                }
                if (!suprapus) {
                    pozitiiCapcane.emplace_back(cx, cy);
                    dauneCapcane.push_back(damageDist(rng));
                    harta.seteazaEntitate(cx, cy, 'T');
                }
            }
        }
    }

    void curataInamici() {
        for (auto& inamic : inamici) {
            delete inamic;
        }
        inamici.clear();
    }

public:
    explicit MotorJoc(int dimHarta = 14)
        : harta(dimHarta), jucator(1, 1, 40), xDestinatie(dimHarta-2), yDestinatie(dimHarta-2),
          scor(0), cheiePlasata(false) {
        inamici.push_back(new VanatorAI(dimHarta-3, 3));
        inamici.push_back(new Fantoma(dimHarta/2, dimHarta/2));
        inamici.push_back(new Fantoma(dimHarta-4, dimHarta-4));
        genereazaLoot();
    }

    ~MotorJoc() {
        curataInamici();
    }

    void ruleazaJoc() {
        cout << "\n============================================\n";
        cout << "  BUN VENIT IN LABIRINTUL BLESTEMAT (V2.0)  \n";
        cout << "============================================\n";
        cout << "Legenda: J=Tu, V=Inamic Chaser, F=Fantoma, B=Baterie,\n";
        cout << "         P=Teleportor, E=Elixir, K=Cheie, T=Capcana, D=Iesire, ?=Ceata\n\n";
        cout << "Scor initial: " << scor << "\n";

        uniform_int_distribution<int> moveDist(0, 99);
        int dim = harta.preiaDimensiunea();
        uniform_int_distribution<int> tpDist(0, dim - 1);

        while (true) {
            int prevJX = jucator.preiaX();
            int prevJY = jucator.preiaY();
            vector<pair<int,int>> prevInamici;
            for (auto& inamic : inamici) {
                prevInamici.emplace_back(inamic->preiaX(), inamic->preiaY());
            }

            harta.seteazaEntitate(xDestinatie, yDestinatie, 'D');

            for (size_t i = 0; i < coordObiecte.size(); i++) {
                if (coordObiecte[i].first != -1) {
                    harta.seteazaEntitate(coordObiecte[i].first, coordObiecte[i].second, obiectePeHarta[i].preiaSimbol());
                }
            }

            for (const auto& p : pozitiiCapcane) {
                harta.seteazaEntitate(p.first, p.second, 'T');
            }

            for (size_t i = 0; i < coordObiecte.size(); i++) {
                if (coordObiecte[i].first == jucator.preiaX() && 
                    coordObiecte[i].second == jucator.preiaY()) {
                    if (jucator.adunaObiect(obiectePeHarta[i])) {
                        cout << "\n[!] Ai gasit: " << obiectePeHarta[i].preiaNume() << "\n";
                        scor += 10;
                        coordObiecte[i] = {-1, -1};
                    }
                }
            }

            if (harta.preiaCelula(jucator.preiaX(), jucator.preiaY()) == 'K') {
                jucator.adunaObiect(Obiect("Cheie", 0, 'K'));
                harta.seteazaEntitate(jucator.preiaX(), jucator.preiaY(), '.');
                scor += 25;
            }

            for (size_t i = 0; i < pozitiiCapcane.size(); i++) {
                if (pozitiiCapcane[i].first == jucator.preiaX() && pozitiiCapcane[i].second == jucator.preiaY()) {
                    int dauna = dauneCapcane[i];
                    cout << "\n[!!!] Ai calcat intr-o capcana! Pierzi " << dauna << " energie.\n";
                    jucator.scadeEnergie(dauna);
                    scor -= 5;
                    pozitiiCapcane.erase(pozitiiCapcane.begin() + static_cast<long>(i));
                    dauneCapcane.erase(dauneCapcane.begin() + static_cast<long>(i));
                    i--;
                }
            }

            harta.seteazaEntitate(jucator.preiaX(), jucator.preiaY(), 'J');
            for (auto& inamic : inamici) {
                harta.seteazaEntitate(inamic->preiaX(), inamic->preiaY(), inamic->preiaSimbol());
            }

            harta.calculeazaCampVizual(jucator.preiaX(), jucator.preiaY(), 3);

            cout << *this;
            cout << "\nScor curent: " << scor << "\n";
            cout << "Actiuni: [w/a/s/d]=Misca | [e]=Foloseste Baterie Rucsac | [t]=Teleportor | [q]=Abandon\nAlege miscare: ";

            bool capturat = false;
            for (auto& inamic : inamici) {
                if (jucator.preiaX() == inamic->preiaX() && jucator.preiaY() == inamic->preiaY()) {
                    capturat = true;
                    break;
                }
            }

            if (capturat) {
                cout << "\n>>> INFRANGERE! Un spectru ti-a furat sufletul! Game Over. <<<\n";
                break;
            }
            if (jucator.preiaX() == xDestinatie && jucator.preiaY() == yDestinatie) {
                if (jucator.posedaCheie()) {
                    cout << "\n>>> SUCCES! Ai scapat din labirint si ai vazut din nou lumina soarelui! <<<\n";
                    scor += 50 + jucator.preiaEnergie() / 2;
                    cout << "Scor final: " << scor << "\n";
                    break;
                } else {
                    cout << "\n[!] Iesirea este incuiata. Ai nevoie de cheie.\n";
                }
            }
            if (jucator.preiaEnergie() <= 0) {
                cout << "\n>>> INFRANGERE! Lanterna s-a stins definitiv. Te pierzi in intuneric. <<<\n";
                break;
            }

            char optiune;
            if (!(cin >> optiune) || optiune == 'q') {
                cout << "\nAi abandonat cautarea. Scor final: " << scor << "\n";
                break;
            }

            if (optiune == 'e') {
                jucator.beaPotiuneDinInventar();
            } else if (optiune == 't') {
                jucator.folosesteTeleportor();
                int nx, ny;
                do {
                    nx = tpDist(rng);
                    ny = tpDist(rng);
                } while (harta.esteZid(nx, ny));
                jucator.seteazaPozitia(nx, ny);
                cout << "Te-ai teleportat la (" << nx << ", " << ny << ").\n";
            } else if (optiune == 'w' || optiune == 'a' || optiune == 's' || optiune == 'd') {
                jucator.muta(optiune, harta);
                for (auto& inamic : inamici) {
                    if (moveDist(rng) > 20) {
                        inamic->muta(jucator, harta);
                    }
                }
            } else {
                cout << "\nComanda invalida.";
            }

            harta.curataEntitate(prevJX, prevJY);
            for (size_t i = 0; i < inamici.size(); i++) {
                harta.curataEntitate(prevInamici[i].first, prevInamici[i].second);
            }
        }
    }

    friend ostream& operator<<(ostream& os, const MotorJoc& mj);
};

ostream& operator<<(ostream& os, const MotorJoc& mj) {
    os << mj.jucator << "\n";
    for (auto inamic : mj.inamici) {
        os << *inamic << "\n";
    }
    os << mj.harta;
    return os;
}

int main() {
    (void)rd;
    (void)rng;

    Example exemplu;
    exemplu.g();

    Obiect obj1("Baterie Duracell", 20, 'B');
    cout << "Obiect: " << obj1 << '\n';
    cout << "Nume: " << obj1.preiaNume()
         << ", Bonus: " << obj1.preiaBonus()
         << ", Simbol: " << obj1.preiaSimbol() << '\n';

    Inventar inv(3);
    cout << "Inventar initial: " << inv << '\n';
    inv.afiseazaInventar();

    inv.adauga(Obiect("Potiune", 10, 'P'));
    inv.adauga(Obiect("Elixir", 25, 'E'));
    inv.adauga(Obiect("Cristal", 5, 'C'));
    inv.afiseazaInventar();

    cout << "Adaug peste capacitate: "
         << (inv.adauga(Obiect("Surplus", 0, 'O')) ? "Reusit" : "Esuat") << '\n';

    int bonusConsumat = inv.consumaPrimulObiect();
    cout << "Am consumat primul obiect, bonus = " << bonusConsumat << '\n';
    inv.afiseazaInventar();

    Harta h_demo(6);
    cout << "Dimensiune h_demo: " << h_demo.preiaDimensiunea() << '\n';
    cout << "Este zid (0,0)? " << h_demo.esteZid(0, 0) << '\n';
    cout << "Este zid (1,1)? " << h_demo.esteZid(1, 1) << '\n';

    h_demo.seteazaEntitate(2, 2, 'J');
    h_demo.curataEntitate(2, 2);
    h_demo.calculeazaCampVizual(3, 3, 2);
    cout << h_demo;

    Harta h_copy = h_demo;
    Harta h_assign;
    h_assign = h_copy;
    cout << "Harta copiata (h_copy):\n" << h_copy;
    cout << "Harta asignata (h_assign):\n" << h_assign;

    Harta h_test(8);
    Jucator juc(1, 1, 50);
    VanatorAI van(4, 4);
    Fantoma fant(5, 5);

    cout << "Jucator dupa creare:\n" << juc << '\n';
    cout << "Vanator dupa creare:\n" << van << '\n';
    cout << "Fantoma dupa creare:\n" << fant << '\n';

    juc.incarcaEnergie(5);
    cout << "Energie dupa incarcare: " << juc.preiaEnergie() << '\n';

    Obiect b1("Piatra", 3, 'S');
    cout << "Adaug obiect in rucsac: " << (juc.adunaObiect(b1) ? "da" : "nu") << '\n';
    juc.afiseazaStatusInventar();

    juc.beaPotiuneDinInventar();
    cout << "Energie dupa consum: " << juc.preiaEnergie() << '\n';

    juc.muta('d', h_test);
    cout << "Pozitie jucator dupa mutare: ("
         << juc.preiaX() << ", " << juc.preiaY() << ")\n";

    van.reseteazaPozitie(2, 2);
    van.muta(juc, h_test);
    cout << "Vanator dupa mutare: ("
         << van.preiaX() << ", " << van.preiaY() << ")\n";

    fant.reseteazaPozitie(6, 6);
    fant.muta(juc, h_test);
    cout << "Fantoma dupa mutare: ("
         << fant.preiaX() << ", " << fant.preiaY() << ")\n";

    MotorJoc jocReal(13);
    jocReal.ruleazaJoc();

    return 0;
}