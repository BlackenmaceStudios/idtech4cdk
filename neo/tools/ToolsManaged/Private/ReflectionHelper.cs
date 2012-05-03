using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Reflection;
namespace ToolsManaged.Private
{
    public static class ReflectionHelper
    {
        public static Type FindTypeInAssembly(string type)
        {
            // Check to see if we get the type via reflection.
            Type[] assemblyTypes = Assembly.GetExecutingAssembly().GetTypes();

            for (int i = 0; i < assemblyTypes.Length; i++)
            {
                if (assemblyTypes[i].Name.Contains("." + type))
                {
                    return assemblyTypes[i];
                }
            }

            return null;
        }

        public static object InitObjectFromType(Type type, params object[] parems)
        {
            return Activator.CreateInstance(type, parems);
        }
    }
}
