
#include "Graf.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <stack>

Graf::Graf() : subtasks{}, numberOfVertices(0), numberOfEdges(0) {
    for (int i = 0; i < 300; ++i) {
        for (int j = 0; j < 300; ++j) {
            matrix[i][j] = -1;
        }
    }
}

void Graf::createVertices(int ile) { numberOfVertices = ile; }

void Graf::addEdge(int i_Vertex_Index_1, int i_Vertex_Index_2, int weight) {
    if (i_Vertex_Index_1 >= numberOfVertices ||
        i_Vertex_Index_2 >= numberOfVertices) {
        std::cerr << "Niepoprawne indeksy wierzcholkow\n";
        return;
    }
    if (i_Vertex_Index_1 == i_Vertex_Index_2) {
        std::cerr << "Nie mo�na doda� kraw�dzi do wierzcholka\n";
        return;
    }
    matrix[i_Vertex_Index_1][i_Vertex_Index_2] = weight;
    numberOfEdges++;
}

void Graf::removeEdge(int i_Vertex_Index_1, int i_Vertex_Index_2) {
    if (matrix[i_Vertex_Index_1][i_Vertex_Index_2] >= 0) {
        matrix[i_Vertex_Index_1][i_Vertex_Index_2] = 0;
        numberOfEdges--;
    }
}

bool Graf::checkEdge(int i_Vertex_Index_1, int i_Vertex_Index_2) {
    if (matrix[i_Vertex_Index_1][i_Vertex_Index_2] >= 0) return true;
    return false;
}

int Graf::vertexDegree(int idx) {
    int sum = 0;
    for (int i = 0; i < numberOfVertices; i++) {
        if (matrix[idx][i] >= 0) sum++;
    }
    return sum;
}
std::vector<int> Graf::getNeighbourIndices(int idx) {
    std::vector<int> neighbours;
    for (int i = 0; i < numberOfVertices; i++) {
        if (matrix[idx][i] >= 0) neighbours.push_back(i);
    }
    return neighbours;
}

std::vector<int> Graf::getPredecessorIndices(int idx) {
    std::vector<int> predecessors;
    for (int i = 0; i < numberOfVertices; i++) {
        if (matrix[i][idx] >= 0) predecessors.push_back(i);
    }
    return predecessors;
}
void Graf::printNeighbourIndices(int idx) {
    std::vector<int> neighbours = getNeighbourIndices(idx);
    for (auto index : neighbours) {
        std::cout << index << " ";
    }
    std::cout << std::endl;
}
int Graf::getNumberOfEdges() {
    return numberOfEdges;
}
int Graf::getNumberOfVertices() {
    return numberOfVertices;
}

int Graf::getNumberOfPE() {
    return numberOfPE;
}
int Graf::is_PP(int idx) {
    if (procs[idx][1] == 1) {
        return 1;
    }
    return 0;
}
void Graf::readFromFile(std::string path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "Nie można otworzyć pliku " << path << std::endl;
        return;
    }

    std::string line;
    // Wczytywanie tasków
    std::getline(file, line);
    std::istringstream ss(line);
    std::string tasksNum;
    ss >> tasksNum >> tasksNum;

    int num = std::stoi(tasksNum);
    createVertices(num);
    int parentnum = 0;
    std::string token;
    for (int i = 0; i < num; i++) {
        getline(file, line);
        std::istringstream iss(line);

        iss >> token;

        iss >> token;
        while (iss >> token) {
            if (token.back() == ')') {
                token.pop_back();
                size_t pos = token.find("(");
                int child = std::stoi(token.substr(0, pos));
                int weight = std::stoi(token.substr(pos + 1));
                addEdge(parentnum, child, weight);
            }
            else {
                token.pop_back();
                size_t pos = token.find("[");
                int subtasksNum = stoi(token.substr(pos + 1));
                token.pop_back();
                token.pop_back();
                pos = token.find("(");
                int child = std::stoi(token.substr(0, pos));
                subtasks[child] = subtasksNum;
                int weight = std::stoi(token.substr(pos + 1));
                addEdge(parentnum, child, weight);
            }
        }
        parentnum++;
    }

    // Wczytywanie jednostek
    getline(file, line);
    std::istringstream sss(line);
    std::string proc_s;
    sss >> proc_s >> proc_s;
    numberOfPE = stoi(proc_s);
    procs.resize(numberOfPE, std::vector<int>(2, 0)); // Inicjalizacja proc jako wektora wektorów z dwiema kolumnami

    for (int i = 0; i < numberOfPE; i++) {
        getline(file, line);
        std::istringstream iss(line);
        iss >> token;
        int token_i = stoi(token);
        procs[i][0] = token_i; // Ustawienie wartości pierwszej kolumny na wartość token_i

        // Pomijam drugą kolumnę, bo jest w sumie bezużyteczna
        iss >> token;
        iss >> token;
        token_i = stoi(token);
        procs[i][1] = token_i; // Ustawienie wartości drugiej kolumny na wartość token_i
    }

    // Wczytywanie czasów
    getline(file, line);
    //times.resize(num, std::vector<int>(numberOfPE, 0)); // Inicjalizacja times
    subtasksTimes.resize(num, std::vector<std::vector<int>>(numberOfPE, std::vector<int>(0, 0)));
    for (int i = 0; i < num; i++) {
        getline(file, line);
        std::istringstream iss(line);
        if (subtasks[i] == 0) {
            for (int j = 0; j < numberOfPE; j++) {
                iss >> token;
                int token_i = stoi(token);
                subtasksTimes[i][j].push_back(token_i);
            }
        }
        else {
            for (int j = 0; j < numberOfPE; j++) {
                iss >> token;
                token.erase(0, 1);
                token.pop_back();
                std::string temp;
                std::istringstream iss2(token);

                // Rozdziel string i przekonwertuj na int
                while (std::getline(iss2, temp, '+')) {
                    subtasksTimes[i][j].push_back(std::stoi(temp));
                }
            }
        }
    }

    // Wczytywanie kosztów
    getline(file, line);
    //costs.resize(num, std::vector<int>(numberOfPE, 0)); // Inicjalizacja costs
    subtasksCosts.resize(num, std::vector<std::vector<int>>(numberOfPE, std::vector<int>(0, 0)));
    for (int i = 0; i < num; i++) {
        getline(file, line);
        std::istringstream iss(line);
        if (subtasks[i] == 0) {
            for (int j = 0; j < numberOfPE; j++) {
                iss >> token;
                int token_i = stoi(token);
                subtasksCosts[i][j].push_back(token_i);
            }
        }
        else {
            for (int j = 0; j < numberOfPE; j++) {
                iss >> token;
                token.erase(0, 1);
                token.pop_back();
                std::string temp;
                std::istringstream iss2(token);
                // Rozdziel string i przekonwertuj na int
                while (std::getline(iss2, temp, '+')) {
                    subtasksCosts[i][j].push_back(std::stoi(temp));
                }
            }
        }
    }

    file.close();

}
std::unordered_set<int> Graf::getAllPredecessors(int idx) {
    std::unordered_set<int> allPredecessors;
    std::stack<int> stack;
    stack.push(idx);

    while (!stack.empty()) {
        int current = stack.top();
        stack.pop();

        for (int pred : getPredecessorIndices(current)) {
            if (allPredecessors.find(pred) == allPredecessors.end()) {
                allPredecessors.insert(pred);
                stack.push(pred);
            }
        }
    }

    return allPredecessors;
}
