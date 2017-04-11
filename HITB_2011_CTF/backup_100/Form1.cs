using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Security.Cryptography;

namespace backup_100
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private byte[] KLKS1(string name)
        {
            //hitb2011kl - 23846-19269-3729-4093
            name = name.ToUpper();
            char[] chArray1 = name.ToCharArray();
            char[] chArray2 = "{74D7B34A-FEED-FACE-C7F0-A929E2B9D47C}".ToCharArray();
            if (name.Length < 5)
            {
                name = name + "weallknowyoushouldnotbehere!";
            }

            long num1 = 0;
            for (int num2 = 0; num2 < chArray1.Length; num2++)
            {
                num1 += (((long)chArray1[num2]) * (num2 + 1));
            }
            num1 *= 0x8EF97;

            for (int num3 = 0; (num3 < chArray2.Length) && (num3 < chArray1.Length); num3++)
            {
                num1 += ((long)((chArray1[num3] % '\x0003') * chArray2[num3]));
            }

            long num4 = num1 % ((long)0x8635);
            string text1 = num4.ToString();
            num4 = num1 % ((long)0xFE72);
            string text2 = num4.ToString();
            num4 = num1 % ((long)0xD327);
            string text3 = num4.ToString();
            num4 = num1 % ((long)0x79d);
            string text4 = num4.ToString();
            text1 = text1.PadLeft(4, '0');
            text2 = text2.PadRight(4, '1');
            text3 = text3.PadLeft(4, '2');
            text4 = text4.PadRight(4, '3');
            string[] textArray1 = new string[7] { text1, "-", text2, "-", text3, "-", text4 };

            byte[] tmpSource = ASCIIEncoding.ASCII.GetBytes(string.Concat(textArray1));
            byte[] tmpHash = new MD5CryptoServiceProvider().ComputeHash(tmpSource);

            //Debug
            //txtSerial.Text = string.Concat(textArray1);

            return tmpHash;
        }

        private bool KLKS2(byte[] val1, string val2)
        {
            byte[] t2 = ASCIIEncoding.ASCII.GetBytes(string.Concat(val2));
            byte[] t3 = new MD5CryptoServiceProvider().ComputeHash(t2);
            if (val1.Length == t3.Length)
            {
                int i = 0;
                while (i < val1.Length)
                {
                    if(t3[i] != val1[i])
                    {
                        return false;
                    }
                    i += 1;
                }
            }
            else
            {
                return false;
            }
            return true;
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            string s1 = txtName.Text;
            string s2 = txtSerial.Text;
            if(s1.Length == 0 || s2.Length == 0)
            {
                MessageBox.Show("Please fill in a name and serial!");
                return;
            }
            if (s1 == "hitb2011kl")
            {
                MessageBox.Show("Sorry this name has been blacklisted");
                return;
            }
            if (KLKS2(KLKS1(s1), s2))
            {
                MessageBox.Show("This looks good, but will it work?");
            }
            else
            {
                MessageBox.Show("How about another try?");
            }
        }
    }
}
