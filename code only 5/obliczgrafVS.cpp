#include <fstream>
#include <limits>
#include "Graf.h"
#include <algorithm>



struct fastest {
    int time;
    int procNum;
};
int find_longest_path(Graf* G, int start, fastest arr[], int previous_times) {
    std::vector<int> children = G->getNeighbourIndices(start);

    if (children.size() == 0)
        return previous_times;

    int longest_path = previous_times;

    for (auto child : children) {
        int total_time = previous_times + arr[child].time;
        longest_path = std::max(longest_path, find_longest_path(G, child, arr, total_time));
    }

    return longest_path;
}
int main() {

    Graf* G = new Graf();
    G->readFromFile("graph.20.dat");

    fastest arr[50];

    std::ifstream file("graph.20.dat");
    std::string line;
    int numTasks = G->getNumberOfVertices();

    getline(file, line);


    for (int i = 0; i < numTasks; i++) {
        getline(file, line);
    }
    getline(file, line);
    std::istringstream ss(line);
    std::string procNum_s;
    ss >> procNum_s >> procNum_s;
    int procNum = std::stoi(procNum_s);

    //pomijamy linie, które obsłużyliśmy wcześniej
    for (int i = 0; i < procNum; i++) {
        getline(file, line);
    }

    int tab[50];

    getline(file, line); //pomijam linie @times
    for (int i = 0; i < numTasks; i++) {
        int task = i;
        getline(file, line);
        std::istringstream ss(line);
        std::string time_s;
        int time_i;
        int procidx = 0;
        while (ss >> time_s) {
            time_i = std::stoi(time_s);
            tab[procidx] = time_i;
            procidx++;
        }


        int min = std::numeric_limits<int>::max();
        int min_indeks;
        for (int j = 0; j < procNum; ++j) {
            if (tab[j] < min) {
                min = tab[j];
                min_indeks = j;
            }
        }
        arr[task] = { min,min_indeks };

    }
    std::cout << "Najszybszy czas wykonania wszystkich zadan: " << find_longest_path(G, 0, arr, arr[0].time) << std::endl;

    delete G;
    return 0;


}