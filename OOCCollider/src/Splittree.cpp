/**
 * @file	Splittree.cpp
 * @author  Tim Suess <tsuess@upb.de>
 * @version 1.0
 * @date	Created on: 06.11.2007
 * @brief 	Splittree class definition.
 */

#include "Splittree.h"
#include <iostream>

double oldRatioScreen = 0.5, ratioScreen = 0.5;
double oldRatioTime = 0.5, ratioTime = 0.5;
unsigned int lowerTile = 0;

Splittree::Splittree(unsigned int _numofchildren, SplitAxis _splitLine, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, int &_id)
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

		init(_id);
	}
	else
	{
		myID = ++_id;
	}
}

Splittree::~Splittree()
{
}

void Splittree::init(int &_id)
{
	costPicBeforeFirst = numFirstChildren  = numofchildren / 2;
	costPicBeforeSecond = numSecondChildren = numofchildren / 2;
	if(splitLine == HORIZONTAL)
	{
		firstWidth   = width;
		secondWidth  = width;
		firstHeight  = height/2;
		secondHeight = height/2;

		children[0] = new Splittree(numFirstChildren,  VERTICAL, x, y,               firstWidth,  firstHeight, _id);
		children[1] = new Splittree(numSecondChildren, VERTICAL, x, y + firstHeight, secondWidth, secondHeight, _id);
	}
	else
	{
		firstWidth   = width/2;
		secondWidth  = width/2;
		firstHeight  = height;
		secondHeight = height;
		children[0] = new Splittree(numFirstChildren,  HORIZONTAL, x,              y, firstWidth,  firstHeight, _id);
		children[1] = new Splittree(numSecondChildren, HORIZONTAL, x + firstWidth, y, secondWidth, secondHeight, _id);
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

		optCosts = totalTime/2.0;

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

    int oldH = oldFirstHeight+oldSecondHeight;
    float rat = ((float)height)/((float)oldH);
    innerSplitY = (int)(rat*innerSplitY);
    oldFirstHeight = (int)(rat*oldFirstHeight);
    oldSecondHeight = (int)(rat*oldSecondHeight);


	if(!firstRound)
	{
        if(firstChildTime > secondChildTime)
        {
            summandHor = (int)(((float)abs(firstChildTime - optCosts))/((float)totalTime)*((float)oldFirstHeight));
        }
        else
        {
            summandHor = (int)(((float)abs(secondChildTime - optCosts))/((float)totalTime)*((float)oldSecondHeight));
        }

        if(firstChildTime < optCosts)
        {
            innerSplitY += summandHor;//(int)(((double)(totalTime-firstChildTime)/(double)totalTime)*(double)width);
        }
        else if(firstChildTime > optCosts)
        {
            innerSplitY -= summandHor;
        }

        if(innerSplitY%4 != 0)
            innerSplitY -= innerSplitY%4;

	}
    {
        innerSplitY = height / 2;
        firstRound = false;
    }
    if(innerSplitY < (int)numofchildren )
    {
        innerSplitY = numofchildren;
    }
    if(height - innerSplitY < (int)numofchildren )
    {
        innerSplitY = height - numofchildren;
    }

    firstCoords.xPos = x;
    firstCoords.yPos = y;
    firstCoords.width = width;
    firstCoords.height = innerSplitY;

    secondCoords.xPos = x;
    secondCoords.yPos = (y + innerSplitY);
    secondCoords.width = width;
    secondCoords.height = (height - innerSplitY);

    oldFirstHeight = innerSplitY;
    oldSecondHeight = height - innerSplitY;
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

    int oldH = oldFirstWidth+oldSecondWidth;
    float rat = ((float)width)/((float)oldH);
    innerSplitX = (int)(rat*innerSplitX);
    oldFirstWidth = (int)(rat*oldFirstWidth);
    oldSecondWidth = (int)(rat*oldSecondWidth);

	if(!firstRound)
	{
        if(firstChildTime > secondChildTime)
        {
            summandVert = (int)(((float)abs(firstChildTime - optCosts))/((float)totalTime)*((float)oldFirstWidth));
        }
        else
        {
            summandVert = (int)(((float)abs(secondChildTime - optCosts))/((float)totalTime)*((float)oldSecondWidth));
        }

        if(firstChildTime < optCosts)
        {
            innerSplitX += summandVert;
        }
        else if(firstChildTime > optCosts)
        {
            innerSplitX -= summandVert;
        }

        if(innerSplitX%4 != 0)
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
    oldFirstWidth = innerSplitX;
    oldSecondWidth = width - innerSplitX;

}
