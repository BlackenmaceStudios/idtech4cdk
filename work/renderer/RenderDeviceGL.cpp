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
};

bmOpenGLRenderDevice glRenderDeviceLocal;
bmRenderDevice		*renderDevice = &glRenderDeviceLocal;

void R_ToggleSmpFrame( void );

/*
====================
bmOpenGLRenderDevice::Init
====================
*/
void bmOpenGLRenderDevice::Init( void ) {

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
		qwglMakeCurrent(NULL, NULL);
	}

	return true;
}