#include <iostream>
#include <math.h> // Para utilização do valor "e^x"
#include <omp.h> // Para paralelização OpenMP
#include <chrono> // Para cáculo de tempo de processamento

using namespace std;
using  ns = chrono::milliseconds;
using get_time = chrono::steady_clock;

int nprocs, nthreads, tid;

// A variável test define qual a função a ser utilizada, 
// poupando código em funções repetitivas para teste.
// Para os seguintes valores de test:
// test = 0 : e^x dx
// test = 1 : sqrt(1 - x^2) dx
// test = 2 : e^(-x^2) dx

// Retorna resultado da função definida pela variável test
float Funcao(float x, int test)
{
	switch (test)
	{
	case 0:
		return exp(x);
		break;
	case 1:
		return sqrt(1 - pow(x, 2));
		break;
	case 2:
		return exp(-pow(x, 2));
		break;
	default:
		return 0;
		break;
	}

}

// Retorna resultado da derivada 2 definida pela variável test
float Derivada2(float x, int test)
{
	switch (test)
	{
	case 0:
		return exp(x);
		break;
	case 1:
		return 1 / pow((1 - pow(x, 2)), 3 / 2);
		break;
	case 2:
		return -2 * (exp(-pow(x, 2)) * x);
		break;
	default:
		return 0;
		break;
	}
}

// Retorna resultado da derivada 4 definida pela variável test
float Derivada4(float x, int test)
{
	switch (test)
	{
	case 0:
		return exp(x);
		break;
	case 1:
		return (12 * pow(x, 2)) + 3 / pow((1 - pow(x, 2)), 7 / 2);
		break;
	case 2:
		return 4 * exp(-pow(x, 2)) * (4 * pow(x, 4) - 12 * pow(x, 2) + 3);
		break;
	default:
		return 0;
		break;
	}
}

// Retorna o resultado pelo método Trapézio
float trapezium(float a, float b, int test)
{
	return (b - a) * ((Funcao(a, test) + Funcao(b, test)) / 2);
}

// Retorna o erro do método Trapézio
float trapeziumError(float a, float b, int test)
{
	return -((pow((b - a), 3) / 12) * Derivada2(a + ((b - a) / 2), test));
}

// Retorna o erro composto do método Trapézio
float trapeziumCompositeError(float a, float b, int n, int test)
{
	return -((pow((b - a), 3) / (12 * pow(n, 2))) * Derivada2(a + ((b - a) / 2), test));
}

// Retorna o resultado do método Simpson
float simpson(float a, float b, int test)
{
	return (b - a) * ((Funcao(a, test) + (4 * Funcao((a + b) / 2, test)) + Funcao(b, test)) / 6);
}

// Retorna o erro do método Simpson
float simpsonError(float a, float b, int test)
{
	return -((pow((b - a), 5) / 2880) * Derivada4(a + ((b - a) / 2), test));
}

// Retorna o erro composto do método Simpson
float simpsonCompsiteError(float a, float b, int n, int test)
{
	return -((pow((b - a), 5) / (180 * pow(n, 4))) * Derivada4(a + ((b - a) / 2), test));
}


int main()
{
	nprocs = omp_get_num_procs();
	// nprocs = 1;
	cout << "Number of processes: " << nprocs << endl << endl;

	omp_set_num_threads(nprocs);

	float a = 0;
	float b = 1;
	int segments = 10000000;					// Define a quantidade de segmentos para processar
	float interval = (b - a) / segments;	// Define o intervalo dos segmentos para processar

	cout << "Segment Count: " << segments << endl;
	cout << "Testing Interval: " << interval << endl << endl;

	// Teste das funções com o intervalo [0,1]
	cout << "Testing functions on step [0,1]:" << endl << endl;

	// Loop principal
	for (int i = 0; i < 3; i++)
	{
		double metodo1 = 0;
		double metodo2 = 0;
		auto startTime = get_time::now();
		

#pragma omp parallel for reduction (+:metodo1)
		for (int j = 1; j <= segments; j++)
		{
			metodo1 += trapezium((j - 1) * interval, j * interval, i);
			
		}

#pragma omp parallel for reduction (+:metodo2)
		for (int j = 1; j <= segments; j++)
		{
			metodo2 += simpson((j - 1) * interval, j * interval, i);
		}
		
		auto endTime = get_time::now();
		auto totalTime = endTime - startTime;

		float erro1 = trapeziumCompositeError(a, b, segments, i);
		float erro2 = simpsonCompsiteError(a, b, segments, i);

		switch (i)
		{
		case 0:
			cout << "Function: e^x dx" << endl << endl;
			break;
		case 1:
			cout << "Function: sqrt(1 - x^2) dx" << endl << endl;
			break;
		case 2:
			cout << "Function: e^(-x^2) dx" << endl << endl;
			break;
		default:
			break;
		}

		cout << "Trapezium result:   " << metodo1 << " | error: " << erro1 << endl;
		cout << "Simpson result:     " << metodo2 << " | error: " << erro2 << endl;
		cout << "Calculation time: " << chrono::duration_cast<ns>(totalTime).count() << " ms" << endl << endl;

	}
	cin.get();
}