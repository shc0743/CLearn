#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<algorithm>
using namespace std;
#include "activity.h"

void str_replace(string& strBase, string strSrc, string strDes)
{
    //参数1：待修改的字符串；参数2：要替换的内容；参数3：替换后的内容，比如把AA替换成BB，这里要填的是BB
    //替换完后会返回给原字符串变量也就是strBase。
    string::size_type pos = 0;
    string::size_type srcLen = strSrc.size();
    string::size_type desLen = strDes.size();
    pos = strBase.find(strSrc, pos);
    while ((pos != string::npos))
    {
        strBase.replace(pos, srcLen, strDes);
        pos = strBase.find(strSrc, (pos + desLen));
    }
}
void str_split(const std::string& src, const std::string& separator, std::vector<std::string>& dest) //字符串分割到数组
{

    //参数1：要分割的字符串；参数2：作为分隔符的字符；参数3：存放分割后的字符串的vector向量

    string str = src;
    string substring;
    string::size_type start = 0, index;
    dest.clear();
    index = str.find_first_of(separator, start);
    do
    {
        if (index != string::npos)
        {
            substring = str.substr(start, index - start);
            dest.push_back(substring);
            start = index + separator.size();
            index = str.find(separator, start);
            if (start == string::npos) break;
        }
    } while (index != string::npos);

    //the last part
    substring = str.substr(start);
    dest.push_back(substring);
}


CActivitySoftware::CActivitySoftware()
{
}

CActivitySoftware::CActivitySoftware(vector<string> &keys, string Product, string Company)
{
    for (auto i : keys)
    {
        this->keylist.push_back(i.c_str());
    }
    this->product = Product.c_str();
    this->company = Company.c_str();
}

CActivitySoftware::~CActivitySoftware()
{
}

bool CActivitySoftware::ActivityProduct(string key)
{
    for (auto i = 0; i < keylist.size();++i)
    {
        if (keylist[i] == key.c_str()) { 
            return this->activitied = true;
        }
    }
    return false;
}

bool CActivitySoftware::CheckActivity()
{
    return this->activitied;
}

bool CActivitySoftware::GetInformation(CActivitySoftwareIFMS& ifms) {
    ifms.company = this->company;
    ifms.product = this->product;
    return true;
}
bool CActivitySoftware::GetIFMS(CActivitySoftwareIFMS& ifms) {
    ifms.company = this->company;
    ifms.product = this->product;
    return true;
}

void ActKeyFileToObj(CActivitySoftwareIFMS& ifms,string filename)
{	
    fstream fp(filename, ios::in | ios::binary);
    char getmp[4095]; bool isinblock=false;
	while (fp.getline(getmp, 4095)) {
		string getmp_s = ((string)getmp);
        if (isinblock && getmp_s.find("}") == getmp_s.npos) continue;
        if (isinblock && getmp_s.find("}") != getmp_s.npos) {
            isinblock = false; continue; 
        }
        //Main
		     if (getmp_s.find("KEYLIST={") != getmp_s.npos) {
            isinblock = true; continue;
		} 
		else if (getmp_s.find("ProductName=") != getmp_s.npos) {
            ifms.product = getmp_s.erase(0, 12);
		}
		else if (getmp_s.find("CompanyName=") != getmp_s.npos) {
            ifms.company = getmp_s.erase(0, 12);
		}
		else if (getmp_s.find("BuyUrl=") != getmp_s.npos) {
            ifms.buyurl = getmp_s.erase(0, 7);
		}
		else if (getmp_s.find("CopyRightInformation=") != getmp_s.npos) {
            ifms.copyright = getmp_s.erase(0, 21);
		}
	};
    fp.close();
}
vector<string>& GetActKeyList(vector<string> &ls,string filename) {
    fstream fp(filename, ios::in | ios::binary);
    char getmp[4095];
	while (fp.getline(getmp, 4095)) {
		string getmp_s = ((string)getmp);
        if (getmp_s.find("KEYLIST={") != getmp_s.npos) {
            while (fp.getline(getmp, 4095)) {
                string getmp_s = ((string)getmp);
                if (getmp_s.find("}") != getmp_s.npos) break;
                ls.push_back(getmp_s);
            }
            break;
        } else continue;
	};
    fp.close();
    return ls;
}