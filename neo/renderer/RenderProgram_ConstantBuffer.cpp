// RenderProgram_ConstantBuffer.cpp
//

#include "precompiled.h"

#include "tr_local.h"

/*
===========================
bmRenderProgramConstantBuffer::Attach
===========================
*/
bool bmRenderProgramConstantBuffer::Attach( const char *varName, void *buffer, int bufferSize, glRenderProgramHandle_t progHandle ) {
	/*
	// Attach to the program constant buffer.
	cbufferHandleIndex = qglGetUniformBlockIndex(progHandle, varName );
	if(cbufferHandle < 0) {
		return false;
	}

	qglGenBuffersARB( 1, &cbufferHandle );

	Bind();
	qglUniformBlockBinding( progHandle, cbufferHandleIndex, 0 );

	Blit( NULL, bufferSize );

	UnBind();
	*/
	return false;
}

/*
===================
bmRenderProgramConstantBuffer::Blit
===================
*/
void bmRenderProgramConstantBuffer::Blit( void *buffer, int bufferSize ) {
//	qglBufferDataARB(GL_UNIFORM_BUFFER, bufferSize, buffer, GL_DYNAMIC_DRAW);
}

/*
===================
bmRenderProgramConstantBuffer::Bind
===================
*/
void bmRenderProgramConstantBuffer::Bind( void ) {
	assert( cbufferHandle > 0);
	//qglBindBufferBase(GL_UNIFORM_BUFFER, 0, cbufferHandle);

}

/*
===================
bmRenderProgramConstantBuffer::UnBind
===================
*/
void bmRenderProgramConstantBuffer::UnBind( void ) {
	//qglBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);
}

/*
===========================
bmRenderProgramConstantBuffer::Free
===========================
*/
void bmRenderProgramConstantBuffer::Free( void ) {
	if(cbufferHandle > 0) {
		//qglDeleteBuffersARB( 1, &cbufferHandle );
		cbufferHandle = -1;
	}
}