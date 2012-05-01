// RenderSystem_ext.cpp
//

#include "precompiled.h"
#pragma hdrstop

#include "tr_local.h"

// Vista OpenGL wrapper check
#ifdef _WIN32
#include "../sys/win32/win_local.h"
#endif

void ( APIENTRY * qglMultiTexCoord2fARB )( GLenum texture, GLfloat s, GLfloat t );
void ( APIENTRY * qglMultiTexCoord2fvARB )( GLenum texture, GLfloat *st );
void ( APIENTRY * qglActiveTextureARB )( GLenum texture );
void ( APIENTRY * qglClientActiveTextureARB )( GLenum texture );

void ( APIENTRY *qglCombinerParameterfvNV )( GLenum pname, const GLfloat *params );
void ( APIENTRY *qglCombinerParameterivNV )( GLenum pname, const GLint *params );
void ( APIENTRY *qglCombinerParameterfNV )( GLenum pname, const GLfloat param );
void ( APIENTRY *qglCombinerParameteriNV )( GLenum pname, const GLint param );
void ( APIENTRY *qglCombinerInputNV )( GLenum stage, GLenum portion, GLenum variable, GLenum input,
											  GLenum mapping, GLenum componentUsage );
void ( APIENTRY *qglCombinerOutputNV )( GLenum stage, GLenum portion, GLenum abOutput, GLenum cdOutput, 
											   GLenum sumOutput, GLenum scale, GLenum bias, GLboolean abDotProduct,
											   GLboolean cdDotProduct, GLboolean muxSum );
void ( APIENTRY *qglFinalCombinerInputNV )( GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage );


void (APIENTRY *qglVertexArrayRangeNV)( GLsizei length, void *pointer );
// TTimo: wgl vs glX
// http://oss.sgi.com/projects/ogl-sample/registry/NV/vertex_array_range.txt
// since APIs are the same anyway, let's be wgl/glX agnostic
void *(APIENTRY *qAllocateMemoryNV)( GLsizei size, float readFrequency, float writeFrequency, float priority);
void (APIENTRY *qFreeMemoryNV)( void *pointer );
#ifdef GLX_VERSION_1_1
#define Q_ALLOCATE_MEMORY_NV "glXAllocateMemoryNV"
#define Q_FREE_MEMORY_NV "glXFreeMemoryNV"
#else
#define Q_ALLOCATE_MEMORY_NV "wglAllocateMemoryNV"
#define Q_FREE_MEMORY_NV "wglFreeMemoryNV"
#endif

void (APIENTRY *qglTexImage3D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *);

void (APIENTRY * qglColorTableEXT)( int, int, int, int, int, const void * );


// ATI_fragment_shader
PFNGLGENFRAGMENTSHADERSATIPROC			qglGenFragmentShadersATI;
PFNGLBINDFRAGMENTSHADERATIPROC			qglBindFragmentShaderATI;
PFNGLDELETEFRAGMENTSHADERATIPROC		qglDeleteFragmentShaderATI;
PFNGLBEGINFRAGMENTSHADERATIPROC			qglBeginFragmentShaderATI;
PFNGLENDFRAGMENTSHADERATIPROC			qglEndFragmentShaderATI;
PFNGLPASSTEXCOORDATIPROC				qglPassTexCoordATI;
PFNGLSAMPLEMAPATIPROC					qglSampleMapATI;
PFNGLCOLORFRAGMENTOP1ATIPROC			qglColorFragmentOp1ATI;
PFNGLCOLORFRAGMENTOP2ATIPROC			qglColorFragmentOp2ATI;
PFNGLCOLORFRAGMENTOP3ATIPROC			qglColorFragmentOp3ATI;
PFNGLALPHAFRAGMENTOP1ATIPROC			qglAlphaFragmentOp1ATI;
PFNGLALPHAFRAGMENTOP2ATIPROC			qglAlphaFragmentOp2ATI;
PFNGLALPHAFRAGMENTOP3ATIPROC			qglAlphaFragmentOp3ATI;
PFNGLSETFRAGMENTSHADERCONSTANTATIPROC	qglSetFragmentShaderConstantATI;

// EXT_stencil_two_side
PFNGLACTIVESTENCILFACEEXTPROC			qglActiveStencilFaceEXT;

// ATI_separate_stencil
PFNGLSTENCILOPSEPARATEATIPROC			qglStencilOpSeparateATI;
PFNGLSTENCILFUNCSEPARATEATIPROC			qglStencilFuncSeparateATI;

// ARB_texture_compression
// jmarshall
PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC qglCompressedTexSubImage2DARB;
// jmarshall end
PFNGLCOMPRESSEDTEXIMAGE2DARBPROC		qglCompressedTexImage2DARB;
PFNGLGETCOMPRESSEDTEXIMAGEARBPROC		qglGetCompressedTexImageARB;

// ARB_vertex_buffer_object
PFNGLBINDBUFFERARBPROC					qglBindBufferARB;
PFNGLDELETEBUFFERSARBPROC				qglDeleteBuffersARB;
PFNGLGENBUFFERSARBPROC					qglGenBuffersARB;
PFNGLISBUFFERARBPROC					qglIsBufferARB;
PFNGLBUFFERDATAARBPROC					qglBufferDataARB;
PFNGLBUFFERSUBDATAARBPROC				qglBufferSubDataARB;
PFNGLGETBUFFERSUBDATAARBPROC			qglGetBufferSubDataARB;
PFNGLMAPBUFFERARBPROC					qglMapBufferARB;
PFNGLUNMAPBUFFERARBPROC					qglUnmapBufferARB;
PFNGLGETBUFFERPARAMETERIVARBPROC		qglGetBufferParameterivARB;
PFNGLGETBUFFERPOINTERVARBPROC			qglGetBufferPointervARB;

// ARB_vertex_program / ARB_fragment_program
PFNGLBINDATTRIBLOCATIONARBPROC			qglBindAttribLocationARB;
PFNGLENABLEVERTEXATTRIBARRAYARBPROC		qglEnableVertexAttribArrayARB;
PFNGLDISABLEVERTEXATTRIBARRAYARBPROC	qglDisableVertexAttribArrayARB;
PFNGLVERTEXATTRIBPOINTERARBPROC			qglVertexAttribPointerARB;
/*
PFNGLENABLEVERTEXATTRIBARRAYARBPROC		qglEnableVertexAttribArrayARB;
PFNGLDISABLEVERTEXATTRIBARRAYARBPROC	qglDisableVertexAttribArrayARB;
PFNGLPROGRAMSTRINGARBPROC				qglProgramStringARB;
PFNGLBINDPROGRAMARBPROC					qglBindProgramARB;
PFNGLGENPROGRAMSARBPROC					qglGenProgramsARB;
PFNGLPROGRAMENVPARAMETER4FVARBPROC		qglProgramEnvParameter4fvARB;
PFNGLPROGRAMLOCALPARAMETER4FVARBPROC	qglProgramLocalParameter4fvARB;
*/

PFNGLDRAWBUFFERSARBPROC qglDrawBuffers;

// GL_EXT_depth_bounds_test
PFNGLDEPTHBOUNDSEXTPROC                 qglDepthBoundsEXT;

PFNGLCREATESHADEROBJECTARBPROC		qglCreateShaderObjectARB;
PFNGLDELETEOBJECTARBPROC				qglDeleteObjectARB;
PFNGLSHADERSOURCEARBPROC				qglShaderSourceARB;
PFNGLCOMPILESHADERARBPROC			qglCompileShaderARB;
PFNGLGETOBJECTPARAMETERIVARBPROC		qglGetObjectParameterivARB;
PFNGLCREATEPROGRAMOBJECTARBPROC		qglCreateProgramObjectARB;
PFNGLATTACHOBJECTARBPROC				qglAttachObjectARB;
PFNGLDETACHOBJECTARBPROC				qglDetachObjectARB;
PFNGLLINKPROGRAMARBPROC				qglLinkProgramARB;
PFNGLUSEPROGRAMOBJECTARBPROC			qglUseProgramObjectARB;
PFNGLGETUNIFORMLOCATIONARBPROC		qglGetUniformLocationARB;
PFNGLGETATTRIBLOCATIONARBPROC		qglGetAttributeLocationARB;
PFNGLUNIFORM1FARBPROC				qglUniform1fARB;
PFNGLUNIFORM1IARBPROC				qglUniform1iARB;
PFNGLUNIFORM1FVARBPROC				qglUniform1fvARB;
PFNGLUNIFORM2FVARBPROC				qglUniform2fvARB;
PFNGLUNIFORM3FVARBPROC				qglUniform3fvARB;
PFNGLUNIFORM4FVARBPROC				qglUniform4fvARB;
PFNGLGETINFOLOGARBPROC				qglGetInfoLogARB;
PFNGLVERTEXATTRIB4FVPROC			qglVertexAttrib4fv;
PFNGLUNIFORMMATRIX4FVARBPROC		qglUniformMatrix4fvARB;

PFNGLGENFRAMEBUFFERSEXTPROC qglGenFramebuffersEXT;
PFNGLBINDFRAMEBUFFEREXTPROC qglBindFramebufferEXT;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC qglFramebufferTexture2DEXT;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC qglCheckFramebufferStatusEXT;
PFNGLGENRENDERBUFFERSEXTPROC qglGenRenderbuffersEXT;
PFNGLBINDRENDERBUFFEREXTPROC qglBindRenderbufferEXT;
PFNGLRENDERBUFFERSTORAGEEXTPROC qglRenderbufferStorageEXT;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC qglFramebufferRenderbufferEXT;
PFNGLDELETERENDERBUFFERSEXTPROC qglDeleteRenderbuffersEXT;

PFNGLGETUNIFORMBLOCKINDEXPROC qglGetUniformBlockIndex;
PFNGLGETACTIVEUNIFORMBLOCKIVPROC qglGetActiveUniformBlockiv;
PFNGLBINDBUFFERBASEPROC qglBindBufferBase ;
PFNGLGETUNIFORMINDICESPROC qglGetUniformIndices;
PFNGLGETACTIVEUNIFORMSIVPROC qglGetActiveUniformsiv;
PFNGLUNIFORMBLOCKBINDINGPROC qglUniformBlockBinding;

PFNGLFENCESYNCPROC qglFenceSync;
PFNGLISSYNCPROC qglIsSync;
PFNGLDELETESYNCPROC qglDeleteSync;
PFNGLCLIENTWAITSYNCPROC qglClientWaitSync;
PFNGLWAITSYNCPROC qglWaitSync;
PFNGLGETINTEGER64VPROC qglGetInteger64v;
PFNGLGETSYNCIVPROC qglGetSynciv;

/*
==================
R_CheckPortableExtensions

==================
*/
void R_CheckPortableExtensions( void ) {
	glConfig.glVersion = atof( glConfig.version_string );

	// GL_ARB_multitexture
	glConfig.multitextureAvailable = R_CheckExtension( "GL_ARB_multitexture" );
	if ( glConfig.multitextureAvailable ) {
		qglMultiTexCoord2fARB = (void(APIENTRY *)(GLenum, GLfloat, GLfloat))GLimp_ExtensionPointer( "glMultiTexCoord2fARB" );
		qglMultiTexCoord2fvARB = (void(APIENTRY *)(GLenum, GLfloat *))GLimp_ExtensionPointer( "glMultiTexCoord2fvARB" );
		qglActiveTextureARB = (void(APIENTRY *)(GLenum))GLimp_ExtensionPointer( "glActiveTextureARB" );
		qglClientActiveTextureARB = (void(APIENTRY *)(GLenum))GLimp_ExtensionPointer( "glClientActiveTextureARB" );
		qglGetIntegerv( GL_MAX_TEXTURE_UNITS_ARB, (GLint *)&glConfig.maxTextureUnits );
		if ( glConfig.maxTextureUnits > MAX_MULTITEXTURE_UNITS ) {
			glConfig.maxTextureUnits = MAX_MULTITEXTURE_UNITS;
		}
		if ( glConfig.maxTextureUnits < 2 ) {
			glConfig.multitextureAvailable = false;	// shouldn't ever happen
		}
		qglGetIntegerv( GL_MAX_TEXTURE_COORDS_ARB, (GLint *)&glConfig.maxTextureCoords );
		qglGetIntegerv( GL_MAX_TEXTURE_IMAGE_UNITS_ARB, (GLint *)&glConfig.maxTextureImageUnits );
	}

	// GL_ARB_texture_env_combine
	glConfig.textureEnvCombineAvailable = R_CheckExtension( "GL_ARB_texture_env_combine" );

	// GL_ARB_texture_cube_map
	glConfig.cubeMapAvailable = R_CheckExtension( "GL_ARB_texture_cube_map" );

	// GL_ARB_texture_env_dot3
	glConfig.envDot3Available = R_CheckExtension( "GL_ARB_texture_env_dot3" );

	// GL_ARB_texture_env_add
	glConfig.textureEnvAddAvailable = R_CheckExtension( "GL_ARB_texture_env_add" );

	// GL_ARB_texture_non_power_of_two
	glConfig.textureNonPowerOfTwoAvailable = R_CheckExtension( "GL_ARB_texture_non_power_of_two" );

	// GL_ARB_texture_compression + GL_S3_s3tc
	// DRI drivers may have GL_ARB_texture_compression but no GL_EXT_texture_compression_s3tc
	if ( R_CheckExtension( "GL_ARB_texture_compression" ) && R_CheckExtension( "GL_EXT_texture_compression_s3tc" ) ) {
		glConfig.textureCompressionAvailable = true;
		qglCompressedTexImage2DARB = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)GLimp_ExtensionPointer( "glCompressedTexImage2DARB" );
		qglGetCompressedTexImageARB = (PFNGLGETCOMPRESSEDTEXIMAGEARBPROC)GLimp_ExtensionPointer( "glGetCompressedTexImageARB" );
		// jmarshall
		qglCompressedTexSubImage2DARB = (PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC)GLimp_ExtensionPointer( "glCompressedTexSubImage2DARB" );
		// jmarshall end
	} else {
		glConfig.textureCompressionAvailable = false;
	}

	// GL_EXT_texture_filter_anisotropic
	glConfig.anisotropicAvailable = R_CheckExtension( "GL_EXT_texture_filter_anisotropic" );
	if ( glConfig.anisotropicAvailable ) {
		qglGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &glConfig.maxTextureAnisotropy );
		common->Printf( "   maxTextureAnisotropy: %f\n", glConfig.maxTextureAnisotropy );
	} else {
		glConfig.maxTextureAnisotropy = 1;
	}

	// GL_EXT_texture_lod_bias
	// The actual extension is broken as specificed, storing the state in the texture unit instead
	// of the texture object.  The behavior in GL 1.4 is the behavior we use.
	if ( glConfig.glVersion >= 1.4 || R_CheckExtension( "GL_EXT_texture_lod" ) ) {
		common->Printf( "...using %s\n", "GL_1.4_texture_lod_bias" );
		glConfig.textureLODBiasAvailable = true;
	} else {
		common->Printf( "X..%s not found\n", "GL_1.4_texture_lod_bias" );
		glConfig.textureLODBiasAvailable = false;
	}

	// GL_EXT_shared_texture_palette
	glConfig.sharedTexturePaletteAvailable = R_CheckExtension( "GL_EXT_shared_texture_palette" );
	if ( glConfig.sharedTexturePaletteAvailable ) {
		qglColorTableEXT = ( void ( APIENTRY * ) ( int, int, int, int, int, const void * ) ) GLimp_ExtensionPointer( "glColorTableEXT" );
	}

	// GL_EXT_texture3D (not currently used for anything)
	glConfig.texture3DAvailable = R_CheckExtension( "GL_EXT_texture3D" );
	if ( glConfig.texture3DAvailable ) {
		qglTexImage3D = 
			(void (APIENTRY *)(GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *) )
			GLimp_ExtensionPointer( "glTexImage3D" );
	}

	// EXT_stencil_wrap
	// This isn't very important, but some pathological case might cause a clamp error and give a shadow bug.
	// Nvidia also believes that future hardware may be able to run faster with this enabled to avoid the
	// serialization of clamping.
	if ( R_CheckExtension( "GL_EXT_stencil_wrap" ) ) {
		tr.stencilIncr = GL_INCR_WRAP_EXT;
		tr.stencilDecr = GL_DECR_WRAP_EXT;
	} else {
		tr.stencilIncr = GL_INCR;
		tr.stencilDecr = GL_DECR;
	}

	// GL_NV_register_combiners
	glConfig.registerCombinersAvailable = R_CheckExtension( "GL_NV_register_combiners" );
	if ( glConfig.registerCombinersAvailable ) {
		qglCombinerParameterfvNV = (void (APIENTRY *)( GLenum pname, const GLfloat *params ))
			GLimp_ExtensionPointer( "glCombinerParameterfvNV" );
		qglCombinerParameterivNV = (void (APIENTRY *)( GLenum pname, const GLint *params ))
			GLimp_ExtensionPointer( "glCombinerParameterivNV" );
		qglCombinerParameterfNV = (void (APIENTRY *)( GLenum pname, const GLfloat param ))
			GLimp_ExtensionPointer( "glCombinerParameterfNV" );
		qglCombinerParameteriNV = (void (APIENTRY *)( GLenum pname, const GLint param ))
			GLimp_ExtensionPointer( "glCombinerParameteriNV" );
		qglCombinerInputNV = (void (APIENTRY *)( GLenum stage, GLenum portion, GLenum variable, GLenum input,
											  GLenum mapping, GLenum componentUsage ))
			GLimp_ExtensionPointer( "glCombinerInputNV" );
		qglCombinerOutputNV = (void (APIENTRY *)( GLenum stage, GLenum portion, GLenum abOutput, GLenum cdOutput, 
											   GLenum sumOutput, GLenum scale, GLenum bias, GLboolean abDotProduct,
											   GLboolean cdDotProduct, GLboolean muxSum ))
			GLimp_ExtensionPointer( "glCombinerOutputNV" );
		qglFinalCombinerInputNV = (void (APIENTRY *)( GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage ))
			GLimp_ExtensionPointer( "glFinalCombinerInputNV" );
	}

	// GL_EXT_stencil_two_side
	glConfig.twoSidedStencilAvailable = R_CheckExtension( "GL_EXT_stencil_two_side" );
	if ( glConfig.twoSidedStencilAvailable ) {
		qglActiveStencilFaceEXT = (PFNGLACTIVESTENCILFACEEXTPROC)GLimp_ExtensionPointer( "glActiveStencilFaceEXT" );
	} else {
		glConfig.atiTwoSidedStencilAvailable = R_CheckExtension( "GL_ATI_separate_stencil" );
		if ( glConfig.atiTwoSidedStencilAvailable ) {
			qglStencilFuncSeparateATI  = (PFNGLSTENCILFUNCSEPARATEATIPROC)GLimp_ExtensionPointer( "glStencilFuncSeparateATI" );
			qglStencilOpSeparateATI = (PFNGLSTENCILOPSEPARATEATIPROC)GLimp_ExtensionPointer( "glStencilOpSeparateATI" );
		}
	}

	// GL_ATI_fragment_shader
	glConfig.atiFragmentShaderAvailable = R_CheckExtension( "GL_ATI_fragment_shader" );
	if (! glConfig.atiFragmentShaderAvailable ) {
		// only on OSX: ATI_fragment_shader is faked through ATI_text_fragment_shader (macosx_glimp.cpp)
		glConfig.atiFragmentShaderAvailable = R_CheckExtension( "GL_ATI_text_fragment_shader" );
	}
	if ( glConfig.atiFragmentShaderAvailable ) {
		qglGenFragmentShadersATI = (PFNGLGENFRAGMENTSHADERSATIPROC)GLimp_ExtensionPointer( "glGenFragmentShadersATI" );
		qglBindFragmentShaderATI = (PFNGLBINDFRAGMENTSHADERATIPROC)GLimp_ExtensionPointer( "glBindFragmentShaderATI" );
		qglDeleteFragmentShaderATI = (PFNGLDELETEFRAGMENTSHADERATIPROC)GLimp_ExtensionPointer( "glDeleteFragmentShaderATI" );
		qglBeginFragmentShaderATI = (PFNGLBEGINFRAGMENTSHADERATIPROC)GLimp_ExtensionPointer( "glBeginFragmentShaderATI" );
		qglEndFragmentShaderATI = (PFNGLENDFRAGMENTSHADERATIPROC)GLimp_ExtensionPointer( "glEndFragmentShaderATI" );
		qglPassTexCoordATI = (PFNGLPASSTEXCOORDATIPROC)GLimp_ExtensionPointer( "glPassTexCoordATI" );
		qglSampleMapATI = (PFNGLSAMPLEMAPATIPROC)GLimp_ExtensionPointer( "glSampleMapATI" );
		qglColorFragmentOp1ATI = (PFNGLCOLORFRAGMENTOP1ATIPROC)GLimp_ExtensionPointer( "glColorFragmentOp1ATI" );
		qglColorFragmentOp2ATI = (PFNGLCOLORFRAGMENTOP2ATIPROC)GLimp_ExtensionPointer( "glColorFragmentOp2ATI" );
		qglColorFragmentOp3ATI = (PFNGLCOLORFRAGMENTOP3ATIPROC)GLimp_ExtensionPointer( "glColorFragmentOp3ATI" );
		qglAlphaFragmentOp1ATI = (PFNGLALPHAFRAGMENTOP1ATIPROC)GLimp_ExtensionPointer( "glAlphaFragmentOp1ATI" );
		qglAlphaFragmentOp2ATI = (PFNGLALPHAFRAGMENTOP2ATIPROC)GLimp_ExtensionPointer( "glAlphaFragmentOp2ATI" );
		qglAlphaFragmentOp3ATI = (PFNGLALPHAFRAGMENTOP3ATIPROC)GLimp_ExtensionPointer( "glAlphaFragmentOp3ATI" );
		qglSetFragmentShaderConstantATI = (PFNGLSETFRAGMENTSHADERCONSTANTATIPROC)GLimp_ExtensionPointer( "glSetFragmentShaderConstantATI" );
	}

	if(R_CheckExtension("GL_ARB_sync", true)) {
		 qglFenceSync = (PFNGLFENCESYNCPROC )GLimp_ExtensionPointer( "glFenceSync" );
		 qglIsSync= (PFNGLISSYNCPROC)GLimp_ExtensionPointer( "glIsSync" );
		 qglDeleteSync = (PFNGLDELETESYNCPROC)GLimp_ExtensionPointer( "glDeleteSync" );
		 qglClientWaitSync = (PFNGLCLIENTWAITSYNCPROC)GLimp_ExtensionPointer( "glClientWaitSync" );
		 qglWaitSync = (PFNGLWAITSYNCPROC)GLimp_ExtensionPointer( "glWaitSync" );
		 qglGetInteger64v = (PFNGLGETINTEGER64VPROC)GLimp_ExtensionPointer( "glGetInteger64v" );
		 qglGetSynciv = (PFNGLGETSYNCIVPROC)GLimp_ExtensionPointer( "glGetSynciv" );
	}

	if(R_CheckExtension( "GL_ARB_uniform_buffer_object", true)) {
		 qglGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC)GLimp_ExtensionPointer( "glGetUniformBlockIndex" );
		 qglGetActiveUniformBlockiv = (PFNGLGETACTIVEUNIFORMBLOCKIVPROC)GLimp_ExtensionPointer( "glGetActiveUniformBlockiv" );
		 qglBindBufferBase = (PFNGLBINDBUFFERBASEPROC)GLimp_ExtensionPointer( "glBindBufferBase" );
		 qglGetUniformIndices = (PFNGLGETUNIFORMINDICESPROC)GLimp_ExtensionPointer( "glGetUniformIndices" );
		 qglGetActiveUniformsiv = (PFNGLGETACTIVEUNIFORMSIVPROC)GLimp_ExtensionPointer( "glGetActiveUniformsiv" );
		 qglUniformBlockBinding = (PFNGLUNIFORMBLOCKBINDINGPROC)GLimp_ExtensionPointer( "glUniformBlockBinding" );
	}

	// GL_ARB_framebuffer_object
	if(R_CheckExtension( "GL_ARB_framebuffer_object", true)) {
		qglDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC)GLimp_ExtensionPointer( "glDeleteRenderbuffersEXT" );
		qglDrawBuffers = (PFNGLDRAWBUFFERSARBPROC)GLimp_ExtensionPointer( "glDrawBuffersARB" );
		qglGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)GLimp_ExtensionPointer("glGenFramebuffersEXT");
		qglBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)GLimp_ExtensionPointer("glBindFramebufferEXT");
		qglFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)GLimp_ExtensionPointer("glFramebufferTexture2DEXT");
		qglCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)GLimp_ExtensionPointer("glCheckFramebufferStatusEXT");
		qglGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC)GLimp_ExtensionPointer("glGenRenderbuffersEXT");
		qglBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC)GLimp_ExtensionPointer("glBindRenderbufferEXT");
		qglRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC)GLimp_ExtensionPointer("glRenderbufferStorageEXT");
		qglFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)GLimp_ExtensionPointer("glFramebufferRenderbufferEXT");
	}
	
	// ARB_vertex_buffer_object
	glConfig.ARBVertexBufferObjectAvailable = R_CheckExtension( "GL_ARB_vertex_buffer_object" );
	if(glConfig.ARBVertexBufferObjectAvailable) {
		qglBindBufferARB = (PFNGLBINDBUFFERARBPROC)GLimp_ExtensionPointer( "glBindBufferARB");
		qglDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)GLimp_ExtensionPointer( "glDeleteBuffersARB");
		qglGenBuffersARB = (PFNGLGENBUFFERSARBPROC)GLimp_ExtensionPointer( "glGenBuffersARB");
		qglIsBufferARB = (PFNGLISBUFFERARBPROC)GLimp_ExtensionPointer( "glIsBufferARB");
		qglBufferDataARB = (PFNGLBUFFERDATAARBPROC)GLimp_ExtensionPointer( "glBufferDataARB");
		qglBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)GLimp_ExtensionPointer( "glBufferSubDataARB");
		qglGetBufferSubDataARB = (PFNGLGETBUFFERSUBDATAARBPROC)GLimp_ExtensionPointer( "glGetBufferSubDataARB");
		qglMapBufferARB = (PFNGLMAPBUFFERARBPROC)GLimp_ExtensionPointer( "glMapBufferARB");
		qglUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)GLimp_ExtensionPointer( "glUnmapBufferARB");
		qglGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)GLimp_ExtensionPointer( "glGetBufferParameterivARB");
		qglGetBufferPointervARB = (PFNGLGETBUFFERPOINTERVARBPROC)GLimp_ExtensionPointer( "glGetBufferPointervARB");
	}

	// GL_ARB_vertex_shader and GL_ARB_fragment_shader
	if(R_CheckExtension( "GL_ARB_vertex_shader", true ) && R_CheckExtension( "GL_ARB_fragment_shader", true )) {
		qglCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)GLimp_ExtensionPointer( "glCreateShaderObjectARB");
		qglDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC)GLimp_ExtensionPointer( "glDeleteObjectARB");
		qglShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)GLimp_ExtensionPointer( "glShaderSourceARB");
		qglCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)GLimp_ExtensionPointer( "glCompileShaderARB");
		qglGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)GLimp_ExtensionPointer( "glCompileShaderARB");
		qglCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)GLimp_ExtensionPointer( "glCreateProgramObjectARB");
		qglAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)GLimp_ExtensionPointer( "glAttachObjectARB");
		qglDetachObjectARB = (PFNGLDETACHOBJECTARBPROC)GLimp_ExtensionPointer( "glDetachObjectARB");
		qglLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)GLimp_ExtensionPointer( "glLinkProgramARB");
		qglUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)GLimp_ExtensionPointer( "glUseProgramObjectARB");
		qglGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)GLimp_ExtensionPointer( "glGetUniformLocationARB");
		//qglGetAttributeLocationARB  = (PFNGLGETATTRIBLOCATIONARBPROC)GLimp_ExtensionPointer( "glGetAttributeLocationARB");
		qglUniform1fARB = (PFNGLUNIFORM1FARBPROC)GLimp_ExtensionPointer( "glUniform1fARB");
		qglUniform1iARB = (PFNGLUNIFORM1IARBPROC)GLimp_ExtensionPointer( "glUniform1iARB");
		qglUniform1fvARB = (PFNGLUNIFORM1FVARBPROC)GLimp_ExtensionPointer( "glUniform1fvARB");
		qglUniform2fvARB = (PFNGLUNIFORM2FVARBPROC)GLimp_ExtensionPointer( "glUniform2fvARB");
		qglUniform3fvARB = (PFNGLUNIFORM3FVARBPROC)GLimp_ExtensionPointer( "glUniform3fvARB");
		qglUniform4fvARB = (PFNGLUNIFORM4FVARBPROC)GLimp_ExtensionPointer( "glUniform4fvARB");
		qglGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)GLimp_ExtensionPointer( "glGetInfoLogARB");
		qglVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)GLimp_ExtensionPointer( "glVertexAttrib4fv");
		qglUniformMatrix4fvARB = (PFNGLUNIFORMMATRIX4FVARBPROC)GLimp_ExtensionPointer( "glUniformMatrix4fvARB");
	}


	// ARB_vertex_program

	glConfig.ARBVertexProgramAvailable = R_CheckExtension( "GL_ARB_vertex_program" );
	if (glConfig.ARBVertexProgramAvailable) {
		qglVertexAttribPointerARB = (PFNGLVERTEXATTRIBPOINTERARBPROC)GLimp_ExtensionPointer( "glVertexAttribPointerARB" );
		qglEnableVertexAttribArrayARB = (PFNGLENABLEVERTEXATTRIBARRAYARBPROC)GLimp_ExtensionPointer( "glEnableVertexAttribArrayARB" );
		qglDisableVertexAttribArrayARB = (PFNGLDISABLEVERTEXATTRIBARRAYARBPROC)GLimp_ExtensionPointer( "glDisableVertexAttribArrayARB" );
		qglBindAttribLocationARB = (PFNGLBINDATTRIBLOCATIONARBPROC)GLimp_ExtensionPointer( "glBindAttribLocationARB" );
		//qglProgramStringARB = (PFNGLPROGRAMSTRINGARBPROC)GLimp_ExtensionPointer( "glProgramStringARB" );
		//qglBindProgramARB = (PFNGLBINDPROGRAMARBPROC)GLimp_ExtensionPointer( "glBindProgramARB" );
		//qglGenProgramsARB = (PFNGLGENPROGRAMSARBPROC)GLimp_ExtensionPointer( "glGenProgramsARB" );
		//qglProgramEnvParameter4fvARB = (PFNGLPROGRAMENVPARAMETER4FVARBPROC)GLimp_ExtensionPointer( "glProgramEnvParameter4fvARB" );
		//qglProgramLocalParameter4fvARB = (PFNGLPROGRAMLOCALPARAMETER4FVARBPROC)GLimp_ExtensionPointer( "glProgramLocalParameter4fvARB" );
	}
/*
	// ARB_fragment_program
	if ( r_inhibitFragmentProgram.GetBool() ) {
		glConfig.ARBFragmentProgramAvailable = false;
	} else {
		glConfig.ARBFragmentProgramAvailable = R_CheckExtension( "GL_ARB_fragment_program" );
		if (glConfig.ARBFragmentProgramAvailable) {
			// these are the same as ARB_vertex_program
			qglProgramStringARB = (PFNGLPROGRAMSTRINGARBPROC)GLimp_ExtensionPointer( "glProgramStringARB" );
			qglBindProgramARB = (PFNGLBINDPROGRAMARBPROC)GLimp_ExtensionPointer( "glBindProgramARB" );
			qglProgramEnvParameter4fvARB = (PFNGLPROGRAMENVPARAMETER4FVARBPROC)GLimp_ExtensionPointer( "glProgramEnvParameter4fvARB" );
			qglProgramLocalParameter4fvARB = (PFNGLPROGRAMLOCALPARAMETER4FVARBPROC)GLimp_ExtensionPointer( "glProgramLocalParameter4fvARB" );
		}
	}
*/



	// check for minimum set
	if ( !glConfig.multitextureAvailable || !glConfig.textureEnvCombineAvailable || !glConfig.cubeMapAvailable
		|| !glConfig.envDot3Available ) {
			common->Error( common->GetLanguageDict()->GetString( "#str_06780" ) );
	}

 	// GL_EXT_depth_bounds_test
 	glConfig.depthBoundsTestAvailable = R_CheckExtension( "EXT_depth_bounds_test" );
 	if ( glConfig.depthBoundsTestAvailable ) {
 		qglDepthBoundsEXT = (PFNGLDEPTHBOUNDSEXTPROC)GLimp_ExtensionPointer( "glDepthBoundsEXT" );
 	}

}