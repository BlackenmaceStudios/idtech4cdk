// tr_rprogshared.h
//

#ifndef BM_RENDERPROG
#define vec2 idVec2
#define vec3 idVec3
#define vec4 idPlane
#define mat4 idMat4
#endif

#ifdef BM_RENDERPROG
uniform int				    numLights;
uniform vec3 				r_lightOrigin[40];   
uniform vec3 				r_diffuseColor[40];   
uniform vec3 				r_specularColor[40];  
uniform vec4 				r_lightProjectionS[40]; 
uniform vec4				r_lightProjectionT[40]; 
uniform vec4				r_lightProjectionQ[40]; 
uniform vec4 				r_lightFalloff[40]; 
#else
struct bmRenderSceneLights_t
{
	int					numLights;

	vec3 				r_lightOrigin[40];   
	vec3 				r_diffuseColor[40];   
	vec3 				r_specularColor[40];  
	vec4 				r_lightProjectionS[40]; 
	vec4 				r_lightProjectionT[40]; 
	vec4 				r_lightProjectionQ[40]; 
	vec4 				r_lightFalloff[40]; 
};
#endif
