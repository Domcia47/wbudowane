#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class Graf {
	// od razu musimy zadeklarowa� maksymaln� ilo�� wierzcho�k�w - tutaj 100;
	int matrix[100][100];
	int numberOfVertices;
	int numberOfEdges;

public:
	Graf();
	void createVertices(int ile);
	void addEdge(int i_Vertex_Index_1, int i_Vertex_Index_2);
	void removeEdge(int i_Vertex_Index_1, int i_Vertex_Index_2);
	bool checkEdge(int i_Vertex_Index_1, int i_Vertex_Index_2);
	int vertexDegree(int idx);
	std::vector<int> getNeighbourIndices(int idx);
	std::vector<int> getPredecessorIndices(int idx);
	void printNeighbourIndices(int idx);
	int getNumberOfEdges();
	int getNumberOfVertices();
	void readFromFile(std::string path);

private:
	void clear();
};
