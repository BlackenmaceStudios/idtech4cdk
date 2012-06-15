// RenderDeviceGL.cpp
//

#include "precompiled.h"

#include "tr_local.h"
#include "../sys/win32/win_local.h"

//
// bmOpenGLRenderDevice
//
class bmOpenGLRenderDevice : public bmRenderDevice {
public:
	virtual void			Init( void );
	virtual bool			BindDeviceToWindow( void *windowHandle );
	virtual void			SwapBuffers( void *windowHandle );
	virtual void			SetDeviceState( int state );
	virtual void			SetDevice2DState( void );
	virtual void			ToggleSMPFrame( void );
	virtual void			DeriveLightData(class idRenderLightLocal *l) { R_DeriveLightData( l ); }
	virtual void			ForceDeviceSync( void );
	virtual void			BeginDeviceSync( void ) { sync = qglFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);  };
	
	virtual void			Set2DViewMatrix( void );
	virtual void			DrawImage(float x, float y, float w, float h, float s1, float t1, float s2, float t2, idImage *image);
	virtual void			RestoreViewMatrix( void );
	virtual void			RenderSurfaceFromCache( const srfTriangles_t *tris );
	virtual void			RenderSurfaceFromVertexCache( const srfTriangles_t *tris );
	virtual bmRenderProgram			*LoadRenderProgram( const char *path, int numPasses );
	virtual void			SelectTextureNoClient( int unit ) { GL_SelectTextureNoClient( unit ); };
private:
	GLsync sync;
};

 #define MTS_SYNC_TIMEOUT 100000000  // 100ms

bmOpenGLRenderDevice glRenderDeviceLocal;
bmRenderDevice		*renderDevice = &glRenderDeviceLocal;

void R_ToggleSmpFrame( void );

/*
====================
bmOpenGLRenderDevice::LoadRenderProgram
====================
*/
bmRenderProgram	* bmOpenGLRenderDevice::LoadRenderProgram( const char *path, int numPasses ) {
	bmRenderProgram	*program = new bmRenderProgram( path, numPasses );
	return program;
}

/*
====================
bmOpenGLRenderDevice::Init
====================
*/
void bmOpenGLRenderDevice::Init( void ) {

}

/*
====================
bmOpenGLRenderDevice::RenderSurfaceFromCache
====================
*/
extern bmRenderProgram *activeRenderProgram;
void bmOpenGLRenderDevice::RenderSurfaceFromCache( const srfTriangles_t *tris ) {
	if(tris->ambientCache == NULL)
	{
		if(!R_CreateAmbientCache( (srfTriangles_t *)tris, false )) {
			common->FatalError("RenderSurfaceFromCache: Failed to create cache for tris\n");
		}
	}

	idDrawVert	*ac = (idDrawVert *)vertexCache.Position( tris->ambientCache );
	qglColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( idDrawVert ), ac->color );
	qglTexCoordPointer(  2, GL_FLOAT, sizeof( idDrawVert ), ac->st.ToFloatPtr() );
	qglVertexPointer( 3, GL_FLOAT, sizeof( idDrawVert ), ac->xyz.ToFloatPtr() );

	RB_DrawElementsWithCounters( tris );
}

/*
====================
bmOpenGLRenderDevice::RenderSurfaceFromCache
====================
*/
void bmOpenGLRenderDevice::RenderSurfaceFromVertexCache( const srfTriangles_t *tris ) {
	if(tris->ambientCache == NULL)
	{
		if(!R_CreateAmbientCache( (srfTriangles_t *)tris, false )) {
			common->FatalError("RenderSurfaceFromCache: Failed to create cache for tris\n");
		}
	}

	idDrawVert	*ac = (idDrawVert *)vertexCache.Position( tris->ambientCache );
	qglColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( idDrawVert ), ac->color );
	qglVertexAttribPointerARB( 8, 2, GL_FLOAT, false, sizeof( idDrawVert ), ac->st.ToFloatPtr() );
	qglVertexPointer( 3, GL_FLOAT, sizeof( idDrawVert ), ac->xyz.ToFloatPtr() );

	RB_DrawElementsWithCounters( tris );
}

/*
====================
bmOpenGLRenderDevice::Set2DViewMatrix
====================
*/
void bmOpenGLRenderDevice::Set2DViewMatrix( void ) {
	backEnd.depthFunc = GLS_DEPTHFUNC_ALWAYS;
	
	GL_Cull( CT_TWO_SIDED );	// so mirror views also get it
	//GL_State( GLS_SRCBLEND_DST_COLOR | GLS_DSTBLEND_ZERO );


}

/*
====================
bmOpenGLRenderDevice::RestoreViewMatrix
====================
*/
void bmOpenGLRenderDevice::RestoreViewMatrix( void ) {
	backEnd.depthFunc = GLS_DEPTHFUNC_LESS;
	
	GL_Cull( CT_FRONT_SIDED );
}

/*
====================
bmOpenGLRenderDevice::DrawImage
====================
*/
void bmOpenGLRenderDevice::DrawImage(float x, float y, float w, float h, float s1, float t1, float s2, float t2,idImage *image) {
	qglDrawArrays(GL_POINTS, 0, 1);

}

/*
====================
bmOpenGLRenderDevice::ForceDeviceSync
====================
*/
void bmOpenGLRenderDevice::ForceDeviceSync( void ) {
	qglFlush();
    GLenum syncResult = qglClientWaitSync(sync,GL_SYNC_FLUSH_COMMANDS_BIT, -1);
	while( true ) {
		switch (syncResult) { 
    			case GL_ALREADY_SIGNALED: 
     			case GL_CONDITION_SATISFIED: 
					qglDeleteSync(sync);
     				return; 
				case GL_TIMEOUT_EXPIRED:
					qglDeleteSync(sync);
					return;
     			case GL_WAIT_FAILED: 
					common->Warning("ForceDeviceSync: Wait failed...\n");
					qglDeleteSync(sync);
     				return;
				default:
					common->Warning("ForceDeviceSync: Unknown sync failure...\n");
					qglDeleteSync(sync);
					return;

		}
		syncResult = qglClientWaitSync(sync,GL_SYNC_FLUSH_COMMANDS_BIT, -1);
	}
	qglDeleteSync(sync);
}

/*
====================
bmOpenGLRenderDevice::ToggleSMPFrame
====================
*/
void bmOpenGLRenderDevice::ToggleSMPFrame( void ) {
	R_ToggleSmpFrame();
}

/*
====================
bmOpenGLRenderDevice::SetDevice2DState
====================
*/
void bmOpenGLRenderDevice::SetDevice2DState( void ) {
	RB_SetGL2D();
}

/*
====================
bmOpenGLRenderDevice::SetDeviceState
====================
*/
void bmOpenGLRenderDevice::SetDeviceState( int state ) {
	GL_State( state );
}

/*
====================
bmOpenGLRenderDevice::SwapBuffers
====================
*/
void bmOpenGLRenderDevice::SwapBuffers( void *windowHandle ) {
	qwglSwapBuffers( (HDC)windowHandle );
}

/*
====================
bmOpenGLRenderDevice::BindDeviceToWindow
====================
*/
bool bmOpenGLRenderDevice::BindDeviceToWindow( void *windowHandle ) {

	if(windowHandle)
	{
		if (!qwglMakeCurrent((HDC)windowHandle, win32.hGLRC)) {
			common->Warning("BindDeviceToWindow failed, the viewport will not be rendered correctly\n");
			return false;
		}
	}
	else {
		qwglMakeCurrent(win32.hDC, win32.hGLRC);
	}

	return true;
}