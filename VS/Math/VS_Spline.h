/*
 *  VS_Spline.h
 *  MMORPG
 *
 *  Created by Trevor Powell on 12/08/08.
 *  Copyright 2009 Trevor Powell. All rights reserved.
 *
 */

#ifndef VS_SPLINE_H
#define VS_SPLINE_H

#include "VS/Math/VS_Vector.h"
#include "VS/Graphics/VS_Color.h"

class vsSpline1D
{
	float	m_start;
	float	m_startVelocity;
	float	m_end;
	float	m_endVelocity;

public:

	vsSpline1D();
	vsSpline1D( float start, float startVelocity, float end, float endVelocity );

	void Set( float start, float startVelocity, float end, float endVelocity );

		// evaluates spline position at time 't'.
		// 't' must be in the range [0..1] from the start to the end of the spline
    float GetStart() const { return m_start; }
    float GetEnd() const { return m_end; }
    float GetStartVelocity() const { return m_startVelocity; }
    float GetEndVelocity() const { return m_endVelocity; }

	float PositionAtTime( float t ) const;
	float VelocityAtTime( float t ) const;
	vsSpline1D Slice( float t1, float t2 ) const;

	bool operator==(const vsSpline1D& b) const
	{
		return ( m_start == b.m_start &&
				m_startVelocity == b.m_startVelocity &&
				m_end == b.m_end &&
				m_endVelocity == b.m_endVelocity );
	}
	bool operator!=(const vsSpline1D& b) const
	{
		return ( m_start != b.m_start ||
				m_startVelocity != b.m_startVelocity ||
				m_end != b.m_end ||
				m_endVelocity != b.m_endVelocity );
	}
};

class vsSpline2D
{
	vsVector2D	m_start;
	vsVector2D	m_startVelocity;
	vsVector2D	m_end;
	vsVector2D	m_endVelocity;

public:

	vsSpline2D();
	vsSpline2D( const vsVector2D &start, const vsVector2D &startVelocity, const vsVector2D &end, const vsVector2D &endVelocity );

	void Set( const vsVector2D &start, const vsVector2D &startVelocity, const vsVector2D &end, const vsVector2D &endVelocity );

		// evaluates spline position at time 't'.
		// 't' must be in the range [0..1] from the start to the end of the spline
    const vsVector2D & GetStart() const { return m_start; }
    const vsVector2D & GetEnd() const { return m_end; }
    const vsVector2D & GetStartVelocity() const { return m_startVelocity; }
    const vsVector2D & GetEndVelocity() const { return m_endVelocity; }

	vsVector2D	PositionAtTime( float t ) const;
	vsVector2D	VelocityAtTime( float t ) const;
	vsSpline2D Slice( float t1, float t2 ) const;

	bool operator==(const vsSpline2D& b) const
	{
		return ( m_start == b.m_start &&
				m_startVelocity == b.m_startVelocity &&
				m_end == b.m_end &&
				m_endVelocity == b.m_endVelocity );
	}
	bool operator!=(const vsSpline2D& b) const
	{
		return ( m_start != b.m_start ||
				m_startVelocity != b.m_startVelocity ||
				m_end != b.m_end ||
				m_endVelocity != b.m_endVelocity );
	}
};

class vsSpline3D
{
	vsVector3D	m_start;
	vsVector3D	m_startVelocity;
	vsVector3D	m_end;
	vsVector3D	m_endVelocity;

public:

	vsSpline3D();
	vsSpline3D( const vsVector3D &start, const vsVector3D &startVelocity, const vsVector3D &end, const vsVector3D &endVelocity );

	void Set( const vsVector3D &start, const vsVector3D &startVelocity, const vsVector3D &end, const vsVector3D &endVelocity );

	// evaluates spline position at time 't'.
	// 't' must be in the range [0..1] from the start to the end of the spline
    const vsVector3D & GetStart() const { return m_start; }
    const vsVector3D & GetEnd() const { return m_end; }
    const vsVector3D & GetStartVelocity() const { return m_startVelocity; }
    const vsVector3D & GetEndVelocity() const { return m_endVelocity; }

	vsVector3D PositionAtTime( float t ) const;
	vsVector3D VelocityAtTime( float t ) const;

	float Length() const;
	float TimeAtLength(float distance) const;

	// Note:  In general, use ClosestTimeTo in preference to ClosestPointTo,
	// as these are expensive functions, and once you have the time you can
	// get the position or whatever else you want.
	float ClosestTimeTo( const vsVector3D& position ) const;
	vsVector3D ClosestPointTo( const vsVector3D& position ) const;

	// Slice() carves out a segment of this spline into a new spline, exactly
	// matching the spline position during the specified interval of this
	// spline within the new spline's [0..1] range.  This isn't an immediately
	// intuitive operation, so I'm putting it here as a convenience operation.
	//
	// Note that it IS legal to take a backward slice, with t1 > t2!
	//
	// The only tricky bit here is that we need to adjust velocity values,
	// since velocity is the rate a position changes over time, and we're
	// changing the time over which the position changes.  So.. while position
	// values will match precisely, velocity values will not!
	vsSpline3D Slice( float t1, float t2 ) const;

	bool operator==(const vsSpline3D& b) const
	{
		return ( m_start == b.m_start &&
				m_startVelocity == b.m_startVelocity &&
				m_end == b.m_end &&
				m_endVelocity == b.m_endVelocity );
	}
	bool operator!=(const vsSpline3D& b) const
	{
		return ( m_start != b.m_start ||
				m_startVelocity != b.m_startVelocity ||
				m_end != b.m_end ||
				m_endVelocity != b.m_endVelocity );
	}
};


class vsSplineColor
{
	vsColor		m_start;
	vsColor		m_startVelocity;
	vsColor		m_end;
	vsColor		m_endVelocity;

public:

	vsSplineColor();
	vsSplineColor( const vsColor &start, const vsColor &middle, const vsColor &end );

	void Set( const vsColor &start, const vsColor &middle, const vsColor &end );

	const vsColor& Start() const { return m_start; }
	const vsColor& End() const { return m_end; }
	vsColor Middle() const { return m_start + m_startVelocity; }

	// evaluates spline position at time 't'.
	// 't' must be in the range [0..1] from the start to the end of the spline

	vsColor	ColorAtTime( float t ) const;
	vsColor	VelocityAtTime( float t ) const;

	vsSplineColor Slice( float t1, float t2 ) const;

	bool operator==(const vsSplineColor& b) const
	{
		return ( m_start == b.m_start &&
				m_startVelocity == b.m_startVelocity &&
				m_end == b.m_end &&
				m_endVelocity == b.m_endVelocity );
	}
	bool operator!=(const vsSplineColor& b) const
	{
		return ( m_start != b.m_start ||
				m_startVelocity != b.m_startVelocity ||
				m_end != b.m_end ||
				m_endVelocity != b.m_endVelocity );
	}
};


#endif // VS_SPLINE_H

