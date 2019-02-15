#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>        // используем очередь
#include <functional>   // для лямбд
#include <assert.h>     // используется assert

using namespace std;

struct TNode {                  // Тип информации о вершинах графа
	bool Visited;               // посещалась ли
	int Row, Col;               // строка, столбец
	vector<int> Neighbours;     // соседи
};

class CPathFinder {
private:
	const int ScanRange = 2;    // ScanRange - дальность сканера
	vector<string> Array;       // Входная карта лабиринта
	vector< vector<int> > Maze; // Карта с номерами вершин графа
	int Rows;                   // number of rows.
	int Cols;                   // number of columns.
	int Alarm;                  // number of rounds between the time the alarm countdown is activated and the time the alarm goes off.
	int KirkRow;                // row where Kirk is located.
	int KirkCol;                // column where Kirk is located.
	vector<TNode> Graph;        // Граф лабиринта
	int GraphPos = 0;           // Текущая позиция в графе
	string Command;              // Команда на движение. Вынесена на случай, если нужно будет знать последний ход
	vector <int> CrossRoads,    // Список перекрестков (вершин с 3-4 соседями)
				 PathBack,      // Путь возврата (список  вершин)	
				 PathFromStart; // Текущий путь от начала (клубок ниток)
	void ScanAll();
	void AddNode(int Row, int Col);	
	int GoBackToStart();
public:
	CPathFinder(int ParamRows, int ParamCols, int ParamAlarm);
	void DoStep(int Pos);
	void GoBack();
	int SelectDestination();
	void GetData();
	bool CheckForControlRoom();
	void InitStartPosition();
#ifdef DEBUG
	void ShowScan();  // Отладочная функция. Выводит видимую область вокруг Кирка
#endif
};
		
#ifdef DEBUG
void CPathFinder::ShowScan()  // Отладочная функция. Выводит видимую область вокруг Кирка
{
	int StartRow = max(KirkRow, ScanRange) - ScanRange;
	int RowsToScan = min(KirkRow + ScanRange + 1, Rows);
	int StartCol = max(KirkCol, ScanRange) - ScanRange;
	int ColsToScan = min(KirkCol + ScanRange + 1, Cols);

	for (int i = StartRow; i < RowsToScan; i++) {
		for (int j = StartCol; j < ColsToScan; j++) {
			cerr << Maze[i][j] << ' ';
		}
		cerr << endl;
	}
}
#endif

CPathFinder::CPathFinder(int ParamRows, int ParamCols, int ParamAlarm) {
	Rows = ParamRows;
	Cols = ParamCols;
	Alarm = ParamAlarm;
	Array.resize(ParamRows);              // Символьная карта лабиринта
	Maze.resize(ParamRows);
	for (int i = 0; i < ParamRows; i++)
		Maze[i].resize(ParamCols, -1);    // Карта R x C соответствия вершинам в  графе
}

void CPathFinder::InitStartPosition() {
	AddNode(KirkRow, KirkCol);            // Начальную позицию в граф, она посещена и перекресток
	Graph[0].Visited = true;
	CrossRoads.push_back(0);
	ScanAll();                            // Осмотр всех точек в пределах видимости
}

void CPathFinder::AddNode(int Row, int Col)
// Добавляет вершину в граф. Связывает ребрами с соседними.
// Если соседей нет в графе, то они добавляются рекурсивно.
{
	assert(Row >= 0 && Row < Rows && "Неверный номер строки в CPathFinder::AddNode");    // Проверка границ
	assert(Col >= 0 && Col < Cols && "Неверный номер столбца в CPathFinder::AddNode");
	if (Maze[Row][Col] != -1) return;    // Вершина уже есть в графе

	TNode Node = { false, Row, Col };
	Graph.push_back(Node);               // добавляем вершину в граф
	int GIndex = Graph.size() - 1;
	Maze[Row][Col] = GIndex;             // отмечаем на карте

	// Моя первая лямбда), делает ребра
	function<void(int, int)> MakeLink = [GIndex, this](int LRow, int LCol) {
		int LocalIndex = Maze[LRow][LCol];
		if (LocalIndex == -1) {    // Соседнего поля нет в графе, добавляем
			AddNode(LRow, LCol);
			LocalIndex = Maze[LRow][LCol];
		}
		auto &Neighbours = Graph[GIndex].Neighbours;
		if (std::find(Neighbours.begin(), Neighbours.end(), LocalIndex)
			== Neighbours.end()) {               // Если ребра еще нет
			Neighbours.push_back(LocalIndex);    // Делаем ребро 
			Graph[LocalIndex].Neighbours.push_back(GIndex);
		}
	};

	if (Col > 0 && Array[Row][Col - 1] == '.')             // Слева есть путь
		MakeLink(Row, Col - 1);
	if (Col < (Cols - 1) && Array[Row][Col + 1] == '.')    // Справа есть путь
		MakeLink(Row, Col + 1);
	if (Row > 0 && Array[Row - 1][Col] == '.')             // Сверху есть путь
		MakeLink(Row - 1, Col);
	if (Row < (Rows - 1) && Array[Row + 1][Col] == '.')    // Снизу есть путь
		MakeLink(Row + 1, Col);
}

void CPathFinder::DoStep(int Pos)
// Делает ход от текущей вершины графа к заданной соседней
{
	Command = "Wrong Destination";    // Неправильный ход

	int Horizontal = Graph[Pos].Col - Graph[GraphPos].Col;
	if (Horizontal < 0)    // По горизонтали
		Command = "LEFT";
	else if (Horizontal> 0)
		Command = "RIGHT";

	int Vertical = Graph[Pos].Row - Graph[GraphPos].Row;
	if (Vertical < 0)      // По вертикали
		Command = "UP";
	else if (Vertical > 0)
		Command = "DOWN";

	cout << Command << endl;          // Делаем ход	
	GraphPos = Pos;                   // Текущая позиция
}

void CPathFinder::ScanAll()
// Сканирует видимую область ((2*SR+1)х(2*SR+1) вокруг Кирка) c учетом границ лабиринта
// Увиденные вершины добавляются в граф.
{
	int StartRow = max(KirkRow, ScanRange) - ScanRange;
	int RowsToScan = min(KirkRow + ScanRange + 1, Rows);
	int StartCol = max(KirkCol, ScanRange) - ScanRange;
	int ColsToScan = min(KirkCol + ScanRange + 1, Cols);

	for (int i = StartRow; i < RowsToScan; i++) {
		for (int j = StartCol; j < ColsToScan; j++) {
			if (Array[i][j] == '.') AddNode(i, j);
		}
	}
}

int CPathFinder::GoBackToStart()
// Делает путь возврата из комнаты управления в начало поиском в ширину по графу
{
	const int Dest = 0;        // Цель - стартовая позиция
	int n = Graph.size();      // число вершин в графе
	queue<int> Queue;          // очередь вершин
	vector<bool> Visited(n);   // обработанные вершины
	vector<int> Dist(n);       // Длина пути до вершины (не нужна пока)
	vector<int> Prev(n);       // откуда пришли
	Queue.push(GraphPos);      // Начинаем с текущей позиции
	Visited[GraphPos] = true;
	Prev[GraphPos] = -1;
	bool StartFound = false;

	while (!StartFound && !Queue.empty()) {    // Берем из очереди вершину и добавляем туда
		int Pos = Queue.front();               // непосещенных соседей
		Queue.pop();
		for (int Neighbour : Graph[Pos].Neighbours) {
			if (!Visited[Neighbour]) {
				Visited[Neighbour] = true;
				Queue.push(Neighbour);
				Dist[Neighbour] = Dist[Pos] + 1;
				Prev[Neighbour] = Pos;
			}
			StartFound = Neighbour == Dest;
			if (StartFound) break;
		}
	}

	assert(Visited[Dest] && "Не найден путь назад в CPathFinder::GoBackToStart()!");
	for (int Pos = Dest; Prev[Pos] != -1; Pos = Prev[Pos])
		PathBack.push_back(Pos);        // Путь к вершине назначения Dest
	return Dist[Dest];                  // Возвращаем длину пути
}

int CPathFinder::SelectDestination()    // Выбирает вершину для следующего хода
{
	int n = PathBack.size();
	if (n != 0) {                                   // Если список возврата не пуст, берем из него
		int Next = PathBack[n - 1];
		PathBack.pop_back();
		if (PathBack.empty()) CrossRoads.pop_back();
		return Next;
	}

	if (Graph[GraphPos].Neighbours.size() > 2) {    // Перекресток - добавляем
		CrossRoads.push_back(GraphPos);
	}

	for (int Neighbour : Graph[GraphPos].Neighbours)    // Ищем непосещенного соседа
	{
		int Next = Neighbour;
		if (Graph[Next].Visited) continue;
		PathFromStart.push_back(GraphPos);    // Добавляем в список от старта (разматываем нить)
		return Next;
	}    // Нет непосещенных проходов

	if (Graph[GraphPos].Neighbours.size() > 2) CrossRoads.pop_back();    // Перекресток обследован, убираем
	return -1;    // Исследовать нечего, сигнал к возврату
}

void CPathFinder::GetData()    // Считываем  входные данные нового хода
{
	cin >> KirkRow >> KirkCol; cin.ignore();    // Позиция Кирка
	for (int i = 0; i < Rows; i++) {            // Символьная  карта
		cin >> Array[i]; cin.ignore();
	}
}

bool CPathFinder::CheckForControlRoom()
{
	if (!Graph[GraphPos].Visited) {
		ScanAll();                         // Осматриваемся // Можно оптимизировать
		Graph[GraphPos].Visited = true;    // Помечаем, что были здесь
	}

	bool ControlRoomFound = false;         // Контрольную комнату не нашли
	string MoveCommand;
	// Лямбда для входа в комнату управления сверху, снизу, слева, справа
	function <void(string, int, int)> AddControlRoom = [&](string S, int LR, int LC) {
		MoveCommand = S;
		AddNode(LR, LC);
		GraphPos = Maze[LR][LC];
		ControlRoomFound = true;
	};

	if (KirkCol > 0 && Array[KirkRow][KirkCol - 1] == 'C') AddControlRoom("LEFT", KirkRow, KirkCol - 1);
	if (KirkCol < Cols - 1 && Array[KirkRow][KirkCol + 1] == 'C') AddControlRoom("RIGHT", KirkRow, KirkCol + 1);
	if (KirkRow > 0 && Array[KirkRow - 1][KirkCol] == 'C') AddControlRoom("UP", KirkRow - 1, KirkCol);
	if (KirkRow < Rows - 1 && Array[KirkRow + 1][KirkCol] == 'C') AddControlRoom("DOWN", KirkRow + 1, KirkCol);

	if (ControlRoomFound) {                       // Нашли комнату управления
		int Dist = GoBackToStart();               // Прокладываем кратчайший путь возврата 
		if (Dist > Alarm) {                       // и если он длиннее нужного, идем дальше 
			GraphPos = Maze[KirkRow][KirkCol];    // возвращаем текущую позицию
			PathBack.clear();                     // очищаем список возврата
		}
		else {
			cout << MoveCommand << endl;   // все хорошо - заходим в комнату упраления
			GetData();
			return true;
		}
	}

	return false;
}

void CPathFinder::GoBack()
// Делает путь возврата к последнему перекрестку(от тупика или исследованного перекрестка)
{
	int Dest = CrossRoads[CrossRoads.size() - 1];    // Последний перекресток
	int Index = PathFromStart.size() - 1;            // Предыдущая позиция
	int Node;

	do {
		Node = PathFromStart[Index];                 
		PathBack.push_back(Node);                    // В путь возврата заносим
		Index--;
		PathFromStart.pop_back();                    // Из пути от начала убираем (смытываем нить)
	} while (Dest != Node);

	reverse(PathBack.begin(), PathBack.end());
}

int main()
{
	int Rows, Cols, Alarm;
	cin >> Rows >> Cols >> Alarm; cin.ignore(); // Размер карты и время сигнализации
	CPathFinder PathFinder{ Rows, Cols, Alarm };
	PathFinder.GetData();
	PathFinder.InitStartPosition();
	bool Escaping = false;    // Контрольную комнату не нашли

	while (1) {
		if (!Escaping)        // Если нашли комнату управления, просто бежим назад кратчайшим путем
			Escaping = PathFinder.CheckForControlRoom();
		int Dest = PathFinder.SelectDestination();    // Выбираем, куда идти 
		if (Dest == -1) {                             // Если некуда, отступаем к развилке
			PathFinder.GoBack();
			Dest = PathFinder.SelectDestination();
		}
		PathFinder.DoStep(Dest);    // Делаем шаг
		PathFinder.GetData();       // Читаем входные данные
	}
}        