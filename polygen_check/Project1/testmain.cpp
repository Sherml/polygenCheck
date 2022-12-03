#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
//#include "func.h"
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

bool Exclusion(Point p1,Point p2,Point q1, Point q2)//快速排斥
{
	bool s = min(p1.x, p2.x) <= max(q1.x, q2.x) && min(q1.x, q2.x) <= max(p1.x, p2.x) &&
		min(p1.y, p2.y) <= max(q1.y, q2.y) && min(q1.y, q2.y) <= max(p1.y, p2.y);
	return s;
}

double mult(Point p0, Point p1, Point p2)  //叉积计算,p0为公用节点
{
	return (p0.x - p1.x) * (p0.y - p2.y) - (p0.y - p1.y) * (p0.x - p2.x);
}

bool Cross(Point& p1, Point& p2, Point& q1, Point& q2)//跨立实验
{
	if (mult(q1, p1, q2) * mult(q1, q2, p2) <= 0) return false;
	if (mult(p1, q1, p2) * mult(p1, p2, q2) <= 0) return false;
	return true;
}

int Location(Segment &s1, Segment &s2)
{
	if (Exclusion(s1.p1,s1.p2,s2.p1,s2.p2))
	{
		if (Cross(s1.p1,s1.p2,s2.p1,s2.p2)==1)//通过跨立，代表有正常交点了
		{
			return 4;//有非顶点的交点必相交
		}
		//除了跨立相交情况，还会有相离相含，以及奇怪的相交
	}
	return 0;
}

int Ray(const vector <Point>& polygon, Point pt) //射线法 1在边上，2在外面，3在里面
{
	int Num = polygon.size();
	Point p1, p2;
	int Count = 0;
	for (int i = 0; i < Num; i++)
	{
		p1 = polygon[i];
		p2 = polygon[(i + 1) % Num];
		if ((pt.x == p1.x && pt.y == p1.y) || (pt.x == p2.x && pt.y == p2.y)) return 1;
		if (pt.y >= min(p1.y, p2.y) && pt.y <= max(p1.y, p2.y))
		{
			if (p2.x == p1.x)//此时垂直
			{
				if (pt.x < p1.x)  Count++;
				else if (pt.x == p1.x) return 1;
			}
			else if (p2.y == p1.y)//平行
			{
				if (((pt.x >= p1.x) && (pt.x <= p2.x)) || ((pt.x >= p2.x) && (pt.x <= p1.x))) return 1;
				else if (pt.x < min(p1.x, p2.x))
				{
					int c, d, e, f;
					e = f = 0;
					c = (i + Num - 1) % Num;
					d = (i + 1) % Num;
					while (polygon[c].y == pt.y)
					{
						c--;
						c = (c + Num) % Num;
					}
					if (polygon[c].y > 0) e = 1;
					else e = -1;
					while (polygon[d].y == pt.y)
					{
						d++;
						d = d % Num;
					}
					if (polygon[d].y > 0) f = 1;
					else f = -1;
					if (e != f)
					{
						Count++;
					}
				}
			}
			else
			{
				double a = (p2.y - p1.y) / (p2.x - p1.x);
				double b = p1.x + a * (pt.x - p1.x);
				if (pt.x == b)	    return 1;
				else if (pt.x < b)	Count++;
			}
		}
	}
	return Count % 2 ? 3 : 2;
}

//判断多边形1所有点与多边形2关系
void Pointtype(const vector<Point>& polygon1, const vector<Point>& polygon2, vector <int>& num1)
{
	int i;
	vector <int> a;
	for (i = 0; i < polygon1.size(); i++)	// 判断1点对图形2的位置关系
	{
		num1.push_back(Ray(polygon2, polygon1[i]));
	}
}
//判断线段和多边形关系,0相交，1重合，2在外面，3在里面
void Segmenttype(const vector<Point>& polygon1, const vector <Point>& polygon2, vector <int>& num1, vector <int>& num2)
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
		if (((p == 1 || p == 2) && q == 2 ) || ((q == 1 || q == 2 ) && p == 2))
		{
			num2.push_back(2);
		}
		if (((p == 1 || p == 3) && q == 3 ) || ((q == 1 || q == 3) && p == 3))
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
int Circletype(const vector<Point>& polygon1, const vector<Point>& polygon2, vector <int>& num1, vector <int>& num2)
{
	Segmenttype(polygon1, polygon2, num1, num2);
	int i = num2.size();
	int j, o, p, q;
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
	else              return 1;
}

//得知两多边形互相关系后判断 1:1在2里 2: 2在1里 3： 它们相离  4 它们相交 5:重合
int last(int num1, int num2)
{
	if (num1 == 4 || num2 == 4) return 4;
	if (num1 == 1 && num2 == 1) return 5;
	if (num1 == 1 && num2 == 2) return 1;
	if (num1 == 1 && num2 == 3) return 2;
	if (num2 == 1 && num1 == 2) return 2;
	if (num2 == 1 && num1 == 3) return 1;
	if (num1 == 2 && num2 == 2) return 3;
	if (num1 == 2 && num2 == 3) return 2;
	if (num1 == 3 && num2 == 2) return 1;
	if (num1 == 3 && num2 == 3) return 4;
}

int func(const vector<Point>& polygon1, const vector<Point>& polygon2)
{
	int i,j,o,m;
	for (i = 0; i < polygon1.size(); i++)
	{
		Segment1.p1 = polygon1[i];
		Segment1.p2 = polygon1[(i + 1)%polygon1.size()];
		for (j = 0; j < polygon2.size(); j++)
		{
			Segment2.p1 = polygon2[j];
			Segment2.p2 = polygon2[(j + 1)%polygon2.size()];	

			o=Location(Segment1,Segment2);
			if (o == 4)
			{
				return 4;
			}
		}	
	}
	vector <int> num11, num12, num21, num22;
	i = Circletype(polygon1, polygon2, num11, num12);
	j = Circletype(polygon2, polygon1, num21, num22);
	o = last(i, j);
	return o;
}
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

TEST_CASE("testing Polygon") {

    vector<Point>   polygon1 ={{0,0}, {100, 0}, {100,100}, {0, 100} };
    vector<Point>   polygon2;

    polygon2 = {{10,10}, {20, 10}, {20,40} };
    CHECK( func(polygon1,  polygon2)  == 2) ;
    CHECK( func(polygon2,  polygon1)  == 1) ;

    polygon2 = {{10, 0}, {20, 0}, {20, 30} };
    CHECK( func(polygon1,  polygon2)  == 2) ;
    CHECK( func(polygon2,  polygon1)  == 1) ;


    polygon2 = {{80, 0}, {100, 20}, {50, 50} };
    CHECK( func(polygon1,  polygon2)  == 2) ;
    CHECK( func(polygon2,  polygon1)  == 1) ;


    polygon2 = {{0, 50}, {50, 100}, {100, 50}, {50, 0} };
    CHECK( func(polygon1,  polygon2)  == 2) ;
    CHECK( func(polygon2,  polygon1)  == 1) ;


    polygon2 = {{0, 0},   {100, 0}, {100, 100},   {0, 100} , {0, 60}, {10, 60}, {10, 50}, {0, 50}};
   CHECK( func(polygon1,  polygon2)  == 2) ;
   CHECK( func(polygon2,  polygon1)  == 1) ;


    polygon2 = {{0, 0},   {100, 0}, {100, 100},   {0, 100} , {0, 60}, {10, 10}, {0, 50}};
    CHECK( func(polygon1,  polygon2)  == 2) ;
    CHECK( func(polygon2,  polygon1)  == 1) ;


    polygon2 = {{0, 0}, {100, 0}, {100, 10}, {0, 10} };
    CHECK( func(polygon1,  polygon2)  == 2) ;
    CHECK( func(polygon2,  polygon1)  == 1) ;

    polygon2 = {{0, 0}, {0, 10}, {100, 10}, {100, 0} };
    CHECK( func(polygon1,  polygon2)  == 2) ;
    CHECK( func(polygon2,  polygon1)  == 1) ;


    polygon2 = {{0, 0}, {0, 10}, {10, 0} };
    CHECK( func(polygon1,  polygon2)  == 2) ;
    CHECK( func(polygon2,  polygon1)  == 1) ;


    polygon2 = {{0, 0},   {100, 0}, {100, 100},   {0, 100} };
    CHECK( func(polygon1,  polygon2)  == 5) ;
    CHECK( func(polygon2,  polygon1)  == 5) ;

    polygon2 = {{0,0}, {10,0}, {20,0},  {100, 0}, {100, 50},  {100,100}, {0, 100},  {0, 50} };
    CHECK( func(polygon1,  polygon2)  == 5) ;
    CHECK( func(polygon2,  polygon1)  == 5) ;

    polygon2 = {{50, 100},   {0, 100},  {0, 70}, {0,30}, {0,0}, {60, 0} , {100, 0}, {100,40}, {100, 80}, {100, 90} , {100, 100}};
    CHECK( func(polygon1,  polygon2)  == 5) ;
    CHECK( func(polygon2,  polygon1)  == 5) ;

    polygon2 = {{50, 100},   {100, 100},  {100, 70}, {100,30}, {100,0}, {60, 0} , {30, 0}, {1,0}, {0, 0}, {0, 90} , {0, 100}};
    CHECK( func(polygon1,  polygon2)  == 5) ;
    CHECK( func(polygon2,  polygon1)  == 5) ;

    polygon2 = {{0, 0}, {0, -2}, {-1, -1} };
    CHECK( func(polygon1,  polygon2)  == 3) ;
    CHECK( func(polygon2,  polygon1)  == 3) ;

    polygon2 = {{100, 0}, {0, 0}, {-5, 0} };
    CHECK( func(polygon1,  polygon2)  == 3) ;
    CHECK( func(polygon2,  polygon1)  == 3) ;

    polygon2 = {{100, 0}, {0, 0}, {0, 100}, {-10, -10} };
    CHECK( func(polygon1,  polygon2)  == 3) ;
    CHECK( func(polygon2,  polygon1)  == 3) ;


    polygon2 = {{100, 0}, {0, 0}, {0, 100}, {-10, -10} };
    CHECK( func(polygon1,  polygon2)  == 3) ;
    CHECK( func(polygon2,  polygon1)  == 3) ;

    polygon2 = {{0, 100}, {0, 0}, {100, 0}, {100, 100}, {120, 100}, {120, -10}, {-10, -10}, {-10, 100}};
    CHECK( func(polygon1,  polygon2)  == 3) ;
    CHECK( func(polygon2,  polygon1)  == 3) ;

    polygon2 = {{0, 100}, {0, 0}, {100, 0}, {100, 100}, {120, 100}, {120, -10}, {-10, -10}, {-10, 100}};
    CHECK( func(polygon1,  polygon2)  == 3) ;
    CHECK( func(polygon2,  polygon1)  == 3) ;

    polygon2 = {{50, 0}, {50, -20}, {150, -20}, {150, 150}, {100, 150}, {100, 120}, {110, 120}, {120, -10}, { 80,-10}, {80, 0} };
    CHECK( func(polygon1,  polygon2)  == 3) ;
    CHECK( func(polygon2,  polygon1)  == 3) ;


    polygon2 = {{100, 0}, {110, 0}, {110, 10}, {100, 10} };
    CHECK( func(polygon1,  polygon2)  == 3) ;
    CHECK( func(polygon2,  polygon1)  == 3) ;

    polygon2 = {{110,120}, {110, 130}, {120,140} };
    CHECK( func(polygon1,  polygon2)  == 3) ;
    CHECK( func(polygon2,  polygon1)  == 3) ;

    polygon2 = {{0, 100}, {0, 0}, {100, 0}, {100, 100}, {100, 110}, {120, 110}, {120, -10},
                {-10, -10}, {-10, 140}, {120, 140}, {120, 130}, {0, 130}};
    CHECK( func(polygon1,  polygon2)  == 3) ;
    CHECK( func(polygon2,  polygon1)  == 3) ;

    polygon2 = {{-10, -10}, {-10, 110}, {110, 110}, {110, -20}, {-40, -20}, {-40, 90}, {-50, 90},
                {-50, -60}, {120, -60}, {120, 140}, {-15, 140}, {-15, -10}};
    CHECK( func(polygon1,  polygon2)  == 3) ;
    CHECK( func(polygon2,  polygon1)  == 3) ;

    polygon2 = {{50, 50}, {0, 50}, {0, 100}, {-10, 100}, {-10, -10}, {0, -10}, {0, 0}, {50, 0}  };
    CHECK( func(polygon1,  polygon2)  == 4) ;
    CHECK( func(polygon2,  polygon1)  == 4) ;

    polygon2 = {{90, 90}, {90, -10}, {110, -10}, {110, 90} };
    CHECK( func(polygon1,  polygon2)  == 4) ;
    CHECK( func(polygon2,  polygon1)  == 4) ;

    polygon2 = {{0, 0},   {100, 0}, {100, 100},   {0, 100} , {0, 60}, {55, 55}, {-5, 50}, {0, 50}};
    CHECK( func(polygon1,  polygon2)  == 4) ;
    CHECK( func(polygon2,  polygon1)  == 4) ;

    polygon2 = {{-5, 50}, {50, 105}, {105, 50}, {50, -5} };
    CHECK( func(polygon1,  polygon2)  == 4) ;
    CHECK( func(polygon2,  polygon1)  == 4) ;

    polygon2 = {{0, 50}, {100, 50}, {100, -20}, {0, -20} };
    CHECK( func(polygon1,  polygon2)  == 4) ;
    CHECK( func(polygon2,  polygon1)  == 4) ;

    polygon2 = {{0, 50}, {100, 50}, {100,0}, {100, -20},  {0, -20}, {0, 0} };
    CHECK( func(polygon1,  polygon2)  == 4) ;
    CHECK( func(polygon2,  polygon1)  == 4) ;

}
