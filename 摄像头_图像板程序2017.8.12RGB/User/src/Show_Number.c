#include "Show_Number.h"


void number_show(int x,int y,int a,int n)
{
	char t[4] = {'0'};
	sprintf(t,"%d",a);
	GUI_Text(x,y,(u8*)t,n,Black,White);
}
void float_number_show(int x,int y,float b)
{
	char s[8] = {'0'};
	sprintf(s,"%5.2f",b);
	GUI_Text(x,y,(u8*)s,5,Black,White);
}
