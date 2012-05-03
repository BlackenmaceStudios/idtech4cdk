using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace ToolsManaged.Private.idLib
{
    public class idDictTypeConverter : ExpandableObjectConverter
    {
        
        
        public override PropertyDescriptorCollection GetProperties(ITypeDescriptorContext context, object value, Attribute[] attributes)
        {
            return base.GetProperties(context, value, attributes);
        }

        public override bool CanConvertTo(ITypeDescriptorContext context,
                                  System.Type destinationType)
        {
         //   if (!IsTypeExpandable(destinationType))
         //       return false;

            return base.CanConvertTo(context,destinationType);
        }

        public override object ConvertTo(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value, Type destinationType)
        {
          //  if (!IsTypeExpandable(value.GetType()))
          //      return false;

            object ret = base.ConvertTo(context, culture, value, destinationType);

            return ret;
        }

        public override bool CanConvertFrom(ITypeDescriptorContext context, Type sourceType)
        {
            

            return base.CanConvertFrom(context, sourceType); 
        }

        public override object ConvertFrom(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value)
        {
            
            return base.ConvertFrom(context, culture, value);
        }
    }
    [TypeConverterAttribute(typeof(idDictTypeConverter))]
    public class idDictCollectionPropertyDescriptor : PropertyDescriptor
    {
        private idLibNativeAPI.idDictNative dict = null;
        private int index = -1;
        private bool useHardVals = false;
        private string name;
        private object val;
        private Type _type;
        private idDictTypeConverter converter;

        public idDictCollectionPropertyDescriptor(idLibNativeAPI.idDictNative dict, int idx, Type type)
            : base("#" + idx.ToString(), null)
        {
            this.dict = dict;
            this.index = idx;
            _type = type;
            converter = new idDictTypeConverter();
        }

        public idDictCollectionPropertyDescriptor(idLibNativeAPI.idDictNative dict, int idx, string name, string val, Type type)
            : base("#" + idx.ToString(), null)
        {
            this.dict = dict;
            this.index = idx;
            this.name = name;
            this.val = val;
            useHardVals = true;
            _type = type;
            converter = new idDictTypeConverter();
        }

        private bool IsTypeExpandable(System.Type type)
        {
            object[] attributes = type.GetCustomAttributes(true);
            if (attributes == null)
                return false;

            for (int i = 0; i < attributes.Length; i++)
            {
                if (attributes[i] is Editor.EditorShowProperties)
                    return true;
            }

            return false;
        }

        public override TypeConverter Converter
        {
            get
            {
                if (IsTypeExpandable(_type))
                {
                    return converter;
                }
                else
                {
                    return base.Converter;
                }
            }
        }

        public override bool CanResetValue(object component)
        {
            return true;
        }

        public override Type ComponentType
        {
            get
            {
                return this.dict.GetType();
            }
        }

        public override string DisplayName
        {
            get
            {
                if (!useHardVals)
                {
                    return this.dict.GetKeyValInstance(index).key;
                }
                else
                {
                    return name;
                }
            }
        }

        public override string Description
        {
            get
            {
                return "Property sheet for " + GetValue("name");
            }
        }

        public override object GetValue(object component)
        {
            if (!useHardVals)
            {
                val = this.dict.GetKeyValInstance(index).value;
            }

            if (_type == typeof(bool))
            {
                if (val == "1")
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else if (_type == typeof(int))
            {
                return int.Parse((string)val);
            }
            else if (_type == typeof(string))
            {
                return val;
            }
            else
            {
                return ReflectionHelper.InitObjectFromType(_type, val);
            }

        }

        public override string Category
        {
            get
            {
                if (IsReadOnly)
                {
                    return "Private Attributes";
                }

                return "Public Attributes";
            }
        }

        public override bool IsReadOnly
        {
            get {
                if (DisplayName == "classname" || DisplayName == "model" || DisplayName == "spawnclass")
                {
                    return true;
                }
                return false; 
            }
        }

        public override string Name
        {
            get { return "#" + index.ToString(); }
        }

        public override Type PropertyType
        {
            get { return _type; }
        }

        public override void ResetValue(object component) { }

        public override bool ShouldSerializeValue(object component)
        {
            return true;
        }

        public override void SetValue(object component, object value)
        {
            if (_type == typeof(bool))
            {
                bool b = (bool)value;

                if (b)
                {
                    val = "1";
                }
                else
                {
                    val = "0";
                }
            }
            else if (_type == typeof(int))
            {
                val = "" + (int)value;
            }
            else if (_type == typeof(string))
            {
                val = (string)value;
            }
            else // .net managed object.
            {
                val = value.ToString();
            }
           
            dict.Set(DisplayName, (string)val);
            if (dict.childDict != null)
            {
                dict.childDict.Set(DisplayName, (string)val);
            }
            // this.collection[index] = value;
        }
    }
}
