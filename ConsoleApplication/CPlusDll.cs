using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace ConsoleApplication
{
    public class CPlusDll
    {
        //根据DLL中的回调函数的原型声明一个委托类型
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate int CPlusAlarmRun(IntPtr data,int dataLen);

        //实例化这个委托。
        static CPlusAlarmRun _alarmCallBack = new CPlusAlarmRun(Receiver.AlarmCallBack);

        //DLL的函数alarmRun。
        [DllImport(@"..\Debug\Win32Project.dll", EntryPoint = "StartRun", CallingConvention = CallingConvention.Cdecl)]
        extern static void StartRun(CPlusAlarmRun alarmCallBack);

        public static void StartReceive()
        {
            string path = System.IO.Directory.GetCurrentDirectory();
            //执行DLL的函数，传入C#回调函数的指针。
            StartRun(_alarmCallBack);
        }
    }
}
