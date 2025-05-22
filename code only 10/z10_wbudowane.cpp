#include <iostream>
#include <map>
#include <vector>
#include "Graf.h"


using namespace std;

struct unit_for_task {
    int procNum;
    int procNumIndex;
    int time;
    int cost;
    int start_time;
    int end_time;
};
class TasksGraph : public Graf {
public:
    int num_of_tasks;
    int numOfPE;
    vector<unit_for_task> chosen; // indeks to numer zadania
    map<int, vector<unit_for_task>> subtasksChosen;
    vector<vector<pair<int, int>>> work_times[30];

    TasksGraph(string path) {
        readFromFile(path);
        num_of_tasks = getNumberOfVertices();
        numOfPE = getNumberOfPE();
        for (int i = 0; i < num_of_tasks; i++) {
            chosen.push_back({ -1,-1,-1,-1,-1,-1 });
        }
    }
    void assign_unit(int task, int start_time) {
        int subtask_start_time;
        int assigned_unit = rand() % numOfPE;
        if (subtasks[task] == 0) {
            if (!is_PP(assigned_unit)) {
                work_times[assigned_unit].push_back(std::vector<std::pair<int, int>>());
                int idx = work_times[assigned_unit].size();
                chosen[task] = { assigned_unit,idx,subtasksTimes[task][assigned_unit][0],subtasksCosts[task][assigned_unit][0],start_time,start_time + subtasksTimes[task][assigned_unit][0]};
            }
            //zakładam że nie ma celu monitorować czasu pracy jednostek HC
            else if (work_times[assigned_unit].empty()) {
                chosen[task] = { assigned_unit,0,subtasksTimes[task][assigned_unit][0],subtasksCosts[task][assigned_unit][0],start_time,start_time + subtasksTimes[task][assigned_unit][0]};
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
                        chosen[task] = { assigned_unit,idx_of_PP,subtasksTimes[task][assigned_unit][0],subtasksCosts[task][assigned_unit][0],start_time,start_time + subtasksTimes[task][assigned_unit][0]};
                        vec_of_times.push_back(std::pair<int, int>(start_time, chosen[task].end_time));
                        found_free_PP = true;
                        break;
                    }
                    idx_of_PP++;
                }
                if (!found_free_PP) {
                    chosen[task] = { assigned_unit,idx_of_PP,subtasksTimes[task][assigned_unit][0],subtasksCosts[task][assigned_unit][0],start_time,start_time + subtasksTimes[task][assigned_unit][0]};
                    work_times[assigned_unit].push_back(std::vector<std::pair<int, int>>());
                    work_times[assigned_unit][idx_of_PP].push_back(std::pair<int, int>(start_time, chosen[task].end_time));
                }
            }
        }
        else {
            int subtask_end_time;
            subtask_start_time=start_time;
            for (int i = 0; i < subtasks[task]; i++) {
                assigned_unit = rand() % numberOfPE;
                
                if (!is_PP(assigned_unit)) {
                    work_times[assigned_unit].push_back(std::vector<std::pair<int, int>>());
                    int idx = work_times[assigned_unit].size();
                    //subtask_end_time = subtask_start_time + subtasksTimes[task][assigned_unit][0];
                    subtasksChosen[task].push_back({assigned_unit,idx,subtasksTimes[task][assigned_unit][0],subtasksCosts[task][assigned_unit][0],subtask_start_time,subtask_start_time + subtasksTimes[task][assigned_unit][0]});
                    subtask_start_time = subtasksChosen[task][i].end_time;
                    

                }
                //zakładam że nie ma celu monitorować czasu pracy jednostek HC
                else if (work_times[assigned_unit].empty()) {
                    //subtasksChosen[task].push_back(unit_for_task);
                    subtasksChosen[task].push_back( {assigned_unit,0,subtasksTimes[task][assigned_unit][0],subtasksCosts[task][assigned_unit][0],subtask_start_time,subtask_start_time + subtasksTimes[task][assigned_unit][0]});
                    work_times[assigned_unit].push_back(std::vector<std::pair<int, int>>());
                    work_times[assigned_unit][0].push_back(std::pair<int, int>(subtask_start_time, subtasksChosen[task][i].end_time));
                    subtask_start_time = subtasksChosen[task][i].end_time;
                }
                else {
                    //sprawdzam która jednostka zakończy pracę przed koniecznościa rozpoczęcia obecnego zadania
                    int idx_of_PP = 0;
                    bool found_free_PP = false;
                    for (auto& vec_of_times : work_times[assigned_unit]) {
                        if (vec_of_times.empty()) continue;
                        if (vec_of_times.back().second <= subtask_start_time) {
                            subtasksChosen[task].push_back({assigned_unit,idx_of_PP,subtasksTimes[task][assigned_unit][0],subtasksCosts[task][assigned_unit][0],subtask_start_time,subtask_start_time + subtasksTimes[task][assigned_unit][0]});
                            vec_of_times.push_back(std::pair<int, int>(subtask_start_time, subtasksChosen[task][i].end_time));
                            subtask_start_time = subtasksChosen[task][i].end_time;
                            found_free_PP = true;
                            break;
                        }
                        idx_of_PP++;
                    }
                    if (!found_free_PP) {
                        subtasksChosen[task].push_back({assigned_unit,idx_of_PP,subtasksTimes[task][assigned_unit][0],subtasksCosts[task][assigned_unit][0],subtask_start_time,subtask_start_time + subtasksTimes[task][assigned_unit][0]});
                        work_times[assigned_unit].push_back(std::vector<std::pair<int, int>>());
                        work_times[assigned_unit][idx_of_PP].push_back(std::pair<int, int>(subtask_start_time, subtasksChosen[task][i].end_time));
                        subtask_start_time = subtasksChosen[task][i].end_time;
                    }
                }
            }
            //do tablicy używanej przez zywkłe zadania dodaje czas zakończenia wszytskich subtasków 
            chosen[task].end_time = subtask_start_time;
        }
        
    }

    void assign_units_recursively(int start) {
        std::vector<int> children = getNeighbourIndices(start);
        for (int child : children) {
            if (chosen[child].procNum == -1) {
                assign_unit(child, chosen[start].end_time);
            }
            assign_units_recursively(child); // Rekurencyjne wywołanie dla dzieci dzieci
        }
    }
    void display_assigned_units() {
        int total_time = 0;
        int max_time = 0, max_time_idx = -1;
        for (int i = 0; i < num_of_tasks; i++) {
            //obliczanie najdluzszego czasu wykonania zadania
            if (subtasks[i] == 0) {
                if (chosen[i].end_time > max_time) {
                    max_time = chosen[i].end_time;
                    max_time_idx = i;
                }
                if (is_PP(chosen[i].procNum)) {
                    std::cout << "Zadanie T" << i << " wykonano na PP" << chosen[i].procNum << "_" << chosen[i].procNumIndex << " w czasie " << chosen[i].start_time << " - " << chosen[i].end_time << std::endl;
                }
                else {
                    std::cout << "Zadanie T" << i << " wykonano na HC" << chosen[i].procNum << "_" << chosen[i].procNumIndex << " w czasie " << chosen[i].start_time << " - " << chosen[i].end_time << std::endl;
                }
            }
            else {
                for (int j = 0; j < subtasks[i]; j++) {
                    if (subtasksChosen[i][j].end_time > max_time) {
                        max_time = chosen[i].end_time;
                        max_time_idx = i;
                    }
                    if (is_PP(subtasksChosen[i][j].procNum)) {
                        std::cout << "Zadanie T" << i <<"_"<<j << " wykonano na PP" << subtasksChosen[i][j].procNum << "_" << subtasksChosen[i][j].procNumIndex << " w czasie " << subtasksChosen[i][j].start_time << " - " << subtasksChosen[i][j].end_time << std::endl;
                    }
                    else {
                        std::cout << "Zadanie T" << i << "_" << j << " wykonano na HC" << subtasksChosen[i][j].procNum << "_" << subtasksChosen[i][j].procNumIndex << " w czasie " << subtasksChosen[i][j].start_time << " - " << subtasksChosen[i][j].end_time << std::endl;
                    }
                }
            }
        }
        total_time = max_time;


        std::cout << "Czas wykonania wszystkich zadan: " << total_time << std::endl;

    }
    int calculate_system_cost() {
        int total_cost=0;
        std::vector<std::pair<int, int>> v;
        for (int i = 0; i < num_of_tasks; i++) {
            if (subtasks[i] == 0) {
   
                
                total_cost += chosen[i].cost;
                if (is_PP(chosen[i].procNum)) {
                    std::pair<int, int> PE = { chosen[i].procNum,chosen[i].procNumIndex };
                    auto it = std::find(v.begin(), v.end(), PE);
                    if (it == v.end()) {
                        total_cost += subtasksCosts[i][chosen[i].procNum][0];
                        v.push_back(PE);
                    }
                }
                
            }
            else {
                for (int j = 0; j < subtasks[i]; j++) {
                    
                    total_cost += subtasksChosen[i][j].cost;
                    if (is_PP(subtasksChosen[i][j].procNum)) {
                        std::pair<int, int> PE = { subtasksChosen[i][j].procNum,subtasksChosen[i][j].procNumIndex};
                        auto it = std::find(v.begin(), v.end(), PE);
                        if (it == v.end()) {
                            total_cost += subtasksCosts[i][subtasksChosen[i][j].procNum][j];
                            v.push_back(PE);
                        }
                    }
                }
            }
        }
        return total_cost;
    }
    
};


int main()
{
    TasksGraph* G = new TasksGraph("graph.20.dat");
    G->assign_unit(0,0);
    G->assign_units_recursively(0);
    G->display_assigned_units();
    cout << "Calkowity koszt systemu wynosi: " << G->calculate_system_cost();
    delete G;
}

