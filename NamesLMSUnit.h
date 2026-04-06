#ifndef NamesLMSUnitH
#define NamesLMSUnitH

#include <vector>
#include <cmath>
#include <algorithm>
#include <vcl.h>

// Структура для хранения строк вместо входных Х
struct TPersonEntry
{
	UnicodeString Name;
	double Y;
	double X; // Рассчитанный X [-1, 1]
};

// Структура итогового результата
struct TFinalResult
{
	std::vector<TPersonEntry> SortedList;
	std::vector<double> PolyCoeffs; // Коэффициенты a0, a1... для y = a0 + a1*x + ...
	double RMSE;
	bool Success;
};

class TNameCalculator
{
private:
	// Сортировка по Y для создания плавной (монотонной) функции
	static bool CompareY(const TPersonEntry& a, const TPersonEntry& b)
	{
		return a.Y < b.Y;
	}

	// Решение СЛАУ (Метод Гаусса)
	static bool SolveGauss(std::vector< std::vector<double> >& matrix, std::vector<double>& res)
	{
		int n = (int)res.size();
		for (int i = 0; i < n; i++)
		{
			int maxR = i;
			for (int k = i + 1; k < n; k++)
				if (std::abs(matrix[k][i]) > std::abs(matrix[maxR][i])) maxR = k;
			std::swap(matrix[maxR], matrix[i]);
			if (std::abs(matrix[i][i]) < 1e-22)
				return false;

			for (int k = i + 1; k < n; k++)
			{
				double c = -matrix[k][i] / matrix[i][i];
				for (int j = i; j <= n; j++) matrix[k][j] += (i == j) ? 0 : c * matrix[i][j];
			}
		}
		for (int i = n - 1; i >= 0; i--)
		{
			res[i] = matrix[i][n] / matrix[i][i];
			for (int k = i - 1; k >= 0; k--) matrix[k][n] -= matrix[k][i] * res[i];
		}
		return true;
	}

public:
	static TFinalResult Run(std::vector<UnicodeString> names, std::vector<double> values, int degree)
	{
		TFinalResult res;
		res.Success = false;
		int n = (int)names.size();
		if (n <= degree || degree < 0)
			return res;

		// 1. Автоматическое создание списка и сортировка по Y
		for (int i = 0; i < n; i++)
		{
			TPersonEntry entry;
			entry.Name = names[i];
			entry.Y = values[i];
			res.SortedList.push_back(entry);
		}
		std::sort(res.SortedList.begin(), res.SortedList.end(), CompareY);

		// 2. Назначение уникальных X в диапазоне [-1, 1]
		std::vector<double> vx(n), vy(n);
		for (int i = 0; i < n; i++)
		{
			res.SortedList[i].X = -1.0 + (2.0 * i) / (double)(n - 1);
			vx[i] = res.SortedList[i].X;
			vy[i] = res.SortedList[i].Y;
		}

		// 3. Расчет МНК через базис Чебышёва
		int m = degree + 1;
		std::vector< std::vector<double> > matrix(m, std::vector<double>(m + 1, 0.0));
		std::vector<double> basis(m);

		for (int k = 0; k < n; k++)
		{
			basis[0] = 1.0;
			if (m > 1) basis[1] = vx[k];
			for (int i = 2; i < m; i++) basis[i] = 2.0 * vx[k] * basis[i - 1] - basis[i - 2];

			for (int i = 0; i < m; i++)
			{
				for (int j = 0; j < m; j++) matrix[i][j] += basis[i] * basis[j];
				matrix[i][m] += vy[k] * basis[i];
			}
		}

		std::vector<double> chebCoeffs(m);
		if (!SolveGauss(matrix, chebCoeffs))
			return res;

		// 4. Раскрытие Чебышёва в стандартный полином (a0 + a1*x + a2*x^2...)
		// Т.к. x уже в [-1, 1], трансформация A и B не нужна (A=1, B=0)
		std::vector< std::vector<double> > poly(m, std::vector<double>(m, 0.0));
		poly[0][0] = 1.0;
		if (m > 1) poly[1][1] = 1.0;
		for (int i = 2; i < m; i++)
		{
			for (int j = 0; j < i; j++) poly[i][j + 1] += 2.0 * poly[i - 1][j]; // 2 * x * T(i-1)
			for (int j = 0; j < i - 1; j++) poly[i][j] -= poly[i - 2][j];      // - T(i-2)
		}

		res.PolyCoeffs.assign(m, 0.0);
		for (int i = 0; i < m; i++)
			for (int j = 0; j <= i; j++) res.PolyCoeffs[j] += chebCoeffs[i] * poly[i][j];

		// 5. Проверка точности (RMSE)
		double sse = 0;
		for (int k = 0; k < n; k++)
		{
			double yp = 0, xp = 1.0;
			for (int j = 0; j < m; j++)
				{ yp += res.PolyCoeffs[j] * xp; xp *= vx[k]; }
			sse += (vy[k] - yp) * (vy[k] - yp);
		}
		res.RMSE = std::sqrt(sse / n);
		res.Success = true;
		return res;
	}
};

#endif