#pragma once
#include "Brep.h"

class EulerOp
{
public:
	EulerOp() :solidNum(0) {};
	Solid* mvfs(GLdouble*, Vertex *&,int id);
	Halfedge* mev(Loop *, Vertex*, Vertex*&, GLdouble*,int id);//v1起始点，v2新点
	Loop* mef(Vertex*, Vertex*, Loop*);//v1->v2为顺时针方向，逆右手法则
	Loop* kemr(Vertex*, Vertex*, Loop*);//v1为外环点，v2为内环点
	Solid* kfmrh(Face*, Face*);//删除f1，加入f2内环
	void sweep(Face*, GLdouble*, GLdouble, int&);

	Loop* findLoop(Solid* s, int id1, int id2, int id3);
private:
	int solidNum;

public:
	std::map<int, Solid*> solidlist;
	std::map<int, Vertex*> vertexlist;
};


