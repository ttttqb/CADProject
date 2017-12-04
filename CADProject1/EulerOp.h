#pragma once
#include "Brep.h"

class EulerOp
{
public:
	EulerOp() :solidNum(0) {};
	Solid* mvfs(GLdouble*, Vertex *&,int id);
	Halfedge* mev(Loop *, Vertex*, Vertex*&, GLdouble*,int id);//v1��ʼ�㣬v2�µ�
	Loop* mef(Vertex*, Vertex*, Loop*);//v1->v2Ϊ˳ʱ�뷽�������ַ���
	Loop* kemr(Vertex*, Vertex*, Loop*);//v1Ϊ�⻷�㣬v2Ϊ�ڻ���
	Solid* kfmrh(Face*, Face*);//ɾ��f1������f2�ڻ�
	void sweep(Face*, GLdouble*, GLdouble, int&);

	Loop* findLoop(Solid* s, int id1, int id2, int id3);
private:
	int solidNum;

public:
	std::map<int, Solid*> solidlist;
	std::map<int, Vertex*> vertexlist;
};


