using System;
using System.Diagnostics;
using System.Net.Sockets;
using Advantech.Adam;

namespace Usb_connect
{
    class Program
    {
        static private AdamSocket adamModbus;
        static private string m_szIP;
        static private int m_iPort;
        static private int m_Coil;
        static private string m_Command;
        static private int index = 0;

        //example: Usb_connect.exe reset 1 192.168.81.224
        static void Main(string[] args)
        {

            if (args.Length < 2 || args.Length > 3)
            {
                System.Console.WriteLine("usage: <command> <port> [ip]");
                System.Console.WriteLine("\tcommand = on/off/reset, port=Adam port(see adam switch), ip=Adam ip address\n");
                return;
            }

            m_Command = args[0];
            m_Coil = int.Parse(args[1]) + 17;
            if (args.Length == 3)
                m_szIP = args[2];
            else
                m_szIP = "192.168.81.224";	// modbus slave IP address
            
            m_iPort = 502;				                // modbus TCP port is 502
            adamModbus = new AdamSocket();
            adamModbus.SetTimeout(1000, 1000, 1000);    // set timeout for TCP
            adamModbus.Connect(m_szIP, ProtocolType.Tcp, m_iPort);

            switch (m_Command)
            {
                case "on":
                    adamModbus.Modbus().ForceSingleCoil(m_Coil, 1);
                    break;
                case "off":
                    adamModbus.Modbus().ForceSingleCoil(m_Coil, 0);
                    break;
                case "reset":
                    adamModbus.Modbus().ForceSingleCoil(m_Coil, 0);
                    System.Threading.Thread.Sleep(1000);
                    adamModbus.Modbus().ForceSingleCoil(m_Coil, 1);
                    break;
                default:
                    break;
            }
            adamModbus.Disconnect();                    // disconnect slave
        }
    }
}

