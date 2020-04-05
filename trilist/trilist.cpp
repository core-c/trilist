// trilist.cpp :
//	The .map file contains only worlspawn brushes.
//	The .map file contains only triangle shaped terrain brushes (all sides caulk, only 1 side is the ground triangle).

#include "pch.h"
#include <string>

#include "mymath.h"
#include "etmap.h"

int main()
{
	mapClass *etmap = new mapClass("trilist.map");
	delete etmap;
}
