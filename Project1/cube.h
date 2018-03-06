#ifndef CUBE_H
#define CUBE_H


int cubeVertexCount=24;

float cubeVertices[]={
	-1,-1,-1,
	-1, 1,-1,
	 1, 1,-1,
	 1,-1,-1,

	-1,-1, 1,
	-1, 1, 1,
	 1, 1, 1,
	 1,-1, 1,

	-1,-1,-1,
	-1,-1, 1,
	 1,-1, 1,
	 1,-1,-1,

	-1, 1,-1,
	-1, 1, 1,
	 1, 1, 1,
	 1, 1,-1,

	-1,-1,-1,
	-1,-1, 1,
	-1, 1, 1,
	-1, 1,-1,

	 1,-1,-1,
	 1,-1, 1,
	 1, 1, 1,
	 1, 1,-1
};

float cubeColors[]={
	1,0,0, 1,0,0, 1,0,0, 1,0,0,
	0,1,0, 0,1,0, 0,1,0, 0,1,0,
	0,0,1, 0,0,1, 0,0,1, 0,0,1,
	1,1,0, 1,1,0, 1,1,0, 1,1,0,
	0,1,1, 0,1,1, 0,1,1, 0,1,1,
	1,1,1, 1,1,1, 1,1,1, 1,1,1
};



#endif