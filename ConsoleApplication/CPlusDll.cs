using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

/***
 * C# 使用 c++ Dll库 实现 键盘监听,并接受回调结果
 */
namespace Console_KeyHook_Demo
{

    /**
     * Main函数 入口
     */
    class Program
    {
        static void Main(string[] args)
        {
            
            CPlusDll.StartReceive(); // 启动监听

            Console.ReadLine(); // 保活控制台
        }
    }

    public class CPlusDll
    {
        //根据DLL中的回调函数的原型声明一个委托类型
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate int CPlusAlarmRun(int vkCode, int scanCode, int actionId);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate int CPlusAlarmRunPort(char[] data);

        //实例化这个委托。
        static CPlusAlarmRun _alarmCallBack = new CPlusAlarmRun(Receiver.AlarmCallBack);
        static CPlusAlarmRunPort _alarmCallBackPort = new CPlusAlarmRunPort(Receiver.AlarmCallBackPort);



        //DLL的函数 键盘钩子 启动函数
        [DllImport(@"./KeyHookP.dll", EntryPoint = "StartRun", CallingConvention = CallingConvention.Cdecl)]
        extern static void StartRun(CPlusAlarmRun alarmCallBack);
        [DllImport(@"./KeyHookP.dll", EntryPoint = "SendKey", CallingConvention = CallingConvention.Cdecl)]
        extern static void SendKey(int keyCode);

        [DllImport(@"./ScreenCapture.dll", EntryPoint = "MakeWhere", CallingConvention = CallingConvention.Cdecl)]
        extern static void MakeWhere();

        [DllImport(@"./Readcomdata.dll", EntryPoint = "start", CallingConvention = CallingConvention.Cdecl)]
        extern static void start(int com, CPlusAlarmRunPort alarmCallBack);

        //[DllImport(@"./Cashier.dll", EntryPoint = "CashierMakePic", CallingConvention = CallingConvention.Cdecl)]
        //extern static void CashierMakePic();

        //[DllImport(@"./ConsoleOcr.dll", EntryPoint = "MakeOcr", CallingConvention = CallingConvention.Cdecl)]
        //extern static float MakeOcr();

        // 启动键盘钩子服务,后台线程
        public static void StartReceive()
        {
            string path = System.IO.Directory.GetCurrentDirectory();
            //执行DLL的函数，传入C#回调函数的指针。
             StartRun(_alarmCallBack);

            for (int i = 0; i < 100; i++)
            {
                SendKey(13);
                Thread.Sleep(1000);
            }
            
   
            //  start(1, _alarmCallBackPort);
            // MakeWhere();
            // CashierMakePic();
            //float a = MakeOcr();

            System.Console.ReadKey();
            System.Console.ReadKey();
            System.Console.ReadKey();
            System.Console.ReadKey();
            System.Console.ReadKey();
        }
    }
}
