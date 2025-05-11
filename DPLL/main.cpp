#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream>

using namespace std;

typedef vector<int> Clauza;
typedef vector<Clauza> CNF;

bool citesteCNFDinFisier(const string& nume_fisier, CNF& cnf, int& nr_clauze) {
    ifstream fisier(nume_fisier);
    if (!fisier.is_open()) {
        cerr << "Eroare la deschiderea fișierului: " << nume_fisier << endl;
        return false;
    }

    string line;
    while (getline(fisier, line)) {
        if (line.empty() || line[0] == 'c') continue;
        if (line[0] == 'p') {

            istringstream iss(line);
            string tmp;
            int vars;
            iss >> tmp >> tmp >> vars >> nr_clauze;
            continue;
        }

        istringstream iss(line);
        Clauza clauza;
        int lit;
        while (iss >> lit && lit != 0) {
            clauza.push_back(lit);
        }
        if (!clauza.empty()) {
            cnf.push_back(clauza);
        }
    }

    fisier.close();
    return true;
}

bool esteSatisfacuta(const CNF& cnf, const unordered_map<int, bool>& asignari) {
    for (const auto& clauza : cnf) {
        bool satisfacuta = false;
        for (int lit : clauza) {
            int var = abs(lit);
            if (asignari.count(var)) {
                if ((lit > 0 && asignari.at(var)) || (lit < 0 && !asignari.at(var))) {
                    satisfacuta = true;
                    break;
                }
            }
        }
        if (!satisfacuta) return false;
    }
    return true;
}

bool propagareUnitate(CNF& cnf, unordered_map<int, bool>& asignari) {
    bool modificare = true;
    while (modificare) {
        modificare = false;
        for (const auto& clauza : cnf) {
            int neatribuite = 0;
            int ultimaLit = 0;
            bool satisfacuta = false;
            for (int lit : clauza) {
                int var = abs(lit);
                if (asignari.count(var)) {
                    if ((lit > 0 && asignari[var]) || (lit < 0 && !asignari[var])) {
                        satisfacuta = true;
                        break;
                    }
                } else {
                    neatribuite++;
                    ultimaLit = lit;
                }
            }

            if (satisfacuta) continue;

            if (neatribuite == 0) {
                return false;
            }

            if (neatribuite == 1) {
                int var = abs(ultimaLit);
                bool val = ultimaLit > 0;
                asignari[var] = val;
                modificare = true;
                break;
            }
        }
    }
    return true;
}

// DPLL recursiv
bool dpll(CNF cnf, unordered_map<int, bool> asignari) {
    if (!propagareUnitate(cnf, asignari)) {
        return false;
    }

    if (esteSatisfacuta(cnf, asignari)) {
        cout << "SATISFIABILA\n";
        return true;
    }

    unordered_set<int> variabile_folosite;
    for (const auto& clauza : cnf) {
        for (int lit : clauza) {
            variabile_folosite.insert(abs(lit));
        }
    }

    for (int var : variabile_folosite) {
        if (!asignari.count(var)) {

            auto asignari_copie = asignari;
            asignari_copie[var] = true;
            if (dpll(cnf, asignari_copie)) return true;


            asignari_copie[var] = false;
            if (dpll(cnf, asignari_copie)) return true;

            return false;
        }
    }

    return false;
}

int main() {
    CNF cnf;
    int nr_clauze = 0;

    string nume_fisier = "input3.txt";
    if (!citesteCNFDinFisier(nume_fisier, cnf, nr_clauze)) {
        return -1;
    }

    unordered_map<int, bool> asignari;

    if (!dpll(cnf, asignari)) {
        cout << "NESATISFIABILA\n";
    }

    return 0;
}
