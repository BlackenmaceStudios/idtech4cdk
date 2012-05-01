// RenderProgram.h
//

#define ID_RNDERPROGS_MAXPASSES			5

typedef unsigned int glRenderProgramHandle_t; // Handle to the GL renderprogram handle.
typedef int glRenderProgramVarHandle_t;
typedef unsigned int glRenderProgramCBufferHandle_t;


#define ID_RENDERPROG_PASSTOKEN(x)  va("ID_RENDERPROGRAM_PASS%d", x + 1)

/*
================================
Render Program

A renderprogram handles all the low level glsl code.
================================
*/

//
// RenderProgramType_t
//
enum RenderProgramType_t {
	PROGRAM_VERTEX,
	PROGRAM_GEOMETRY,
	PROGRAM_TESSALATION,
	PROGRAM_FRAGMENT,

	RENDERPROGRAM_MAXTYPES
};

//
// bmRenderProgramConstantBuffer
//
class bmRenderProgramConstantBuffer {
public:
						bmRenderProgramConstantBuffer() { name = NULL; cbufferHandle = (glRenderProgramCBufferHandle_t)-1; }
	void				Bind( void );
	void				UnBind( void );
	void				Blit( void *buffer, int bufferSize );
	bool				Attach( const char *varName, void *buffer, int bufferSize, glRenderProgramHandle_t progHandle );
	void				Free( void );
	const char *		GetName( void ) { return name; }
private:
	const char *		name;
	
	glRenderProgramVarHandle_t cbufferHandleIndex;
	glRenderProgramCBufferHandle_t cbufferHandle;
};

//
// bmRenderProgram
//
class bmRenderProgram {
public:
						bmRenderProgram( const char *path, int numPasses );
	void				Bind( void );
	void				UnBind( void );
	void				Reload( const char *path, int numPasses );
	void				BindTextureVar( unsigned int progVarHandle );
	void				BindVar( unsigned int progVarHandle, const char *varName );
	void				BindVar( unsigned int progVarHandle, int attrId, const char *varName );
	void				SetVar4fv( unsigned int progVarHandle, int count, const float *ptr );
	void				SetVar3fv( unsigned int progVarHandle, int count, const float *ptr );
	void				SetVar1fi( unsigned int progVarHandle, int var );
	void				SetVar4fv( unsigned int progVarHandle, const float *ptr );
	void				SetAttrVar4fv( unsigned int progVarHandle, const float *ptr );
	void				SetVarMatrix4fv( unsigned int progVarHandle, int count, const float *ptr );
	void				SetCurrentPass( int pass ) { currentPass = pass; }

	bmRenderProgramConstantBuffer *BindConstantBuffer( const char *varName, void *buffer, int bufferSize );
private:
	idStr				FormatShaderError( const char *buffer, const char *errorBuffer );
	idStr				GetErrorLine( int line, const char *buffer );
	void				LoadShader( const char *buffer, unsigned int shadertype );
	void				Free();
	void				ReloadPass( const char *path, int pass );

	idStr				name;

	idStr				ReadSourceFile( const char *path );
	void				PreprocessProgram( idStr &buffer, idStr &vertexProgBuffer, idStr &fragmentProgBuffer, idStr &geometryShader );

	glRenderProgramVarHandle_t	progVars[ID_RNDERPROGS_MAXPASSES][70];
	glRenderProgramHandle_t		programPassHandle[ID_RNDERPROGS_MAXPASSES];
	glRenderProgramHandle_t		handles[RENDERPROGRAM_MAXTYPES];

	int currentPass;
	int numcbuffers;
	bmRenderProgramConstantBuffer cbuffers[20];
};

