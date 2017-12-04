#pragma once

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <glut.h>
#endif

#include <list>
#include <map>

class Vertex;
class Edge;
class Face;
class Halfedge;
class Loop;
class Solid;
class Vertex;

class Vertex
{
public:
	Vertex() {};
	Vertex(int iid, GLdouble xx, GLdouble yy, GLdouble zz) :starthe(nullptr), id(iid), x(xx), y(yy), z(zz)
	{
		cord[0] = xx;
		cord[1] = yy;
		cord[2] = zz;
	}
	~Vertex() {};
	int Vertex::getid()
	{
		return this->id;
	}
	GLdouble getx() { return this->x; }
	GLdouble gety() { return this->y; }
	GLdouble getz() { return this->z; }
	GLdouble* getCord() { return this->cord; }
	Halfedge* starthe;//从这个点出发的半边
private:
	int id;
	GLdouble x, y, z;
	GLdouble cord[3];
};

class Loop
{
public:
	Loop() :firsthe(nullptr), face(nullptr) {};
	~Loop() {};

	//private:
	Halfedge* firsthe;
	Face *face;
};

class Halfedge
{
public:
	Halfedge() {};
	~Halfedge() {};

	//private:
	Edge* edge;
	Vertex* startv;
	Loop* loop;
	Halfedge *nexthe, *prevhe;
};

class Face
{
public:
	Face() :outLoop(nullptr), solid(nullptr) {};
	~Face() { looplist.clear(); }
	//int getid();
	Loop * Face::getoutLoop()
	{
		return this->outLoop;
	}
	void Face::setoutLoop(Loop * loop)
	{
		this->outLoop = loop;
	}
	Solid * Face::getSolid()
	{
		return this->solid;
	}
	void Face::setSolid(Solid * solid)
	{
		this->solid = solid;
	}

private:
	//int id;
	Loop* outLoop;
	Solid* solid;

public:
	std::list<Loop*> looplist;
	Face* nextf;
	Face* prevf;

};

//int Face::getid()
//{
//	return this->id;
//}









class Edge
{
public:
	Edge() {};
	~Edge() {
		delete lhe;
		delete rhe;
	};
	Halfedge *lhe;
	Halfedge *rhe;
	Edge *nexte, *preve;
};

class Solid
{
public:
	Solid() {};
	~Solid() {
		for (auto f = firstFace; f != nullptr; f = f->nextf)
			delete f;
		for (auto e = firstEdge; e != nullptr; e = e->nexte)
			delete e;
	};
	int Solid::getid()
	{
		return this->id;
	}
	void Solid::setid(int id)
	{
		this->id = id;
	}

	Face* firstFace;
	Edge* firstEdge;
	//map<int, Vertex*> vertex;

private:
	int id;
};