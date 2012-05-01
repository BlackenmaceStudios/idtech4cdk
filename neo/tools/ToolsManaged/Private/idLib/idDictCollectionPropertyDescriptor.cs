using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace ToolsManaged.Private.idLib
{
    public class idDictCollectionPropertyDescriptor : PropertyDescriptor
    {
        private idLibNativeAPI.idDictNative dict = null;
        private int index = -1;

        public idDictCollectionPropertyDescriptor(idLibNativeAPI.idDictNative dict, int idx)
            : base("#" + idx.ToString(), null)
        {
            this.dict = dict;
            this.index = idx;
        }

        public override AttributeCollection Attributes
        {
            get
            {
                return new AttributeCollection(null);
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
                return this.dict.GetKeyValInstance(index).key;
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
            return this.dict.GetKeyValInstance(index).value;
        }

        public override bool IsReadOnly
        {
            get {
                if (DisplayName == "classname" || DisplayName == "model")
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
            get { return typeof(string); }
        }

        public override void ResetValue(object component) { }

        public override bool ShouldSerializeValue(object component)
        {
            return true;
        }

        public override void SetValue(object component, object value)
        {
            dict.Set(DisplayName, (string)value);
            if (dict.childDict != null)
            {
                dict.childDict.Set(DisplayName, (string)value);
            }
            // this.collection[index] = value;
        }
    }
}
