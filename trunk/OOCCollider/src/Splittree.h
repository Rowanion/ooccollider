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
	Splittree(unsigned int _numofchildren, SplitAxis _splitLine, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, int &id, map <int, Tile > &_initSize);
	virtual ~Splittree();

	/**
	 * @brief
	 * @param _renderTimes
	 * @param toSplit
	 * @param _xywh
	 */
	void split(vector <double> _renderTimes, Tile toSplit, map<int, Tile > &_xywh);

private:
	void init(int &_id, map <int, Tile > &_initSize);
	void splitHor();
	void splitVert();

	unsigned int numofchildren;
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
	vector <double> timesFirstChildren, timesSecondChildren;
	Tile toSplit;
	double heightSplit, widthSplit;
	Tile firstCoords;
	Tile secondCoords;

	SplitAxis splitLine;
	SplittreeChilds children;
	unsigned int costPicBeforeFirst, costPicBeforeSecond;
};

#endif /*SPLITTREE_H_*/
