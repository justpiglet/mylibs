#include "tools.h"

bool _CANNP_NAME::lineIntersection::IsInLinePart(const Point& pi, const Point& pj, const Point& pk, bool check /*= false*/)
{
	bool intersect = true;
	if (check)
		intersect =IsEqualFloat((pi.y - pk.y)*(pj.x - pk.x), (pj.y - pk.y)*(pi.x - pk.x));

	if (intersect)
		return (_CANNP_NAME::_min(pi.x, pj.x) <= pk.x&&pk.x <= _CANNP_NAME::_max(pi.x, pj.x) && _CANNP_NAME::_min(pi.y, pj.y) <= pk.y&&pk.y <= _CANNP_NAME::_max(pi.y, pj.y));
	else
		return false;
}

bool _CANNP_NAME::lineIntersection::AreLinesOverlap(const Point& pi, const Point& pj, const Point& pk, const Point& pl)
{
	bool intersect = IsEqualFloat((pi.y - pk.y)*(pj.x - pk.x), (pj.y - pk.y)*(pi.x - pk.x));
	if (intersect)
		return (IsInLinePart(pi, pj, pk) || IsInLinePart(pi, pj, pl) || IsInLinePart(pk, pl, pi) || IsInLinePart(pk, pl, pj));
	else
		return false;
}
DECIMALS _CANNP_NAME::lineIntersection::Determinant(DECIMALS v1, DECIMALS v2, DECIMALS v3, DECIMALS v4)  //  行列式 ###
{
	return (v1*v4 - v2*v3);
}

bool _CANNP_NAME::lineIntersection::IsIntersection(const Point& aa, const Point& bb, const Point& cc, const Point& dd)
{
	DECIMALS delta = Determinant(bb.x - aa.x, cc.x - dd.x, bb.y - aa.y, cc.y - dd.y);
	if (IsZero( delta ))  // delta=0 表示两线段重合或平行  ##
	{
		return AreLinesOverlap(aa, bb, cc, dd);
	}
	double namenda = Determinant(cc.x - aa.x, cc.x - dd.x, cc.y - aa.y, cc.y - dd.y) / delta;
	if (namenda > 1 || namenda < 0)
	{
		return false;
	}
	double miu = Determinant(bb.x - aa.x, cc.x - aa.x, bb.y - aa.y, cc.y - aa.y) / delta;
	if (miu>1 || miu < 0)
	{
		return false;
	}
	return true;
}



int8 _CANNP_NAME::lineIntersection::JudgeDecimals(DECIMALS d)
{
	if (_abs(d) < EPS)
		return 0;
	else 
		return (d > 0) ? 1 : -1;
}

DECIMALS _CANNP_NAME::lineIntersection::CrossProduct(const Point &A, const Point &B, const Point &C)
{
	return (B.x - A.x) * (C.y - A.y) - (B.y - A.y) * (C.x - A.x);
}


uint8 _CANNP_NAME::lineIntersection::IntersectionPos(const Point &a, const Point &b, const Point &c, const Point &d, Point &p)
{
	DECIMALS crossVal[4];
	int8 decimals[4];
	const Point* pos[4][3] = { { &a, &b, &c }, { &a, &b, &d }, { &c, &d, &a }, { &c, &d, &b } };

	for (int i = 0; i < 4; ++i)
	{
		decimals[i] = JudgeDecimals(crossVal[i] = CrossProduct(*pos[i][0], *pos[i][1], *pos[i][2]));
		if (decimals[i] == 0 && IsInLinePart(*pos[i][0], *pos[i][1], *pos[i][2],true))
		{
			p = *pos[i][2];
			return 2;
		}
	}
	
	//判断规范相交 交点不会在端点上####
	if ((decimals[0] ^ decimals[1]) == -2 && (decimals[2] ^ decimals[3]) == -2)
	{
		p.x = (c.x * crossVal[1] - d.x * crossVal[0]) / (crossVal[1] - crossVal[0]);
		p.y = (c.y * crossVal[1] - d.y * crossVal[0]) / (crossVal[1] - crossVal[0]);
		return 1;
	}
	else
		return 0;
}

bool _CANNP_NAME::lineIntersection::isLineInterCircle(const Point& ptStart, const Point& ptEnd, const Point& ptCenter, const DECIMALS Radius)
{
	uint8 nResult(0);

	DECIMALS fDis = _sqrt((ptEnd.x - ptStart.x) * (ptEnd.x - ptStart.x) + (ptEnd.y - ptStart.y) * (ptEnd.y - ptStart.y));

	Point d;
	d.x = (ptEnd.x - ptStart.x) / fDis;
	d.y = (ptEnd.y - ptStart.y) / fDis;

	Point E;
	E.x = ptCenter.x - ptStart.x;
	E.y = ptCenter.y - ptStart.y;

	DECIMALS a = E.x * d.x + E.y * d.y;
	DECIMALS a2 = a * a;

	DECIMALS e2 = E.x * E.x + E.y * E.y;

	DECIMALS r2 = Radius * Radius;

	if ((r2 - e2 + a2) < 0)
	{
		return false;
	}
	else
	{
		DECIMALS f = _sqrt(r2 - e2 + a2);
		DECIMALS t[] = { a - f, a + f };
		nResult = 0;
		for (uint8 i = 0; i < 2; ++i)
		{
			if (t[i]>-EPS && (t[i] - fDis) < EPS)
			{
				++nResult;
				break;
			}
		}
		
		return nResult>0;
	}
}
/**
* @brief 求线段与圆的交点
* @return 如果有交点返回n个交点
* @note 与圆可能存在两个交点，如果存在两个交点在ptInter1和ptInter2都为有效值，如果有一个交点，则ptInter2的值为
*       无效值，此处为65536.0###
*/
uint8 _CANNP_NAME::lineIntersection::LineInterCirclePos(
	const Point& ptStart, // 线段起点  ##
	const Point& ptEnd, // 线段终点  ##
	const Point& ptCenter, // 圆心坐标 ## 
	const DECIMALS Radius,
	Point& ptInter1,
	Point& ptInter2)
{
	uint8 nResult(0);
	ptInter1.x = ptInter2.x = 65535.0f;
	ptInter1.y = ptInter2.y = 65535.0f;

	DECIMALS fDis = _sqrt((ptEnd.x - ptStart.x) * (ptEnd.x - ptStart.x) + (ptEnd.y - ptStart.y) * (ptEnd.y - ptStart.y));

	Point d;
	d.x = (ptEnd.x - ptStart.x) / fDis;
	d.y = (ptEnd.y - ptStart.y) / fDis;

	Point E;
	E.x = ptCenter.x - ptStart.x;
	E.y = ptCenter.y - ptStart.y;

	DECIMALS a = E.x * d.x + E.y * d.y;
	DECIMALS a2 = a * a;

	DECIMALS e2 = E.x * E.x + E.y * E.y;

	DECIMALS r2 = Radius * Radius;

	if ((r2 - e2 + a2) < 0)
	{
		return nResult;
	}
	else
	{
		DECIMALS f = _sqrt(r2 - e2 + a2);
		DECIMALS t[] = { a - f, a + f };
		Point* pt[2] = { &ptInter1, &ptInter2 };
		nResult = 0;
		for (uint8 i = 0; i < 2; ++i)
		{
			if (t[i]>-EPS && (t[i] - fDis) < EPS)
			{
				pt[nResult]->x = ptStart.x + t[i] * d.x;
				pt[nResult++]->y = ptStart.y + t[i] * d.y;
			}
		}
		if (IsEqualFloat(f, 0.0f, EPS_LOW))
			nResult = 1;

		return nResult;
	}
}

DECIMALS _CANNP_NAME::lineIntersection::DistanceTwoPoints(const Point& a, const Point& b)
{
	return _sqrt((b.x - a.x)*(b.x - a.x) + (b.y - a.y)*(b.y - a.y));
}
bool _CANNP_NAME::lineIntersection::IsVaildDistance2Points(const Point& a, const Point& b, const DECIMALS& vdis)
{
	DECIMALS dis = DistanceTwoPoints(a, b);
	return IsEqualFloat(dis, vdis) || dis < vdis;
}