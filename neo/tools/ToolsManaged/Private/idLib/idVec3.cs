using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Runtime.InteropServices;

using ToolsManaged.Private.Editor;

namespace ToolsManaged.Private.idLib
{
    [EditorShowProperties]
    public class Vector3
    {
        public float x{ get; set; }
        public float y{ get; set; }
        public float z{ get; set; }

        public float this[int index]
        {
            get
            {
                switch(index)
                {
                    case 0:
                        return x;

                    case 1:
                        return y;

                    case 2:
                        return z;
                    default:
                        throw new Exception("Vector3: Index out of range\n");

                }
            }

            set
            {
                switch(index)
                {
                    case 0:
                        x = value;
                        break;
                    case 1:
                        y = value;
                        break;
                    case 2:
                        z = value;
                        break;
                    default:
                        throw new Exception("Vector3: Index out of range\n");

                }
            }
        }



        public Vector3(bool nullParam = false)
        {
            x = 0;
            y = 0;
            z = 0;
        }

        public Vector3(float x, float y, float z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public Vector3(string val)
        {
            string[] vec = val.Split(' ');
            x = float.Parse(vec[0]);
            y = float.Parse(vec[1]);
            z = float.Parse(vec[2]);
        }

        public override string ToString()
        {
            return x + " " + y + " " + z;
        }

        public static Vector3 operator *(Vector3 c1, float m)
        {
            Vector3 v = new Vector3();

            v.x = c1.x * m;
            v.y = c1.y * m;
            v.z = c1.z * m;

            return v;
        }

        public static Vector3 operator +(Vector3 c1, Vector3 c2)
        {
            Vector3 v = new Vector3();

            v.x = c1.x + c2.x;
            v.y = c1.y + c2.y;
            v.z = c1.z + c2.z;

            return v;
        }

        public static Vector3 operator -(Vector3 c1, Vector3 c2)
        {
            Vector3 v = new Vector3();

            v.x = c1.x - c2.x;
            v.y = c1.y - c2.y;
            v.z = c1.z - c2.z;

            return v;
        }

        public float Normalize() {
	        float sqrLength, invLength;

	        sqrLength = x * x + y * y + z * z;
            invLength = idTechMath.InvSqrt(sqrLength);
	        x *= invLength;
	        y *= invLength;
	        z *= invLength;
	        return invLength * sqrLength;
        }

        /*
        =============
        ToYaw
        =============
        */
        public float ToYaw() {
	        float yaw;
	
	        if ( ( y == 0.0f ) && ( x == 0.0f ) ) {
		        yaw = 0.0f;
	        } else {
                yaw = idTechMath.RAD2DEG((float)Math.Atan2(y, x));
		        if ( yaw < 0.0f ) {
			        yaw += 360.0f;
		        }
	        }

	        return yaw;
        }

        
        /*
        =================
        * Normalize360
        returns angles normalized to the range [0 <= angle < 360]
        =================
        */
        public void Normalize360() {
	        int i;

	        for ( i = 0; i < 3; i++ ) {
		        if ( ( this[i] >= 360.0f ) || ( this[i] < 0.0f ) ) {
			        this[i] -= (float)Math.Floor( this[i] / 360.0f ) * 360.0f;

			        if ( this[i] >= 360.0f ) {
				        this[i] -= 360.0f;
			        }
			        if ( this[i] < 0.0f ) {
				        this[i] += 360.0f;
			        }
		        }
	        }

	
        }

        public void Normalize180() {
            Normalize360();

	        if ( x > 180.0f ) {
		       x -= 360.0f;
	        }
	
	        if ( y > 180.0f ) {
		        y -= 360.0f;
	        }

	        if ( z > 180.0f ) {
		        z -= 360.0f;
	        }
        }

        /*
        =============
        ToPitch
        =============
        */
        public float ToPitch()
        {
	        float	forward;
	        float	pitch;
	
	        if ( ( x == 0.0f ) && ( y == 0.0f ) ) {
		        if ( z > 0.0f ) {
			        pitch = 90.0f;
		        } else {
			        pitch = 270.0f;
		        }
	        } else {
		        forward = ( float )Math.Sqrt( x * x + y * y );
                pitch = idTechMath.RAD2DEG((float)Math.Atan2(z, forward));
		        if ( pitch < 0.0f ) {
			        pitch += 360.0f;
		        }
	        }

	        return pitch;
        }
    }

   
}
