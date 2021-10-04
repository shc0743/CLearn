#pragma once
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
using namespace std;

extern class CActivitySoftwareIFMS {
public:
    string product;
    string company;
    string buyurl;
    string copyright;
};

extern class CActivitySoftware {
public:
     CActivitySoftware();
     CActivitySoftware(
         vector<string> &keys,
         string Product, 
         string Company
     );
     ~CActivitySoftware();
protected:
    vector<const char*> keylist;
public:
    const char* product;
    const char* company;
protected:
    bool activitied = false;
public:
    bool ActivityProduct(string key);
    bool CheckActivity();
    bool GetInformation(CActivitySoftwareIFMS &ifms);
    bool GetIFMS(CActivitySoftwareIFMS &ifms);
};

void str_replace(std::string& strBase, std::string strSrc, std::string strDes);
void str_split(const std::string& src, const std::string& separator, std::vector<std::string>& dest);//º¯ÊýÔ­ÐÍ
void ActKeyFileToObj(CActivitySoftwareIFMS& ifms, string filename = "act.key");
vector<string>& GetActKeyList(vector<string>& ls, string filename = "act.key");
