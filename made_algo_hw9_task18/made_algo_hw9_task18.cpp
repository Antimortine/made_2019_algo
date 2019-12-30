//Задача 18. Построение выпуклой оболочки (5 баллов)
//Дано множество точек на плоскости(x, y). Постройте выпуклую оболочку этого множества и вычислите ее периметр.
//Вариант 2. С помощью алгоритма Джарвиса.
//Обратите внимание : три и более точки из множества могут лежать на одной прямой.

#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>

constexpr double epsilon = 1e-15;

bool is_zero(double value)
{
	return std::abs(value) < epsilon;
}

struct Point
{
	explicit Point(double x, double y) : x(x), y(y) {};
	double x;
	double y;

	bool operator==(const Point& other) const
	{
		return is_zero(x - other.x) && is_zero(y - other.y);
	}
};

struct Vector
{
	explicit Vector(const Point& start, const Point& end)
	{
		x = end.x - start.x;
		y = end.y - start.y;
	}
	double x;
	double y;

	double length() const
	{
		return std::sqrt(x * x + y * y);
	}

	double static vector_product(const Vector& v1, const Vector& v2)
	{
		return v1.x * v2.y - v2.x * v1.y;
	}
};

// Строит выпуклую оболочку для заданного набора точек с помощью алгоритма Джарвиса
std::vector<Point> build_convex_hull(const std::vector<Point>& points)
{
	assert(points.size() > 1);
	std::vector<Point> convex_hull;

	// Находим самую нижнюю точку. Если их несколько, то берём самую левую среди них
	Point start = points[0];
	for (const Point& point : points)
	{
		if (point.y < start.y || is_zero(point.y - start.y) && point.x < start.x)
			start = point;
	}
	convex_hull.emplace_back(start);

	// Последовательно добавляем в выпуклую оболочку такие точки,
	// чтобы вектор от последней добавленной точки до новой лежал правее всех остальных точек
	// Останавливаемся, когда новой точкой оказалась стартовая
	while (true)
	{
		const Point& last_added = convex_hull.back();
		Point rightmost = points[0];
		for (size_t i = 1; i < points.size(); ++i)
		{
			const Point& current = points[i];
			Vector rightmost_vector = Vector(last_added, rightmost);
			Vector current_vector = Vector(last_added, current);

			double vector_product = Vector::vector_product(rightmost_vector, current_vector);
			// Если точки лежат на одной прямой, отбираем самую дальнюю от последней добавленной
			if (vector_product < 0 || is_zero(vector_product) && current_vector.length() > rightmost_vector.length())
				rightmost = current;
		}
		if (rightmost == start)
			break;
		convex_hull.emplace_back(rightmost);
	}
	return convex_hull;
}

double get_polygon_perimeter(const std::vector<Point>& points)
{
	assert(points.size() > 1);
	double perimeter = 0;
	for (size_t i = 1; i < points.size(); ++i)
	{
		const Point& previous = points[i - 1];
		const Point& current = points[i];
		Vector vector(previous, current);
		perimeter += vector.length();
	}
	Vector vector(points.back(), points.front());
	perimeter += vector.length();
	return perimeter;
}

int main()
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);
	std::cout.precision(9);

	size_t n = 0;
	std::cin >> n;

	std::vector<Point> points;
	points.reserve(n);

	double x = 0;
	double y = 0;

	for (size_t i = 0; i < n; ++i)
	{
		std::cin >> x >> y;
		points.emplace_back(Point(x, y));
	}

	std::vector<Point> convex_hull = build_convex_hull(points);

	double perimeter = get_polygon_perimeter(convex_hull);
	std::cout << perimeter;

	std::cout.flush();
	return 0;
}
