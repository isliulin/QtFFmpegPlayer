using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Text.RegularExpressions;

namespace UDPSendMsg
{
    class Program
    {
        private static Socket client;
        private static Regex rgx = new Regex(@"^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$");
        static void Main(string[] args)
        {
            client = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
            Console.WriteLine("参数格式: [ipv4] [port1] [port2] ... [portn]");
            Console.Write("请输入参数:");
            string[] paras = Console.ReadLine().Split(' ');
            while (!rgx.IsMatch(paras[0].Trim()))
            {
                Console.WriteLine("IP地址不正确,请重新输入");
                paras = Console.ReadLine().Split(' ');
            }
            List<IPEndPoint> ipeps = new List<IPEndPoint>();
            for (int i = 1; i < paras.Length; i++)
            {
                int port = 0;
                if(int.TryParse(paras[i], out port))
                {
                    if (port > 65535) continue;
                    ipeps.Add(new IPEndPoint(IPAddress.Parse(paras[0]), port));
                }
            }
            while (true)
            {
                Console.Write("请输入指令:");
                byte[] buff = Encoding.Default.GetBytes(Console.ReadLine());

                for (int i = 0; i < ipeps.Count; i++)
                {
                    client.SendTo(buff, ipeps[i]);
                }
            }
        }
    }
}
