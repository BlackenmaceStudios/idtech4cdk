//RenderDevice.h
//

//
// bmRenderDevice
//
class bmRenderDevice {
public:
	virtual void			Init( void ) = 0;
	virtual bool			BindDeviceToWindow( void *windowHandle ) = 0; 
	virtual void			SwapBuffers( void *windowHandle ) = 0;
	virtual void			SetDeviceState( int state ) = 0;
	virtual void			SetDevice2DState( void ) = 0;
	virtual void			DeriveLightData(class idRenderLightLocal *) = 0;
	virtual void			ToggleSMPFrame( void ) = 0;
};

extern bmRenderDevice		*renderDevice;