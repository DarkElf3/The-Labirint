#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>		// используем очередь
#include <functional>   // для лямбд
#include <assert.h>		// используется assert

using namespace std;

struct TNode {                  // Тип информации о вершинах графа
	bool Visited;				// посещалась ли
	int Row, Col;				// строка, столбец
	vector<int> Neighbours;		// соседи
};

class CPathFinder {
private:
	const int ScanRange = 2;	// ScanRange - дальность сканера
	vector<string> Array;		// Входная карта лабиринта
	vector< vector<int> > Maze;	// Карта с номерами вершин графа
	int Rows;					// number of rows.
	int Cols;					// number of columns.
	int Alarm;					// number of rounds between the time the alarm countdown is activated and the time the alarm goes off.
	int KirkRow;				// row where Kirk is located.
	int KirkCol;				// column where Kirk is located.
	vector<TNode> Graph;		// Граф лабиринта
	int GraphPos = 0;			// Текущая позиция в графе
	char *Command;				// Команда на движение. Вынесена на случай, если нужно будет знать последний ход
	vector <int> CrossRoads,	// Список перекрестков (вершин с 3-4 соседями)
				 PathBack;		// Путь возврата (список  вершин)	
	void ScanAll();
	void AddNode(int Row, int Col);	
public:
	CPathFinder();
	void DoStep(int Pos);
	int GoBack();
	int SelectDestination();
	void GetData();
	bool CheckForControlRoom();
#ifdef DEBUG
	void ShowScan();  // Отладочная функция. Выводит видимую область (5х5 вокруг Кирка)
#endif
};
		
#ifdef DEBUG
void CPathFinder::ShowScan()  // Отладочная функция. Выводит видимую область (5х5 вокруг Кирка)
{
	int StartRow = (KirkRow < ScanRange ? ScanRange : KirkRow) - ScanRange;
	int RowsToScan = KirkRow + ScanRange + 1 > Rows ? Rows : KirkRow + ScanRange + 1;
	int StartCol = (KirkCol < ScanRange ? ScanRange : KirkCol) - ScanRange;
	int ColsToScan = KirkCol + ScanRange + 1 > Cols ? Cols : KirkCol + ScanRange + 1;
	for (int i = StartRow; i < RowsToScan; i++) {
		for (int j = StartCol; j < ColsToScan; j++) {
			cerr << Maze[i][j] << ' ';
		}
		cerr << endl;
	}
}
#endif
CPathFinder::CPathFinder() {
	cin >> Rows >> Cols >> Alarm; cin.ignore();	// Размер карты и время сигнализации
	Array.resize(Rows);					        // Символьная карта лабиринта
	Maze.resize(Rows);
	for (int i = 0; i < Rows; i++)
		Maze[i].resize(Cols, -1);			    // Карта R x C соответствия вершинам в  графе
	GetData();							        // Считываем  входные данные нового хода
	AddNode(KirkRow, KirkCol);					// Начальную позицию в граф, она посещена и перекресток
	Graph[0].Visited = true;
	CrossRoads.push_back(0);
	ScanAll();							        // Осмотр всех точек в пределах видимости
}

void CPathFinder::AddNode(int Row, int Col)
// Добавляет вершину в граф. Связывает ребрами с соседними.
// Если соседей нет в графе, то они добавляются рекурсивно.
{
	if (Row < 0 || Row > Rows - 1 || Col < 0 || Col > Cols - 1) assert("Неверные координаты PathFinder::AddNode"); // return; // Проверка границ
	if (Maze[Row][Col] != -1) return;   // Вершина уже есть в графе
	TNode Node = { false, Row, Col };
	Graph.push_back(Node);				// добавляем вершину в граф
	int GIndex = Graph.size() - 1;
	Maze[Row][Col] = GIndex;			// отмечаем на карте

	// Моя первая лямбда), делает ребра
	function<void(int, int)> MakeWay = [GIndex, this](int LRow, int LCol) {
		int LocalIndex = Maze[LRow][LCol];
		if (LocalIndex == -1) { // Соседнего поля нет в графе, добавляем
			AddNode(LRow, LCol);
			LocalIndex = Maze[LRow][LCol];
		}
		vector<int> *Neighbours = &Graph[GIndex].Neighbours;
		if (std::find(Neighbours->begin(), Neighbours->end(), LocalIndex)
			== Neighbours->end()) {					 // Если ребра еще нет
			Neighbours->push_back(LocalIndex);		 // Делаем ребро 
			Graph[LocalIndex].Neighbours.push_back(GIndex);
		}
	};

	if (Col > 0 && Array[Row][Col - 1] == '.')          // Слева есть путь
		MakeWay(Row, Col - 1);
	if (Col < (Cols - 1) && Array[Row][Col + 1] == '.') // Справа есть путь
		MakeWay(Row, Col + 1);
	if (Row > 0 && Array[Row - 1][Col] == '.')          // Сверху есть путь
		MakeWay(Row - 1, Col);
	if (Row < (Rows - 1) && Array[Row + 1][Col] == '.') // Снизу есть путь
		MakeWay(Row + 1, Col);
}

void CPathFinder::DoStep(int Pos)
// Делает ход от текущей вершины графа к заданной
{
	int Horizontal = Graph[Pos].Col - Graph[GraphPos].Col;
	Command = "Wrong Destination";  // Неправильный ход
	if (Horizontal != 0) {          // По горизонтали
		if (Horizontal < 0)
			Command = "LEFT";
		else
			Command = "RIGHT";
	}
	int Vertical = Graph[Pos].Row - Graph[GraphPos].Row;
	if (Vertical != 0) {            // По вертикали
		if (Vertical < 0)
			Command = "UP";
		else
			Command = "DOWN";
	}
	cout << Command << endl;        // Делаем ход
	GraphPos = Pos;                 // Текущая позиция
}

void CPathFinder::ScanAll()
// Сканирует видимую область ((2*SR+1)х(2*SR+1) вокруг Кирка) c учетом границ лабиринта
// Увиденные вершины добавляются в граф.
{
	int StartRow = (KirkRow < ScanRange ? ScanRange : KirkRow) - ScanRange;
	int RowsToScan = KirkRow + ScanRange + 1 > Rows ? Rows : KirkRow + ScanRange + 1;
	int StartCol = (KirkCol < ScanRange ? ScanRange : KirkCol) - ScanRange;
	int ColsToScan = KirkCol + ScanRange + 1 > Cols ? Cols : KirkCol + ScanRange + 1;
	for (int i = StartRow; i < RowsToScan; i++) {
		for (int j = StartCol; j < ColsToScan; j++) {
			if (Array[i][j] == '.') AddNode(i, j);
		}
	}
}

int CPathFinder::GoBack()
// Делает путь возврата к последнему перекрестку(от тупика,
// исследованного перекрестка, комнаты управления) поиском в ширину по графу
{
	int n = Graph.size();      // число вершин в графе
	queue<int> Queue;          // очередь вершин
	vector<bool> Visited(n);   // обработанные вершины
	vector<int> Dist(n);       // Длина пути до вершины (не нужна пока)
	vector<int> Prev(n);       // откуда пришли
	Queue.push(GraphPos);      // Начинаем с текущей позиции
	Visited[GraphPos] = true;
	Prev[GraphPos] = -1;
	while (!Queue.empty()) {     // Берем из очереди вершину и добавляем туда
		int Pos = Queue.front(); // непосещенных соседей
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
	// Можно завернуть в лямбду и прерваться на точке назначения
	int Dest = CrossRoads[CrossRoads.size() - 1];
	if (!Visited[Dest])     // 
		cerr << "No path!";
	else {
		for (int Pos = Dest; Prev[Pos] != -1; Pos = Prev[Pos])
			PathBack.push_back(Pos);    // Путь к вершине назначения Dest
	}
	return Dist[Dest];    // Возвращаем длину пути
}

int CPathFinder::SelectDestination()	// Выбирает вершину для следующего хода
{
	int n = PathBack.size();
	if (n != 0) {		// Если список возврата не пуст, берем из него
		n = PathBack[n - 1];
		PathBack.pop_back();
		if (PathBack.empty()) CrossRoads.pop_back();
		return n;
	}
	if (Graph[GraphPos].Neighbours.size() > 2) {	// Перекресток - добавляем
		CrossRoads.push_back(GraphPos);
	}
	for (size_t i = 0; i < Graph[GraphPos].Neighbours.size(); i++) // Ищем непосещенного соседа
	{
		int Next = Graph[GraphPos].Neighbours[i];
		if (Graph[Next].Visited) continue;
		return Next;
	}  // Нет непосещенных проходов
	if (Graph[GraphPos].Neighbours.size() > 2) CrossRoads.pop_back();// Перекресток обследован, убираем
	return -1; // Исследовать нечего, сигнал к возврату
}

void CPathFinder::GetData()// Считываем  входные данные нового хода
{
	cin >> KirkRow >> KirkCol; cin.ignore();  // Позиция Кирка
	for (int i = 0; i < Rows; i++) {          // Символьная  карта
		cin >> Array[i]; cin.ignore();
	}
}

bool CPathFinder::CheckForControlRoom()
{
	if (!Graph[GraphPos].Visited) {
		ScanAll();              // Осматриваемся // Можно оптимизировать
		Graph[GraphPos].Visited = true; // Помечаем, что были здесь
	}
	bool Found = false; // Контрольную комнату не нашли
	string Command;
	// Лямбда для входа в комнату управления сверху, снизу, слева, справа
	function <void(string, int, int)> EnterControlRoom = [&Found, &Command, this](string S, int LR, int LC) {
		Command = S;
		AddNode(LR, LC);
		GraphPos = Maze[LR][LC];
		Found = true;
	};
	if (KirkCol > 0 && Array[KirkRow][KirkCol - 1] == 'C') EnterControlRoom("LEFT", KirkRow, KirkCol - 1);
	if (KirkCol < Cols - 1 && Array[KirkRow][KirkCol + 1] == 'C') EnterControlRoom("RIGHT", KirkRow, KirkCol + 1);
	if (KirkRow > 0 && Array[KirkRow - 1][KirkCol] == 'C') EnterControlRoom("UP", KirkRow - 1, KirkCol);
	if (KirkRow < Rows - 1 && Array[KirkRow + 1][KirkCol] == 'C') EnterControlRoom("DOWN", KirkRow + 1, KirkCol);
	if (Found) {	// Нашли комнату управления
		CrossRoads.push_back(0);		            // Точка возврата - позиция 0 в графе
		int Dist = GoBack();			            // Прокладываем кратчайший путь возврата 
		if (Dist > Alarm) {					        // и если он длиннее нужного, идем дальше 
			GraphPos = Maze[KirkRow][KirkCol];		// возвращаем текущую позицию
			PathBack.clear();			            // очищаем список возврата
			CrossRoads.pop_back();		            // убирает точку возврата
		}
		else {
			cout << Command << endl;	// все хорошо - заходим в комнату упраления
			GetData();
			return true;
		}
	}
	return false;
}

int main()
{
	CPathFinder PathFinder;
	bool Escaping = false; // Контрольную комнату не нашли
	while (1) {
		if (!Escaping)  // Если нашли комнату управления, просто бежим назад кратчайшим путем
			Escaping = PathFinder.CheckForControlRoom();
		int Dest = PathFinder.SelectDestination(); // Выбираем, куда идти 
		if (Dest == -1) {                          // Если некуда, отступаем к развилке
			PathFinder.GoBack();
			Dest = PathFinder.SelectDestination();
		}
		PathFinder.DoStep(Dest);               // Делаем шаг
		PathFinder.GetData();                  // Читаем входные данные
		
	}
}        