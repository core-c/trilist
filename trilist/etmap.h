#pragma once

#include "pch.h"
//#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "mymath.h"

//
//	read .map file, and generate the triangle list needed for the lua-script of the etmover
//

// Here's a typical set of ET brushes:
/*
// brush 0
{
( 640 -192 -256 ) ( 576 192 -256 ) ( 640 -192 0 ) common/caulk 0 0  180 0.500000 -0.500000 0 4 0
( 640 -192 -256 ) ( 640 -192 0 ) ( 1024 192 0 ) common/caulk 0 0 -180 0.500000 -0.500000 0 4 0
( 4776 192 0 ) ( -3416 192 0 )  (-3416 192 -256 ) common/caulk 0 63 -180 0.500000 -0.500000 0 4 0
( 1024 192 0 ) ( 640 -192 0 ) ( 576 192 -96 ) desert_sd/sand_dirt_medium 0 0 0 0.500000 0.500000 0 0 0
( 1024 192 -256 ) ( 576 192 -256 ) ( 640 -192 -256 ) common/caulk 63 0 -180 0.500000 0.500000 0 4 0
}
// brush 1
{
( 320 -64 -256 ) ( 640 -192 -256 ) ( 576 192 -256 ) common/caulk -1 -63 -180 0.500000 0.500000 0 4 0
( 320 -64 0 ) ( 576 192 -96 ) ( 640 -192 0 ) desert_sd/sand_disturb_desert 0 0 0 0.500000 0.500000 0 0 0
( 576 192 0 ) ( 320 -64 -256 ) ( 576 192 -256 ) common/caulk 0 62 -180 0.500000 -0.500000 0 4 0
( 640 -192 -256 ) ( 640 -192 0 ) ( 576 192 -72 ) common/caulk 0 0 -180 0.500000 -0.500000 0 4 0
( 640 -192 -256 ) ( 320 -64 -256 ) ( 320 -64 0 ) common/caulk 0 0 -180 0.500000 -0.500000 0 4 0
}
*/

#define N_BIG 10000000.f
#define N_SMALL -10000000.f

#define S_TRIM_STRING "\n\r\t\v\f "

typedef struct {
	tVec3 p0; // point
	tVec3 p1;
	tVec3 p2;
	bool isTerrain; // true if texture is not "common/caulk"
	bool isBottom; // true if this is the bottom plane of the brush
} tBrushLineHelper;

typedef struct {
	tBrushLineHelper line[5]; // any triangularly shaped terrain brush has 5 lines declared
} tBrushHelper;

typedef struct {
	tPlane p[3];
	tPlane pTerrain;
	tVec3 pointOnPlane[3]; // these are points on the terrain plane (the corner positions)
	int cross[3]; // -1 means no crossing plane[i] allowed. otherwise cross[i] is the triangle index it crosses into
} tTriPlanes;



class mapClass {
private:
	std::vector<tTriPlanes> allTriPlanes;
	std::ifstream myfile;
	std::string LTrim(std::string s);
	std::string RTrim(std::string s);
	std::string stringTrim(std::string s);
	int stringTokenize(std::string s, std::vector<std::string> *tokens, std::string delimiters);
	bool processBrush(tTriPlanes *triPlanes);
	bool findCrossableEdges();
	tVec3 triangleCentroid(int tri);
	bool processMapFile();
public:
	mapClass(std::string filename);
	~mapClass();
};
