/*******************************************************************************
 * point.cpp
 *
 * This module implements the point & spot light source primitive.
 *
 * ---------------------------------------------------------------------------
 * Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
 * Copyright 1991-2013 Persistence of Vision Raytracer Pty. Ltd.
 *
 * POV-Ray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * POV-Ray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------------
 * POV-Ray is based on the popular DKB raytracer version 2.12.
 * DKBTrace was originally written by David K. Buck.
 * DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
 * ---------------------------------------------------------------------------
 * $File: //depot/public/povray/3.x/source/backend/lighting/point.cpp $
 * $Revision: #1 $
 * $Change: 6069 $
 * $DateTime: 2013/11/06 11:59:40 $
 * $Author: chrisc $
 *******************************************************************************/

// frame.h must always be the first POV file included (pulls in platform config)
#include "backend/frame.h"
#include "backend/math/vector.h"
#include "backend/math/matrices.h"
#include "backend/scene/objects.h"
#include "backend/colour/colour.h"
#include "point.h"

// this must be the last file included
#include "base/povdebug.h"

namespace pov
{

/*****************************************************************************
*
* FUNCTION
*
*   All_Light_Source_Intersections
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

bool LightSource::All_Intersections(const Ray& ray, IStack& Depth_Stack, TraceThreadData *Thread)
{
	if(!children.empty())
	{
		if(children[0]->Bound.empty() || Ray_In_Bound(ray, children[0]->Bound, Thread))
		{
			if(children[0]->All_Intersections(ray, Depth_Stack, Thread))
				return true;
		}
	}

	return false;
}



/*****************************************************************************
*
* FUNCTION
*
*   Inside_Light_Source
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

bool LightSource::Inside(const VECTOR IPoint, TraceThreadData *Thread) const
{
	if(!children.empty())
	{
		if(children[0]->Inside(IPoint, Thread))
			return true;
	}

	return false;
}



/*****************************************************************************
*
* FUNCTION
*
*   Light_Source_Normal
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void LightSource::Normal(VECTOR Result, Intersection *Inter, TraceThreadData *Thread) const
{
	if(!children.empty())
		children[0]->Normal(Result, Inter, Thread);
}



/*****************************************************************************
*
* FUNCTION
*
*   Translate_Light_Source
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void LightSource::Translate(const VECTOR Vector, const TRANSFORM *tr)
{
	VAddEq(Center, Vector);
	VAddEq(Points_At, Vector);

	if(!children.empty())
		Translate_Object(children[0], Vector, tr);

	if(Projected_Through_Object != NULL )
		Translate_Object(Projected_Through_Object, Vector, tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Rotate_Light_Source
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void LightSource::Rotate(const VECTOR, const TRANSFORM *tr)
{
	Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Scale_Light_Source
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void LightSource::Scale(const VECTOR, const TRANSFORM *tr)
{
	Transform(tr);
}



/*****************************************************************************
*
* FUNCTION
*
*   Transform_Light_Source
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void LightSource::Transform(const TRANSFORM *tr)
{
	DBL len;

	MTransPoint(Center, Center,    tr);
	MTransPoint(Points_At, Points_At, tr);
	MTransDirection(Axis1, Axis1,     tr);
	MTransDirection(Axis2, Axis2,     tr);

	MTransDirection(Direction, Direction, tr);

	/* Make sure direction has unit length. */

	VLength(len, Direction);

	if(len > EPSILON)
		VInverseScaleEq(Direction, len);

	if(!children.empty())
		Transform_Object(children[0], tr);

	if(Projected_Through_Object != NULL)
		Transform_Object(Projected_Through_Object, tr);
}
	
	


/*****************************************************************************
*
* FUNCTION
*
*   Invert_Light_Source
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void LightSource::Invert()
{
	if(!children.empty())
		Invert_Object(children[0]);
}



/*****************************************************************************
*
* FUNCTION
*
*   Create_Light_Source
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

LightSource::LightSource() : CompoundObject(LIGHT_OBJECT)
{
	Set_Flag(this, NO_SHADOW_FLAG);

	colour = RGBColour(1.0);
	Make_Vector(Direction, 0.0, 0.0, 0.0);
	Make_Vector(Center,    0.0, 0.0, 0.0);
	Make_Vector(Points_At, 0.0, 0.0, 1.0);
	Make_Vector(Axis1,     0.0, 0.0, 1.0);
	Make_Vector(Axis2,     0.0, 1.0, 0.0);

	Coeff   = 0.0;
	Radius  = 0.0;
	Falloff = 0.0;

	Fade_Distance = 0.0;
	Fade_Power    = 0.0;

// TODO  Shadow_Cached_Object = NULL;
	Projected_Through_Object= NULL;
	blend_map            = NULL;

	Light_Type = POINT_SOURCE;

	Area_Light = false;
	Use_Full_Area_Lighting = false; // JN2007: Full area lighting
	Jitter     = false;
	Orient     = false;
	Circular   = false;
	Parallel   = false;
	Photon_Area_Light = false;

	Area_Size1 = 0;
	Area_Size2 = 0;

	Adaptive_Level = 100;

	Media_Attenuation = false;
	Media_Interaction = true;

// TODO  for(i = 0; i < 6; i++)
// TODO    Light_Buffer[i] = NULL;
}



/*****************************************************************************
*
* FUNCTION
*
*   Copy_Light_Source
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

ObjectPtr LightSource::Copy()
{
	LightSource *New = new LightSource();

	/* Copy light source. */

	*New = *this;

	if(!children.empty())
		New->children[0] = Copy_Object(children[0]);
	New->Projected_Through_Object = Copy_Object(Projected_Through_Object);

	/* NK phmap */
	New->blend_map = Copy_Blend_Map(blend_map);

	return (New);
}



/*****************************************************************************
*
* FUNCTION
*
*   Destroy_Light_Source
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

LightSource::~LightSource()
{
	if(blend_map)
	{
		Destroy_Blend_Map(blend_map);
		blend_map=NULL;
	}

	Destroy_Object(Projected_Through_Object);
}

/*****************************************************************************
*
* FUNCTION
*
*   cubic_spline
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   Cubic spline that has tangents of slope 0 at x == low and at x == high.
*   For a given value "pos" between low and high the spline value is returned.
*
* CHANGES
*
*   -
*
******************************************************************************/

DBL cubic_spline(DBL low, DBL  high, DBL  pos)
{
	/* Check to see if the position is within the proper boundaries. */
	if(pos < low)
		return 0.0;
	else
	{
		if(pos >= high)
			return 1.0;
	}

	/* Normalize to the interval [0...1]. */
	pos = (pos - low) / (high - low);

	/* See where it is on the cubic curve. */
	return(3 - 2 * pos) * pos * pos;
}



/*****************************************************************************
*
* FUNCTION
*
*   Attenuate_Light
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   POV-Ray Team
*   
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   Jan 1995 : Added attenuation due to atmospheric scattering and light
*              source distance. Added cylindrical light source. [DB]
*
******************************************************************************/

DBL Attenuate_Light (const LightSource *Light, const Ray &ray, DBL Distance)
{
	DBL len, k, costheta;
	DBL Attenuation = 1.0;
	VECTOR P, V1;

	/* If this is a spotlight then attenuate based on the incidence angle. */

	switch (Light->Light_Type)
	{
		case SPOT_SOURCE:

			VDot(costheta, ray.Direction, Light->Direction);

			if(Distance>0.0) costheta = -costheta;

			if (costheta > 0.0)
			{
				Attenuation = pow(costheta, Light->Coeff);

				if (Light->Radius > 0.0 && costheta < Light->Radius)
				{
					Attenuation *= cubic_spline(Light->Falloff, Light->Radius, costheta);
				}
			}
			else
			{
				return 0.0; //Attenuation = 0.0;
			}

			break;

		case CYLINDER_SOURCE:

			// Project light->point onto light direction
			// to make sure that we're on the correct side of the light
			VSub(V1, ray.Origin, Light->Center);
			VDot(k, V1, Light->Direction);

			if (k > 0.0)
			{
				// Now subtract that from the light-direction.  This will
				// give us a vector showing us the distance from the
				// point to the center of the cylinder.
				VLinComb2(P, 1.0, V1, -k, Light->Direction);
				VLength(len, P);

				if (len < Light->Falloff)
				{
					DBL dist = 1.0 - len / Light->Falloff;

					Attenuation = pow(dist, Light->Coeff);

					if (Light->Radius > 0.0 && len > Light->Radius)
					{
						Attenuation *= cubic_spline(0.0, 1.0 - Light->Radius / Light->Falloff, dist);
					}
				}
				else
				{
					return 0.0; //Attenuation = 0.0;
				}
			}
			else
			{
				return 0.0; //Attenuation = 0.0;
			}

			break;
	}

	if (Attenuation > 0.0)
	{
		/* Attenuate light due to light source distance. */

		if ((Light->Fade_Power > 0.0) && (fabs(Light->Fade_Distance) > EPSILON))
		{
			Attenuation *= 2.0 / (1.0 + pow(Distance / Light->Fade_Distance, Light->Fade_Power));
		}
	}

	return(Attenuation);
}

/*****************************************************************************
*
* FUNCTION
*
*   Light_Source_UVCoord
*
* INPUT
*   
* OUTPUT
*   
* RETURNS
*   
* AUTHOR
*
*   Nathan Kopp -- adapted from Light_Source_Normal by the POV-Ray Team
*
* DESCRIPTION
*
*   -
*
* CHANGES
*
*   -
*
******************************************************************************/

void LightSource::UVCoord(UV_VECT Result, const Intersection *Inter, TraceThreadData *Thread) const
{
	if(!children.empty())
		children[0]->UVCoord(Result, Inter, Thread);
}

}
