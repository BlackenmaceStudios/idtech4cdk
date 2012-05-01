// tr_glsl.cpp
//

#include "precompiled.h"
#pragma hdrstop

#include "tr_local.h"

bmRenderProgram *activeRenderProgram = NULL;

/*
===================
bmRenderProgram::bmRenderProgram
===================
*/
bmRenderProgram::bmRenderProgram( const char *path, int numPasses ) {
	numcbuffers = 0;
	memset( &handles, 0, sizeof( unsigned int ) * RENDERPROGRAM_MAXTYPES);

	currentPass = 0;
	for(int i = 0; i < ID_RNDERPROGS_MAXPASSES; i++)
	{
		programPassHandle[i] = 0;
	}

	Reload( path, numPasses );

	
}
/*
===================
bmRenderProgram::Free

===================
*/
void bmRenderProgram::Free( void ) {
	for(int i = 0; i < RENDERPROGRAM_MAXTYPES; i++ ) {
		if(handles[i] > 0) {
			qglDeleteObjectARB( handles[i] );
		}

		handles[i] = 0;
	}

	for(int i = 0; i < numcbuffers; i++)
	{
		cbuffers[i].Free();
	}

	for(int i = 0; i < ID_RNDERPROGS_MAXPASSES; i++)
	{
		if(programPassHandle[i] > 0) {
			common->Printf("......Clearing pass handle %d\n",programPassHandle[i] );
			qglDeleteObjectARB( programPassHandle[i] );
			programPassHandle[i] = 0;
			memset( &progVars[i][0], -1, sizeof(glRenderProgramVarHandle_t) * 70 );
		}
	}

	currentPass = 0;

	numcbuffers = 0;

	

	
}

/*
===================
bmRenderProgram::BindConstantBuffer
===================
*/
bmRenderProgramConstantBuffer *bmRenderProgram::BindConstantBuffer( const char *varName, void *buffer, int bufferSize ) {
	for(int i = 0; i < numcbuffers; i++) {
		if( !idStr::Cmp(cbuffers[i].GetName(), varName)) {
			return &cbuffers[i];
		}
	}

	if(!cbuffers[numcbuffers].Attach( varName,buffer, bufferSize, programPassHandle[currentPass] )) {
		return NULL;
	}

	return &cbuffers[numcbuffers++];
}

/*
===================
bmRenderProgram::SetVar1fi
===================
*/
void bmRenderProgram::SetVar1fi( unsigned int progVarHandle, int var ) {
	if(progVars[currentPass][progVarHandle] == -1) {
		return;
	}
	qglUniform1iARB( progVars[currentPass][progVarHandle], var );
}
/*
===================
bmRenderProgram::SetVar3fv
===================
*/
void bmRenderProgram::SetVar3fv( unsigned int progVarHandle, int count, const float *ptr ) {
	if(progVars[currentPass][progVarHandle] == -1) {
		return;
	}
	qglUniform3fvARB( progVars[currentPass][progVarHandle], count, ptr );
}

/*
===================
bmRenderProgram::BindTextureVar
===================
*/
void bmRenderProgram::BindTextureVar( unsigned int progVarHandle ) {
	if(progVars[currentPass][progVarHandle] == -1) {
		return;
	}
	qglUniform1iARB( progVars[currentPass][progVarHandle], backEnd.glState.currenttmu );
}

/*
===================
bmRenderProgram::ReloadPass
===================
*/
void bmRenderProgram::ReloadPass( const char *path, int pass ) {
	idStr programBuffer;
	idStr vertexProgBuffer, fragmentProgBuffer, geometryProgBuffer;

	name = path;

	if(pass >= 1) {
		
		pass = pass;
		
	}

	SetCurrentPass( pass );

	programPassHandle[currentPass] = qglCreateProgramObjectARB();
	programBuffer = ReadSourceFile( path );
	GL_CheckErrors(true);

	PreprocessProgram(programBuffer, vertexProgBuffer, fragmentProgBuffer, geometryProgBuffer);
	GL_CheckErrors(true);

	LoadShader( vertexProgBuffer.c_str(), GL_VERTEX_SHADER_ARB );

	GL_CheckErrors(true);

	BindVar( PP_ATTR_TEXCOORD, 8, "attr_TexCoord" );
	BindVar( PP_ATTR_TANGENT, 9, "attr_Tangent" );
	BindVar( PP_ATTR_BINORMAL, 10, "attr_Binormal" );
	BindVar( PP_ATTR_NORMAL, 11, "attr_Normal" );


	LoadShader( fragmentProgBuffer.c_str(), GL_FRAGMENT_SHADER_ARB);

	if(geometryProgBuffer.Length() > 0) {
		LoadShader( geometryProgBuffer.c_str(), GL_GEOMETRY_SHADER_ARB);
	}
	qglLinkProgramARB(programPassHandle[currentPass]);

	GL_CheckErrors(true);



	// Try to bind all the default vars.
	BindVar( PP_LIGHT_ORIGIN, "r_lightOrigin" );
	BindVar( PP_VIEW_ORIGIN, "u_viewOrigin" );
	BindVar( PP_LIGHT_PROJECT_S, "r_lightProjectionS" );
	BindVar( PP_LIGHT_PROJECT_T, "r_lightProjectionT" );
	BindVar( PP_LIGHT_PROJECT_Q, "r_lightProjectionQ" );
	BindVar( PP_LIGHT_FALLOFF_S, "r_lightFalloff" );

	BindVar( PP_COLOR_MODULATE, "u_colorModulate" );
	BindVar( PP_COLOR_ADD, "u_colorAdd" );

	BindVar( PP_COLOR_DIFFUSE, "r_diffuseColor" );
	BindVar( PP_COLOR_SPEC, "r_specularColor" );

	
	BindVar( PP_TEX_NORMAL, "u_normalTexture" );
	BindVar( PP_TEX_LIGHTFALLOFF, "u_lightFalloffTexture" );
	BindVar( PP_TEX_LIGHTPROJECTION, "u_lightProjectionTexture" );
	BindVar( PP_TEX_DIFFUSE, "u_diffuseTexture" );
	BindVar( PP_TEX_SPEC, "u_specularTexture" );

	BindVar(PP_DIFFUSE_MATRIX_S, "u_diffuseMatrixS" );
	BindVar(PP_DIFFUSE_MATRIX_T, "u_diffuseMatrixT" );
	
	BindVar( VV_TEX_VERTEX, "u_vertTexture" );
	BindVar( VV_TEX_VERTEX2, "u_vertTexture2" );
	BindVar( VV_TEX_VERTEX3, "u_vertTexture3" );
	BindVar( VV_TEX_VERTEX4, "u_vertTexture4" );
	BindVar( VV_TEX_VERTEX5, "u_vertTexture5" );

	BindVar( VV_TEX_MATVIEW, "u_matview" );
	BindVar( PP_VIEWMAT, "u_viewmat" );
	BindVar( PP_NUMLIGHTS, "numLights" );
	BindVar( PP_LIGHTNUM, "lightNum" );

	BindVar( PP_SHADOW, "ShadowMap" );


	BindVar( PP_TEX_UNLITTRANSPARENT, "u_unlitTransparentTexture" );

	BindVar( PP_VT_VTOFFSET,"vtOffset");
	BindVar( PP_DIFFUSEONLY, "diffuseOnly");
	BindVar( PP_TEX_INDEXTEX, "u_indexTexture" );

	GL_CheckErrors(true);
}

/*
===================
bmRenderProgram::bmRenderProgram
===================
*/
void bmRenderProgram::Reload( const char *path, int numPasses ) {
	Free();

	for(int i = 0; i < numPasses; i++) {
		ReloadPass( path, i );
	}

	SetCurrentPass( 0 );
}

/*
===================
bmRenderProgram::SetVar4fv
===================
*/
void bmRenderProgram::SetVar4fv( unsigned int progVarHandle, const float *ptr ) {
	if(progVars[currentPass][progVarHandle] == -1) {
		return;
	}
	SetVar4fv( progVarHandle, 1, ptr );
}

/*
===================
bmRenderProgram::SetVar4fv
===================
*/
void bmRenderProgram::SetVar4fv( unsigned int progVarHandle, int count, const float *ptr ) {
	if(progVars[currentPass][progVarHandle] == -1) {
		return;
	}

	qglUniform4fvARB( progVars[currentPass][progVarHandle], count, ptr );
}

/*
===================
bmRenderProgram::SetVar4fv
===================
*/
void bmRenderProgram::SetVarMatrix4fv( unsigned int progVarHandle, int count, const float *ptr ) {
	if(progVars[currentPass][progVarHandle] == -1) {
		return;
	}

	qglUniformMatrix4fvARB( progVars[currentPass][progVarHandle], count, false, ptr );
}


/*
===================
bmRenderProgram::SetAttrVar4fv
===================
*/
void bmRenderProgram::SetAttrVar4fv( unsigned int progVarHandle, const float *ptr ) {
	if(progVars[currentPass][progVarHandle] == -1) {
		return;
	}

	qglVertexAttrib4fv( progVars[currentPass][progVarHandle], ptr );
}



/*
===================
bmRenderProgram::LoadShader
===================
*/
void bmRenderProgram::LoadShader( const char *buffer, unsigned int shadertype ) {
	int compiled = 0;
	char str[4096];
	idStr shaderBuffer = buffer;
	glRenderProgramHandle_t handle;
	
	// Create the GLSL shader handle.
	handle = qglCreateShaderObjectARB(shadertype);


	// Load in the shader sources.
	qglShaderSourceARB(handle, 1, &buffer, NULL);
	qglCompileShaderARB(handle);
	qglGetInfoLogARB( handle, sizeof(str), NULL, str );

	if (strlen( str ) > 0 && strstr(str, "error")) {
		idStr buffer = FormatShaderError( shaderBuffer.c_str(), str );
		common->FatalError( "LoadShader(%s-%d): %s\n", name.c_str(), currentPass + 1, buffer.c_str() );
		return;
	}

	qglAttachObjectARB(programPassHandle[currentPass], handle);
}

/*
===================
bmRenderProgram::FormatShaderError
===================
*/
idStr bmRenderProgram::FormatShaderError( const char *buffer, const char *errorBuffer ) {
	idStr finalBuffer;
	const char *buferpos = errorBuffer;
	int i = 0, d= 0;

	while( true ) {
		const char *token = strstr( buferpos, "(" );
		const char *token2 = strstr( buferpos, ")" );
		char temp[5];

		i = 0;

		if(token == NULL)
			break;

		for(i = (int)token, d=0; i < (int)token2; i++, d++) {
			temp[d] = token[d+ 1];
		}
		temp[d] = 0;
		buferpos = token2 + 1;
		finalBuffer += GetErrorLine( atoi(temp), buffer );
		finalBuffer += "\n\r";
	}

	idStr errStr = "Shader Compiler Error:\n\rError Line Data;\n\r" + finalBuffer;
	errStr += "\n\rGPU Error(s)\n\r";
	errStr += errorBuffer;


	return errStr;
}

/*
===================
bmRenderProgram::GetErrorLine
===================
*/
idStr bmRenderProgram::GetErrorLine( int line, const char *buffer ) {
	int currentLine = 0;
	const char *buf = (char*)buffer;
	for(int i = 0; i < strlen(buffer); i++) {
		if(currentLine >= line) {
			buf += i;
			break;
		}

		if(buffer[i] == '\n' || buffer[i] == '\r') {
			currentLine++;
		}
	}

	idStr finalBuffer = buf;

	for(int i = 0; i < finalBuffer.Length(); i++) {
		if(buf[i] == '\n' || buf[i] == '\r') {
			finalBuffer[i] = 0;
			break;
		}
	}

	
	return &finalBuffer[0];
}

/*
===================
bmRenderProgram::Bind
===================
*/
void bmRenderProgram::Bind( void ) {
	if(activeRenderProgram != NULL) {
		common->FatalError("RenderProgram::Bind: Program already bound");
	}
	activeRenderProgram = this;
	qglUseProgramObjectARB(programPassHandle[currentPass]);
}

/*
===================
bmRenderProgram::UnBind
===================
*/
void bmRenderProgram::UnBind( void ) {
	activeRenderProgram = NULL;
	qglUseProgramObjectARB(NULL);
}

/*
===================
bmRenderProgram::BindVar
===================
*/
void bmRenderProgram::BindVar( unsigned int progVarHandle, const char *varName ) {
	progVars[currentPass][progVarHandle] = qglGetUniformLocationARB( programPassHandle[currentPass], varName );
}

/*
===================
bmRenderProgram::BindVar
===================
*/
void bmRenderProgram::BindVar( unsigned int progVarHandle, int attrId, const char *varName ) {
	qglBindAttribLocationARB( programPassHandle[currentPass], attrId, varName );
	progVars[currentPass][progVarHandle] = attrId;
}

/*
===================
bmRenderProgram::PreprocessProgram
===================
*/

#define GLSL_EXTENSIONS  "#extension GL_EXT_gpu_shader4 : enable\n\r#extension GL_ARB_uniform_buffer_object : enable\n\r#extension GL_ARB_shading_language_420pack: enable\n\r#extension GL_ARB_texture_rectangle : enable\n\r";
void bmRenderProgram::PreprocessProgram( idStr &buffer, idStr &vertexProgBuffer, idStr &fragmentProgBuffer, idStr &geometryShaderBuffer ) {
	idParser parser( buffer.c_str(), buffer.Length(), name.c_str(), LEXFL_NOSTRINGCONCAT );
	idToken token;
	idStr preprocessedStr;

	// Support for legacy tokens.
	if(strstr( buffer.c_str(), "varying")) {
		preprocessedStr = "#version 120\n\r";
	}
	else {
		preprocessedStr = "#version 330 compatibility\n\r";
	}

	preprocessedStr += GLSL_EXTENSIONS;

	// If we are on greater than pass 1 ensure its defined in the shader.
	if(currentPass > 1) {
		if(strstr( buffer.c_str(),ID_RENDERPROG_PASSTOKEN( currentPass ) ) == NULL) {
			common->FatalError( "Program missing pass %d\n", currentPass + 1);
		}
	}

	// Support for additional passes.
	parser.AddDefine(ID_RENDERPROG_PASSTOKEN( currentPass ));

	//bool valid = false;

	parser.AddDefine( "BM_RENDERPROG" );

	if(!r_deferredRenderer.GetBool()) {
		parser.AddDefine( "DOOM_FORWARD_RENDERER" );
	}

	// Parse through the program.
	while(parser.ReadToken( &token )) {
		preprocessedStr += token;
		if(token.linesCrossed) {
			preprocessedStr += "\n";
		}
		else {
			preprocessedStr += " ";
		}
	}

	vertexProgBuffer = preprocessedStr;
	fragmentProgBuffer = preprocessedStr;
	geometryShaderBuffer = preprocessedStr;

	//vertexProgBuffer.Replace( "EndProgram", "//" );
	//fragmentProgBuffer.Replace( "EndProgram", "//" );
	vertexProgBuffer.Replace( "EndFragmentProgram", "*/" );
	vertexProgBuffer.Replace( "FragmentProgram", "/*" );
	vertexProgBuffer.Replace( "EndVertexProgram", " " );
	vertexProgBuffer.Replace( "VertexProgram", " " );
	vertexProgBuffer.Replace( "EndGeometryShader", "*/" );
	vertexProgBuffer.Replace( "GeometryShader", "/*" );
	
	fragmentProgBuffer.Replace( "EndVertexProgram", "*/" );
	fragmentProgBuffer.Replace( "VertexProgram", "/*" );
	fragmentProgBuffer.Replace( "EndFragmentProgram", " " );
	fragmentProgBuffer.Replace( "FragmentProgram", " " );
	fragmentProgBuffer.Replace( "EndGeometryShader", "*/" );
	fragmentProgBuffer.Replace( "GeometryShader", "/*" );
	
	if(strstr(geometryShaderBuffer.c_str(),"GeometryShader"))
	{
		geometryShaderBuffer.Replace( "#version 120", "#version 150 compatibility" );
		geometryShaderBuffer.Replace( "EndVertexProgram", "*/" );
		geometryShaderBuffer.Replace( "VertexProgram", "/*" );
		geometryShaderBuffer.Replace( "EndFragmentProgram", "*/" );
		geometryShaderBuffer.Replace( "FragmentProgram", "/*" );
		geometryShaderBuffer.Replace( "EndGeometryShader", " " );
		geometryShaderBuffer.Replace( "GeometryShader", " " );
	}
	else {
		geometryShaderBuffer = "";
	}
	
}

/*
===================
bmRenderProgram::ReadSourceFile
===================
*/
idStr bmRenderProgram::ReadSourceFile( const char *path ) {
	idStr srcBuffer;
	char *rawBuffer;

	if(fileSystem->ReadFile( va("renderprogs/%s", path), (void **)&rawBuffer ) <= 0) {
		common->FatalError( "bmRenderProgram::ReadSourceFile: Failed to open %s\n", path );
	}

	srcBuffer = rawBuffer;

	fileSystem->FreeFile( rawBuffer );

	return srcBuffer;
}