//point 
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct Point
{
	double x;
	double y;
};

struct Segment
{
	Point p1, p2;
}Segment1, Segment2;

int Ray(const vector <Point>& polygon, Point pt) //射线法 1在边上，2在外面，3在里面
{
	int Num = polygon.size();
	Point p1, p2;
	int Count = 0;
	for (int i = 0; i < (Num - 1); i++)
	{
		p1 = polygon[i];
		p2 = polygon[i + 1];
		if (((pt.y >= p1.y) && (pt.y <= p2.y)) || ((pt.y >= p2.y) && (pt.y <= p1.y)))
		{
			double a = (pt.y - p1.y) / (p2.y - p1.y);
			double b = p1.x + a * (p2.x - p1.x);
			if (pt.x == b)
				return 1;
			if (pt.x > b)
				Count++;
		}
	}
	return Count % 2 ? 3 : 2;
}

//判断多边形1所有点与多边形2关系
void Pointtype(const vector<Point>& polygon1, const vector<Point>& polygon2, vector <int> num1)
{
	int i;
	vector <int> a;
	for (i = 0; i < polygon1.size(); i++)	// 判断1点对图形2的位置关系
	{
		num1.push_back(Ray(polygon2, polygon1[i]));
	}
}
//判断线段和多边形关系,0相交，1重合，2在外面，3在里面
void Segmenttype(const vector<Point>& polygon1,const vector <Point>& polygon2, vector <int> num1, vector <int> num2)
{
	Pointtype(polygon1, polygon2, num1);
	int i = num1.size();
	int j, p, q;
	for (j = 0; j <= (i - 1); j++)
	{
		p = num1[j];
		q = num1[(j + 1) % i];
		if ((p == 2 && q == 3) || (p == 3 && q == 2))
		{
			num2.push_back(0);
		}
		if ((p == (1 || 2) && q == 2) || (q == (1 || 2) && p == 2))
		{
			num2.push_back(2);
		}
		if ((p == (1 || 3) && q == 3) || (q == (1 || 3) && p == 3))
		{
			num2.push_back(3);
		}
		if (p == 1 && q == 1)//存在特殊情况，要么需重新判断此时中点位置以判断方向，甚至需要判断线段是否有交点才行
		//但是也可以去判断另外一个图形中的线段和第一个图形的位置关系以去除奇怪相交点为1图形点的情况，因此此处不做更为细致的判断
		{
			Point point0;
			point0.x = (polygon1[j].x + polygon1[(j + 1) % i].x) / 2;
			point0.y = (polygon1[j].y + polygon1[(j + 1) % i].y) / 2;
			int o = Ray(polygon2, point0);
			num2.push_back(o);
		}
	}
}
//判断多边形1对于多边形2的关系,4相交，1重合，2在外面，3在里面
int Circletype(const vector<Point>& polygon1,const vector<Point>& polygon2, vector <int> num1, vector <int> num2)
{
	Segmenttype(polygon1, polygon2, num1, num2);
	int i = num2.size();
	int j,o,p,q;
	o = p = q = 0;
	for (j = 0; j < i; j++)
	{
		if (num2[j] == 0)    return 4;
		if (num2[j] == 1)
		{
			o++;
		}
		if (num2[j] == 2)
		{
			p++;
		}
		if (num2[j] == 3)
		{
			q++;
		}
	}
	if ((p > 0) && (q > 0)) return 4;
	else if (p > 0)     return 2;
	else if (q > 0)     return 3;
	else             return 1;
}

//得知两多边形互相关系后判断 1:1在2里 2: 2在1里 3： 它们相离  4 它们相交 5:重合
int last(int num1, int num2)
{
	if (num1 == 4 || num2 == 4) return 4;
	if (num1 == 1 && num2 == 1) return 5;
	if (num1 == 2 && num2 == 3) return 2;
	if (num1 == 3 && num2 == 2) return 1;
	if (num1 == 2 && num2 == 2) return 3;

}

int func(const vector<Point>& polygon1, const vector<Point>& polygon2)
{
	vector <int> num11, num12, num21, num22;
	int i = Circletype(polygon1, polygon2, num11, num12);
	int j = Circletype(polygon2, polygon1, num21, num22);
	int o = last(i, j);
	return o;
}