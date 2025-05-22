#include "Graf.h"
#include <limits>
#include <cmath>
#include <ctime>
#include <vector>
#include <utility>
int total_cost = 0, total_time = 0;
int Tmax = 700;
const int MAX = std::numeric_limits<int>::max();
struct unit_for_task {
    int procNum;
    int procNumIndex;
    int time;
    int cost;
    int start_time;
    int end_time;
};
void assign_units_recursively(Graf& graph, int start);
void assign_units_recursively_unpredicted(Graf& graph, int start);

int num_of_tasks;
int numOfPE;
std::vector<std::vector<std::pair<int, int>>> work_times[30]; // indeks to numer procesora
std::vector<std::vector<std::pair<int, int>>> new_work_times[30];
std::vector<unit_for_task> chosen; // indeks to numer zadania
std::vector<unit_for_task> new_chosen; // indeks to numer zadania

static int min_time_cost(int task, Graf& graph) {
    int min_product = MAX;
    
    int index = -1;
    for (int i = 0; i < numOfPE; i++) {
        int time = graph.times[task][i];
        int cost = graph.costs[task][i]; 
    }
    for (int i = 0; i < numOfPE; i++) {
        int time = graph.times[task][i];
        int cost = graph.costs[task][i];
        int product = time * cost;
        if (product < min_product) {
            min_product = product;
            index = i;
        }
    }
    return index;
}

void assign_unit(Graf& g, int task, int start_time, int ancestor) {
    int assigned_unit = min_time_cost(task, g);
    if (!g.is_PP(assigned_unit)) {
        work_times[assigned_unit].push_back(std::vector<std::pair<int, int>>());
        int idx = work_times[assigned_unit].size();
        chosen[task] = { assigned_unit,idx,g.times[task][assigned_unit],g.costs[task][assigned_unit],start_time,start_time + g.times[task][assigned_unit] };
    }
    else if (work_times[assigned_unit].empty()) {
        chosen[task] = { assigned_unit,0,g.times[task][assigned_unit],g.costs[task][assigned_unit],start_time,start_time + g.times[task][assigned_unit] };
        work_times[assigned_unit].push_back(std::vector<std::pair<int, int>>());
        work_times[assigned_unit][0].push_back(std::pair<int, int>(start_time, chosen[task].end_time));
    }
    else {
        //sprawdzam która jednostka zakończy pracę przed koniecznościa rozpoczęcia obecnego zadania
        int idx_of_PP = 0;
        bool found_free_PP = false;
        for (auto& vec_of_times : work_times[assigned_unit]) {
            if (vec_of_times.empty()) continue;
            if (vec_of_times.back().second <= start_time) {
                chosen[task] = { assigned_unit,idx_of_PP,g.times[task][assigned_unit],g.costs[task][assigned_unit],start_time,start_time + g.times[task][assigned_unit] };
                vec_of_times.push_back(std::pair<int, int>(start_time, chosen[task].end_time));
                found_free_PP = true;
                break;
            }
            idx_of_PP++;
        }
        if (!found_free_PP) {
            chosen[task] = { assigned_unit,idx_of_PP,g.times[task][assigned_unit],g.costs[task][assigned_unit],start_time,start_time + g.times[task][assigned_unit] };
            work_times[assigned_unit].push_back(std::vector<std::pair<int, int>>());
            work_times[assigned_unit][idx_of_PP].push_back(std::pair<int, int>(start_time, chosen[task].end_time));
        }
    }

}

void allocate_task_on_existing_PP(Graf& g, int assigned_unit, int idx_of_PP, int start_time, int task) {
    int idx_of_PP_begin = idx_of_PP;
    bool flag = false;
    if (assigned_unit == -1) std::cout << "-1";
    int last_task_end_time;
    while (true) {
        if (new_work_times[assigned_unit][idx_of_PP].empty()) last_task_end_time = 0;
        else last_task_end_time = new_work_times[assigned_unit][idx_of_PP].back().second;
        if (last_task_end_time <= start_time) {
            new_chosen[task] = { assigned_unit,idx_of_PP,g.times[task][assigned_unit],g.costs[task][assigned_unit],start_time,start_time + g.times[task][assigned_unit] };
            new_work_times[assigned_unit].push_back(std::vector<std::pair<int, int>>());
            new_work_times[assigned_unit][idx_of_PP].push_back(std::pair<int, int>(start_time, new_chosen[task].end_time));
            return;
        }
        if (flag == true) {
            new_chosen[task] = { assigned_unit,idx_of_PP,g.times[task][assigned_unit],g.costs[task][assigned_unit],last_task_end_time,last_task_end_time + g.times[task][assigned_unit] };
            new_work_times[assigned_unit].push_back(std::vector<std::pair<int, int>>());
            new_work_times[assigned_unit][idx_of_PP].push_back(std::pair<int, int>(last_task_end_time, new_chosen[task].end_time));
            return;
        }
        else {
            idx_of_PP++;
            idx_of_PP %= work_times[assigned_unit].size();
            if (idx_of_PP == idx_of_PP_begin) flag = true;
            
        }
    }
}
void assign_unit_unpredicted(Graf& g, int task, int ancestor) {
    if (task == 5) {
        allocate_task_on_existing_PP(g, chosen[ancestor].procNum, chosen[ancestor].procNumIndex, chosen[ancestor].end_time, task);
        return;
    }
    allocate_task_on_existing_PP(g, chosen[task].procNum, chosen[task].procNumIndex, chosen[ancestor].end_time, task);
}
int main() {
    srand(time(nullptr));
    Graf graph;
    graph.readFromFile("GRAF.00_UT.txt");
    numOfPE = graph.getNumberOfPE();
    int min_time_T0 = MAX;
    num_of_tasks = graph.numberOfVertices;
    int min_index = 0;

    for (int i = 0; i < numOfPE; i++) {
        //najmniejszy spośród uniwersalnych
        if (graph.times[0][i] < min_time_T0 && graph.procs[i][1] > 0) {
            min_time_T0 = graph.times[0][i];
            min_index = i;
        }
    }
    for (int i = 0; i < num_of_tasks; i++) {
        chosen.push_back({ -1,-1,-1,-1,-1,-1 });
    }

    chosen[0] = { min_index,0,graph.times[0][min_index], graph.costs[0][min_index], 0, graph.times[0][min_index] };
    if (graph.is_PP(min_index)) {
        work_times[min_index].push_back(std::vector<std::pair<int, int>>());
        work_times[min_index][0].push_back(std::pair<int, int>(0, chosen[0].end_time));
    }
    total_cost += graph.procs[min_index][0];
    int start = 0;
    assign_units_recursively(graph, start);

    int max_time = 0, max_time_idx = -1;
    for (int i = 0; i < num_of_tasks; i++) {
        //obliczanie najdluzszego czasu wykonania zadania
        if (i == 5) continue;
        if (chosen[i].end_time > max_time) {
            max_time = chosen[i].end_time;
            max_time_idx = i;
        }
        if (graph.is_PP(chosen[i].procNum)) {
            std::cout << "Zadanie T" << i << " wykonano na PP" << chosen[i].procNum << "_" << chosen[i].procNumIndex << " w czasie " << chosen[i].start_time << " - " << chosen[i].end_time << std::endl;
        }
        else {
            std::cout << "Zadanie T" << i << " wykonano na HC" << chosen[i].procNum << "_" << chosen[i].procNumIndex << " w czasie " << chosen[i].start_time << " - " << chosen[i].end_time << std::endl;
        }
    }
    total_time = max_time;


    std::cout << "Czas wykonania wszystkich zadan: " << total_time<<std::endl<<std::endl;
    std::cout << "Teraz zostanie przydzielone nieprzewidzanie zadanie: " << std::endl;
  
    
    for (int i = 0; i < num_of_tasks; i++) {
        
       new_chosen.push_back({ -1,-1,-1,-1,-1,-1 });
    }
    new_chosen[0] = { min_index,0,graph.times[0][min_index], graph.costs[0][min_index], 0, graph.times[0][min_index] };
    new_work_times[min_index].push_back(std::vector<std::pair<int, int>>());
    new_work_times[min_index][0].push_back(std::pair<int, int>(0, chosen[0].end_time));
    assign_units_recursively_unpredicted(graph, 0);
    max_time = 0, max_time_idx = -1;
    for (int i = 0; i < num_of_tasks; i++) {
        //obliczanie najdluzszego czasu wykonania zadania

        if (new_chosen[i].end_time > max_time) {
            max_time = new_chosen[i].end_time;
            max_time_idx = i;
        }
        if (graph.is_PP(new_chosen[i].procNum)) {
            std::cout << "Zadanie T" << i << " wykonano na PP" << new_chosen[i].procNum << "_" << new_chosen[i].procNumIndex << " w czasie " << new_chosen[i].start_time << " - " << new_chosen[i].end_time << std::endl;
        }
        else {
            std::cout << "Zadanie T" << i << " wykonano na HC" << new_chosen[i].procNum << "_" << new_chosen[i].procNumIndex << " w czasie " << new_chosen[i].start_time << " - " << new_chosen[i].end_time << std::endl;
        }
    }
    total_time = max_time;
    std::cout << "Czas wykonania wszystkich zadan: " << total_time << std::endl << std::endl;


    return 0;
}

void assign_units_recursively(Graf& graph, int start) {
    std::vector<int> children = graph.getNeighbourIndices(start);
    for (int child : children) {
        //pomijam narazie nieprzewidziane zadanie
        if (child == 5) continue;
        if (chosen[child].procNum == -1) {
            assign_unit(graph, child, chosen[start].end_time, start);
        }
        assign_units_recursively(graph, child); // Rekurencyjne wywołanie dla dzieci dzieci
    }
}
void assign_units_recursively_unpredicted(Graf& graph, int start) {
    std::vector<int> children = graph.getNeighbourIndices(start);
    for (int child : children) {
        if (new_chosen[child].procNum == -1) {
            assign_unit_unpredicted(graph, child, start);
        }
        assign_units_recursively_unpredicted(graph, child); // Rekurencyjne wywołanie dla dzieci dzieci
    }
}


