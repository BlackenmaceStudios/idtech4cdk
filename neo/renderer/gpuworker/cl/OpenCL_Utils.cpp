// OpenCL_Utils.cpp
//

#include <precompiled.h>

#include "cl_utils.h"

#define HDASHLINE "-----------------------------------------------------------\n"

cl_context g_clDeviceContext;
cl_command_queue g_clDeviceQueue;

cl_context oclGetGlobalContext( ) {
  return g_clDeviceContext;
}

cl_command_queue oclGetGlobalQueue( ) {
  return g_clDeviceQueue;
}

//////////////////////////////////////////////////////////////////////////////
//! Gets the platform ID for NVIDIA if available, otherwise default
//!
//! @return the id
//! @param clSelectedPlatformID         OpenCL platoform ID
//////////////////////////////////////////////////////////////////////////////
cl_int oclGetPlatformID(cl_platform_id* clSelectedPlatformID)
{
  char chBuffer[1024];
  cl_uint num_platforms;
  cl_platform_id* clPlatformIDs;
  cl_int ciErrNum;
  *clSelectedPlatformID = NULL;

  // Get OpenCL platform count
  ciErrNum = clGetPlatformIDs (0, NULL, &num_platforms);
  if (ciErrNum != CL_SUCCESS)
  {
    common->FatalError(" Error %i in clGetPlatformIDs Call !!!\n\n", ciErrNum);
    return -1000;
  }
  else
  {
    if(num_platforms == 0)
    {
      common->FatalError("No OpenCL platform found!\n\n");
      return -2000;
    }
    else
    {
      // if there's a platform or more, make space for ID's
      if ((clPlatformIDs = (cl_platform_id*)malloc(num_platforms * sizeof(cl_platform_id))) == NULL)
      {
        common->FatalError("Failed to allocate memory for cl_platform ID's!\n\n");
        return -3000;
      }

      // get platform info for each platform and trap the NVIDIA platform if found
      ciErrNum = clGetPlatformIDs (num_platforms, clPlatformIDs, NULL);
      for(cl_uint i = 0; i < num_platforms; ++i)
      {
        ciErrNum = clGetPlatformInfo (clPlatformIDs[i], CL_PLATFORM_NAME, 1024, &chBuffer, NULL);
        if(ciErrNum == CL_SUCCESS)
        {
          if(strstr(chBuffer, "NVIDIA") != NULL)
          {
            *clSelectedPlatformID = clPlatformIDs[i];
            break;
          }
        }
      }

      // default to zeroeth platform if NVIDIA not found
      if(*clSelectedPlatformID == NULL)
      {
        common->Warning("NVIDIA OpenCL platform not found - defaulting to first platform!\n\n");
        *clSelectedPlatformID = clPlatformIDs[0];
      }

      free(clPlatformIDs);
    }
  }

  return CL_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
//! Get the first device associated with the given context
//!
//! @return the first device id associated with the context
//! @param cxGPUContext the context with which the device is associated
//////////////////////////////////////////////////////////////////////////////
cl_device_id oclGetFirstDev(cl_context cxGPUContext)
{
    size_t szParmDataBytes;
    cl_device_id* cdDevices;

    // get the list of GPU devices associated with context
    clGetContextInfo(cxGPUContext, CL_CONTEXT_DEVICES, 0, NULL, &szParmDataBytes);
    cdDevices = (cl_device_id*) malloc(szParmDataBytes);

    clGetContextInfo(cxGPUContext, CL_CONTEXT_DEVICES, szParmDataBytes, cdDevices, NULL);

    cl_device_id first = cdDevices[0];
    free(cdDevices);

    return first;
}

//////////////////////////////////////////////////////////////////////////////
//! Initializes the global context and command queue
//////////////////////////////////////////////////////////////////////////////
void oclInit( ) {
  cl_platform_id cpPlatform;
  cl_device_id cdDevice;
  cl_int ciErrNum;

  ciErrNum = oclGetPlatformID(&cpPlatform);
  oclCheckError(ciErrNum, CL_SUCCESS);

  ciErrNum = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &cdDevice, NULL);
  oclCheckError(ciErrNum, CL_SUCCESS);

  g_clDeviceContext = clCreateContext(0, 1, &cdDevice, NULL, NULL, &ciErrNum);
  oclCheckError(ciErrNum, CL_SUCCESS);

  g_clDeviceQueue = clCreateCommandQueue(g_clDeviceContext, cdDevice, 0, &ciErrNum);
  oclCheckError(ciErrNum, CL_SUCCESS);
}

//////////////////////////////////////////////////////////////////////////////
//! Releases the global context and command queue
//!
//! @param clContext     the context to release
//! @param clQueue       the command queue to release
//////////////////////////////////////////////////////////////////////////////
void oclShutdown(cl_context clContext, cl_command_queue clQueue) {
  cl_int ciErrNum;
  ciErrNum  = clReleaseCommandQueue(clQueue);
  ciErrNum |= clReleaseContext(clContext);
  oclCheckError(ciErrNum, CL_SUCCESS);
}

//////////////////////////////////////////////////////////////////////////////
//! Loads a Program file and prepends the cPreamble to the code.
//!
//! @return the source string if succeeded, 0 otherwise
//! @param cFilename        program filename
//! @param cPreamble        code that is prepended to the loaded file, typically a set of #defines or a header
//! @param szFinalLength    returned length of the code string
//////////////////////////////////////////////////////////////////////////////
char* oclLoadProgSource(const char* cFilename, const char* cPreamble, size_t* szFinalLength)
{
  // locals
  FILE* pFileStream = NULL;
  size_t szSourceLength;

  // open the OpenCL source code file
#ifdef _WIN32   // Windows version
  if(fopen_s(&pFileStream, cFilename, "rb") != 0)
  {
    return NULL;
  }
#else           // Linux version
  pFileStream = fopen(cFilename, "rb");
  if(pFileStream == 0)
  {
    return NULL;
  }
#endif

  size_t szPreambleLength = strlen(cPreamble);

  // get the length of the source code
  fseek(pFileStream, 0, SEEK_END);
  szSourceLength = ftell(pFileStream);
  fseek(pFileStream, 0, SEEK_SET);

  // allocate a buffer for the source code string and read it in
  char* cSourceString = (char *)malloc(szSourceLength + szPreambleLength + 1);
  memcpy(cSourceString, cPreamble, szPreambleLength);
  if (fread((cSourceString) + szPreambleLength, szSourceLength, 1, pFileStream) != 1)
  {
    fclose(pFileStream);
    free(cSourceString);
    return 0;
  }

  // close the file and return the total length of the combined (preamble + source) string
  fclose(pFileStream);
  if(szFinalLength != 0)
  {
    *szFinalLength = szSourceLength + szPreambleLength;
  }
  cSourceString[szSourceLength + szPreambleLength] = '\0';

  return cSourceString;
}

//////////////////////////////////////////////////////////////////////////////
//! Get the binary (PTX) of the program associated with the device
//!
//! @param cpProgram    OpenCL program
//! @param cdDevice     device of interest
//! @param binary       returned code
//! @param length       length of returned code
//////////////////////////////////////////////////////////////////////////////
void oclGetProgBinary( cl_program cpProgram, cl_device_id cdDevice, char** binary, size_t* length)
{
  // Grab the number of devices associated witht the program
  cl_uint num_devices;
  clGetProgramInfo(cpProgram, CL_PROGRAM_NUM_DEVICES, sizeof(cl_uint), &num_devices, NULL);

  // Grab the device ids
  cl_device_id* devices = (cl_device_id*) malloc(num_devices * sizeof(cl_device_id));
  clGetProgramInfo(cpProgram, CL_PROGRAM_DEVICES, num_devices * sizeof(cl_device_id), devices, 0);

  // Grab the sizes of the binaries
  size_t* binary_sizes = (size_t*)malloc(num_devices * sizeof(size_t));
  clGetProgramInfo(cpProgram, CL_PROGRAM_BINARY_SIZES, num_devices * sizeof(size_t), binary_sizes, NULL);

  // Now get the binaries
  char** ptx_code = (char**) malloc(num_devices * sizeof(char*));
  for( unsigned int i=0; i<num_devices; ++i) {
    ptx_code[i]= (char*)malloc(binary_sizes[i]);
  }
  clGetProgramInfo(cpProgram, CL_PROGRAM_BINARIES, 0, ptx_code, NULL);

  // Find the index of the device of interest
  unsigned int idx = 0;
  while( idx<num_devices && devices[idx] != cdDevice ) ++idx;

  // If it is associated prepare the result
  if( idx < num_devices )
  {
    *binary = ptx_code[idx];
    *length = binary_sizes[idx];
  }

  // Cleanup
  free( devices );
  free( binary_sizes );
  for( unsigned int i=0; i<num_devices; ++i) {
    if( i != idx ) free(ptx_code[i]);
  }
  free( ptx_code );
}

//////////////////////////////////////////////////////////////////////////////
//! Get and log the binary (PTX) from the OpenCL compiler for the requested program & device
//!
//! @param cpProgram                   OpenCL program
//! @param cdDevice                    device of interest
//! @param const char*  cPtxFileName   optional PTX file name
//////////////////////////////////////////////////////////////////////////////
void oclLogPtx(cl_program cpProgram, cl_device_id cdDevice, const char* cPtxFileName)
{
  // Grab the number of devices associated with the program
  cl_uint num_devices;
  clGetProgramInfo(cpProgram, CL_PROGRAM_NUM_DEVICES, sizeof(cl_uint), &num_devices, NULL);

  // Grab the device ids
  cl_device_id* devices = (cl_device_id*) malloc(num_devices * sizeof(cl_device_id));
  clGetProgramInfo(cpProgram, CL_PROGRAM_DEVICES, num_devices * sizeof(cl_device_id), devices, 0);

  // Grab the sizes of the binaries
  size_t* binary_sizes = (size_t*)malloc(num_devices * sizeof(size_t));
  clGetProgramInfo(cpProgram, CL_PROGRAM_BINARY_SIZES, num_devices * sizeof(size_t), binary_sizes, NULL);

  // Now get the binaries
  char** ptx_code = (char**)malloc(num_devices * sizeof(char*));
  for( unsigned int i=0; i<num_devices; ++i)
  {
    ptx_code[i] = (char*)malloc(binary_sizes[i]);
  }
  clGetProgramInfo(cpProgram, CL_PROGRAM_BINARIES, 0, ptx_code, NULL);

  // Find the index of the device of interest
  unsigned int idx = 0;
  while((idx < num_devices) && (devices[idx] != cdDevice))
  {
    ++idx;
  }

  // If the index is associated, log the result
  if(idx < num_devices)
  {

    // if a separate filename is supplied, dump ptx there
    if (NULL != cPtxFileName)
    {
      common->Printf("\nWriting ptx to separate file: %s ...\n\n", cPtxFileName);
      FILE* pFileStream = NULL;
#ifdef _WIN32
      fopen_s(&pFileStream, cPtxFileName, "wb");
#else
      pFileStream = fopen(cPtxFileName, "wb");
#endif

      fwrite(ptx_code[idx], binary_sizes[idx], 1, pFileStream);
      fclose(pFileStream);
    }
    else // log to logfile and console if no ptx file specified
    {
      common->Printf("\n%s\nProgram Binary:\n%s\n%s\n", HDASHLINE, ptx_code[idx], HDASHLINE);
    }
  }

  // Cleanup
  free(devices);
  free(binary_sizes);
  for(unsigned int i = 0; i < num_devices; ++i)
  {
    free(ptx_code[i]);
  }
  free( ptx_code );
}

//////////////////////////////////////////////////////////////////////////////
//! Get and log the binary (PTX) from the OpenCL compiler for the requested program & device
//!
//! @param cpProgram    OpenCL program
//! @param cdDevice     device of interest
//////////////////////////////////////////////////////////////////////////////
void oclLogBuildInfo(cl_program cpProgram, cl_device_id cdDevice)
{
  char *cBuildLog;
  size_t retValSize;

  clGetProgramBuildInfo(cpProgram, cdDevice, CL_PROGRAM_BUILD_LOG, 0, NULL, &retValSize);
  cBuildLog = new char[retValSize+1];

  clGetProgramBuildInfo(cpProgram, cdDevice, CL_PROGRAM_BUILD_LOG, retValSize, cBuildLog, NULL);
  cBuildLog[retValSize] = '\0';

  common->Printf("\n%s\nBUILD LOG:\n%s\n%s\n", HDASHLINE, cBuildLog, HDASHLINE);

  delete[] cBuildLog;
}

// Helper function to get OpenCL error string from constant
// *********************************************************************
const char* oclErrorString(cl_int error)
{
  static const char* errorString[] = {
    "CL_SUCCESS",
    "CL_DEVICE_NOT_FOUND",
    "CL_DEVICE_NOT_AVAILABLE",
    "CL_COMPILER_NOT_AVAILABLE",
    "CL_MEM_OBJECT_ALLOCATION_FAILURE",
    "CL_OUT_OF_RESOURCES",
    "CL_OUT_OF_HOST_MEMORY",
    "CL_PROFILING_INFO_NOT_AVAILABLE",
    "CL_MEM_COPY_OVERLAP",
    "CL_IMAGE_FORMAT_MISMATCH",
    "CL_IMAGE_FORMAT_NOT_SUPPORTED",
    "CL_BUILD_PROGRAM_FAILURE",
    "CL_MAP_FAILURE",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "CL_INVALID_VALUE",
    "CL_INVALID_DEVICE_TYPE",
    "CL_INVALID_PLATFORM",
    "CL_INVALID_DEVICE",
    "CL_INVALID_CONTEXT",
    "CL_INVALID_QUEUE_PROPERTIES",
    "CL_INVALID_COMMAND_QUEUE",
    "CL_INVALID_HOST_PTR",
    "CL_INVALID_MEM_OBJECT",
    "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
    "CL_INVALID_IMAGE_SIZE",
    "CL_INVALID_SAMPLER",
    "CL_INVALID_BINARY",
    "CL_INVALID_BUILD_OPTIONS",
    "CL_INVALID_PROGRAM",
    "CL_INVALID_PROGRAM_EXECUTABLE",
    "CL_INVALID_KERNEL_NAME",
    "CL_INVALID_KERNEL_DEFINITION",
    "CL_INVALID_KERNEL",
    "CL_INVALID_ARG_INDEX",
    "CL_INVALID_ARG_VALUE",
    "CL_INVALID_ARG_SIZE",
    "CL_INVALID_KERNEL_ARGS",
    "CL_INVALID_WORK_DIMENSION",
    "CL_INVALID_WORK_GROUP_SIZE",
    "CL_INVALID_WORK_ITEM_SIZE",
    "CL_INVALID_GLOBAL_OFFSET",
    "CL_INVALID_EVENT_WAIT_LIST",
    "CL_INVALID_EVENT",
    "CL_INVALID_OPERATION",
    "CL_INVALID_GL_OBJECT",
    "CL_INVALID_BUFFER_SIZE",
    "CL_INVALID_MIP_LEVEL",
    "CL_INVALID_GLOBAL_WORK_SIZE",
  };

  const int errorCount = sizeof(errorString) / sizeof(errorString[0]);
  const int index = -error;

  return (index >= 0 && index < errorCount) ? errorString[index] : "Unspecified Error";
}
