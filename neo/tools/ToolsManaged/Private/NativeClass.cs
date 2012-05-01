using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Reflection;
using System.Runtime.InteropServices;

namespace ToolsManaged.Private
{
    //
    // NativeClass
    //
    public unsafe class NativeClass
    {
        private IntPtr PinnedAddress;
        private delegate void __Constructor_t();
        private delegate void __DeConstructor_t();

        private __Constructor_t __Constructor = null;
        private __DeConstructor_t __DeConstructor = null;

        //
        // __NativeClassRaw
        //
        private struct __NativeClassRaw
        {
            public IntPtr _vTable;
        }

        public IntPtr GetNativeAddress()
        {
            return PinnedAddress;
        }

        //
        // AttachToMemory
        //
        public void AttachToMemory(Type type, IntPtr pAddress)
        {

            FieldInfo[] fields = this.GetType().GetFields();

            IntPtr vTable = GetVirtualTableFromAddress(pAddress);
            PinnedAddress = pAddress;

            for(int i = 0; i < fields.Length; i++, vTable+=IntPtr.Size)
            {



               // if (i >= 2)
                {
                    MapNativeFunctionToField(type, fields[i], vTable);
                }
            }

           
        }



        //
        // GetNativeAddressForClass
        //
        private IntPtr GetNativeAddressForClass()
        {
            return GCHandle.Alloc(this, GCHandleType.Pinned).AddrOfPinnedObject();
        }

        //
        // apNativeFunctionToField
        //
        private void MapNativeFunctionToField(Type type, FieldInfo field, IntPtr funcAddress)
        {
            Delegate func;
            Int64* vFuncPtr = *(Int64**)funcAddress.ToPointer();

            func = Marshal.GetDelegateForFunctionPointer( new IntPtr(vFuncPtr), field.FieldType ); 
            field.SetValue(this, func);
        }

        //
        // GetVirtualTableFromAddress
        //
        private IntPtr GetVirtualTableFromAddress(IntPtr address)
        {
            Int64* vTablePtr = *(Int64**)address.ToPointer();

            return new IntPtr(vTablePtr);
        }
    }
}
