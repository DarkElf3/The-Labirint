#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
* Auto-generated code below aims at helping you parse
* the standard input according to the problem statement.
**/
static string *PArray; // Входная карта лабиринта
static vector<int> Maze; // Карта с номерами вершин графа
static vector< vector<int> > Graph; // Граф лабиринта
static int R; // number of rows.
static int C; // number of columns.
static int A; // number of rounds between the time the alarm countdown is activated and the time the alarm goes off.

void AddNode(int Row, int Col)
// Добавляет вершину в граф. Связывает ребрами с соседними.
// Если соседей нет в графе, то они добавляются рекурсивно.
{
	int Index = Row * R + Col;
	if (Maze[Index] != -1) return;
	vector<int> Node;
	Graph.push_back(Node);
	int GIndex = Graph.size() - 1;
	Maze[Index] = GIndex;

	if (Col > 0 && PArray[Row][Col - 1] == '.') { // Слева есть путь
		int LeftGIndex = Maze[Index - 1];
		if (LeftGIndex == -1) { // Поля слева нет в графе, добавляем
			Graph.push_back(Node);
			LeftGIndex = Graph.size() - 1;
			Maze[Index - 1] = LeftGIndex;
		}
		Graph[GIndex].push_back(LeftGIndex); // Делаем ребро 
		Graph[LeftGIndex].push_back(GIndex);
	}

	if (Col < (C - 1) && PArray[Row][Col + 1] == '.') { // Справа есть путь
		int RightGIndex = Maze[Index + 1];
		if (RightGIndex == -1) { // Поля справа нет в графе, добавляем
			Graph.push_back(Node);
			RightGIndex = Graph.size() - 1;
			Maze[Index - 1] = RightGIndex;
		}
		Graph[GIndex].push_back(RightGIndex); // Делаем ребро 
		Graph[RightGIndex].push_back(GIndex);
	}

	if (Row > 0 && PArray[Row - 1][Col] == '.') { // Сверху есть путь
		int UpGIndex = Maze[Index - C];
		if (UpGIndex == -1) { // Поля сверху нет в графе, добавляем
			Graph.push_back(Node);
			UpGIndex = Graph.size() - 1;
			Maze[Index - C] = UpGIndex;
		}
		Graph[GIndex].push_back(UpGIndex); // Делаем ребро 
		Graph[UpGIndex].push_back(GIndex);
	}

	if (Row < (R - 1) && PArray[Row + 1][Col] == '.') { // Снизу есть путь
		int DownGIndex = Maze[Index + C];
		if (DownGIndex == -1) { // Поля снизу нет в графе, добавляем
			Graph.push_back(Node);
			DownGIndex = Graph.size() - 1;
			Maze[Index + C] = DownGIndex;
		}
		Graph[GIndex].push_back(DownGIndex); // Делаем ребро 
		Graph[DownGIndex].push_back(GIndex);
	}

}

int main()
{
	cin >> R >> C >> A; cin.ignore(); // Размер карты и время сигнализации
	string Array[R];    // Символьная карта
	PArray = Array;     // и глобальный указатель на нее
	Maze.resize(R * C, -1); // Карта соответствия вершинам в  графе

	int KR; // row where Kirk is located.
	int KC; // column where Kirk is located.
	cin >> KR >> KC; cin.ignore();
	for (int i = 0; i < R; i++) {
		//            string ROW; // C of the characters in '#.TC?' (i.e. one line of the ASCII maze).
		cin >> PArray[i]; cin.ignore(); // В массив
	}

	AddNode(KR, KC);    // Начальную позицию в граф

						// game loop
	while (1) {

		// Write an action using cout. DON'T FORGET THE "<< endl"
		// To debug: cerr << "Debug messages..." << endl;

		cout << "RIGHT" << endl; // Kirk's next move (UP DOWN LEFT or RIGHT).

		cin >> KR >> KC; cin.ignore(); // Читаем данные хода
		for (int i = 0; i < R; i++) {
			cin >> PArray[i]; cin.ignore();
		}
	}
}