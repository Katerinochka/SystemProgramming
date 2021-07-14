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

namespace Sh_L01
{
    public partial class Form2 : Form
    {
        Process Child = null;
        [DllImport("MFCLibrary1.dll", CharSet = CharSet.Ansi)] static extern void createEvents();
        [DllImport("MFCLibrary1.dll", CharSet = CharSet.Ansi)] static extern void setEventStart();
        [DllImport("MFCLibrary1.dll", CharSet = CharSet.Ansi)] static extern void setEventStop();
        [DllImport("MFCLibrary1.dll", CharSet = CharSet.Ansi)] static extern void setEventExit();
        [DllImport("MFCLibrary1.dll", CharSet = CharSet.Ansi)] static extern void setEventMessage();
        [DllImport("MFCLibrary1.dll", CharSet = CharSet.Ansi)] static extern void waitEventConfirm();
        [DllImport("MFCLibrary1.dll", CharSet = CharSet.Ansi)] static extern void setAllMsg();
        [DllImport("MFCLibrary1.dll", CharSet = CharSet.Ansi)] static extern void setEventConfirm();
        [DllImport("MFCLibrary1.dll", CharSet = CharSet.Ansi)] static extern void closeEvents();
        [DllImport("MFCLibrary1.dll", CharSet = CharSet.Ansi)] static extern void setStr(string str_box, int num);
        public Form2()
        {
            InitializeComponent();
            comboBox1.SelectedIndex = 0;
            createEvents();
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
            if (!(Child == null || (Child.HasExited)))
            {
               for(int i = 0; i < num; ++i)
                {
                    setEventStart();
                    waitEventConfirm();

                    listBox1.Items.Add($"Thread\t{i}");
                }
            }
            else
            {
                listBox1.Items.Clear();
                Child = Process.Start("L01.exe");
                listBox1.Items.Add("Все потоки");
                listBox1.Items.Add("Главный поток");
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (Child == null || Child.HasExited) return;

            if (listBox1.Items.Count - 2 == 0)
            {
                setEventExit();
                waitEventConfirm();
                listBox1.Items.Clear();
            }
            else
            {
                setEventStop();
                waitEventConfirm();
                listBox1.Items.RemoveAt(listBox1.Items.Count - 1);
            }
        }

        private void Form2_Load(object sender, EventArgs e)
        {
            
        }

        private void Form2_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (!(Child == null || Child.HasExited))
            {
                setEventExit();
                waitEventConfirm();
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            int num;
            string text;
            List<int> recipients = get_current_num();
            num = listBox1.SelectedIndex;
            text = textBox1.Text;
            if (text.Length == 0 || recipients.Count == 0) return;
            setStr(text, recipients[num]);
        }
    }
}
