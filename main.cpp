#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

class Harta {
private:
    vector<vector<char>> grila;

public:
    Harta() {
        grila = vector<vector<char>>(10, vector<char>(10, '.'));
        for(int i = 0; i < 10; i++) {
            grila[0][i] = '#';
            grila[9][i] = '#';
            grila[i][0] = '#';
            grila[i][9] = '#';
        }
        grila[2][2] = '#'; 
        grila[2][3] = '#'; 
        grila[3][2] = '#';
        grila[5][5] = '#';
        grila[5][6] = '#';
    }

    bool esteZid(int x, int y) const {
        if(x < 0 || x >= 10 || y < 0 || y >= 10) return true;
        return grila[x][y] == '#';
    }

    void seteazaEntitate(int x, int y, char c) { 
        grila[x][y] = c; 
    }

    void curataEntitate(int x, int y) { 
        grila[x][y] = '.'; 
    }

    friend ostream& operator<<(ostream& os, const Harta& h);
};

ostream& operator<<(ostream& os, const Harta& h) {
    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 10; j++) {
            os << h.grila[i][j] << ' ';
        }
        os << '\n';
    }
    return os;
}

class Jucator {
private:
    int x, y;
    int energie;

public:
    Jucator(int startX, int startY) : x(startX), y(startY), energie(40) {}

    int preiaX() const { return x; }
    int preiaY() const { return y; }
    int preiaEnergie() const { return energie; }

    void muta(char directie, const Harta& harta) {
        int nouX = x, nouY = y;
        if(directie == 'w') nouX--;
        else if(directie == 's') nouX++;
        else if(directie == 'a') nouY--;
        else if(directie == 'd') nouY++;

        if(!harta.esteZid(nouX, nouY) && energie > 0) {
            x = nouX;
            y = nouY;
            energie--;
        }
    }
};

class VanatorAI {
private:
    int x, y;

    int distantaManhattan(int x1, int y1, int x2, int y2) const {
        return abs(x1 - x2) + abs(y1 - y2);
    }

public:
    VanatorAI(int startX, int startY) : x(startX), y(startY) {}

    int preiaX() const { return x; }
    int preiaY() const { return y; }

    void mutaVanator(const Jucator& jucator, const Harta& harta) {
        int tintX = jucator.preiaX();
        int tintY = jucator.preiaY();
        int celMaiBunX = x;
        int celMaiBunY = y;
        int minDist = 9999;

        int dx[] = {-1, 1, 0, 0};
        int dy[] = {0, 0, -1, 1};

        for(int i = 0; i < 4; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];

            if(!harta.esteZid(nx, ny)) {
                int dist = distantaManhattan(nx, ny, tintX, tintY);
                if(dist < minDist) {
                    minDist = dist;
                    celMaiBunX = nx;
                    celMaiBunY = ny;
                }
            }
        }
        x = celMaiBunX;
        y = celMaiBunY;
    }
};

class MotorJoc {
private:
    Harta harta;
    Jucator jucator;
    VanatorAI vanator;
    int xDestinatie, yDestinatie;

public:
    MotorJoc() : jucator(1, 1), vanator(8, 8), xDestinatie(8), yDestinatie(1) {}

    void ruleazaJoc() {
        bool activ = true;

        while(activ) {
            harta.seteazaEntitate(xDestinatie, yDestinatie, 'D');
            harta.seteazaEntitate(jucator.preiaX(), jucator.preiaY(), 'J');
            harta.seteazaEntitate(vanator.preiaX(), vanator.preiaY(), 'V');

            cout << "====================\n";
            cout << harta;
            cout << "Energie: " << jucator.preiaEnergie() << " | Miscare (w/a/s/d): ";

            if(jucator.preiaX() == vanator.preiaX() && jucator.preiaY() == vanator.preiaY()) {
                cout << "\nAi fost prins! Game Over!\n";
                break;
            }
            if(jucator.preiaX() == xDestinatie && jucator.preiaY() == yDestinatie) {
                cout << "\nAi ajuns la destinatie! Ai castigat!\n";
                break;
            }
            if(jucator.preiaEnergie() <= 0) {
                cout << "\nFara energie! Game Over!\n";
                break;
            }

            char mutare;
            cin >> mutare;

            harta.curataEntitate(jucator.preiaX(), jucator.preiaY());
            harta.curataEntitate(vanator.preiaX(), vanator.preiaY());

            jucator.muta(mutare, harta);
            vanator.mutaVanator(jucator, harta);
        }
    }
};

int main() {
    MotorJoc joc;
    joc.ruleazaJoc();
    return 0;
}
