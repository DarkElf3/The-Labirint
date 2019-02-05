#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>  // используем очередь

using namespace std;

/**
* Auto-generated code below aims at helping you parse
* the standard input according to the problem statement.
**/
static string *PArray; // Входная карта лабиринта
static vector<int> Maze; // Карта с номерами вершин графа

static int R; // number of rows.
static int C; // number of columns.
static int A; // number of rounds between the time the alarm countdown is activated and the time the alarm goes off.

static int KR; // row where Kirk is located.
static int KC; // column where Kirk is located.

struct TNodeInfo { // Тип информации о вершинах графа
	bool visited;
	int Row, Col;
};

static vector< vector<int> > Graph; // Граф лабиринта
static vector<TNodeInfo> GraphInfo; // Информация о вершинах графа
static int GPos = 0;                // Текущая позиция в графе

/*
void ShowScan()  // Отладочная функция. Выводит видимую область (5х5 вокруг Кирка)
{
	for (int i = (KR < 2 ? 2 : KR) - 2; i < (KR + 3 > R ? R : KR + 3); i++) {
		for (int j = (KC < 2 ? 2 : KC) - 2; j <(KC + 3 > C ? C : KC + 3); j++) {
			cerr << Maze[i * C + j] << ' ';
		}
		cerr << endl;
	}

}
*/
void AddGraphNode(int Row, int Col, int Index, int *GIndex)
// Добавляет вершину с информацией в граф
// Отмечает номер вершины на карте
{
	vector<int> Node;
	Graph.push_back(Node);

	*GIndex = Graph.size() - 1;
	Maze[Index] = *GIndex;

	TNodeInfo NodeInfo = { false, Row, Col };
	GraphInfo.push_back(NodeInfo);
}

void AddNode(int Row, int Col)
// Добавляет вершину в граф. Связывает ребрами с соседними.
// Если соседей нет в графе, то они добавляются рекурсивно.
{
	if (Row < 0 || Row > R - 1 || Col < 0 || Col > C - 1) return;
	int Index = Row * C + Col;
	if (Maze[Index] != -1) return; // Вершина уже есть в графе
	int GIndex;
	AddGraphNode(Row, Col, Index, &GIndex);

	if (Col > 0 && PArray[Row][Col - 1] == '.') { // Слева есть путь
		int LeftGIndex = Maze[Index - 1];
		if (LeftGIndex == -1) { // Поля слева нет в графе, добавляем
			AddNode(Row, Col - 1);
			LeftGIndex = Maze[Index - 1];
		}
		if (std::find(Graph[GIndex].begin(), Graph[GIndex].end(), LeftGIndex)
			== Graph[GIndex].end()) { // Если ребра еще нет
			Graph[GIndex].push_back(LeftGIndex); // Делаем ребро 
			Graph[LeftGIndex].push_back(GIndex);
		}
	}

	if (Col < (C - 1) && PArray[Row][Col + 1] == '.') { // Справа есть путь
		int RightGIndex = Maze[Index + 1];
		if (RightGIndex == -1) { // Поля справа нет в графе, добавляем
			AddNode(Row, Col + 1);
			RightGIndex = Maze[Index + 1];
		}
		if (std::find(Graph[GIndex].begin(), Graph[GIndex].end(), RightGIndex)
			== Graph[GIndex].end()) { // Если ребра еще нет
			Graph[GIndex].push_back(RightGIndex); // Делаем ребро 
			Graph[RightGIndex].push_back(GIndex);
		}
	}

	if (Row > 0 && PArray[Row - 1][Col] == '.') { // Сверху есть путь
		int UpGIndex = Maze[Index - C];
		if (UpGIndex == -1) { // Поля сверху нет в графе, добавляем
			AddNode(Row - 1, Col);
			UpGIndex = Maze[Index - C];
		}
		if (std::find(Graph[GIndex].begin(), Graph[GIndex].end(), UpGIndex)
			== Graph[GIndex].end()) { // Если ребра еще нет
			Graph[GIndex].push_back(UpGIndex); // Делаем ребро 
			Graph[UpGIndex].push_back(GIndex);
		}
	}

	if (Row < (R - 1) && PArray[Row + 1][Col] == '.') { // Снизу есть путь
		int DownGIndex = Maze[Index + C];
		if (DownGIndex == -1) { // Поля снизу нет в графе, добавляем
			AddNode(Row + 1, Col);
			DownGIndex = Maze[Index + C];		
		}
		if (std::find(Graph[GIndex].begin(), Graph[GIndex].end(), DownGIndex)
			== Graph[GIndex].end()) { // Если ребра еще нет
			Graph[GIndex].push_back(DownGIndex); // Делаем ребро 
			Graph[DownGIndex].push_back(GIndex);
		}
	}

}

static char *Command; // Команда на движение. Вынесена на случай,
					  // если нужно будет знать последний ход

void DoStep(int Pos)
// Делает ход от текущей вершины графа к заданной
{
	TNodeInfo CurPosInfo = GraphInfo[GPos], ToPosInfo = GraphInfo[Pos];
	int Horizontal = ToPosInfo.Col - CurPosInfo.Col;
	Command = "Wrong Destination";  // Неправильный ход
	if (Horizontal != 0) {          // По горизонтали
		if (Horizontal < 0)
			Command = "LEFT";
		else
			Command = "RIGHT";
	}
	int Vertical = ToPosInfo.Row - CurPosInfo.Row;
	if (Vertical != 0) {            // По вертикали
		if (Vertical < 0)
			Command = "UP";
		else
			Command = "DOWN";
	}
	cout << Command << endl;        // Делаем ход
	GPos = Pos;                     // Текущая позиция
}

void ScanAll()
// Сканирует видимую область (5х5 вокруг Кирка) c учетом границ лабиринта
// Увиденные вершины добавляются в граф.
{
	for (int i = (KR < 2 ? 2 : KR) - 2; i < (KR + 3 > R ? R : KR + 3); i++) {
		for (int j = (KC < 2 ? 2 : KC) - 2; j <(KC + 3 > C ? C : KC + 3); j++) {
			if (PArray[i][j] == '.') AddNode(i, j);
		}
	}
}

vector <int> CrossRoads, // Список перекрестков (вершин с 3-4 соседями)
			 PathBack;   // Путь возврата (список  вершин)

void GoBack()
// Делает путь возврата к последнему перекрестку(от тупика,
// исследованного перекрестка, комнаты управления) поиском в ширину по графу
{
	int n = Graph.size(); // число вершин
	queue<int> q;
	q.push(GPos);
	vector<bool> used(n);
	vector<int> d(n), p(n);
	used[GPos] = true;
	p[GPos] = -1;
	while (!q.empty()) {
		int v = q.front();
		q.pop();
		for (size_t i = 0; i<Graph[v].size(); ++i) {
			int to = Graph[v][i];
			if (!used[to]) {
				used[to] = true;
				q.push(to);
				d[to] = d[v] + 1;
				p[to] = v;
			}
		}
	}

	int to = CrossRoads[CrossRoads.size() - 1];
	if (!used[to])
		cerr << "No path!";
	else {

		for (int v = to; v != -1; v = p[v])
			PathBack.push_back(v);
		PathBack.pop_back(); 
	}
}

int SelectDestination()	// Выбирает вершину для следующего хода
{
	int n = PathBack.size();
	if (n != 0) {		// Если список возврата не пуст, берем из него
		n = PathBack[n - 1];
		PathBack.pop_back();
		if (PathBack.empty()) CrossRoads.pop_back();
		return n;
	}
	if (Graph[GPos].size() > 2) {	// Перекресток - добавляем
		CrossRoads.push_back(GPos);
	}
	for (int i = 0; i < Graph[GPos].size(); i++) // Ищем непосещенного соседа
	{
		int Next = Graph[GPos][i];
		if (GraphInfo[Next].visited) continue;
		return Next;
	}  // Нет непосещенных проходов
	if (Graph[GPos].size() > 2) CrossRoads.pop_back();// Перекресток обследован, убираем
	return -1; // Исследовать нечего, сигнал к возврату
}

void GetData()
{
	cin >> KR >> KC; cin.ignore();
	for (int i = 0; i < R; i++) {
		cin >> PArray[i]; cin.ignore();
	}
}
int main()
{
	cin >> R >> C >> A; cin.ignore(); // Размер карты и время сигнализации
	string Array[R];    // Символьная карта
	PArray = Array;     // и глобальный указатель на нее
	Maze.resize(R * C, -1); // Карта соответствия вершинам в  графе

	cin >> KR >> KC; cin.ignore();
	for (int i = 0; i < R; i++) {
		//            string ROW; // C of the characters in '#.TC?' (i.e. one line of the ASCII maze).
		cin >> PArray[i]; cin.ignore(); // В массив
	}

	AddNode(KR, KC);    // Начальную позицию в граф, она посещена и перекресток
	GraphInfo[0].visited = true;
	CrossRoads.push_back(0);
	ScanAll();	        // Осмотр всех точек в пределах видимости

	bool Escaping = false; // Контрольную комнату не нашли
	// game loop
	while (1) {
		int Dest = SelectDestination(); // Выбираем, куда идти 
		if (Dest == -1) {            // Если некуда, отступаем к развилке
			GoBack();
			Dest = SelectDestination();
		}
		DoStep(Dest);               // Делаем шаг

		GetData();                  // Читаем входные данные
		if (!GraphInfo[GPos].visited) {
			ScanAll();              // Осматриваемся // Можно оптимизировать
			GraphInfo[GPos].visited = true; // Помечаем, что были здесь
		}
		if (Escaping) continue;  // Нашли комнату управления, бежим назад кратчайшим путем 
		if (KC > 0 && PArray[KR][KC - 1] == 'C') {
			cout << "LEFT" << endl;
			Escaping = true;
		}
		if (KC < C - 1 && PArray[KR][KC + 1] == 'C') {
			cout << "RIGHT" << endl;
			Escaping = true;
		}
		if (KR > 0 && PArray[KR - 1][KC] == 'C') {
			cout << "UP" << endl;
			Escaping = true;
		}
		if (KR < R - 1 && PArray[KR + 1][KC] == 'C') {
			cout << "DOWN" << endl;
			Escaping = true;
		}
		if (Escaping) {
			GetData();	// Считывем данные и  добавляем позицию в граф
			AddNode(KR, KC);
			GPos = Maze[KR * C + KC];
			CrossRoads.push_back(0); // Точка возврата - позиция 0 в графе
			GoBack();             // Прокладываем кратчайший путь возврата
		}// Есть возможность подойти к контрольной комнате не открыв еще
	}     // наикратчайший путь назад. Проверка на это ухудшит читабельность
}         // if (PathBack.size() > A) 