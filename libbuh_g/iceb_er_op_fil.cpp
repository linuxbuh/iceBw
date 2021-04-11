/*$Id: iceb_er_op_fil.c,v 1.10 2013/05/17 14:58:34 sasa Exp $*/
/*31.12.2017	30.10.2003	Белых А.И.	iceb_er_op_fil.c
Сообщение об ошибке открытия файла
*/
#include  "iceb_libbuh.h"


void iceb_er_op_fil(const char *imafil,const char *repl,int kodoh,GtkWidget *wpredok)
{
char stroka[300];
iceb_u_str text;

sprintf(stroka,"%s: %d",gettext("Ошибка открытия файла"),kodoh);
text.plus_ps(stroka);

text.plus_ps(strerror(kodoh));
text.plus(imafil);
if(repl != NULL && repl[0] != '\0')
  text.ps_plus(repl);
iceb_beep(); 
iceb_menu_soob(&text,wpredok);

}
