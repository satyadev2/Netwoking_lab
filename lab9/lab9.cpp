#include <bits/stdc++.h>
using namespace std;

void Print(map<string, char> &L2_switch)
{
    cout << "MAC ADDRESS"
         << "        "
         << "OUTPORT\n";
    for (auto i : L2_switch)
    {
        cout << i.first << "---------" << i.second << endl;
    }
}
void Assign(map<string, char> &L2_switch)
{
    cout << "Enter Mac Adress of node\n";
    string mac;
    cin >> mac;
    cout << "Enter the port you want to assign\n";
    char ch;
    cin >> ch;
    for (auto i : L2_switch)
    {
        if (i.second == ch)
        {
            cout << "Entered Port has been already Assigned\n";
            return;
        }
        if (i.first == mac)
        {
            cout << "Entered MAC address has already been assigned\n";
            return;
        }
    }
    L2_switch[mac] = ch;
    return;
}

void Outport(map<string, char> &L2_switch)
{
    string mac;
    cout << "Enter the MAC address of which you want to know the outport\n";
    cin >> mac;
    int maxi = 0;
    string res_mac;
    char res_port;
    int k = 0;
    for (auto i : L2_switch)
    {
        int count = 0;
        for (auto j : i.first)
        {
            if (mac[k] == j)
            {
                count++;
                k++;
            }
            else
            {
                break;
            }
        }
        if (count > maxi)
        {
            res_port = i.second;
        }
    }
    cout << "Designated Port for your MAC " << mac << " is:" << res_port << endl;
    ;
}
int main()
{
    map<string, char> L2_switch;
    while (1)
    {
        cout << "\n\n***************************************\n\n";
        cout << "What operation you want to perform\n";
        cout << "0.L2_SWITCH Table\n";
        cout << "1.Assign Port to any MAC address\n";
        cout << "2.Know Outport of any MAC address\n";
        cout << "\n\n***************************************\n\n";
        int choice;
        cin >> choice;
        switch (choice)
        {
        case 0:
        {
            Print(L2_switch);
            break;
        }
        case 1:
        {
            Assign(L2_switch);
            break;
        }
        case 2:
        {
            Outport(L2_switch);
        }
        }
    }
}
