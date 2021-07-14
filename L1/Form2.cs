using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using System.Threading;
using System.Runtime.InteropServices;
using System.Threading;

namespace Sh_L01
{
    public partial class Form2 : Form
    {
        Process Child = null;
        [DllImport("MFCLibrary1.dll", CharSet = CharSet.Ansi)] static extern bool ConnectServer();
        [DllImport("MFCLibrary1.dll", CharSet = CharSet.Ansi)] static extern void SendCode(int n);
        [DllImport("MFCLibrary1.dll", CharSet = CharSet.Ansi)] static extern int GetCode();
        [DllImport("MFCLibrary1.dll", CharSet = CharSet.Ansi)] static extern void Disconnect();
        [DllImport("MFCLibrary1.dll", CharSet = CharSet.Ansi)] static extern void SendString(string text, int num);
        public Form2()
        {
            InitializeComponent();
            comboBox1.SelectedIndex = 0;
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private List<int> get_current_num()
        {
            List<int> buff = new List<int>();
            if (listBox1.Items.Count > 0)
            {
                if (listBox1.Items[0].ToString() == "Все потоки")
                    buff.Add(-2);
                if (listBox1.Items[1].ToString() == "Главный поток")
                    buff.Add(-1);
                for (int i = 2; i < listBox1.Items.Count; i++)
                    buff.Add(i - 2);
            }
            return buff;
        }
        private void button1_Click(object sender, EventArgs e)
        {
            int num = comboBox1.SelectedIndex + 1;
            if (ConnectServer())
            {
                //ConnectServer();
                listBox1.Items.Clear();
                listBox1.Items.Add("Все потоки");
                listBox1.Items.Add("Главный поток");
                SendCode(0);
                SendCode(num);
                num = GetCode();
                for (int i = 0; i < num; ++i)
                {
                    SendCode(0);
                    //waitConfirm();
                    listBox1.Items.Add($"Thread\t{i}");
                }
                Disconnect();
            }
            else
            {
                listBox1.Items.Clear();
                listBox1.Items.Add($"No connection");
                //init();
                //listBox1.Items.Add("Все потоки");
                //listBox1.Items.Add("Главный поток");
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            //if (!ProcessisOpen()) return;
            ConnectServer();
            SendCode(1);
            int num = GetCode();
            if (num == 0)
                listBox1.Items.Clear();
            else
                listBox1.Items.RemoveAt(listBox1.Items.Count - 1);
            Disconnect();
            /*Send(1);
            waitConfirm();
            listBox1.Items.RemoveAt(listBox1.Items.Count - 1);*/
            /*if (listBox1.Items.Count - 2 == 0)
            {
                Send(2);
                waitConfirm();
                listBox1.Items.Clear();
            }
            else
            {
                Send(1);
                waitConfirm();
                listBox1.Items.RemoveAt(listBox1.Items.Count - 1);
            }*/
        }

        private void Form2_Load(object sender, EventArgs e)
        {
            
        }

        private void Form2_FormClosing(object sender, FormClosingEventArgs e)
        {
            /*if (!(Child == null || Child.HasExited))
            {
                Send(2);
                waitConfirm();
            }
            if (ProcessisOpen())
            {
                Send(3, "quit");
                waitConfirm();
            }*/
        }

        private void button3_Click(object sender, EventArgs e)
        {
            ConnectServer();
            SendCode(2);
            int num;
            string text;
            List<int> recipients = get_current_num();
            num = listBox1.SelectedIndex;
            text = textBox1.Text;
            if (text.Length == 0 || recipients.Count == 0) return;
            SendString(text, recipients[num]);
        }
    }
}
