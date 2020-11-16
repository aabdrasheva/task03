// dzzzzzz3.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <thread>
#include <mutex>

std::mutex mtx;

//стартовая функция для дочерних потоков
void fillTheMatrix(int iTread, int iTN, int** A, int**B, int size, int**C) {
	for (int k = iTread; k < size; k += iTN) {
		//вычисление элементов матрицы, стоящих в строке k
		for (int i = 0; i < size; i++) {
			C[k][i] = 0;
			for (int j = 0; j < size; j++) {
				C[k][i] += A[k][j] * B[j][i];
			}
			//протокол входа в КС: закрыть двоичный семафор
			//начало критической секции 
			mtx.lock();
			std::cout << "Thread " << iTread << ": element [" << k << "][" << i << "] = " << C[k][i] << std::endl;
			//конец критической секции
			//протокол выхода из КС: 
			mtx.unlock();
			//открыть двоичный семафор
		}
	}
}

int main()
{
	//вводимые пользователем значения размера матрицы и количества потоков
	int matrixSize;
	int threadsNum;

	std::cout << "input matrix size: ";
	std::cin >> matrixSize;
	std::cout << "input threads number: ";
	std::cin >> threadsNum;

	//добавить обработку данных


	int** A = new int* [matrixSize];
	int** B = new int* [matrixSize];
	int** resultMatrix = new int* [matrixSize];

	// заполнение массивов A и B
	for (size_t i = 0; i < matrixSize; i++)
	{
		A[i] = new int[matrixSize];
		B[i] = new int[matrixSize];
		resultMatrix[i] = new int[matrixSize];
		for (size_t j = 0; j < matrixSize; j++)
		{
			A[i][j] = rand() % 20; //числа от рандома получаются слишком большими, просто для удобства возьмем остатки от деления на 20
			B[i][j] = rand() % 20; 
			resultMatrix[i][j] = 0;
		}
	}


	std::cout << "A matrix:" << std::endl;
	//вывод первой матрицы
	for (size_t i = 0; i < matrixSize; i++)
	{
		for (size_t j = 0; j < matrixSize; j++)
		{
			std::cout << A[i][j] << "\t";
		}
		std::cout << "\n";
	}

	std::cout << "\n";
	std::cout << "B matrix:" << std::endl;
	//вывод вторрой матрицы
	for (size_t i = 0; i < matrixSize; i++)
	{
		for (size_t j = 0; j < matrixSize; j++)
		{
			std::cout << B[i][j] << "\t";
		}
		std::cout << "\n";
	}

	std::thread* thr = new std::thread[threadsNum];
	// Создание потоков

	if (matrixSize % threadsNum != 0) {
		for (size_t i = 0; i < threadsNum - 1; i++)
		{
			//в один поток пытаемся положить [matrixSize//threadsNum] строк из matrixSize строк
			thr[i] = std::thread{ fillTheMatrix, i, threadsNum, A, B, matrixSize, std::ref(resultMatrix) };
		}
		//если размерность матрицы не кратна количеству потоков => в последний поток кладем [matrixSize//threadsNum] + остаток от деления [matrixSize%threadsNum] строк
		thr[threadsNum - 1] = std::thread{ fillTheMatrix, threadsNum - 1, 1, A, B, matrixSize, std::ref(resultMatrix) };
	}
	else {
		for (size_t i = 0; i < threadsNum ; i++)
		{
			//в один поток кладем [matrixSize//threadsNum] строк из matrixSize строк
			thr[i] = std::thread{ fillTheMatrix, i, threadsNum, A, B, matrixSize, std::ref(resultMatrix) };
		}
	}

	for (int i = 0; i < threadsNum; i++) {
		thr[i].join();
	}
	delete[] thr;

	std::cout << "\n";
	std::cout << "Result matrix:" << std::endl;
	//вывод итоговой матрицы resultMatrix = A*B
	for (size_t i = 0; i < matrixSize; i++)
	{
		for (size_t j = 0; j < matrixSize; j++)
		{
			std::cout << resultMatrix[i][j] << "\t";
		}
		std::cout << "\n";
	}

	//освобождение памяти для каждого элемнта A[i]
	for (int i = 0; i < matrixSize; i++)
		delete[] A[i];
	//освобождение памяти B
	delete[] A;
	//освобождение памяти для каждого элемнта B[i]
	for (int i = 0; i < matrixSize; i++)
		delete[] B[i];
	//освобождение памяти A
	delete[] B;
	//освобождение памяти для каждого элемнта resultMatrix[i]
	for (int i = 0; i < matrixSize; i++)
		delete[] resultMatrix[i];
	//освобождение памяти resultMatrix
	delete[] resultMatrix;

	system("pause");
	return 0;
}