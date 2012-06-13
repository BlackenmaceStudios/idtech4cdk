using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace ToolsManaged.Private
{
    unsafe class RenderProgram : NativeClass
    {
        public enum renderProgramParameter
        {
	        PP_LIGHT_ORIGIN = 4,
	        PP_VIEW_ORIGIN,
	        PP_LIGHT_PROJECT_S,
	        PP_LIGHT_PROJECT_T,
	        PP_LIGHT_PROJECT_Q,
	        PP_LIGHT_FALLOFF_S,

	        PP_ATTR_TEXCOORD,
	        PP_ATTR_TANGENT,
	        PP_ATTR_BINORMAL,
	        PP_ATTR_NORMAL,

	        PP_TEX_NORMAL,
	        PP_TEX_LIGHTFALLOFF,
	        PP_TEX_LIGHTPROJECTION,
	        PP_TEX_DIFFUSE,
	        PP_TEX_SPEC,
	        PP_TEX_UNLITTRANSPARENT,
	        PP_COLOR_MODULATE,
	        PP_COLOR_ADD,

	        PP_COLOR_DIFFUSE,
	        PP_COLOR_SPEC,
	        PP_DIFFUSE_MATRIX_S,
	        PP_DIFFUSE_MATRIX_T,

	        VV_TEX_VERTEX,
	        VV_TEX_VERTEX2,
	        VV_TEX_VERTEX3,
	        VV_TEX_VERTEX4,
	        VV_TEX_VERTEX5,
	        VV_TEX_MATVIEW,
	        PP_VIEWMAT,
	        PP_NUMLIGHTS,
	        PP_LIGHTNUM,

	        PP_SHADOW,
	        PP_VT_VTOFFSET,
	        PP_DIFFUSEONLY,
	        PP_TEX_INDEXTEX

        /*
	        PP_BUMP_MATRIX_S,
	        PP_BUMP_MATRIX_T,
	
	        PP_SPECULAR_MATRIX_S,
	        PP_SPECULAR_MATRIX_T,
	

	        PP_LIGHT_FALLOFF_TQ = 20	// only for NV programs
        */
        };

        public delegate void				Bind_t(IntPtr nativeAddress);
        public delegate void UnBind_t(IntPtr nativeAddress);
        public delegate void BindTextureVar_t(IntPtr nativeAddress, uint progVarHandle);
        public delegate void SetVar4fvEx_t(IntPtr nativeAddress,uint progVarHandle, int count, float* ptr);
        public delegate void SetVar3fv_t(IntPtr nativeAddress, uint progVarHandle, int count, float* ptr);
        public delegate void SetVar1fi_t(IntPtr nativeAddress, uint progVarHandle, int var);
        public delegate void SetVar4fv_t(IntPtr nativeAddress, uint progVarHandle, float* ptr);
        public delegate void SetAttrVar4fv_t(IntPtr nativeAddress, uint progVarHandle, float* ptr);
        public delegate void SetVarMatrix4fv_t(IntPtr nativeAddress, uint progVarHandle, int count, float* ptr);
        public delegate void SetCurrentPass_t(IntPtr nativeAddress, int pass);

        [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_RendererDevice_LoadRenderProgram")]
        private static extern IntPtr TOOLAPI_RendererDevice_LoadRenderProgram(string path, int numPasses);

        public static RenderProgram LoadRenderProgram(string path, int numPasses)
        {
            RenderProgram program = new RenderProgram();
            IntPtr _nativeAddress;

            _nativeAddress = TOOLAPI_RendererDevice_LoadRenderProgram(path, numPasses);

            program.AttachToMemory(typeof(RenderProgram), _nativeAddress);

            return program;
        }

        public Bind_t Bind;
        public UnBind_t UnBind;
        public BindTextureVar_t BindTextureVar;
        public SetVar4fvEx_t SetVar4fvEx;
        public SetVar3fv_t SetVar3fv;
        public SetVar1fi_t SetVar1fi;
        public SetVar4fv_t SetVar4fv;
        public SetAttrVar4fv_t SetAttrVar4fv;
        public SetVarMatrix4fv_t SetVarMatrix4fv;
        public SetCurrentPass_t SetCurrentPass;
    }
}
