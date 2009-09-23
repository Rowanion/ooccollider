/**
 * @file	Splittree.cpp
 * @author  Tim Suess <tsuess@upb.de>
 * @version 1.0
 * @date	Created on: 06.11.2007
 * @brief 	Splittree class definition.
 */

#include "Splittree.h"
#include <iostream>

Splittree::Splittree(unsigned int _numofchildren, SplitAxis _splitLine, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, int &_id, map <int, Tile > &_initSize)
{
	oldx = x = _x;
	oldy = y = _y;
	oldwidth  = width  = _width;
	oldheight = height = _height;
	splitLine = _splitLine;
	numofchildren = _numofchildren;

	firstRound = true;

	firstCoords.xPos = firstCoords.yPos = firstCoords.width = firstCoords.height = 0;
	secondCoords.xPos = secondCoords.yPos = secondCoords.width = secondCoords.height = 0;

	if(numofchildren != 1)
	{
		children = new Splittree*[2];

		init(_id, _initSize);
	}
	else
	{
//		_id++;
		myID = ++_id;
	}
}

Splittree::~Splittree()
{
}

void Splittree::init(int &_id, map <int, Tile > &_initSize)
{
	costPicBeforeFirst = numFirstChildren  = numofchildren / 2;
	costPicBeforeSecond = numSecondChildren = numofchildren / 2;
	if(splitLine == HORIZONTAL)
	{
		firstWidth   = width;
		secondWidth  = width;
		firstHeight  = height/2;
		secondHeight = height/2;

		children[0] = new Splittree(numFirstChildren,  VERTICAL, x, y,               firstWidth,  firstHeight, _id, _initSize);
		children[1] = new Splittree(numSecondChildren, VERTICAL, x, y + firstHeight, secondWidth, secondHeight, _id, _initSize);
	}
	else
	{
		firstWidth   = width/2;
		secondWidth  = width/2;
		firstHeight  = height;
		secondHeight = height;
		children[0] = new Splittree(numFirstChildren,  HORIZONTAL, x,              y, firstWidth,  firstHeight, _id, _initSize);
		children[1] = new Splittree(numSecondChildren, HORIZONTAL, x + firstWidth, y, secondWidth, secondHeight, _id, _initSize);
	}
}

void Splittree::split(vector<double> _renderTimes, Tile _toSplit, map<int, Tile > &_xywh)
{
	timesFirstChildren.clear();
	timesSecondChildren.clear();
	toSplit = _toSplit;
	firstChildTime = secondChildTime = totalTime = 0;

	if(numofchildren > 1)
	{
		costFirstChildren = costSecondChildren = 0;
		timesFirstChildren.clear();
		timesSecondChildren.clear();
		for(int i = 0; i < numFirstChildren; i++)
		{
			costFirstChildren += _renderTimes[i];
			timesFirstChildren.push_back(_renderTimes[i]);
			firstChildTime += _renderTimes[i];
			totalTime += _renderTimes[i];
		}

		for(unsigned int i = numFirstChildren; i < _renderTimes.size(); i++)
		{
			costSecondChildren += _renderTimes[i];
			timesSecondChildren.push_back(_renderTimes[i]);
			secondChildTime += _renderTimes[i];
			totalTime += _renderTimes[i];
		}

		if(splitLine == HORIZONTAL)
		{
			splitHor();
			children[0]->split(timesFirstChildren,  firstCoords,  _xywh);
			children[1]->split(timesSecondChildren, secondCoords, _xywh);
		}
		else
		{
			splitVert();
			children[0]->split(timesFirstChildren,  firstCoords,  _xywh);
			children[1]->split(timesSecondChildren, secondCoords, _xywh);
		}

		costPicBeforeFirst  = costFirstChildren;
		costPicBeforeSecond = costSecondChildren;
	}
	else
	{
		_xywh.insert( make_pair( myID, toSplit ) );
	}
}
/*
 * means add a horizontal split line
 */
void Splittree::splitHor()
{
	firstCoords.xPos = firstCoords.yPos = firstCoords.width = firstCoords.height = 0;
	secondCoords.xPos = secondCoords.yPos = secondCoords.width = secondCoords.height = 0;
	x = toSplit.xPos;
	y = toSplit.yPos;
	width  = toSplit.width;
	height = toSplit.height;
	if(!firstRound)
	{
		innerSplitY = (int)(((totalTime-firstChildTime)/totalTime)*(double)height);
		innerSplitY -= innerSplitY%4;
	}
	else
	{
		innerSplitY = height / 2;
		firstRound = false;
	}
	if(innerSplitY < (int)numofchildren)
	{
		innerSplitY = numofchildren;
	}
	if((height - innerSplitY) < (int)(numofchildren/2 + 1))
	{
		innerSplitY = height - (numofchildren/2 + 1);
	}

	firstCoords.xPos = x;
	firstCoords.yPos = y;
	firstCoords.width = width;
	firstCoords.height = innerSplitY;

	secondCoords.xPos = x;
	secondCoords.yPos = y + innerSplitY;
	secondCoords.width = width;
	secondCoords.height = height - innerSplitY;
}

/*
 * means add a vertical split line
 */
void Splittree::splitVert()
{
	firstCoords.xPos = firstCoords.yPos = firstCoords.width = firstCoords.height = 0;
	secondCoords.xPos = secondCoords.yPos = secondCoords.width = secondCoords.height = 0;
	x = toSplit.xPos;
	y = toSplit.yPos;
	width  = toSplit.width;
	height = toSplit.height;
	if(!firstRound)
	{
		innerSplitX = (int)(((double)(totalTime-firstChildTime)/(double)totalTime)*(double)width);
		innerSplitX -= innerSplitX%4;
	}
	else
	{
		innerSplitX = width / 2;
		firstRound = false;
	}
	if(innerSplitX < (int)numofchildren)
	{
		innerSplitX = numofchildren;
	}
	if((width - innerSplitX) < (int)(numofchildren/2 + 1))
	{
		innerSplitX = width - (int)(numofchildren/2 + 1);
	}

	firstCoords.xPos = x;
	firstCoords.yPos = y;
	firstCoords.width = innerSplitX;
	firstCoords.height = height;

	secondCoords.xPos = x + innerSplitX;
	secondCoords.yPos = y;
	secondCoords.width = width - innerSplitX;
	secondCoords.height = height;
}
