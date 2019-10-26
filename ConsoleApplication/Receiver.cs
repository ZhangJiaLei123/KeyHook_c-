using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;


namespace Console_KeyHook_Demo
{

    /**
     * 键盘监听回调函数实现
     */
    class Receiver
    {
        /*
         * 
         * vkCode 键盘值, 键盘 一种虚拟键代码。代码必须是 1 到 254 范围中的一个值。
         * scanCode  dword 值,关键硬件扫描代码。
         * actionId 正直按下, 负值释放
         */
        public static int AlarmCallBack(int vkCode, int scanCode, int actionId)
        {
         
            Console.Write("{0}", scanCode);

            return 0;
        }

        public static int AlarmCallBackPort(char[] data)
        {

            Console.Write("{0}", data);


            return 0;
        }
    }
}
