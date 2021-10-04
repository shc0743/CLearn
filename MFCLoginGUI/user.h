#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include<Windows.h>
#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<ctime>

using std::string;
using std::ios;
//基础UserData容器,存储用户名和密码
class userud {
protected:
	string name;
	string password;
public:
	string getUserName();
	bool setUserName(string);
	bool verifyPasswd(string);
	bool changePasswd(string);
public:
	userud();
	userud(string, string);
	~userud();
};
//扩展的UserData容器,存储更多用户个人信息
class userudEx :public userud{
public:
	//构造函数
	userudEx();
	userudEx(string,string);
	//析构函数
	~userudEx();
protected:
	string sex;
public:
	string getSex();
	bool setSex(string sex);
protected:
	string birthday;
public:
	string getBirthday();
	bool setBirthday(string bth);
	int getAge();
protected:
	string occupation;
public:
	string getOccupation();
	bool setOccupation(string occ);
protected:
	string QQNumber;
	string WeiXinHao;
	string GithubID;
public:
	string getQQ();
	bool setQQ(string qq);
	string getWeiXin();
	bool setWeiXin(string wx);
	string getGithub();
	bool setGithub(string ghname);

};
//已登录的用户,存储基本信息
class LoginedUser :public userud {
public:
	void UploadLoginStatus();
protected:
	int logintime = 0;
	bool noForgotPwd = false;
public:
	LoginedUser();
	LoginedUser(string name);
	LoginedUser(string name,bool JiZhuPwd);
	~LoginedUser();
	bool setUsername(string);
	bool changePasswd(string);
};
//std::wstring转换到std::string
std::string ws2s(std::wstring);
//std::string转换到std::wstring
std::wstring s2ws(std::string);
