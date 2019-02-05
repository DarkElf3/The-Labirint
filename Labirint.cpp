#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
* Auto-generated code below aims at helping you parse
* the standard input according to the problem statement.
**/
static string *PArray; // ������� ����� ���������
static vector<int> Maze; // ����� � �������� ������ �����
static vector< vector<int> > Graph; // ���� ���������
static int R; // number of rows.
static int C; // number of columns.
static int A; // number of rounds between the time the alarm countdown is activated and the time the alarm goes off.

void AddNode(int Row, int Col)
// ��������� ������� � ����. ��������� ������� � ���������.
// ���� ������� ��� � �����, �� ��� ����������� ����������.
{
	int Index = Row * R + Col;
	if (Maze[Index] != -1) return;
	vector<int> Node;
	Graph.push_back(Node);
	int GIndex = Graph.size() - 1;
	Maze[Index] = GIndex;

	if (Col > 0 && PArray[Row][Col - 1] == '.') { // ����� ���� ����
		int LeftGIndex = Maze[Index - 1];
		if (LeftGIndex == -1) { // ���� ����� ��� � �����, ���������
			Graph.push_back(Node);
			LeftGIndex = Graph.size() - 1;
			Maze[Index - 1] = LeftGIndex;
		}
		Graph[GIndex].push_back(LeftGIndex); // ������ ����� 
		Graph[LeftGIndex].push_back(GIndex);
	}

	if (Col < (C - 1) && PArray[Row][Col + 1] == '.') { // ������ ���� ����
		int RightGIndex = Maze[Index + 1];
		if (RightGIndex == -1) { // ���� ������ ��� � �����, ���������
			Graph.push_back(Node);
			RightGIndex = Graph.size() - 1;
			Maze[Index - 1] = RightGIndex;
		}
		Graph[GIndex].push_back(RightGIndex); // ������ ����� 
		Graph[RightGIndex].push_back(GIndex);
	}

	if (Row > 0 && PArray[Row - 1][Col] == '.') { // ������ ���� ����
		int UpGIndex = Maze[Index - C];
		if (UpGIndex == -1) { // ���� ������ ��� � �����, ���������
			Graph.push_back(Node);
			UpGIndex = Graph.size() - 1;
			Maze[Index - C] = UpGIndex;
		}
		Graph[GIndex].push_back(UpGIndex); // ������ ����� 
		Graph[UpGIndex].push_back(GIndex);
	}

	if (Row < (R - 1) && PArray[Row + 1][Col] == '.') { // ����� ���� ����
		int DownGIndex = Maze[Index + C];
		if (DownGIndex == -1) { // ���� ����� ��� � �����, ���������
			Graph.push_back(Node);
			DownGIndex = Graph.size() - 1;
			Maze[Index + C] = DownGIndex;
		}
		Graph[GIndex].push_back(DownGIndex); // ������ ����� 
		Graph[DownGIndex].push_back(GIndex);
	}

}

int main()
{
	cin >> R >> C >> A; cin.ignore(); // ������ ����� � ����� ������������
	string Array[R];    // ���������� �����
	PArray = Array;     // � ���������� ��������� �� ���
	Maze.resize(R * C, -1); // ����� ������������ �������� �  �����

	int KR; // row where Kirk is located.
	int KC; // column where Kirk is located.
	cin >> KR >> KC; cin.ignore();
	for (int i = 0; i < R; i++) {
		//            string ROW; // C of the characters in '#.TC?' (i.e. one line of the ASCII maze).
		cin >> PArray[i]; cin.ignore(); // � ������
	}

	AddNode(KR, KC);    // ��������� ������� � ����

						// game loop
	while (1) {

		// Write an action using cout. DON'T FORGET THE "<< endl"
		// To debug: cerr << "Debug messages..." << endl;

		cout << "RIGHT" << endl; // Kirk's next move (UP DOWN LEFT or RIGHT).

		cin >> KR >> KC; cin.ignore(); // ������ ������ ����
		for (int i = 0; i < R; i++) {
			cin >> PArray[i]; cin.ignore();
		}
	}
}