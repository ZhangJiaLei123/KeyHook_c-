using System;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Threading;
//本项目的工具类
namespace LibAlarmService2
{
    public class PathSet
    {
        public readonly string _basePath;

        private static readonly Lazy<PathSet> instance = new Lazy<PathSet>(() => new PathSet());

        public static PathSet Instance
        {
            get
            {
                return instance.Value;
            }
        }

        PathSet()
        {
            string pathName = Assembly.GetExecutingAssembly().Location;
            _basePath = pathName.Substring(0, pathName.LastIndexOf('\\'));
            _basePath += "\\";
        }

    }
}
