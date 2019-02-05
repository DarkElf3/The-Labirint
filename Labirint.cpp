#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>  // используем очередь
#include <functional> // для лямбд

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
	if (Row < 0 || Row > R - 1 || Col < 0 || Col > C - 1) return; // Проверка границ
	int Index = Row * C + Col;
	if (Maze[Index] != -1) return; // Вершина уже есть в графе
	int GIndex;
	AddGraphNode(Row, Col, Index, &GIndex);
	// Моя первая лямбда), делает ребра
	function<void(int, int, int)> MakeWay = [GIndex](int LIndex, int LRow, int LCol) {
		int LocalIndex = Maze[LIndex];
		if (LocalIndex == -1) { // Соседнего поля нет в графе, добавляем
			AddNode(LRow, LCol);
			LocalIndex = Maze[LIndex];
		}
		if (std::find(Graph[GIndex].begin(), Graph[GIndex].end(), LocalIndex)
			== Graph[GIndex].end()) { // Если ребра еще нет
			Graph[GIndex].push_back(LocalIndex);   // Делаем ребро 
			Graph[LocalIndex].push_back(GIndex);
		}
	};

	if (Col > 0 && PArray[Row][Col - 1] == '.') // Слева есть путь
		MakeWay(Index - 1, Row, Col - 1);
	if (Col < (C - 1) && PArray[Row][Col + 1] == '.') // Справа есть путь
		MakeWay(Index + 1, Row, Col + 1);
	if (Row > 0 && PArray[Row - 1][Col] == '.') // Сверху есть путь
		MakeWay(Index - C, Row - 1, Col);
	if (Row < (R - 1) && PArray[Row + 1][Col] == '.') // Снизу есть путь
		MakeWay(Index + C, Row + 1, Col);
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
	int n = Graph.size();   // число вершин в графе
	queue<int> Queue;       // очередь вершин
	vector<bool> Visited(n);// обработанные вершины
							//    vector<int> Dist(n);  // Длина пути до вершины (не нужна пока)
	vector<int> Prev(n);    // откуда пришли
	Queue.push(GPos);      // Начинаем с текущей позиции
	Visited[GPos] = true;
	Prev[GPos] = -1;
	while (!Queue.empty()) {    // Берем из очереди вершину и добавляем туда
		int Pos = Queue.front();// непосещенных соседей
		Queue.pop();
		for (int i = 0; i < Graph[Pos].size(); i++) {
			int Neighbour = Graph[Pos][i];
			if (!Visited[Neighbour]) {
				Visited[Neighbour] = true;
				Queue.push(Neighbour);
				//			    Dist[Neighbour] = Dist[Pos] + 1;
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
			PathBack.push_back(Pos);// Путь к вершине назначения Dest
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

void GetData()// Считываем  входные данные нового хода
{
	cin >> KR >> KC; cin.ignore();  // Позиция Кирка
	for (int i = 0; i < R; i++) {   // Символьная  карта
		cin >> PArray[i]; cin.ignore();
	}
}
int main()
{
	cin >> R >> C >> A; cin.ignore(); // Размер карты и время сигнализации
	string Array[R];    // Символьная карта
	PArray = Array;     // и глобальный указатель на нее
	Maze.resize(R * C, -1); // Карта соответствия вершинам в  графе
	GetData();          // Считываем  входные данные нового хода
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
		if (Escaping) continue;  // Нашли комнату управления, бежим назад кратчайшим путем
		if (!GraphInfo[GPos].visited) {
			ScanAll();              // Осматриваемся // Можно оптимизировать
			GraphInfo[GPos].visited = true; // Помечаем, что были здесь
		}
		// Лямбда для входа в комнату управления сверху, снизу, слева, справа
		function <void(string)> EnterControlRoom = [&Escaping](string S) {
			cout << S << endl;
			Escaping = true;
		};
		if (KC > 0 && PArray[KR][KC - 1] == 'C') EnterControlRoom("LEFT");
		if (KC < C - 1 && PArray[KR][KC + 1] == 'C') EnterControlRoom("RIGHT");
		if (KR > 0 && PArray[KR - 1][KC] == 'C') EnterControlRoom("UP");
		if (KR < R - 1 && PArray[KR + 1][KC] == 'C') EnterControlRoom("DOWN");
		if (Escaping) {	// Вошли в комнату управления, убегаем 
			GetData();	// Считывем данные и  добавляем позицию в граф
			AddNode(KR, KC);
			GPos = Maze[KR * C + KC];
			CrossRoads.push_back(0); // Точка возврата - позиция 0 в графе
			GoBack();             // Прокладываем кратчайший путь возврата
		}// Есть возможность подойти к контрольной комнате не открыв еще
	}     // наикратчайший путь назад. Проверка на это ухудшит читабельность
}         // if (PathBack.size() > A) 