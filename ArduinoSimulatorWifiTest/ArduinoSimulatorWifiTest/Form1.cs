using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Management;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ArduinoSimulatorWifiTest
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        SerialPort port;
        TcpListener listener;

        private void Form1_Load(object sender, EventArgs e)
        {
            string wifiPort = "";

            foreach (COMPortInfo portInfo in COMPortInfo.GetCOMPortsInfo())
            {
                if (portInfo.Description.StartsWith("Prolific")) wifiPort = portInfo.Name;
            }

            port = new SerialPort(wifiPort);
            port.BaudRate = 115200;
            port.DataBits = 8;
            port.Handshake = Handshake.None;
            port.Parity = Parity.None;
            port.StopBits = StopBits.One;
            port.Open();

            port.DataReceived += port_DataReceived;

            SendCommand("AT\r\n");
            SendCommand("AT+CWMODE=2\r\n");
            SendCommand("AT+CWSAP=\"Random\",\"\",3,0\r\n");
            SendCommand("AT+CIFSR\r\n");
            SendCommand("AT+CWLIF\r\n");

            SendCommand("AT+CIPMUX=1\r\n");
            SendCommand("AT+CIPSERVER=1,2539\r\n");

            /*listener = new TcpListener(2539);
            listener.Start();

            Thread t = new Thread(new ThreadStart(HandleClients));
            t.Start();*/
        }

        void HandleClients()
        {
            while (true)
            {
                try
                {
                    TcpClient client = listener.AcceptTcpClient();
                    Thread t = new Thread(new ParameterizedThreadStart(HandleClient));
                    t.Start(client);
                }
                catch (Exception e)
                {
                }
            }
        }

        bool button = false;

        void HandleClient(object clientObject)
        {
            try
            {
                TcpClient client = (TcpClient)clientObject;
                BinaryReader reader = new BinaryReader(client.GetStream());
            
                while (true)
                {
                    button = reader.ReadBoolean();
                }
            }
            catch (Exception e)
            {
            }
        }

        void SendCommand(string command)
        {
            port.Write(command);
            Thread.Sleep(10);
        }

        const int bufferSize = 1024;
        int pos = 0, prevPos = 0, lastPlus = 0, lastOk = 0;
        byte[] buffer = new byte[bufferSize];
        byte[] tmpBuffer = new byte[512];
        byte[] recvBuffer = new byte[512];
        Queue<string> messages = new Queue<string>();

        void port_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {   
            int count = port.BytesToRead;
            port.Read(tmpBuffer, 0, count);
            string str = System.Text.Encoding.Default.GetString(tmpBuffer, 0, count);
            messages.Enqueue(str);

            for (int i = 0; i < count; i++)
            {
                buffer[pos] = tmpBuffer[i];
                if (tmpBuffer[i] == '+') lastPlus = pos;
                if (buffer[pos] == 'K' && buffer[prevPos] == 'O') lastOk = prevPos;

                prevPos = pos;
                pos = (pos + 1) % bufferSize;
            }

            CheckMessage();
        }

        void CheckMessage()
        {
            if (lastOk - lastPlus > 5 || (lastOk < lastPlus && lastPlus - lastOk > bufferSize / 2))
            {
                int pos = lastPlus;
                bool recv = true;
                string search = "+IPD";

                for (int i = 0; i < search.Length; i++)
                {
                    if (buffer[pos] != search[i]) recv = false;
                    pos = (pos + 1) % bufferSize;
                }

                if (recv)
                {
                    pos = (pos + 3) % bufferSize;
                    int length = 0;
                    while (char.IsDigit((char)buffer[pos]))
                    {
                        length = length * 10 + buffer[pos] - 48;
                        pos = (pos + 1) % bufferSize;
                    }

                    pos = (pos + 1) % bufferSize;

                    for (int i = 0; i < length; i++)
                    {
                        recvBuffer[i] = buffer[pos];
                        pos = (pos + 1) % bufferSize;
                    }

                    button = Convert.ToBoolean(recvBuffer[0]);
                }
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            Text = button.ToString();
            while (messages.Count > 0)
            {
                textBox1.AppendText(messages.Dequeue());
            }
        }
    }
}
