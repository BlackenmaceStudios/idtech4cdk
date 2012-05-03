using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Reflection;
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

            private Type GetKeyType(string keyname)
            {
                // If there isn't a framework decl defined just force all val types to be a string.
                if (childDict == null)
                {
                    return typeof(string);
                }

                // Hardcoded hacks.
                if(keyname == "origin")
                {
                    return typeof(idLib.Vector3);
                }

                for (int c = 0; c < childDict.GetNumKeyVals(); c++)
                {
                    Type type;
                    idKeyValueInstancePtrManaged instance = childDict.GetKeyValInstance(c);
                    string compstr;

                    if (instance.key.Contains("editor_bool"))
                    {
                        type = typeof(bool);
                        compstr = instance.key.Substring(("editor_bool ").Length);
                    }
                    else if (instance.key.Contains("editor_val"))
                    {
                        type = typeof(int);
                        compstr = instance.key.Substring(("editor_val ").Length);
                    }
                    else
                    {
                        // editor_netval 
                        if (instance.key.Contains("editor_netval"))
                        {
                            string[] table = instance.key.Split(' ');

                            type = ReflectionHelper.FindTypeInAssembly(table[1]);
                            if (type == null)
                                continue;

                            compstr = table[2];
                        }
                        else
                        {
                            continue;
                        }
                    }

                    if (compstr == keyname)
                    {
                        return type;
                    }
                }

                Type keyType = ReflectionHelper.FindTypeInAssembly(keyname);
                if (keyType != null)
                    return keyType;

                // Couldn't find any type defined just use default string.
                return typeof(string);
            }

            

            public PropertyDescriptorCollection GetProperties()
            {
                int i;

                // Create a new collection object PropertyDescriptorCollection
                PropertyDescriptorCollection pds = new PropertyDescriptorCollection(null);

                // Iterate the list of employees
                for (i = 0; i < GetNumKeyVals(); i++)
                {
                    // For each employee create a property descriptor 
                    // and add it to the 
                    // PropertyDescriptorCollection instance
                    idDictCollectionPropertyDescriptor pd = new
                                  idDictCollectionPropertyDescriptor(this, i, GetKeyType( GetKeyValInstance( i ).key ));

                    
                    pds.Add(pd);
                }

                if (childDict != null)
                {
                    for (int c = 0; c < childDict.GetNumKeyVals(); c++)
                    {
                        idKeyValueInstancePtrManaged instance = childDict.GetKeyValInstance( c );

                        // Ignore any of the editor helper keys.
                        if (instance.key.Contains("editor_"))
                            continue;

                        // Check to see if this value is already there.
                        for (int g = 0; g < GetNumKeyVals(); g++)
                        {
                            idKeyValueInstancePtrManaged testval = GetKeyValInstance(g);

                            if (testval.key == instance.key)
                            {
                                instance = null;
                                break;
                            }
                        }

                        if (instance == null)
                            continue;

                        

                        // For each employee create a property descriptor 
                        // and add it to the 
                        // PropertyDescriptorCollection instance
                        idDictCollectionPropertyDescriptor pd = new
                                      idDictCollectionPropertyDescriptor(this, c + i, instance.key, instance.value, GetKeyType(instance.key));
                        
                        pds.Add(pd);
                    }
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
