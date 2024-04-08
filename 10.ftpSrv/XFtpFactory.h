#pragma once
#include "XTask.h"

class XFtpFactory
{
public:
	XTask *CreateTask();
	static XFtpFactory *Get();

private:
	XFtpFactory();
};
