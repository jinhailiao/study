//---------------------------------------------------------------------------------
// Copyright jinhailiao 2009-2010
// E-mail:   jinhailiao@163.com
// Project:			HDJS
// File:			main.cpp
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2009-11-11  Create                  Kingsea
//---------------------------------------------------------------------------------
#include "Command.h"

int main(int argc,char* argv[])
{
	Command command(argc,argv);

	return command.Exec();
}

