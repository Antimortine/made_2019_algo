// Задача 19. Поиск точек в прямоугольнике (5 баллов)
// На вход подаются точки и прямоугольники.
// Точка задается двумя координатами (x, y).
// Прямоугольник - четверкой чисел [left, bottom, right, top].
// Точка (left, bottom) - принадлежит прямоугольнику, точка(right, top) - нет.
// (left < right, bottom < top)
// Для каждого прямоугольника нужно вывести, сколько добавленных точек он содержит.
//	X  в диапазоне[-180, 180)
//	Y - [-90, 90)
//	Количество точек <= 100000, Количество прямоугольников <= 1000
//	Для решения задачи необходимо реализовать алгоритм “Geohashing”.

#include <cassert>
#include <iostream>
#include <vector>
#include <memory>

constexpr double lon_min = -180;
constexpr double lon_max = 180;
constexpr double lat_min = -90;
constexpr double lat_max = 90;

struct Point
{
	double lon;
	double lat;
};

struct Bbox
{
	double left;
	double bottom;
	double right;
	double top;

	bool contains(const Point& point) const
	{
		return left <= point.lon && bottom <= point.lat &&
			point.lon < right && point.lat < top;
	}
};

class GeoHashTree
{
public:
	explicit GeoHashTree(std::vector<Point>& points);

	int count_points_in_bbox(const Bbox& bbox);

private:
	struct Node
	{
		Point point;
		std::unique_ptr<Node> left;
		std::unique_ptr<Node> right;

		explicit Node(const Point& point) : point(point) {}
	};

	Point dummy{ 1e6, 1e6 };
	std::unique_ptr<Node> root;

	void add_point(
		const Point& point,
		const std::unique_ptr<Node>& node,
		double lon_left,
		double lon_right,
		double lat_lower,
		double lat_upper,
		bool split_by_longitude);

	static int count_points_in_bbox(
		const Bbox& bbox,
		const std::unique_ptr<Node>& node,
		double lon_left,
		double lon_right,
		double lat_lower,
		double lat_upper,
		bool split_by_longitude);
};

GeoHashTree::GeoHashTree(std::vector<Point>& points)
{
	root = std::make_unique<Node>(dummy);
	for (const Point& point : points)
		add_point(point, root, lon_min, lon_max, lat_min, lat_max, true);
}

void GeoHashTree::add_point(
	const Point& point,
	const std::unique_ptr<GeoHashTree::Node>& node,
	double lon_left,
	double lon_right,
	double lat_lower,
	double lat_upper,
	bool split_by_longitude)
{
	if (split_by_longitude)
	{
		double mid = (lon_left + lon_right) / 2;
		if (point.lon < mid)
		{
			if (node->left)
				add_point(point, node->left, lon_left, mid, lat_lower, lat_upper, !split_by_longitude);
			else
				node->left = std::make_unique<Node>(point);
		}
		else
		{
			if (node->right)
				add_point(point, node->right, mid, lon_right, lat_lower, lat_upper, !split_by_longitude);
			else
				node->right = std::make_unique<Node>(point);
		}
	}
	else
	{
		double mid = (lat_lower + lat_upper) / 2;
		if (point.lat < mid)
		{
			if (node->left)
				add_point(point, node->left, lon_left, lon_right, lat_lower, mid, !split_by_longitude);
			else
				node->left = std::make_unique<Node>(point);
		}
		else
		{
			if (node->right)
				add_point(point, node->right, lon_left, lon_right, mid, lat_upper, !split_by_longitude);
			else
				node->right = std::make_unique<Node>(point);
		}
	}
}

int GeoHashTree::count_points_in_bbox(
	const Bbox& bbox,
	const std::unique_ptr<Node>& node,
	double lon_left,
	double lon_right,
	double lat_lower,
	double lat_upper,
	bool split_by_longitude)
{
	int count = 0;
	if (bbox.contains(node->point))
		++count;
	if (split_by_longitude)
	{
		double mid = (lon_left + lon_right) / 2;
		if (node->left && bbox.left < mid)
			count += count_points_in_bbox(bbox, node->left, lon_left, mid, lat_lower, lat_upper, !split_by_longitude);

		if (node->right && mid < bbox.right)
			count += count_points_in_bbox(bbox, node->right, mid, lon_right, lat_lower, lat_upper, !split_by_longitude);
	}
	else
	{
		double mid = (lat_lower + lat_upper) / 2;
		if (node->left && bbox.bottom < mid)
			count += count_points_in_bbox(bbox, node->left, lon_left, lon_right, lat_lower, mid, !split_by_longitude);

		if (node->right && mid < bbox.top)
			count += count_points_in_bbox(bbox, node->right, lon_left, lon_right, mid, lat_upper, !split_by_longitude);
	}
	return count;
}

int GeoHashTree::count_points_in_bbox(const Bbox& bbox)
{
	return count_points_in_bbox(bbox, root, lon_min, lon_max, lat_min, lat_max, true);
}

int main()
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	size_t n_points = 0;
	std::cin >> n_points;

	std::vector<Point> points;
	points.reserve(n_points);

	for (size_t i = 0; i < n_points; ++i)
	{
		double lon, lat;
		std::cin >> lon >> lat;
		points.emplace_back(Point{ lon, lat });
	}

	GeoHashTree tree(points);

	size_t n_bboxes = 0;
	std::cin >> n_bboxes;

	for (size_t i = 0; i < n_bboxes; ++i)
	{
		double left;
		double bottom;
		double right;
		double top;
		std::cin >> left >> bottom >> right >> top;
		Bbox bbox{ left, bottom, right, top };

		size_t points_count = tree.count_points_in_bbox(bbox);
		std::cout << points_count << "\n";
	}

	std::cout.flush();
	return 0;
}
