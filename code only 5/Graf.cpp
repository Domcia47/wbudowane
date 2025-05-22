
#include "Graf.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

Graf::Graf() : matrix{}, numberOfVertices(0), numberOfEdges(0) {}

void Graf::createVertices(int ile) { numberOfVertices = ile; }

void Graf::addEdge(int i_Vertex_Index_1, int i_Vertex_Index_2) {
    if (i_Vertex_Index_1 >= numberOfVertices ||
        i_Vertex_Index_2 >= numberOfVertices) {
        std::cerr << "Niepoprawne indeksy wierzcho³ków\n";
        return;
    }
    if (i_Vertex_Index_1 == i_Vertex_Index_2) {
        std::cerr << "Nie mo¿na dodaæ krawêdzi do wierzcho³ka\n";
        return;
    }
    matrix[i_Vertex_Index_1][i_Vertex_Index_2] = 1;
    numberOfEdges++;
}

void Graf::removeEdge(int i_Vertex_Index_1, int i_Vertex_Index_2) {
    if (matrix[i_Vertex_Index_1][i_Vertex_Index_2] == 1) {
        matrix[i_Vertex_Index_1][i_Vertex_Index_2] = 0;
        numberOfEdges--;
    }
}

bool Graf::checkEdge(int i_Vertex_Index_1, int i_Vertex_Index_2) {
    if (matrix[i_Vertex_Index_1][i_Vertex_Index_2] == 1) return true;
    return false;
}

int Graf::vertexDegree(int idx) {
    int sum = 0;
    for (int i = 0; i < numberOfVertices; i++) {
        if (matrix[idx][i] == 1) sum++;
    }
    return sum;
}
std::vector<int> Graf::getNeighbourIndices(int idx) {
    std::vector<int> neighbours;
    for (int i = 0; i < numberOfVertices; i++) {
        if (matrix[idx][i] == 1) neighbours.push_back(i);
    }
    return neighbours;
}

std::vector<int> Graf::getPredecessorIndices(int idx) {
    std::vector<int> predecessors;
    for (int i = 0; i < numberOfVertices; i++) {
        if (matrix[i][idx] == 1) predecessors.push_back(i);
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
void Graf::readFromFile(std::string path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "Nie mo¿na otworzyæ pliku " << path << std::endl;
        return;
    }

    std::string line;
    std::getline(file, line);
    std::istringstream ss(line);
    std::string tasksNum;
    ss >> tasksNum >> tasksNum;

    int num = std::stoi(tasksNum);
    createVertices(num);
    int parentnum = 0;
    for (int i = 0; i < num; i++) {
        getline(file, line);
        std::istringstream iss(line);
        std::string token;
        iss >> token >> token;
        while (iss >> token) {
            if (token.back() == ')')
                token.pop_back();
            token.pop_back();
            token.pop_back();
            int child = std::stoi(token);
            addEdge(parentnum, child);
        }
        parentnum++;
    }
    file.close();
}
