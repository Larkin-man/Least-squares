#ifndef LMSUnitH
#define LMSUnitH
#include <vector>
#include <cmath>
#include <algorithm>
#pragma hdrstop

// Структура LSResult
// Хранит все выходные данные аппроксимации.
struct LSResult
{
	std::vector<double> coeffs; // Вектор найденных коэффициентов полинома [a0, a1, a2...]
	double shiftX;              // Целочисленный сдвиг для защиты от переполнения
	double rmse;                // Среднеквадратичная ошибка (чем ближе к 0, тем лучше)
	double r2;                  // Коэффициент детерминации (1.0 - идеальное совпадение, 0 - плохо)
	bool success;               // Флаг успешности расчета (false, если матрица вырожденная)
};

// Класс TLeastSquares
// Содержит статические методы для расчета МНК полиномом произвольной степени.
class TLeastSquares
{
public:

	 // SolveGauss: Решает систему линейных уравнений методом исключения Гаусса.
	 // Принимает расширенную матрицу (n строк на n+1 столбцов).
	 // Возвращает true при успешном нахождении единственного решения.
	static bool SolveGauss(std::vector< std::vector<double> >& matrix, std::vector<double>& result)
	{
		int n = (int)result.size();
		for (int i = 0; i < n; i++)
      {
			// Поиск ведущего элемента (частичный выбор по столбцу) для стабильности
			int maxRow = i;
			for (int k = i + 1; k < n; k++)
				if (std::abs(matrix[k][i]) > std::abs(matrix[maxRow][i]))
               maxRow = k;
            if (maxRow != i)
               std::swap(matrix[maxRow], matrix[i]); // Перестановка строк

			// Проверка на вырожденность (определитель близок к нулю)
			if (std::abs(matrix[i][i]) < 1e-18)
				return false;

			// Прямой ход Гаусса
			for (int k = i + 1; k < n; k++)
         {
				double c = -matrix[k][i] / matrix[i][i];
				for (int j = i; j <= n; j++)
               matrix[k][j] += (i == j) ? 0 : c * matrix[i][j];
				matrix[k][i] = 0;
			}
		}
		// После того как прямой ход метода Гаусса превратил левую часть матрицы в «треугольник» (под главной диагональю стоят нули)
		// Обратный ход Гаусса
		for (int i = n - 1; i >= 0; i--)
      {
			result[i] = matrix[i][n] / matrix[i][i];
			for (int k = i - 1; k >= 0; k--)
            matrix[k][n] -= matrix[k][i] * result[i];
		}
		return true;
	}

public:
	/**
	 * Calculate: Основной метод МНК.
	 * x, y - входные координаты.
	 * degree - желаемая степень полинома (1-прямая, 2-парабола и т.д.).
	 * 1. Вычисляет оптимальный сдвиг.
	 * 2. Собирает матрицу сумм.
	 * 3. Решает СЛАУ.
	 * 4. Считает статистику точности (RMSE, R2).
	 */
	static LSResult Calculate(const std::vector<double>& x, const std::vector<double>& y, int degree)
	{
		LSResult res;
		res.success = false;
		int n = (int)x.size();

		// Базовые проверки входных данных
		if (n == 0 || n != (int)y.size() || degree < 0 || degree >= n) return res;

		// 1. Находим границы X и вычисляем целочисленный сдвиг (shiftX)
		// Это предотвращает потерю точности при возведении больших X в степени.
		double minX = x[0], maxX = x[0], sumY = 0;
		for (int i = 0; i < n; i++)
      {
			if (x[i] < minX) minX = x[i];
			if (x[i] > maxX) maxX = x[i];
			sumY += y[i];
		}
		res.shiftX = std::floor((minX + maxX) / 2.0);
		double meanY = sumY / n;

		// 2. Формируем уникальные суммы степеней X и произведений X*Y
		int m = degree + 1;
		int num_sums = 2 * degree + 1;
		std::vector<double> sum_x(num_sums, 0.0);
		std::vector<double> sum_xy(m, 0.0);
		sum_x[0] = (double)n;

		for (int i = 0; i < n; i++) {
			double xs = x[i] - res.shiftX; // Работаем с центрированным X
			double xp = 1.0;
			for (int k = 0; k < num_sums; k++)
         {
				if (k > 0) sum_x[k] += xp;
				if (k < m) sum_xy[k] += y[i] * xp;
				xp *= xs;
			}
		}

		// 3. Заполняем матрицу системы и вызываем решение
		std::vector< std::vector<double> > matrix(m, std::vector<double>(m + 1, 0.0));
		for (int i = 0; i < m; i++)
      {
			for (int j = 0; j < m; j++)
            matrix[i][j] = sum_x[i + j];
			matrix[i][m] = sum_xy[i];
		}

		res.coeffs.resize(m);
		if (!SolveGauss(matrix, res.coeffs)) return res;

		// 4. Пост-расчет статистики точности (RMSE и R-квадрат)
		double sse = 0, sst = 0;
		for (int i = 0; i < n; i++)
      {
			double yp = 0, xp = 1, xs = x[i] - res.shiftX;
			for (int j = 0; j < m; j++)
            { yp += res.coeffs[j] * xp;
            xp *= xs; }
			sse += (y[i] - yp) * (y[i] - yp);
			sst += (y[i] - meanY) * (y[i] - meanY);
		}
		res.rmse = std::sqrt(sse / n);
      res.r2 = (1.0 - sse / sst);
		//res.r2 = (sst > 1e-18) ? (1.0 - sse / sst) : 1.0;
		res.success = true;
		return res;
	}
	//---------------------------------------------------------------------------
	/**
	 * Predict: Вычисляет значение Y для любого X на основе результата Calculate.
	 * Автоматически учитывает внутренний сдвиг shiftX.
	 */
	// res - структура с коэффициентами и сдвигом после CalculateLMS
	static double Predict(const LSResult& res, double x)
	{
		if (!res.success)
         return 0;
		double y = 0, xp = 1, xs = x - res.shiftX;
		for (size_t i = 0; i < res.coeffs.size(); i++)
      {
			y += res.coeffs[i] * xp;
			xp *= xs;
		}
		return y;
	}
};

#endif
