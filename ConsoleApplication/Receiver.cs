using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;


namespace ConsoleApplication
{

    class Receiver
    {
        public static int AlarmCallBack(int vkCode, int scanCode, int actionId)
        {
            Console.WriteLine("actionId:{0}, vkCode:{1}, scanCode:{2}", 
                actionId, vkCode, scanCode);

            return 0;
        }
    }
}
