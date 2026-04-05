#ifndef ChebyshevUnitH
#define ChebyshevUnitH

#include <vector>
#include <cmath>
#include <algorithm>
#include <vcl.h>
#include <float.h> // Для DBL_EPSILON
#pragma hdrstop

enum TFitQuality
{
    fqExact,      // Идеальное совпадение (машинный ноль)
    fqNoise,      // Микро-шум (потеря точности double)
    fqApprox,     // Аппроксимация (есть реальное отклонение)
    fqInaccurate  // Плохая точность (ошибка > 1%)
};

typedef double ApproxType;

// Структура для хранения всех результатов аппроксимации
struct TChebResult
{
	std::vector<double> coeffs;	// Коэффициенты в базисе Чебышёва
	double minX, maxX;			// Границы для нормализации
	double rmse;				// Среднеквадратичная ошибка
	double r2;					// Коэффициент детерминации (0..1)
   TFitQuality quality;    // Оценка результата
	bool success;				// Флаг успешности
};

class TChebyshevLMS
{
public:
	// Решение СЛАУ методом Гаусса с выбором ведущего элемента
	static bool SolveGauss(std::vector< std::vector<ApproxType> >& matrix, std::vector<double>& result)
	{
      int n = (int)result.size();
      if (n == 0 || (int)matrix.size() < n || (int)matrix[0].size() < n + 1)
        return false; // Защита от неправильных размеров
		for (int i = 0; i < n; i++)
		{
			int maxRow = i;
			for (int k = i + 1; k < n; k++)
				if (std::abs(matrix[k][i]) > std::abs(matrix[maxRow][i])) 
					maxRow = k;
			std::swap(matrix[maxRow], matrix[i]);
			if (std::abs(matrix[i][i]) < 1e-18)
				return false;
			for (int k = i + 1; k < n; k++)
			{
				double c = -matrix[k][i] / matrix[i][i];
				for (int j = i; j <= n; j++)
					matrix[k][j] += (i == j) ? 0 : c * matrix[i][j];
				matrix[k][i] = 0;
			}
		}
		for (int i = n - 1; i >= 0; i--)
		{
			result[i] = matrix[i][n] / matrix[i][i];
			for (int k = i - 1; k >= 0; k--)
            matrix[k][n] -= matrix[k][i] * result[i];
		}
		return true;
	}

	// Основной расчет МНК по Чебышёву
   static void Calculate(const std::vector<double>& x, const std::vector<double>& y, int degree, TChebResult &res)
	{
		//TChebResult res;
		res.success = false;
		int n = (int)x.size();
		int m = degree + 1;
		if (n < m || degree < 0)
			return;

		// 1. Поиск границ для нормализации x в [-1, 1]
		res.minX = x[0]; res.maxX = x[0];
		double sumY = 0;
		for (int i = 0; i < n; i++)
		{
			if (x[i] < res.minX) res.minX = x[i];
			if (x[i] > res.maxX) res.maxX = x[i];
			sumY += y[i];
		}
		double range = res.maxX - res.minX;
		if (range < 1e-18)
			return;

		// 2. Сбор матрицы сумм Ti(x)*Tj(x)
		std::vector< std::vector<double> > matrix(m, std::vector<double>(m + 1, 0.0));
		std::vector<double> basis(m);

		for (int k = 0; k < n; k++)
		{
			double x_norm = (2.0 * x[k] - (res.maxX + res.minX)) / range;
			// Рекуррентное вычисление базиса Чебышёва (без pow)
			basis[0] = 1.0;
			if (m > 1) basis[1] = x_norm;
			for (int i = 2; i < m; i++)
            basis[i] = 2.0 * x_norm * basis[i - 1] - basis[i - 2];

			for (int i = 0; i < m; i++)
			{
				for (int j = 0; j < m; j++) matrix[i][j] += basis[i] * basis[j];
				matrix[i][m] += y[k] * basis[i];
			}
		}

		res.coeffs.resize(m);
		if (!SolveGauss(matrix, res.coeffs))
			return;

		// 3. Расчет статистики (исправленный)
      double sse = 0, sst = 0, meanY = sumY / n;
      for (int k = 0; k < n; k++)
      {
         // Считаем значение полинома прямо на месте, используя нормализованный x
         double x_norm = (2.0 * x[k] - (res.maxX + res.minX)) / (res.maxX - res.minX);
         double yp = 0, t0 = 1.0, t1 = x_norm;

         for (int i = 0; i < m; i++)
         {
            if (i == 0)
               yp += res.coeffs[i] * t0;
            else if (i == 1)
               yp += res.coeffs[i] * t1;
            else
            {
               double t2 = 2.0 * x_norm * t1 - t0;
               yp += res.coeffs[i] * t2;
               t0 = t1; t1 = t2;
            }
         }

          sse += (y[k] - yp) * (y[k] - yp);
          sst += (y[k] - meanY) * (y[k] - meanY);
      }
      res.rmse = std::sqrt(sse / n);
      // Определяем качество на основе машинного эпсилона
      if (res.rmse <= DBL_EPSILON * 10.0)
          res.quality = fqExact;
      else if (res.rmse < 1e-11)
          res.quality = fqNoise;
      else if (res.rmse < 0.1)
          res.quality = fqApprox;
      else
          res.quality = fqInaccurate;
      res.r2 = (1.0 - sse / sst);
      //res.r2 = (sst > 1e-18) ? (1.0 - sse / sst) : 1.0;
      res.success = true;
	}

	// Прогноз Y для любого X
	static double Predict(const TChebResult& res, double x)
	{
		if (!res.success) return 0;
		double x_norm = (2.0 * x - (res.maxX + res.minX)) / (res.maxX - res.minX);
		int m = (int)res.coeffs.size();
		double y = 0, t_prev = 1.0, t_curr = x_norm;
		for (int i = 0; i < m; i++)
		{
			if (i == 0)
				y += res.coeffs[i] * t_prev;
			else if (i == 1)
				y += res.coeffs[i] * t_curr;
			else
			{
				double t_next = 2.0 * x_norm * t_curr - t_prev;
				y += res.coeffs[i] * t_next;
				t_prev = t_curr; t_curr = t_next;
			}
		}
		return y;
	}

	// Пересчет в стандартные коэффициенты a0 + a1*x + a2*x^2...
	static std::vector<double> GetStandardCoeffs(const TChebResult& res) 
	{
		int m = (int)res.coeffs.size();
		std::vector<double> finalA(m, 0.0);
		if (!res.success) return finalA;

		double A = 2.0 / (res.maxX - res.minX);
		double B = -(res.maxX + res.minX) / (res.maxX - res.minX);
		std::vector< std::vector<double> > poly(m, std::vector<double>(m, 0.0));

		poly[0][0] = 1.0;
		if (m > 1) 
			{ poly[1][0] = B; poly[1][1] = A; }
		for (int i = 2; i < m; i++) 
		{
			for (int j = 0; j < i; j++) 
				poly[i][j] += 2.0 * B * poly[i - 1][j];
			for (int j = 0; j < i; j++) 
				poly[i][j + 1] += 2.0 * A * poly[i - 1][j];
			for (int j = 0; j < i - 1; j++) 
				poly[i][j] -= poly[i - 2][j];
		}
		for (int i = 0; i < m; i++)
			for (int j = 0; j <= i; j++) 
				finalA[j] += res.coeffs[i] * poly[i][j];
		return finalA;
	}

	// Формирование строки уравнения для вывода
	static String GetFormulaString(const TChebResult& res)
	{
		std::vector<double> a = GetStandardCoeffs(res);
		String s = "y = ";
		for (int i = 0; i < (int)a.size(); i++) 
		{
			if (std::abs(a[i]) < 1e-20) 
				continue;
			if (i > 0)
				s += (a[i] >= 0) ? " + " : " - ";
			else if (a[i] < 0) 
				s += "-";
			s += FloatToStrF(std::abs(a[i]), ffFixed, 15, 12);
			if (i > 0)
            if (i == 1)
               s += " * x";
            else
               s += " * x^" + IntToStr(i);
				//s += (i == 1) ? " * x" : " * x^" + IntToStr(i);
		}
		return s;
	}
};

#endif