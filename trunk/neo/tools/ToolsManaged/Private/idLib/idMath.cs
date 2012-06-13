using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ToolsManaged.Private.idLib
{
    public static class idTechMath
    {
        const float	PI				= 3.14159265358979323846f;
        const float	TWO_PI			= 2.0f * PI;
        const float	HALF_PI			= 0.5f * PI;
        const float	ONEFOURTH_PI	= 0.25f * PI;
        const float E				= 2.71828182845904523536f;
        const float SQRT_TWO		= 1.41421356237309504880f;
        const float SQRT_THREE		= 1.73205080756887729352f;
        const float	SQRT_1OVER2		= 0.70710678118654752440f;
        const float	SQRT_1OVER3		= 0.57735026918962576450f;
        const float	M_DEG2RAD		= PI / 180.0f;
        const float	M_RAD2DEG		= 180.0f / PI;
        const float	M_SEC2MS		= 1000.0f;
        const float	M_MS2SEC		= 0.001f;
        const float	INFINITY		= 1e30f;
        const float FLT_EPSILON		= 1.192092896e-07f;

        public static float InvSqrt(float x)
        {
            float xhalf = 0.5f * x;
            int i = BitConverter.ToInt32(BitConverter.GetBytes(x), 0);
            i = 0x5f3759df - (i >> 1);
            x = BitConverter.ToSingle(BitConverter.GetBytes(i), 0);
            x = x * (1.5f - xhalf * x * x);
            return x;
        } 

        public static float DEG2RAD(float a) { return ( (a) * M_DEG2RAD ); }
       public static float RAD2DEG(float  a) { return ( (a) * M_RAD2DEG );}
    }
}
