//////////////////////////////////////////////////////////////////////
//
// glCamera.h: interface for the glCamera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLCAMERA_H__8E3CD02E_6D82_437E_80DA_50023C60C146__INCLUDED_)
#define AFX_GLCAMERA_H__8E3CD02E_6D82_437E_80DA_50023C60C146__INCLUDED_

#include "GlToolkit.h"
#include "glQuaternion.h"
#include "V3f.h"

namespace ooctools{

class glCamera
{
public:
	GLfloat m_MaxPitchRate;
	GLfloat m_MaxHeadingRate;
	GLfloat m_HeadingDegrees;
	GLfloat m_PitchDegrees;
	GLfloat m_MaxForwardVelocity;
	GLfloat m_ForwardVelocity;
	glQuaternion m_qHeading;
	glQuaternion m_qPitch;
	V3f m_Position;
	V3f m_DirectionVector;

	void ChangeVelocity(GLfloat vel);
	void ChangeHeading(GLfloat degrees);
	void ChangePitch(GLfloat degrees);
	void SetPrespective(void);
	void IncrementPosition();
	glCamera();
	virtual ~glCamera();
};

}
#endif // !defined(AFX_GLCAMERA_H__8E3CD02E_6D82_437E_80DA_50023C60C146__INCLUDED_)
