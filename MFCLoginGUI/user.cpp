#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include<Windows.h>
#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<ctime>

#include "pch.h"

#include "user.h"
using namespace std;

string ws2s(wstring wstr)
{
    string result;
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
    if (len <= 0)return result;
    char* buffer = new char[len + 1];
    if (buffer == NULL)return result;
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
    buffer[len] = '\0';
    result.append(buffer);
    delete[] buffer;
    return result;
}
wstring s2ws(string str)
{
    wstring result;
    int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
    if (len < 0)return result;
    wchar_t* buffer = new wchar_t[len + 1];
    if (buffer == NULL)return result;
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
    buffer[len] = '\0';
    result.append(buffer);
    delete[] buffer;
    return result;
}


std::string userud::getUserName()
{
	return this->name;
}

bool userud::setUserName(string u)
{
	this->name = u;
	return true;
}

bool userud::verifyPasswd(string pw)
{
	return pw == this->password;
}

bool userud::changePasswd(string pw){
	this->password = pw;
	return true;
}


userud::userud() {}

userud::userud(string username, string pwd)
{
	this->name = username;
	this->password = pwd;
}

userud::~userud()
{

}

void LoginedUser::UploadLoginStatus()
{
}

LoginedUser::LoginedUser()
{
}

LoginedUser::LoginedUser(string name)
{
	this->name = name;
	this->logintime = time(0);
}

LoginedUser::LoginedUser(string name, bool JiZhuPwd)
{
	this->name = name;
	this->logintime = time(0);
	this->noForgotPwd = JiZhuPwd;
}

LoginedUser::~LoginedUser()
{
}

bool LoginedUser::setUsername(string)
{
	return false;
}

bool LoginedUser::changePasswd(string)
{
	return false;
}

userudEx::userudEx()
{
}

userudEx::userudEx(string n, string p)
{
    this->name = n;
    this->password = p;
}

userudEx::~userudEx()
{
}

string userudEx::getSex()
{
    return this->sex;
}

bool userudEx::setSex(string sex)
{
    this->sex = sex;
    return true;
}

string userudEx::getBirthday()
{
    return this->birthday;
}

bool userudEx::setBirthday(string bth)
{
    this->birthday = bth;
    return true;
}

int userudEx::getAge()
{
    return 0;
}

string userudEx::getOccupation()
{
    return this->occupation;
}

bool userudEx::setOccupation(string occ)
{
    this->occupation = occ;
    return true;
}

string userudEx::getQQ()
{
    return this->QQNumber;
}

bool userudEx::setQQ(string qq)
{
    this->QQNumber = qq;
    return true;
}

string userudEx::getWeiXin()
{
    return this->WeiXinHao;
}

bool userudEx::setWeiXin(string wx)
{
    this->WeiXinHao = wx;
    return true;
}

string userudEx::getGithub()
{
    return this->GithubID;
}

bool userudEx::setGithub(string ghname)
{
    this->GithubID = ghname;
    return true;
}
