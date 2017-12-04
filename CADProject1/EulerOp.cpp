#include "EulerOp.h"



Solid * EulerOp::mvfs(GLdouble* cord, Vertex *& vertex,int id)
{
	vertex = new Vertex(id, cord[0], cord[1], cord[2]);
	Loop *loop = new Loop();
	Face *face = new Face();
	Solid *solid = new Solid();

	loop->face = face;

	face->setoutLoop(loop);
	face->setSolid(solid);
	face->prevf = nullptr;
	face->nextf = nullptr;

	solid->firstFace = face;
	solid->firstEdge = nullptr;
	solid->setid(solidNum);
	solidlist[solidNum++] = solid;
	vertexlist[id] = vertex;
	//solid->vertex[id] = vertex;
	return solid;
}

Halfedge * EulerOp::mev(Loop *loop, Vertex* v1, Vertex*& v2, GLdouble* cord,int id)
{
	if (v2 == nullptr) {
		v2 = new Vertex(id, cord[0], cord[1], cord[2]);
		vertexlist[id] = v2;
	}
	Halfedge *he1, *he2, *e;
	he1 = new Halfedge();
	he2 = new Halfedge();
	Edge* newe = new Edge();

	he1->startv = v1;
	he2->startv = v2;
	newe->lhe = he1;
	newe->rhe = he2;
	he1->edge = newe;
	he2->edge = newe;
	he1->loop = loop;
	he2->loop = loop;
	he1->nexthe = he2;
	he2->prevhe = he1;

	if (loop->firsthe == nullptr) {//孤立的点
		he2->nexthe = he1;
		he1->prevhe = he2;
		loop->firsthe = he1;
	}
	else {//已成环
		int id1 = v1->getid();
		for (e = loop->firsthe; e->nexthe->startv->getid() != id1; e = e->nexthe);
		he2->nexthe = e->nexthe;
		e->nexthe->prevhe = he2;
		e->nexthe = he1;
		he1->prevhe = e;
	}
	//加入新边
	Solid* solid = loop->face->getSolid();
	if (solid->firstEdge == nullptr)
	{
		solid->firstEdge = newe;
		newe->preve = nullptr;
		newe->nexte = nullptr;
	}
	else {
		newe->preve = nullptr;
		solid->firstEdge->preve = newe;
		newe->nexte = solid->firstEdge;
		solid->firstEdge = newe;
	}
	return he1;
}

Loop * EulerOp::mef(Vertex *v1, Vertex *v2, Loop *loop)//v1到v2为顺时针方向
{
	Edge *newe = new Edge();
	Face *newf = new Face();
	Loop *newl = new Loop();
	Halfedge *he1, *he2, *he3, *he4, *e;
	he1 = new Halfedge();
	he2 = new Halfedge();
	he1->startv = v1;
	he2->startv = v2;
	he1->edge = newe;
	he2->edge = newe;
	newe->lhe = he1;
	newe->rhe = he2;
	//寻找v1，v2出发的原环内半边
	int id1 = v1->getid();
	int id2 = v2->getid();
	for (e = loop->firsthe; e->nexthe->startv->getid() != id1; e = e->nexthe);
	he3 = e; e = e->nexthe;
	for (e = loop->firsthe; e->nexthe->startv->getid() != id2; e = e->nexthe);
	he4 = e;
	//剪断原环，插入两个新半边
	he1->nexthe = he4->nexthe;
	he4->nexthe->prevhe = he1;
	he4->nexthe = he2;
	he2->prevhe = he4;

	he2->nexthe = he3->nexthe;
	he3->nexthe->prevhe = he2;
	he3->nexthe = he1;
	he1->prevhe = he3;
	//创建新环，遍历标记
	loop->firsthe = he1;
	he1->loop = loop;
	newl->firsthe = he2;
	he2->loop = newl;
	for (e = he2->nexthe; e->startv->getid() != id2; e = e->nexthe) {
		e->loop = newl;
	}
	//体中加入新面
	Solid* solid = loop->face->getSolid();
	newf->setoutLoop(newl);
	newl->face = newf;
	newf->setSolid(solid);
	newf->prevf = nullptr;
	solid->firstFace->prevf = newf;
	newf->nextf = solid->firstFace;
	solid->firstFace = newf;
	//体中加入新边
	if (solid->firstEdge == nullptr)
	{
		solid->firstEdge = newe;
		newe->preve = nullptr;
		newe->nexte = nullptr;
	}
	else {
		newe->preve = nullptr;
		solid->firstEdge->preve = newe;
		newe->nexte = solid->firstEdge;
		solid->firstEdge = newe;
	}

	return loop;
}

Loop * EulerOp::kemr(Vertex *v1, Vertex *v2, Loop *loop)//v1为外环点，v2为内环点
{
	Solid* solid = loop->face->getSolid();
	Face* face = loop->face;
	Halfedge *he1, *he2, *e;
	Loop *newl;
	Edge *edge;
	newl = new Loop();
	//找到两条半边
	int id1, id2;
	id1 = v1->getid(); id2 = v2->getid();
	for (e = loop->firsthe; e->startv->getid() != id1 || e->nexthe->startv->getid() != id2; e = e->nexthe);
	he1 = e;
	for (; e->startv->getid() != id2 || e->nexthe->startv->getid() != id1; e = e->nexthe);
	he2 = e;
	edge = he1->edge;
	//处理链表前后关系
	he1->nexthe->prevhe = he2->prevhe;
	he2->prevhe->nexthe = he1->nexthe;
	he1->prevhe->nexthe = he2->nexthe;
	he2->nexthe->prevhe = he1->prevhe;
	//递归标记新环
	loop->firsthe = he1->prevhe;
	newl->firsthe = he1->nexthe;
	for (e = newl->firsthe; e->nexthe->startv->getid() != id2; e = e->nexthe)
		e->loop = newl;
	newl->face = face;
	face->looplist.push_back(newl);
	//体中删除边
	if (edge == solid->firstEdge) {
		solid->firstEdge = edge->nexte;
	}
	if (edge->nexte!=nullptr)
		edge->nexte->preve = edge->preve;
	if (edge->preve!=nullptr)
		edge->preve->nexte = edge->nexte;
	delete edge;
	return newl;
}

Solid* EulerOp::kfmrh(Face *f1, Face *f2)//删除f1，加入f2内环
{
	Solid *s1, *s2;
	s1 = f1->getSolid();
	s2 = f2->getSolid();
	//加入f2内环
	Loop *loop;
	loop = f1->getoutLoop();
	if (f2->getoutLoop() == nullptr)
		f2->setoutLoop(loop);
	else
		f2->looplist.push_back(loop);
	loop->face = f2;
	//删除f1
	if (f1 == s1->firstFace)
		s1->firstFace = f1->nextf;
	if (f1->nextf != nullptr)
		f1->nextf->prevf = f1->prevf;
	if (f1->prevf !=nullptr)
		f1->prevf->nextf = f1->nextf;
	delete f1;
	//合并s1,s2
	if (s1->getid() != s2->getid()) {
		Face *f,*f2;
		for (f = s1->firstFace; f != nullptr; f = f->nextf)
			f->setSolid(s2);
		for (f2 = s2->firstFace; f2->nextf != nullptr; f2 = f2->nextf);
		f2->nextf = s1->firstFace;
		s1->firstFace->prevf = f2;
		Edge *e;
		if (s2->firstEdge != nullptr) {
			for (e = s2->firstEdge; e->nexte != nullptr; e = e->nexte);
			e->nexte = s1->firstEdge;
			if (s1->firstEdge != nullptr)
				s1->firstEdge->preve = e;
		}
		else
			s2->firstEdge = s1->firstEdge;
		solidlist.erase(s1->getid());
	}
	return s2;
}

void EulerOp::sweep(Face *face, GLdouble *vector, GLdouble dist, int& vnum)
{
	Loop* l = face->getoutLoop();
	Halfedge* he1, *he2;
	Vertex *v0, *v1, *v2, *v3;
	Face* outFace;
	//扫成外环
	l = l->firsthe->edge->rhe->loop;
	he1 = l->firsthe;
	v1 = he1->startv;
	v2 = new Vertex(vnum, v1->getx() + vector[0] * dist, v1->gety() + vector[1] * dist, v1->getz() + vector[2] * dist);
	vertexlist[vnum] = v2;
	mev(l, v1, v2, v2->getCord(), vnum++);
	v0 = v3 = v2;
	he2 = he1->nexthe;
	while (he2->startv->getid() != he1->startv->getid()) {
		v1 = he2->startv;
		v2 = new Vertex(vnum, v1->getx() + vector[0] * dist, v1->gety() + vector[1] * dist, v1->getz() + vector[2] * dist);
		vertexlist[vnum] = v2;
		mev(l, v1, v2, v2->getCord(), vnum++);
		l = mef(v3, v2, l);
		v3 = v2; he2 = he2->nexthe;
	}
	l = mef(v3, v0, l);
	outFace = l->face;
	//扫成内环
	if (!face->looplist.empty())
	for (auto ll = face->looplist.begin(); ll != face->looplist.end(); ll++) {
		l = *ll;
		//l = findLoop(face->getSolid(), l->firsthe->startv->getid(), l->firsthe->nexthe->startv->getid(), l->firsthe->nexthe->nexthe->startv->getid());
		l = l->firsthe->edge->rhe->loop;
		he1 = l->firsthe;
		v1 = he1->startv;
		v2 = new Vertex(vnum, v1->getx() + vector[0] * dist, v1->gety() + vector[1] * dist, v1->getz() + vector[2] * dist);
		vertexlist[vnum] = v2;
		mev(l, v1, v2, v2->getCord(), vnum++);
		v0 = v3 = v2;
		he2 = he1->nexthe;
		while (he2->startv->getid() != he1->startv->getid()) {
			v1 = he2->startv;
			v2 = new Vertex(vnum, v1->getx() + vector[0] * dist, v1->gety() + vector[1] * dist, v1->getz() + vector[2] * dist);
			vertexlist[vnum] = v2;
			mev(l, v1, v2, v2->getCord(), vnum++);
			l = mef(v3, v2, l);
			v3 = v2; he2 = he2->nexthe;
		}
		l = mef(v3, v0, l);
		kfmrh(l->face, outFace);
		//l = findLoop(face->getSolid(), vnum - 1, vnum - 2, vnum - 3);
	}
	return;
}

Loop* EulerOp::findLoop(Solid* s, int id1, int id2, int id3)
{
	Face *f;
	Loop *l;
	Halfedge *he1, *he2;
	int flag;
	f = s->firstFace;
	while (f != nullptr) {
		flag = 0;
		l = f->getoutLoop();
		he1 = l->firsthe;
		he2 = he1;
		while (he2->nexthe != he1) {
			if (he2->startv->getid() == id1) flag += 1;
			if (he2->startv->getid() == id2) flag += 1;
			if (he2->startv->getid() == id3) flag += 1;
			he2 = he2->nexthe;
			if (flag == 3) break;
		}
		if (he2->startv->getid() == id1) flag += 1;
		if (he2->startv->getid() == id2) flag += 1;
		if (he2->startv->getid() == id3) flag += 1;
		if (flag == 3) break;
		f = f->nextf;
	}
	if (flag == 3) return f->getoutLoop();
	return nullptr;
}