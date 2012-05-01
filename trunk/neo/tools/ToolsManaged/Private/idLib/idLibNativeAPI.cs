using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace ToolsManaged.Private.idLib
{
    public class idLibNativeAPI
    {
        //
        // idDictNative
        //
        public class idDictNative : CollectionBase, ICustomTypeDescriptor

        {
            private IntPtr _address;

            [System.NonSerialized]
            public idDictNative childDict;

            //
            // idDictNative
            //
            public idDictNative(IntPtr address)
            {
                _address = address;
            }

            [StructLayout(LayoutKind.Sequential)]
            public struct idKeyValueInstancePtr
            {
                public IntPtr _keyAddress;
                public IntPtr _valAddress;
            }

            //
            // idKeyValueInstancePtrManaged
            //
            public class idKeyValueInstancePtrManaged
            {
                public string key;
                public string value;

                //
                // idKeyValueInstancePtrManaged
                //
                public idKeyValueInstancePtrManaged(idKeyValueInstancePtr instance)
                {
                    key = Marshal.PtrToStringAnsi(instance._keyAddress);
                    value = Marshal.PtrToStringAnsi(instance._valAddress);
                }
            }

            private static class Internal
            {
                [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_idDict_Set")]
                public extern static void Set(IntPtr nativeHandle, string key, string value);

                [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_idDict_GetString")]
                public extern static IntPtr GetString(IntPtr nativeHandle, string key);

                [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_idDict_GetVector")]
                public extern static Vector3 GetVector(IntPtr nativeHandle, string key);

                [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_idDict_GetNumKeyVals")]
                public extern static int GetNumKeyVals(IntPtr nativeHandle);

                 [DllImport(@"Toolsx64.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi, EntryPoint = "TOOLAPI_idDict_GetKeyValInstance")]
                public extern static idKeyValueInstancePtr GetKeyValInstance(IntPtr nativeHandle, int index);

            }

            //
            // GetKeyValInstance
            //
            public idKeyValueInstancePtrManaged GetKeyValInstance( int index)
            {
                idKeyValueInstancePtrManaged instance;

                instance = new idKeyValueInstancePtrManaged(Internal.GetKeyValInstance(_address, index));

                return instance;
            }

            //
            // GetNumKeyVals
            //
            public int GetNumKeyVals()
            {
                return Internal.GetNumKeyVals(_address);
            }


            //
            // Set
            //
            public void Set(string key, string value)
            {
                Internal.Set(_address, key, value);
            }

            //
            // GetString
            //
            public string GetString(string key)
            {
                IntPtr val = Internal.GetString(_address, key);

                if (val == IntPtr.Zero)
                {
                    return "";
                }

                return Marshal.PtrToStringAnsi(val);
            }

            //
            // GetVector
            //
            public Vector3 GetVector(string key)
            {
                return Internal.GetVector(_address, key);
            }

            public PropertyDescriptorCollection GetProperties(Attribute[] attributes)
            {
                return GetProperties();
            }

            public PropertyDescriptorCollection GetProperties()
            {
                // Create a new collection object PropertyDescriptorCollection
                PropertyDescriptorCollection pds = new PropertyDescriptorCollection(null);

                // Iterate the list of employees
                for (int i = 0; i < GetNumKeyVals(); i++)
                {
                    // For each employee create a property descriptor 
                    // and add it to the 
                    // PropertyDescriptorCollection instance
                    idDictCollectionPropertyDescriptor pd = new
                                  idDictCollectionPropertyDescriptor(this, i);
                    pds.Add(pd);
                }
                return pds;
            }


            
#region TrivalStuff
            // Implementation of ICustomTypeDescriptor: 

            public String GetClassName()
            {
                return TypeDescriptor.GetClassName(this, true);
            }

            public AttributeCollection GetAttributes()
            {
                return TypeDescriptor.GetAttributes(this, true);
            }

            public String GetComponentName()
            {
                return TypeDescriptor.GetComponentName(this, true);
            }

            public TypeConverter GetConverter()
            {
                return TypeDescriptor.GetConverter(this, true);
            }

            public EventDescriptor GetDefaultEvent()
            {
                return TypeDescriptor.GetDefaultEvent(this, true);
            }

            public PropertyDescriptor GetDefaultProperty()
            {
                return TypeDescriptor.GetDefaultProperty(this, true);
            }

            public object GetEditor(Type editorBaseType)
            {
                return TypeDescriptor.GetEditor(this, editorBaseType, true);
            }

            public EventDescriptorCollection GetEvents(Attribute[] attributes)
            {
                return TypeDescriptor.GetEvents(this, attributes, true);
            }

            public EventDescriptorCollection GetEvents()
            {
                return TypeDescriptor.GetEvents(this, true);
            }

            public object GetPropertyOwner(PropertyDescriptor pd)
            {
                return this;
            }
#endregion
        }
    }
}
