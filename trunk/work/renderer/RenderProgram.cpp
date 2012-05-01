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
bmRenderProgram::bmRenderProgram( const char *path ) {
	programHandle = 0;
	numcbuffers = 0;
	memset( &handles, 0, sizeof( unsigned int ) * RENDERPROGRAM_MAXTYPES);
	Reload( path );
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

	if(programHandle > 0) {
		qglDeleteObjectARB( programHandle );
		common->Printf("......Clearing Previous Instance\n" );
	}

	

	numcbuffers = 0;

	memset( &progVars[0], -1, sizeof(glRenderProgramVarHandle_t) * 40 );

	programHandle = 0;
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

	if(!cbuffers[numcbuffers].Attach( varName,buffer, bufferSize, programHandle )) {
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
	qglUniform1iARB( progVars[progVarHandle], var );
}
/*
===================
bmRenderProgram::SetVar3fv
===================
*/
void bmRenderProgram::SetVar3fv( unsigned int progVarHandle, int count, const float *ptr ) {
	qglUniform3fvARB( progVars[progVarHandle], count, ptr );
}

/*
===================
bmRenderProgram::BindTextureVar
===================
*/
void bmRenderProgram::BindTextureVar( unsigned int progVarHandle ) {
	if(progVars[progVarHandle] == -1) {
		//common->FatalError( "SetVar4fv: BindTextureVar == 0");
	}
	qglUniform1iARB( progVars[progVarHandle], backEnd.glState.currenttmu );
}

/*
===================
bmRenderProgram::bmRenderProgram
===================
*/
void bmRenderProgram::Reload( const char *path ) {
	idStr programBuffer;
	idStr vertexProgBuffer, fragmentProgBuffer;

	name = path;

	Free();

	programHandle = qglCreateProgramObjectARB();
	programBuffer = ReadSourceFile( path );

	PreprocessProgram(programBuffer, vertexProgBuffer, fragmentProgBuffer);

	LoadShader( vertexProgBuffer.c_str(), GL_VERTEX_SHADER_ARB );

	BindVar( PP_ATTR_TEXCOORD, 8, "attr_TexCoord" );
	BindVar( PP_ATTR_TANGENT, 9, "attr_Tangent" );
	BindVar( PP_ATTR_BINORMAL, 10, "attr_Binormal" );
	BindVar( PP_ATTR_NORMAL, 11, "attr_Normal" );


	LoadShader( fragmentProgBuffer.c_str(), GL_FRAGMENT_SHADER_ARB);
	qglLinkProgramARB(programHandle);

	

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

	BindVar( PP_SHADOW, "ShadowMap" );
	BindVar( PP_SHADOW1, "ShadowMap1" );
	BindVar( PP_SHADOW2, "ShadowMap2" );
	BindVar( PP_SHADOW3, "ShadowMap3" );
	BindVar( PP_SHADOW4, "ShadowMap4" );
	BindVar( PP_SHADOW5, "ShadowMap5" );
/*
	BindVar( PP_BUMP_MATRIX_S,
	BindVar( PP_BUMP_MATRIX_T,
	BindVar( PP_DIFFUSE_MATRIX_S,
	BindVar( PP_DIFFUSE_MATRIX_T,
	BindVar( PP_SPECULAR_MATRIX_S,
	BindVar( PP_SPECULAR_MATRIX_T,
	BindVar( PP_COLOR_MODULATE,
	BindVar( PP_COLOR_ADD,

	BindVar( PP_LIGHT_FALLOFF_TQ, 
*/
}

/*
===================
bmRenderProgram::SetVar4fv
===================
*/
void bmRenderProgram::SetVar4fv( unsigned int progVarHandle, const float *ptr ) {
	SetVar4fv( progVarHandle, 1, ptr );
}

/*
===================
bmRenderProgram::SetVar4fv
===================
*/
void bmRenderProgram::SetVar4fv( unsigned int progVarHandle, int count, const float *ptr ) {
	if(progVars[progVarHandle] == -1) {
		//common->FatalError( "SetVar4fv: ProgVar == -1");
	}

	qglUniform4fvARB( progVars[progVarHandle], count, ptr );
}

/*
===================
bmRenderProgram::SetVar4fv
===================
*/
void bmRenderProgram::SetVarMatrix4fv( unsigned int progVarHandle, int count, const float *ptr ) {
	if(progVars[progVarHandle] == -1) {
		//common->FatalError( "SetVar4fv: ProgVar == -1");
	}

	qglUniformMatrix4fvARB( progVars[progVarHandle], count, false, ptr );
}


/*
===================
bmRenderProgram::SetAttrVar4fv
===================
*/
void bmRenderProgram::SetAttrVar4fv( unsigned int progVarHandle, const float *ptr ) {
	if(progVars[progVarHandle] == -1) {
		//common->FatalError( "SetVar4fv: ProgVar == -1");
	}

	qglVertexAttrib4fv( progVars[progVarHandle], ptr );
}



/*
===================
bmRenderProgram::LoadShader
===================
*/
void bmRenderProgram::LoadShader( const char *buffer, unsigned int shadertype ) {
	int compiled = 0;
	char str[4096];
	glRenderProgramHandle_t handle;
	
	// Create the GLSL shader handle.
	handle = qglCreateShaderObjectARB(shadertype);


	// Load in the shader sources.
	qglShaderSourceARB(handle, 1, &buffer, NULL);
	qglCompileShaderARB(handle);
	qglGetInfoLogARB( handle, sizeof(str), NULL, str );

	if (strlen( str ) > 0) {
		common->FatalError( "Shader compile error: %s\n", str );
		return;
	}

	qglAttachObjectARB(programHandle, handle);
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
	qglUseProgramObjectARB(programHandle);
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
	progVars[progVarHandle] = qglGetUniformLocationARB( programHandle, varName );
}

/*
===================
bmRenderProgram::BindVar
===================
*/
void bmRenderProgram::BindVar( unsigned int progVarHandle, int attrId, const char *varName ) {
	qglBindAttribLocationARB( programHandle, attrId, varName );
	progVars[progVarHandle] = attrId;
}

/*
===================
bmRenderProgram::PreprocessProgram
===================
*/
void bmRenderProgram::PreprocessProgram( idStr &buffer, idStr &vertexProgBuffer, idStr &fragmentProgBuffer ) {
	idParser parser( buffer.c_str(), buffer.Length(), name.c_str(), LEXFL_NOSTRINGCONCAT );
	idToken token;
	idStr preprocessedStr = "#version 120\n\r #extension GL_EXT_gpu_shader4 : enable\n\r#extension GL_ARB_uniform_buffer_object : enable\n\r#extension GL_ARB_shading_language_420pack: enable\n\r";
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

	//vertexProgBuffer.Replace( "EndProgram", "//" );
	//fragmentProgBuffer.Replace( "EndProgram", "//" );
	vertexProgBuffer.Replace( "EndFragmentProgram", "*/" );
	vertexProgBuffer.Replace( "FragmentProgram", "/*" );
	vertexProgBuffer.Replace( "EndVertexProgram", " " );
	vertexProgBuffer.Replace( "VertexProgram", " " );
	
	fragmentProgBuffer.Replace( "EndVertexProgram", "*/" );
	fragmentProgBuffer.Replace( "VertexProgram", "/*" );
	fragmentProgBuffer.Replace( "EndFragmentProgram", " " );
	fragmentProgBuffer.Replace( "FragmentProgram", " " );
	
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