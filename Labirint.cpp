#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>		// ���������� �������
#include <functional>   // ��� �����
#include <assert.h>		// ������������ assert

using namespace std;

struct TNode { // ��� ���������� � �������� �����
	bool visited;				// ���������� ��
	int Row, Col;				// ������, �������
	vector<int> Neighbours;		// ������
};

class PathFinder {
private:
	const int SR = 2;			// ScanRange - ��������� �������
	vector<string> Array;		// ������� ����� ���������
	vector< vector<int> > Maze;	// ����� � �������� ������ �����
	int R;						// number of rows.
	int C;						// number of columns.
	int A;						// number of rounds between the time the alarm countdown is activated and the time the alarm goes off.
	int KR;						// row where Kirk is located.
	int KC;						// column where Kirk is located.
	vector<TNode> Graph;		// ���� ���������
	int GPos = 0;				// ������� ������� � �����
	char *Command;				// ������� �� ��������. �������� �� ������, ���� ����� ����� ����� ��������� ���
	vector <int> CrossRoads,	// ������ ������������ (������ � 3-4 ��������)
				 PathBack;		// ���� �������� (������  ������)	
	void ScanAll();
	void AddNode(int Row, int Col);	
public:
	PathFinder();
	void DoStep(int Pos);
	int GoBack();
	int SelectDestination();
	void GetData();
	bool CheckForControlRoom();
#ifdef DEBUG
	void ShowScan();  // ���������� �������. ������� ������� ������� (5�5 ������ �����)
#endif
};
		
#ifdef DEBUG
void PathFinder::ShowScan()  // ���������� �������. ������� ������� ������� (5�5 ������ �����)
{
	for (int i = (KR < 2 ? 2 : KR) - 2; i < (KR + 3 > R ? R : KR + 3); i++) {
		for (int j = (KC < 2 ? 2 : KC) - 2; j <(KC + 3 > C ? C : KC + 3); j++) {
			cerr << Maze[i][j] << ' ';
		}
		cerr << endl;
	}
}
#endif
PathFinder::PathFinder() {
	cin >> R >> C >> A; cin.ignore();	// ������ ����� � ����� ������������
	Array.resize(R);					// ���������� ����� ���������
	Maze.resize(R);
	for (int i = 0; i < R; i++)
		Maze[i].resize(C, -1);			// ����� R x C ������������ �������� �  �����
	GetData();							// ���������  ������� ������ ������ ����
	AddNode(KR, KC);					// ��������� ������� � ����, ��� �������� � �����������
	Graph[0].visited = true;
	CrossRoads.push_back(0);
	ScanAll();							// ������ ���� ����� � �������� ���������
}

void PathFinder::AddNode(int Row, int Col)
// ��������� ������� � ����. ��������� ������� � ���������.
// ���� ������� ��� � �����, �� ��� ����������� ����������.
{
	if (Row < 0 || Row > R - 1 || Col < 0 || Col > C - 1) assert("�������� ���������� PathFinder::AddNode"); // return; // �������� ������
	if (Maze[Row][Col] != -1) return; // ������� ��� ���� � �����
	TNode Node = { false, Row, Col };
	Graph.push_back(Node);				// ��������� ������� � ����
	int GIndex = Graph.size() - 1;
	Maze[Row][Col] = GIndex;			// �������� �� �����

	// ��� ������ ������), ������ �����
	function<void(int, int)> MakeWay = [GIndex, this](int LRow, int LCol) {
		int LocalIndex = Maze[LRow][LCol];
		if (LocalIndex == -1) { // ��������� ���� ��� � �����, ���������
			AddNode(LRow, LCol);
			LocalIndex = Maze[LRow][LCol];
		}
		vector<int> *Neighbours = &Graph[GIndex].Neighbours;
		if (std::find(Neighbours->begin(), Neighbours->end(), LocalIndex)
			== Neighbours->end()) {					// ���� ����� ��� ���
			Neighbours->push_back(LocalIndex);		 // ������ ����� 
			Graph[LocalIndex].Neighbours.push_back(GIndex);
		}
	};

	if (Col > 0 && Array[Row][Col - 1] == '.') // ����� ���� ����
		MakeWay(Row, Col - 1);
	if (Col < (C - 1) && Array[Row][Col + 1] == '.') // ������ ���� ����
		MakeWay(Row, Col + 1);
	if (Row > 0 && Array[Row - 1][Col] == '.') // ������ ���� ����
		MakeWay(Row - 1, Col);
	if (Row < (R - 1) && Array[Row + 1][Col] == '.') // ����� ���� ����
		MakeWay(Row + 1, Col);
}

void PathFinder::DoStep(int Pos)
// ������ ��� �� ������� ������� ����� � ��������
{
	int Horizontal = Graph[Pos].Col - Graph[GPos].Col;
	Command = "Wrong Destination";  // ������������ ���
	if (Horizontal != 0) {          // �� �����������
		if (Horizontal < 0)
			Command = "LEFT";
		else
			Command = "RIGHT";
	}
	int Vertical = Graph[Pos].Row - Graph[GPos].Row;
	if (Vertical != 0) {            // �� ���������
		if (Vertical < 0)
			Command = "UP";
		else
			Command = "DOWN";
	}
	cout << Command << endl;        // ������ ���
	GPos = Pos;                     // ������� �������
}

void PathFinder::ScanAll()
// ��������� ������� ������� ((2*SR+1)�(2*SR+1) ������ �����) c ������ ������ ���������
// ��������� ������� ����������� � ����.
{
	for (int i = (KR < SR ? SR : KR) - SR; i < (KR + SR  + 1 > R ? R : KR + SR + 1); i++) {
		for (int j = (KC < SR ? SR : KC) - SR; j <(KC + SR + 1 > C ? C : KC + SR + 1); j++) {
			if (Array[i][j] == '.') AddNode(i, j);
		}
	}
}

int PathFinder::GoBack()
// ������ ���� �������� � ���������� �����������(�� ������,
// �������������� �����������, ������� ����������) ������� � ������ �� �����
{
	int n = Graph.size();   // ����� ������ � �����
	queue<int> Queue;       // ������� ������
	vector<bool> Visited(n);// ������������ �������
	vector<int> Dist(n);  // ����� ���� �� ������� (�� ����� ����)
	vector<int> Prev(n);    // ������ ������
	Queue.push(GPos);      // �������� � ������� �������
	Visited[GPos] = true;
	Prev[GPos] = -1;
	while (!Queue.empty()) {    // ����� �� ������� ������� � ��������� ����
		int Pos = Queue.front();// ������������ �������
		Queue.pop();
		for (size_t i = 0; i < Graph[Pos].Neighbours.size(); i++) {
			int Neighbour = Graph[Pos].Neighbours[i];
			if (!Visited[Neighbour]) {
				Visited[Neighbour] = true;
				Queue.push(Neighbour);
				Dist[Neighbour] = Dist[Pos] + 1;
				Prev[Neighbour] = Pos;
			}
		}
	}
	// ����� ��������� � ������ � ���������� �� ����� ����������
	int Dest = CrossRoads[CrossRoads.size() - 1];
	if (!Visited[Dest])     // 
		cerr << "No path!";
	else {
		for (int Pos = Dest; Prev[Pos] != -1; Pos = Prev[Pos])
			PathBack.push_back(Pos);// ���� � ������� ���������� Dest
	}
	return Dist[Dest]; // ���������� ����� ����
}

int PathFinder::SelectDestination()	// �������� ������� ��� ���������� ����
{
	int n = PathBack.size();
	if (n != 0) {		// ���� ������ �������� �� ����, ����� �� ����
		n = PathBack[n - 1];
		PathBack.pop_back();
		if (PathBack.empty()) CrossRoads.pop_back();
		return n;
	}
	if (Graph[GPos].Neighbours.size() > 2) {	// ����������� - ���������
		CrossRoads.push_back(GPos);
	}
	for (size_t i = 0; i < Graph[GPos].Neighbours.size(); i++) // ���� ������������� ������
	{
		int Next = Graph[GPos].Neighbours[i];
		if (Graph[Next].visited) continue;
		return Next;
	}  // ��� ������������ ��������
	if (Graph[GPos].Neighbours.size() > 2) CrossRoads.pop_back();// ����������� ����������, �������
	return -1; // ����������� ������, ������ � ��������
}

void PathFinder::GetData()// ���������  ������� ������ ������ ����
{
	cin >> KR >> KC; cin.ignore();  // ������� �����
	for (int i = 0; i < R; i++) {   // ����������  �����
		cin >> Array[i]; cin.ignore();
	}
}

bool PathFinder::CheckForControlRoom()
{
	if (!Graph[GPos].visited) {
		ScanAll();              // ������������� // ����� ��������������
		Graph[GPos].visited = true; // ��������, ��� ���� �����
	}
	bool Found = false; // ����������� ������� �� �����
	string Command;
	// ������ ��� ����� � ������� ���������� ������, �����, �����, ������
	function <void(string, int, int)> EnterControlRoom = [&Found, &Command, this](string S, int LR, int LC) {
		Command = S;
		AddNode(LR, LC);
		GPos = Maze[LR][LC];
		Found = true;
	};
	if (KC > 0 && Array[KR][KC - 1] == 'C') EnterControlRoom("LEFT", KR, KC - 1);
	if (KC < C - 1 && Array[KR][KC + 1] == 'C') EnterControlRoom("RIGHT", KR, KC + 1);
	if (KR > 0 && Array[KR - 1][KC] == 'C') EnterControlRoom("UP", KR - 1, KC);
	if (KR < R - 1 && Array[KR + 1][KC] == 'C') EnterControlRoom("DOWN", KR + 1, KC);
	if (Found) {	// ����� ������� ����������
		CrossRoads.push_back(0);		// ����� �������� - ������� 0 � �����
		int Dist = GoBack();			// ������������ ���������� ���� �������� 
		if (Dist > A) {					// � ���� �� ������� �������, ���� ������ 
			GPos = Maze[KR][KC];		// ���������� ������� �������
			PathBack.clear();			// ������� ������ ��������
			CrossRoads.pop_back();		// ������� ����� ��������
		}
		else {
			cout << Command << endl;	// ��� ������ - ������� � ������� ���������
			GetData();
			return true;
		}
	}
	return false;
}

int main()
{
	PathFinder pf;
	bool Escaping = false; // ����������� ������� �� �����
	while (1) {
		if (!Escaping)  // ���� ����� ������� ����������, ������ ����� ����� ���������� �����
			Escaping = pf.CheckForControlRoom();
		int Dest = pf.SelectDestination(); // ��������, ���� ���� 
		if (Dest == -1) {            // ���� ������, ��������� � ��������
			pf.GoBack();
			Dest = pf.SelectDestination();
		}
		pf.DoStep(Dest);               // ������ ���
		pf.GetData();                  // ������ ������� ������
		
	}
}        