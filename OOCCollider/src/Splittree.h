/**
 * @file	Splittree.h
 * @author  Tim Suess <tsuess@upb.de>
 * @version 1.0
 * @date	Created on: 06.11.2007
 * @brief 	Splittree class declaration.
 */
#ifndef SPLITTREE_H_
#define SPLITTREE_H_

#include "declarations.h"
#include <vector>
#include <map>
#include <cmath>

using namespace std;

class Splittree;
typedef  Splittree** SplittreeChilds;


/**
 * @class Splittree
 * @brief Class is used to divide the screen into several tiles which will then be
 * distributed among the render-nodes.
 * It calculates new tile dimensions based on render-times.
 */
class Splittree
{
public:
	enum SplitAxis{
		HORIZONTAL = 1, // got a horizontal split line
		VERTICAL = 2 // got a vertical split line
	};
	Splittree(unsigned _numofchildren, SplitAxis _splitLine, unsigned _x, unsigned _y, unsigned _width, unsigned _height, int &id);
	virtual ~Splittree();

	/**
	 * @brief
	 * @param _renderTimes
	 * @param toSplit
	 * @param _xywh
	 */
	void split(vector <double> _renderTimes, Tile toSplit, map<int, Tile > &_xywh);

private:
	void init(int &_id);
	void splitHor();
	void splitVert();

	unsigned numofchildren;
	int x, y, width, height;
	int oldx, oldy, oldwidth, oldheight;
	int numFirstChildren, numSecondChildren;
	int firstWidth, secondWidth;
	int firstHeight, secondHeight;
	double firstChildTime, secondChildTime, totalTime;
	int oldFirstWidth, oldSecondWidth;
	int oldFirstHeight, oldSecondHeight;
	double costFirstChildren, costSecondChildren;
	bool firstRound;
	int innerSplitX, innerSplitY;
	int myID;
	double optCosts;
	int summandHor, summandVert;
	vector <double> timesFirstChildren, timesSecondChildren;
	Tile toSplit;
	double heightSplit, widthSplit;
	Tile firstCoords;
	Tile secondCoords;

	SplitAxis splitLine;
	SplittreeChilds children;
	unsigned int costPicBeforeFirst, costPicBeforeSecond;
	float oldRatio;
};

#endif /*SPLITTREE_H_*/
