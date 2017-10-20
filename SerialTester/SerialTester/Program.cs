using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.Ports;

namespace SerialTester
{
    class Program
    {

        static SerialPort _serialPort;

        static void Main(string[] args)
        {
            string input, portName;
            int baudRate = 9600;
            Console.WriteLine("Select Com Port:");
            portName = Console.ReadLine();

            Console.WriteLine("Select Mode (0: 19200, 1: 10417):");
            input = Console.ReadLine();
            if (string.Equals(input, "0"))
                baudRate = 19200;
            else if (string.Equals(input, "1"))
                baudRate = 10417;
            
            _serialPort = new SerialPort();
            _serialPort.PortName = portName;
            _serialPort.BaudRate = baudRate;
            _serialPort.Parity = Parity.None;
            _serialPort.DataBits = 8;
            _serialPort.StopBits = StopBits.One;
            _serialPort.Handshake = Handshake.None;

            // Open for business
            _serialPort.Open();

            Byte [] message = new Byte[5];
            while(!string.Equals(input, "q"))
            {
                Console.Write("Command: (r, g, b, l, h, o, q)  ");
                input = Console.ReadLine();

                if (string.Equals(input, "r"))
                {
                    message[0] = 49;
                    message[1] = 255;
                    message[2] = 0;
                    message[3] = 0;
                }
                else if (string.Equals(input, "g"))
                {
                    message[0] = 49;
                    message[1] = 0;
                    message[2] = 255;
                    message[3] = 0;
                }
                else if (string.Equals(input, "b"))
                {
                    message[0] = 49;
                    message[1] = 0;
                    message[2] = 0;
                    message[3] = 255;
                }
                else if (string.Equals(input, "l"))
                {
                    message[0] = 192;
                    message[1] = 1;
                    message[2] = 0;
                    message[3] = 0;
                }
                else if (string.Equals(input, "h"))
                {
                    message[0] = 192;
                    message[1] = 2;
                    message[2] = 0;
                    message[3] = 0;
                }
                else if (string.Equals(input, "o"))
                {
                    message[0] = 192;
                    message[1] = 3;
                    message[2] = 0;
                    message[3] = 0;
                }
                else
                    input = "unknown";

                if (!string.Equals(input, "q") && !string.Equals(input, "unkown"))
                {
                    Byte check = 0;
                    check += message[0];
                    check += message[1];
                    check += message[2];
                    check += message[3];
                    message[4] = check;
                    _serialPort.Write(message, 0, 5);
                    Console.WriteLine("");
                }

            }

            // Closing up shop
            _serialPort.Close();
        }
    }
}
