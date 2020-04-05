#pragma once

#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "mymath.h"
#include "etmap.h"

//
//	read .map file, and generate the triangle list needed for the lua-script of the etmover
//


std::string mapClass::LTrim(std::string s) {
	s.erase(0, s.find_first_not_of(S_TRIM_STRING));
	return s;
}
std::string mapClass::RTrim(std::string s) {
	s.erase(s.find_last_not_of(S_TRIM_STRING) + 1);
	return s;
}
std::string mapClass::stringTrim(std::string s) {
	return LTrim(RTrim(s));
}

// function returns the number of tokens
// tokens is the vector of strings found
// delimiters
int mapClass::stringTokenize(std::string s, std::vector<std::string> *tokens, std::string delimiters) {
	int n = 0;
	size_t pos = 0;
	size_t next;
	std::string token;
	tokens->clear();
	do {
		next = s.find_first_of(delimiters, pos);
		token = stringTrim(s.substr(pos, next - pos));
		tokens->push_back(token);
		n++;
		pos = next + 1;
	} while (next != std::string::npos);
	return n;
}

bool mapClass::processBrush(tTriPlanes *triPlanes) {
std::cout << "\tprocess brush\n";
	std::string line;
	// ignore the next line {
	std::getline(myfile, line);
	tBrushHelper brush;
	int n = 0;
	while (std::getline(myfile, line)) {
		//if (n == 5) return false;
		if (line.find("}") != std::string::npos) break; // read lines until the trailing }
		std::vector<std::string> tokens;
		int t = stringTokenize(line, &tokens, " ");
		if (t != 24) { // we should have 24 tokens now
			tokens.clear();
			return false;
		}
		brush.line[n].p0.x = strtof(tokens[1].c_str(), NULL);
		brush.line[n].p0.y = strtof(tokens[2].c_str(), NULL);
		brush.line[n].p0.z = strtof(tokens[3].c_str(), NULL);
		brush.line[n].p1.x = strtof(tokens[6].c_str(), NULL);
		brush.line[n].p1.y = strtof(tokens[7].c_str(), NULL);
		brush.line[n].p1.z = strtof(tokens[8].c_str(), NULL);
		brush.line[n].p2.x = strtof(tokens[11].c_str(), NULL);
		brush.line[n].p2.y = strtof(tokens[12].c_str(), NULL);
		brush.line[n].p2.z = strtof(tokens[13].c_str(), NULL);
		brush.line[n].isTerrain = (line.find("common/caulk") == std::string::npos);
		tokens.clear();
		n++;
		if (n == 5) break;
	}
	// find the bottom plane of the brush:
	// First find the lowest point
	float lowestZ = N_BIG;
	int lowestIndex = 0;
	for (int i = 0; i < 5; i++) {
		float z = brush.line[i].p0.z;
		if (z < lowestZ) {
			lowestZ = z;
			lowestIndex = i;
		}
	}
	// now find which of the planes has this lowest Z value for all 3 points
	for (int i = 0; i < 5; i++)
		brush.line[i].isBottom = (fabsf(brush.line[i].p0.z - lowestZ) < 0.0001f &&
									fabsf(brush.line[i].p1.z - lowestZ) < 0.0001f &&
									fabsf(brush.line[i].p2.z - lowestZ) < 0.0001f);
	// calculate the triangle planes as needed by the lua etmover script
	for (int i = 0, p = 0; i < 5; i++) {
		if (brush.line[i].isBottom) continue;
		if (brush.line[i].isTerrain) {
			triPlanes->pTerrain = planeFromPoints(brush.line[i].p2, brush.line[i].p1, brush.line[i].p0);
			// i need to swap normals so the correct tri-corner-positions are calculated..
			triPlanes->pTerrain.normal = vectorNeg(triPlanes->pTerrain.normal); // a bit of a hack
std::cout << "\t\tplaneFromPoints terrain = {{" << triPlanes->pTerrain.normal.x << "," << triPlanes->pTerrain.normal.y << "," << triPlanes->pTerrain.normal.z << "}," << triPlanes->pTerrain.distance << "}\n";
		} else {
			triPlanes->p[p] = planeFromPoints(brush.line[i].p2, brush.line[i].p1, brush.line[i].p0);
			triPlanes->p[p].normal = vectorNeg(triPlanes->p[p].normal);
std::cout << "\t\tplaneFromPoints p[" << p << "] = {{" << triPlanes->p[p].normal.x << "," << triPlanes->p[p].normal.y << "," << triPlanes->p[p].normal.z << "}," << triPlanes->p[p].distance << "}\n";
			p++;
		}
	}
	// now we calculate the real corner positions of the top-terrain-triangle face
	bool OK = planesIntersectionPoint(triPlanes->pTerrain, triPlanes->p[0], triPlanes->p[1], &triPlanes->pointOnPlane[0]);
	if (OK) {
		OK = planesIntersectionPoint(triPlanes->pTerrain, triPlanes->p[1], triPlanes->p[2], &triPlanes->pointOnPlane[1]);
		if (OK) {
			OK = planesIntersectionPoint(triPlanes->pTerrain, triPlanes->p[2], triPlanes->p[0], &triPlanes->pointOnPlane[2]);
		}
	}
	if (!OK) return false;
	// round
	for (int p = 0; p < 3; p++) {
		triPlanes->pointOnPlane[p].x = roundf(triPlanes->pointOnPlane[p].x);
		triPlanes->pointOnPlane[p].y = roundf(triPlanes->pointOnPlane[p].y);
		triPlanes->pointOnPlane[p].z = roundf(triPlanes->pointOnPlane[p].z);
std::cout << "\t\tpoint = {" << triPlanes->pointOnPlane[p].x << "," << triPlanes->pointOnPlane[p].y << "," << triPlanes->pointOnPlane[p].z << "}\n";
	}

	// now construct the 3 edge planes of each triangle
	// edgeAB
	tVec3 v = { triPlanes->pointOnPlane[1].x, triPlanes->pointOnPlane[1].y, N_SMALL };
	triPlanes->p[0] = planeFromPoints(triPlanes->pointOnPlane[0], triPlanes->pointOnPlane[1], v);
	// edgeBC
	v = { triPlanes->pointOnPlane[2].x, triPlanes->pointOnPlane[2].y, N_SMALL };
	triPlanes->p[1] = planeFromPoints(triPlanes->pointOnPlane[1], triPlanes->pointOnPlane[2], v);
	// edgeCA
	v = { triPlanes->pointOnPlane[0].x, triPlanes->pointOnPlane[0].y, N_SMALL };
	triPlanes->p[2] = planeFromPoints(triPlanes->pointOnPlane[2], triPlanes->pointOnPlane[0], v);
	// what the hacker..
	triPlanes->pTerrain = planeFromPoints(triPlanes->pointOnPlane[0], triPlanes->pointOnPlane[1], triPlanes->pointOnPlane[2]);
/*
	tTriangle t;
	t.point[0] = triPlanes->pointOnPlane[0];
	t.point[1] = triPlanes->pointOnPlane[1];
	t.point[2] = triPlanes->pointOnPlane[2];
	if (!isClockwise(t)) {
std::cout << "CCW\n";
		tVec3 v = triPlanes->pointOnPlane[0];
		triPlanes->pointOnPlane[0] = triPlanes->pointOnPlane[2];
		triPlanes->pointOnPlane[2] = v;
		triPlanes->p[0].normal = vectorNeg(triPlanes->p[0].normal);
		triPlanes->p[1].normal = vectorNeg(triPlanes->p[1].normal);
		triPlanes->p[2].normal = vectorNeg(triPlanes->p[2].normal);
		triPlanes->p[0].distance = -triPlanes->p[0].distance;
		triPlanes->p[1].distance = -triPlanes->p[1].distance;
		triPlanes->p[2].distance = -triPlanes->p[2].distance;
		triPlanes->pTerrain.normal = vectorNeg(triPlanes->pTerrain.normal);
	}
*/
	// correct all normals if they do not point to the center of the triangle
	tVec3 center = vectorAdd(triPlanes->pointOnPlane[0], triPlanes->pointOnPlane[1]);
	center = vectorAdd(center, triPlanes->pointOnPlane[2]);
	center = vectorScale(center, 0.333333333333f);
	for (int p = 0; p < 3; p++)
		if (planePointBack(triPlanes->p[p], center)) {
			triPlanes->p[p].normal = vectorNeg(triPlanes->p[p].normal);
			//triPlanes->pTerrain.normal = vectorNeg(triPlanes->pTerrain.normal);
			triPlanes->p[p].distance = -triPlanes->p[p].distance;
		}


	// we must check for possible crossing to other triangles later..
	// ..we first need all the triPlanes, before we can check that.
	return true;
}

bool mapClass::findCrossableEdges() {
	// go through all triangles
	tTriPlanes t1;
	tTriPlanes t2;
	//  reset crossings
	for (uint32_t tri = 0; tri < allTriPlanes.size(); tri++)
		for (int c = 0; c < 3; c++) allTriPlanes[tri].cross[c] = -1;
	// and scan anew..
	for (uint32_t tri1 = 0; tri1 < allTriPlanes.size(); tri1++) {
		t1 = allTriPlanes[tri1];
		int count = 0;
		for (uint32_t tri2 = tri1 + 1; tri2 < allTriPlanes.size(); tri2++) {
			t2 = allTriPlanes[tri2];
			// find 2 edges that are the same
			int same1[2] = { -1, -1 }; // values are the (index of the) points of tri1
			int same2[2] = { -1, -1 }; // values are the (index of the) points of tri2
			count = 0; // count the # points that are the same (can be max. 2)
			for (int pp1 = 0; pp1 < 3; pp1++) {
				for (int pp2 = 0; pp2 < 3; pp2++) {
					if (vectorSame(t1.pointOnPlane[pp1], t2.pointOnPlane[pp2], 0.01f)) {
						same1[count] = pp1;
						same2[count] = pp2;
						count++;
						if (count == 2) goto count2;
					}
				}
			}
count2:
			// if 2 pairs of the same points are found, we have a valid edge to cross tri1<->tri2
			if (count == 2) {
std::cout << "\tcrossing found: tris " << tri1 << " and " << tri2 << "\n";
				if ((same1[0] == 0 && same1[1] == 1) || (same1[0] == 1 && same1[1] == 0)) allTriPlanes[tri1].cross[0] = tri2;
				if ((same1[0] == 1 && same1[1] == 2) || (same1[0] == 2 && same1[1] == 1)) allTriPlanes[tri1].cross[1] = tri2;
				if ((same1[0] == 2 && same1[1] == 0) || (same1[0] == 0 && same1[1] == 2)) allTriPlanes[tri1].cross[2] = tri2;

				if ((same2[0] == 0 && same2[1] == 1) || (same2[0] == 1 && same2[1] == 0)) allTriPlanes[tri2].cross[0] = tri1;
				if ((same2[0] == 1 && same2[1] == 2) || (same2[0] == 2 && same2[1] == 1)) allTriPlanes[tri2].cross[1] = tri1;
				if ((same2[0] == 2 && same2[1] == 0) || (same2[0] == 0 && same2[1] == 2)) allTriPlanes[tri2].cross[2] = tri1;
			}
		}
	}
	return true;
}

tVec3 mapClass::triangleCentroid(int tri) {
	tVec3 v = vectorAdd(allTriPlanes[tri].pointOnPlane[0], allTriPlanes[tri].pointOnPlane[1]);
	v = vectorAdd(v, allTriPlanes[tri].pointOnPlane[2]);
	v = vectorScale(v, 0.333333333333f);
	return v;
}

bool mapClass::processMapFile() {
	allTriPlanes.clear();
	std::string line;
	bool OK = false;
std::cout << "-- BRUSHES --\n";
	while (std::getline(myfile, line)) {
		// i only need the brushes..
		if (line.find("// brush") != std::string::npos) {
			tTriPlanes triPlanes;
			OK = processBrush(&triPlanes);
			if (!OK) break;
			allTriPlanes.push_back(triPlanes);
		}
	}
	myfile.close();
	if (!OK) return false;
	// now find the edges that can be crossed to other adjacent triangles
std::cout << "-- EDGES --\n";
	findCrossableEdges();

	// ask the position.xy of the script_mover in Radiant
	int moverX, moverY, moverTri = -1;
	std::cout << "\n\nEnter the Radiant script_mover X coordinate:\n";
	std::cin >> moverX;
	std::cout << "Enter the Radiant script_mover Y coordinate:\n";
	std::cin >> moverY;
	tVec3 moverPos = {(float)moverX, (float)moverY, 0.f};
	// We need to find out on which triangle# it starts..
	for (uint32_t t = 0; t < allTriPlanes.size(); t++) {
		if (planePointFront(allTriPlanes[t].p[0], moverPos) &&
			planePointFront(allTriPlanes[t].p[1], moverPos) &&
			planePointFront(allTriPlanes[t].p[2], moverPos)) {
			moverTri = t;
			break;
		}
	}
	if (moverTri == -1) {
		std::cout << "ERROR: The script_mover is not positioned above a valid triangle.\n";
		return false;
	}


std::cout << "-- COPY/PASTE --\n\n\n\n";

	std::cout << "etmover_tri = " << moverTri << "\n\n";

	// get the center of gravity of the mover's start triangle.
	tVec3 v = triangleCentroid(moverTri);
	std::cout << "etmover_pos = {" << v.x << "," << v.y << "," << v.z << "}\n\n";

	// now all data is calculated. It's time to output the tri-list
	// the list has this format:
	//	{
	//		{planeAB, planeBC, planeCA, planeABC, pointOnPlaneABC, crossAB, crossBC, crossCA}
	//	}
	// Where a plane looks like this:
	//	{normal,distance}				// normal is a vec3, distance is a float
	// A vec3 looks like this:
	//	{x,y,z}							// x,y,z are all floats
	// Cross[c] is a number (or nil, in case no crossing over that edge is allowed)
	std::cout << "tri_array = {\n";	// start list
	for (uint32_t t = 0; t < allTriPlanes.size(); t++) {
		std::cout << "{";	// start tri t

		tPlane plane = allTriPlanes[t].p[0];
		std::cout << "{{";
		std::cout << plane.normal.x;
		std::cout << ",";
		std::cout << plane.normal.y;
		std::cout << ",";
		std::cout << plane.normal.z;
		std::cout << "},";
		std::cout << plane.distance;
		std::cout << "},";

		plane = allTriPlanes[t].p[1];
		std::cout << "{{";
		std::cout << plane.normal.x;
		std::cout << ",";
		std::cout << plane.normal.y;
		std::cout << ",";
		std::cout << plane.normal.z;
		std::cout << "},";
		std::cout << plane.distance;
		std::cout << "},";

		plane = allTriPlanes[t].p[2];
		std::cout << "{{";
		std::cout << plane.normal.x;
		std::cout << ",";
		std::cout << plane.normal.y;
		std::cout << ",";
		std::cout << plane.normal.z;
		std::cout << "},";
		std::cout << plane.distance;
		std::cout << "},";

		plane = allTriPlanes[t].pTerrain;
		std::cout << "{{";
		std::cout << plane.normal.x;
		std::cout << ",";
		std::cout << plane.normal.y;
		std::cout << ",";
		std::cout << plane.normal.z;
		std::cout << "},";
		std::cout << plane.distance;
		std::cout << "},";

		tVec3 point = allTriPlanes[t].pointOnPlane[0];
		std::cout << "{";
		std::cout << point.x;
		std::cout << ",";
		std::cout << point.y;
		std::cout << ",";
		std::cout << point.z;
		std::cout << "},";

		int c = allTriPlanes[t].cross[0];
		if (c < 0) {
			std::cout << "nil,";
		} else {
			std::cout << c+1 << ",";	// +1 because of lua indexes start at 1
		}

		c = allTriPlanes[t].cross[1];
		if (c < 0) {
			std::cout << "nil,";
		}
		else {
			std::cout << c+1 << ",";	// +1 because of lua indexes start at 1
		}

		c = allTriPlanes[t].cross[2];
		if (c < 0) {
			std::cout << "nil";
		}
		else {
			std::cout << c+1;	// +1 because of lua indexes start at 1
		}
		// end tri t.   last tri, or not?
		if (t == allTriPlanes.size() - 1) {
			std::cout << "}\n";
		} else {
			std::cout << "},\n";
		}
	}
	std::cout << "}\n";	// end list
	return OK;
}

// constructor
mapClass::mapClass(std::string filename) {
	allTriPlanes.clear();
	myfile.open(filename, std::ifstream::in | std::ifstream::binary);
	if (myfile.is_open()) processMapFile(); else std::cout << "ERROR: File not found: " << filename << "\n";
}

// destructor
mapClass::~mapClass() {
	allTriPlanes.clear();
	if (myfile.is_open()) myfile.close();
}
