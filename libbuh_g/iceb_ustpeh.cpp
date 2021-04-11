/*$Id: iceb_ustpeh.c,v 1.10 2013/08/13 06:10:14 sasa Exp $*/
/*19.02.2015 	05.09.2009	Белых А.И.  iceb_ustpeh.c
Определение максимально длинной строки в файле
и установка команд для печати файла с нужным ужатием и ориентацией
Возвращает установленный тип ориентации 
 0-портрет
 1-ландшафт
-1-ошибка

в переменной kolstr возвращается количество строк которое помещается на листе в соотвецтвии  
с выбранной ориентацией и указанным меж-строчным интервалом
*/
#include <ctype.h>
#include <errno.h>
#include "iceb_libbuh.h"
#include "iceb_print_operation.h" /*заданы ширина базового символа и коэффициенты перевода размера базового символа в различные режимы печати*/

const double max_razmer_gor=559.275591; /*максимальный размер зоны печати по горизонтали в пунктах в портретной ориентации*/
const double max_razmer_ver=769.889764; /*максимальный размер зоны печати по вертикали в пунктах в портретной ориентации*/

int iceb_t_ustpeh_kus(const char *stroka,int *font_size,double *rmsi);
int iceb_ustpeh_ras(int dlina_str,int orient,int shrift,FILE *ff,GtkWidget *wpredok);

int iceb_ustpeh(const char *imaf,int orient,int *kolstr,GtkWidget *wpredok) /*0-любая 1-только портрет 2-только ландшафт 3-сначала попытаться портрет и если не помещается ландшафт*/
{
class iceb_u_str stroka("");
class iceb_u_str imaf_tmp(imaf);
imaf_tmp.plus(".tmp");
*kolstr=0;
FILE *ff;
FILE *ff_tmp;

if((ff=fopen(imaf,"r")) == NULL)
 {
  iceb_er_op_fil(imaf,__FUNCTION__,errno,wpredok);
  return(-1);
 }

int max_dlina_str=0;
int dlina_str=0;
int font_size=10;
double rmsi=25.4/6; /*стандартный размер межстрочного интервала в милиметрах*/
while(iceb_u_fgets(&stroka,ff) == 0)
 {
  dlina_str=iceb_u_strlen(stroka.ravno());
  dlina_str-=iceb_t_ustpeh_kus(stroka.ravno(),&font_size,&rmsi);
  if(max_dlina_str < dlina_str)
    max_dlina_str=dlina_str;
 }

if((ff_tmp=fopen(imaf_tmp.ravno(),"w")) == NULL)
 {
  iceb_er_op_fil(imaf_tmp.ravno(),__FUNCTION__,errno,wpredok);
  return(-1);
 }

iceb_u_startfil(ff_tmp);

int voz=iceb_ustpeh_ras(max_dlina_str,orient,font_size,ff_tmp,wpredok);

rewind(ff);
while(iceb_u_fgets(&stroka,ff) == 0)
 fprintf(ff_tmp,"%s",stroka.ravno());

fclose(ff);
fclose(ff_tmp);

/*вычиляем сколько строк помещается на листе*/
double razmer=max_razmer_ver;
if(voz == 1)
 razmer=max_razmer_gor;
*kolstr=razmer/(rmsi*PEREVOD_V_PUNKT);
//printf("%s-количество строк на листе %d\n",__FUNCTION__,*kolstr);

rename(imaf_tmp.ravno(),imaf);
return(voz);
}

/************************************/
/*оперделяем количество управляющих символов в строке и концевых пробелов*/
/*****************************************************/
int iceb_t_ustpeh_kus(const char *stroka,
int *font_size, /*размер форнта по вертикали в пунктах*/
double *rmsi)  /*размер межстрочного интервала в милиметрах*/
{
int kolih_simv=0;
int kolih_simv_v_str=strlen(stroka);

/*проверяем может в конце строки есть пробелы*/
for(int nom=kolih_simv_v_str-1; nom >= 0 ; nom--)
 {
  if(stroka[nom] == '\n' || stroka[nom] == '\r') /*учитываем в следующей проверке*/
   continue;
  if(stroka[nom] == ' ')
   kolih_simv++;     
  else
   break;
 }
/* ищем управляющие символы в строке*/
int kod_simv=0;
for(int nom=0; nom < kolih_simv_v_str; nom++)
 {
  if(iscntrl(kod_simv=stroka[nom])) //Проверка на управляющий символ
  {
   switch(kod_simv)
    {
    /*
     *   ESC-последовательности
     */
     case 27: //Esc
      kolih_simv++;
      nom++;
      switch (stroka[nom])
       {
         case 15: /*Включение уплотнённого режима печати*/
           kolih_simv++;
           continue;           

         case '-': //Включить/выключить режим подчеркивания
           kolih_simv++;
           nom++; //Увеличиваем счётчик потому, что нужен следующий элемент строки
           continue;

         case '0':		/* межстрочный интервал 1/8 inch */
           kolih_simv++;
           continue;

         case '2': //Межстрочный интервал 1/6 дюйма
           kolih_simv++;
           *rmsi=25.4/6;
           continue;

         case '3': //Межстрочный интервал n/216 дюйма
           kolih_simv++;
           nom++; //Увеличиваем счётчик потому, что нужен следующий элемент строки
           *rmsi=25.4*stroka[nom]/216;
           continue;

         case '[': //изменение шрифта
           kolih_simv++;
           nom++; //Увеличиваем счётчик потому, что нужен следующий элемент строки
           *font_size=stroka[nom];
           continue;

         case 'A': //Межстрочный интервал n/72 дюйма
           kolih_simv++;
           nom++;  //Увеличиваем счётчик потому, что нужен следующий элемент строки
           *rmsi=25.4*stroka[nom]/72;
           continue;

         case 'G': //Двуударный режим печати
         case 'E': //Выделенный шрифт
           kolih_simv++;
           continue;

         case 'H': //Отмена двуударного режима печати
         case 'F': //Отмена выделенного шрифта
           kolih_simv++;
           continue;

         case 'P': //Десять знаков на дюйм
           kolih_simv++;
           continue;

         case 'M': //Двенадцать знаков на дюйм
           kolih_simv++;
           
           continue;

         case 'g': //Пятнадцать знаков на дюйм (применяестся не на всех принтерах)
           kolih_simv++;
           continue;

                
         case 'l': //Установка левого поля
           kolih_simv++;
           nom++; //Увеличиваем счётчик потому, что нужен следующий элемент строки
           continue;
                
         /* Нереализованные трехсимвольные команды*/
         case 'U': //Включение/выключение однонаправленного режима печати
         case 'x': //Включение/выключение высококачественного режима печати
         case 'S': //Выбор режима печати индексов
         case 'W': //Включение/выключение  режима печати расширенными символами
         case 'a': //Выравнивание текста по правую или левую границу печати
         case 'J': //Перевод бумаги в прямом направлении на заданное количество елементрарных шагов 
         case 'j': //Перевод бумаги в обратном направлении на заданное количество елементрарных шагов
           kolih_simv++;
           nom++;  //Трех-символьная команда игнорируем третий символ
           continue;
                
         case '@': //Инициализация принтера
           kolih_simv++;
            //счётчик не увеличиваем-двубайтная последовательность
            continue;

                
         default: //Игнорируются неизвестные двубайтные последовательности
           kolih_simv++;
           continue;
       }
  
    /*
     *   1-байтные управляющие символы
     */
    case 14: //^N - Двойная ширина символа для одной строки
      kolih_simv++;
      continue;


    case 15: //^O - Включить уплотненный режим печати
      kolih_simv++;
      continue;
  
    case 18: //^R - Выключить уплотненный режим печати
      kolih_simv++;
      continue;

    case 20: //^T - Отмена двойной ширины символа для одной строки
      kolih_simv++;
      continue;
  
    case '\n':  //Перевод строки. 
      kolih_simv++;
      continue;

    case '\r':  /*возврат каретки игнорируем*/
      kolih_simv++;
      continue;

    case '\f':  //Перевод формата
      kolih_simv++;
      continue;

    case '\t':  //Табулятор
      kolih_simv++;
      continue;

    default:
      kolih_simv++;
      /* остальные управляющие символы пропускаем */
      continue;
      
    }
  } 
 }

return(kolih_simv);



}

/******************************************/
/*                           */
/********************************/
int iceb_ustpeh_ras(int dlina_str,int orient,int shrift,FILE *ff,GtkWidget *wpredok)
{

/*эти размеры определены с помощью программы iceb_print_operation.c она определяет зону печати и выводит размеры на экран
причём при чистом просмотре она выдаёт больший рамер по вертикали чем при просмотре из режима печати. Здес применяется меньший из размеров*/

int kolih_simv=0;
int otstup=0;
double hir_simv=hir_simv_10;
double max_razmer=max_razmer_gor;

//printf("%s-шрифт %d пунктов\n",__FUNCTION__,shrift);

if(shrift == 8)
  hir_simv=hir_simv_8;

if(shrift != 8 && shrift != 10)
 {
  class iceb_u_str repl(gettext("Не предусмотренный размер шрифта"));
  repl.plus(" ");
  repl.plus(shrift);
  repl.plus("!");
  iceb_menu_soob(repl.ravno(),wpredok);
  return(-1);
 }

if(orient == 0) /*Ориентация в которой максимальная ширина букв */
 {
  /***********проверяется в следующем порядке: 
  10 портрет
  10 ландшафт
  12 портрет
  12 ландшафт
  10 портрет ужат
  10 ландшафт ужат
  12 портрет ужат
  12 ландшафт ужат
  **********************/
  kolih_simv=max_razmer_gor/hir_simv;
  if(dlina_str <= kolih_simv) /*десять знаков на дюйм в нормальном режиме*/
   {
    fprintf(ff,"\x12"); /*Выключить ужатый режим*/
    fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/

    otstup=kolih_simv-dlina_str;
    if(otstup > 10)
     otstup=10;
    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
   //printf("%s-10 знаков портрет отступ %d\n",__FUNCTION__,otstup);
    return(0);   
   }

  kolih_simv=max_razmer_ver/hir_simv;

  if(dlina_str <= kolih_simv) /*десять знаков на дюйм ландшафт*/
   {
    fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
    fprintf(ff,"\x12"); /*Выключить ужатый режим*/
    fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/
    otstup=kolih_simv-dlina_str;

    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
   //printf("%s-10 знаков ландшафт отступ %d\n",__FUNCTION__,otstup);
    return(1);   
   }

  kolih_simv=max_razmer_gor/hir_simv/koef12;

  if(dlina_str <= kolih_simv) /*двенадцать знаков на дюйм в нормальном режиме*/
   {
    fprintf(ff,"\x12"); /*Выключить ужатый режим*/
    fprintf(ff,"\x1B\x4D"); /*12-знаков*/
    otstup=kolih_simv-dlina_str;
    if(otstup > 10)
     otstup=10;
    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
   //printf("%s-12 знаков портрет отступ %d\n",__FUNCTION__,otstup);
    return(0);   
   }

  kolih_simv=max_razmer_ver/hir_simv/koef12;
  if(dlina_str <= kolih_simv) /*двенадцать знаков на дюйм ландшафт*/
   {
    fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
    fprintf(ff,"\x12"); /*Выключить ужатый режим*/
    fprintf(ff,"\x1B\x4D"); /*12-знаков*/
    otstup=kolih_simv-dlina_str;
    if(otstup > 10)
     otstup=10;
    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
   //printf("%s-12 знаков ландшафт отступ %d\n",__FUNCTION__,otstup);
    return(1);   

   }
  
  /*ужатия*/
  
  kolih_simv=max_razmer_gor/(hir_simv/koef10u);
  if(dlina_str <= kolih_simv) /*десять знаков на дюйм портрет ужат*/
   {
    fprintf(ff,"\x0F"); /*Включить ужатый режим*/
    fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/
    otstup=kolih_simv-dlina_str;

    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
   //printf("%s-10 знаков ужатых подтрет отступ %d\n",__FUNCTION__,otstup);
    return(0);   
   }

  kolih_simv=max_razmer_ver/hir_simv/koef10u;

  if(dlina_str <= kolih_simv) /*десять знаков на дюйм ландшаф ужат*/ 
   {
    fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
    fprintf(ff,"\x0F"); /*Включить ужатый режим*/
    fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/
    otstup=kolih_simv-dlina_str;
    if(otstup > 15)
     otstup=15;
    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
   //printf("%s-10 знаков ужатых ландшафт отступ %d\n",__FUNCTION__,otstup);
    return(1);   

   }

  kolih_simv=max_razmer_gor/hir_simv/koef12u;

  if(dlina_str <= kolih_simv) /*двенадцать знаков на дюйм портрет ужат*/
   {
    fprintf(ff,"\x0F"); /*Включить ужатый режим*/
    fprintf(ff,"\x1B\x4D"); /*12-знаков*/
    otstup=kolih_simv-dlina_str;

    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
   //printf("%s-12 знаков ужатых потртер отступ %d\n",__FUNCTION__,otstup);
    return(0);   
   }

  kolih_simv=max_razmer_ver/(hir_simv/koef12u);

  if(dlina_str <= kolih_simv) /*двенадцать знаков на дюйм ландшаф ужат*/ 
   {
    fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
    fprintf(ff,"\x0F"); /*Включить ужатый режим*/
    fprintf(ff,"\x1B\x4D"); /*12-знаков*/
    otstup=kolih_simv-dlina_str;
    if(otstup > 15)
     otstup=15;
    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
   //printf("%s-12 знаков ужатых ландшафт отступ %d\n",__FUNCTION__,otstup);
    return(1);   
   }

  /*портрет 12 знаков ужат*/
  kolih_simv=max_razmer_gor/(hir_simv/koef12u);

  fprintf(ff,"\x0F"); /*Включить ужатый режим*/
  fprintf(ff,"\x1B\x4D"); /*12-знаков*/
  return(0);   

 } 

/*************************************/

if(orient == 1) /*только портрет*/
 {
  kolih_simv=max_razmer/hir_simv;
  if(dlina_str <= kolih_simv)
   {
    fprintf(ff,"\x12"); /*Выключить ужатый режим*/
    fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/

    otstup=kolih_simv-dlina_str;
    if(otstup > 10)
     otstup=10;
    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/

    return(0);   

   }

  kolih_simv=max_razmer/(hir_simv/koef12);

  if(dlina_str <= kolih_simv) /*двенадцать знаков на дюйм в нормальном режиме*/
   {
    fprintf(ff,"\x12"); /*Выключить ужатый режим*/
    fprintf(ff,"\x1B\x4D"); /*12-знаков*/
    otstup=kolih_simv - dlina_str;
    if(otstup > 10)
     otstup=10;
    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
    return(0);   
   }

  kolih_simv=max_razmer/(hir_simv/koef10u);

  if(dlina_str <= kolih_simv) /*десять знаков на дюйм портрет ужат*/
   {
    fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/
    fprintf(ff,"\x0F"); /*Включить ужатый режим*/
    otstup=kolih_simv-dlina_str;
    if(otstup > 15)
     otstup=15;

    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
    return(0);   
   }

  /*12 знаков на дюйм ужат*/
  fprintf(ff,"\x0F"); /*Включить ужатый режим*/
  fprintf(ff,"\x1B\x4D"); /*12-знаков*/
  otstup=kolih_simv-dlina_str;

  if(otstup > 0)
   fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
  
  return(0);
 }
/*************************************/

if(orient == 2) /*только ландшафт*/
 {
  max_razmer=max_razmer_ver;
  kolih_simv=max_razmer/hir_simv;
  if(dlina_str <= kolih_simv)
   {
    fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
    fprintf(ff,"\x12"); /*Выключить ужатый режим*/
    fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/
    otstup=kolih_simv-dlina_str;

    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
    return(1);   
   }

  kolih_simv=max_razmer/(hir_simv/koef12);

  if(dlina_str <= kolih_simv) /*двенадцать знаков на дюйм в нормальном режиме*/
   {
    fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
    fprintf(ff,"\x12"); /*Выключить ужатый режим*/
    fprintf(ff,"\x1B\x4D"); /*12-знаков*/
    otstup=kolih_simv-dlina_str;
    if(otstup > 10)
     otstup=10;
    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
    return(1);   

   }

  kolih_simv=max_razmer/(hir_simv/koef10u);

  if(dlina_str <= kolih_simv) /*десять знаков на дюйм портрет ужат*/
   {
    fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
    fprintf(ff,"\x0F"); /*Включить ужатый режим*/
    fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/
    otstup=kolih_simv-dlina_str;
    if(otstup > 15)
     otstup=15;
    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
    return(1);   

   }

  kolih_simv=max_razmer/(hir_simv/koef12u);

  if(dlina_str <= kolih_simv ) /*двенадцать знаков на дюйм портрет ужат*/
   {
    fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
    fprintf(ff,"\x0F"); /*Включить ужатый режим*/
    fprintf(ff,"\x1B\x4D"); /*12-знаков*/
    otstup=kolih_simv-dlina_str;
    if(otstup > 15)
     otstup=15;
    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
    return(1);   

   }


  kolih_simv=max_razmer/(hir_simv/koef15u);
  fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
  fprintf(ff,"\x0F"); /*Включить ужатый режим*/
    fprintf(ff,"\x1B%c",'g'); /*15-знаков*/
  otstup=kolih_simv-dlina_str;
  if(otstup > 15)
   otstup=15;
  if(otstup > 0)
   fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
  return(1);   


 }

/*************************************/

if(orient == 3) /*Проверяем сначала портрет а потом ландшафт*/
 {
  /*сначала проверяем не ужатые режимы по возростаний количества знаков на дюйм потом также с ужатием если не нашли то 12 ужатый*/
  kolih_simv=max_razmer/hir_simv;
  if(dlina_str <= kolih_simv) /*10 знаков на дюйм портрет*/
   {
    fprintf(ff,"\x12"); /*Выключить ужатый режим*/
    fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/

    otstup=kolih_simv-dlina_str;

    if(otstup > 10)
     otstup=10;

   //printf("%s-kolih_simv=%d dlina_str=%d otstup=%d 10 знаков на дюйм\n",__FUNCTION__,kolih_simv,dlina_str,otstup);

    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
    return(0);   
   }

  kolih_simv=max_razmer/(hir_simv/koef12);

  if(dlina_str <= kolih_simv) /*12 знаков на дюйм портрет*/
   {
    fprintf(ff,"\x12"); /*Выключить ужатый режим*/
    fprintf(ff,"\x1B\x4D"); /*12-знаков*/
    otstup=kolih_simv-dlina_str;
    if(otstup > 10)
     otstup=10;
   //printf("%s-kolih_simv=%d kolih_simv=%d dlina_str=%d otstup=%d 12 знаков на дюйм\n",__FUNCTION__,kolih_simv,kolih_simv,dlina_str,otstup);
    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
    return(0);   
   }

  kolih_simv=max_razmer/(hir_simv/koef15);

  if(dlina_str <= kolih_simv) /*15 знаков на дюйм портрет*/
   {
    fprintf(ff,"\x12"); /*Выключить ужатый режим*/
    fprintf(ff,"\x1B\x67"); /*15-знаков*/
    otstup=kolih_simv-dlina_str;
    if(otstup > 10)
     otstup=10;
   //printf("%s-kolih_simv=%d kolih_simv=%d dlina_str=%d otstup=%d 15 знаков на дюйм\n",__FUNCTION__,kolih_simv,kolih_simv,dlina_str,otstup);
    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
    return(0);   
   }

  kolih_simv=max_razmer/(hir_simv/koef10u);
  if(dlina_str <= kolih_simv) /*10 знаков на дюйм портрет ужат*/
   {
    fprintf(ff,"\x0F"); /*Включить ужатый режим*/
    fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/
    otstup=kolih_simv-dlina_str;

    if(otstup > 15)
     otstup=15;
   //printf("%s-kolih_simv=%d kolih_simv=%d dlina_str=%d otstup=%d 10 знаков на дюйм портрет ужат\n",__FUNCTION__,kolih_simv,kolih_simv,dlina_str,otstup);

    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
    return(0);   
   }


  kolih_simv=max_razmer/(hir_simv/koef12u);
  if(dlina_str <= kolih_simv) /*двенадцать знаков на дюйм портрет ужат*/
   {
    fprintf(ff,"\x0F"); /*Включить ужатый режим*/
    fprintf(ff,"\x1B\x4D"); /*12-знаков*/
    otstup=kolih_simv-dlina_str;
    if(otstup > 15)
     otstup=15;
//   //printf("%s-%f/(%f/%f)=%f\n",__FUNCTION__,max_razmer,hir_simv,koef12u,max_razmer/(hir_simv/koef12u));
   //printf("%s-kolih_simv=%d kolih_simv=%d dlina_str=%d otstup=%d 12 знаков на дюйм портрет ужат\n",__FUNCTION__,kolih_simv,kolih_simv,dlina_str,otstup);
    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
    return(0);   
   }

  kolih_simv=max_razmer/(hir_simv/koef15u);
  if(dlina_str <= kolih_simv) /*15 знаков на дюйм портрет ужат*/
   {
    fprintf(ff,"\x0F"); /*Включить ужатый режим*/
    fprintf(ff,"\x1B\x67"); /*15-знаков*/
    otstup=kolih_simv-dlina_str;
    if(otstup > 15)
     otstup=15;
   //printf("%s-kolih_simv=%d kolih_simv=%d dlina_str=%d otstup=%d 12 знаков на дюйм портрет ужат\n",__FUNCTION__,kolih_simv,kolih_simv,dlina_str,otstup);
    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
    return(0);   
   }

  //проверяем ландшафтную ориентацию *********************************

  max_razmer=max_razmer_ver; /*переводим в ландшафт*/
  kolih_simv=max_razmer/(hir_simv);
  if(dlina_str <= kolih_simv) /*10 знаков на дюйм ландшафт*/ 
   {
    fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
    fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/
    otstup=kolih_simv-dlina_str;
    if(otstup > 15)
     otstup=15;
   //printf("%s-kolih_simv=%d kolih_simv=%d dlina_str=%d otstup=%d 10 знаков на дюйм ландшаф\n",__FUNCTION__,kolih_simv,kolih_simv,dlina_str,otstup);
    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
    return(1);   
   }
 
  kolih_simv=max_razmer/(hir_simv/koef12);
  if(dlina_str <= kolih_simv) /*12 знаков на дюйм ландшафт*/ 
   {
    fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
    fprintf(ff,"\x1B\x4D"); /*12-знаков*/
    otstup=kolih_simv-dlina_str;
    if(otstup > 15)
     otstup=15;
   //printf("%s-kolih_simv=%d kolih_simv=%d dlina_str=%d otstup=%d 12 знаков на дюйм ландшаф\n",__FUNCTION__,kolih_simv,kolih_simv,dlina_str,otstup);
    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
    return(1);   
   }

  kolih_simv=max_razmer/(hir_simv/koef15);
  if(dlina_str <= kolih_simv) /*15 знаков на дюйм ландшафт*/ 
   {
    fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
    fprintf(ff,"\x1B\x67"); /*15-знаков*/
    otstup=kolih_simv-dlina_str;
    if(otstup > 15)
     otstup=15;
   //printf("%s-kolih_simv=%d kolih_simv=%d dlina_str=%d otstup=%d 15 знаков на дюйм ландшаф\n",__FUNCTION__,kolih_simv,kolih_simv,dlina_str,otstup);
    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
    return(1);   
   }

  kolih_simv=max_razmer/(hir_simv/koef10u);
  if(dlina_str <= kolih_simv) /*10 знаков на дюйм ландшафт ужатый*/ 
   {
    fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
    fprintf(ff,"\x0F"); /*Включить ужатый режим*/
    fprintf(ff,"\x1B\x50"); /*10-знаков на дюйм*/
    otstup=kolih_simv-dlina_str;
    if(otstup > 15)
     otstup=15;
   //printf("%s-kolih_simv=%d kolih_simv=%d dlina_str=%d otstup=%d 10 знаков на дюйм ландшаф ужатый\n",__FUNCTION__,kolih_simv,kolih_simv,dlina_str,otstup);
    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
    return(1);   
   }

  kolih_simv=max_razmer/(hir_simv/koef12u);
  if(dlina_str <= kolih_simv) /*12 знаков на дюйм ландшафт ужат*/ 
   {
    fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
    fprintf(ff,"\x0F"); /*Включить ужатый режим*/
    fprintf(ff,"\x1B\x4D"); /*12-знаков*/
    otstup=kolih_simv-dlina_str;
    if(otstup > 15)
     otstup=15;
   //printf("%s-kolih_simv=%d kolih_simv=%d dlina_str=%d otstup=%d 12 знаков на дюйм ландшаф ужатый\n",__FUNCTION__,kolih_simv,kolih_simv,dlina_str,otstup);
    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
    return(1);   
   }

  kolih_simv=max_razmer/(hir_simv/koef15u);
  if(dlina_str <= kolih_simv) /*15 знаков на дюйм ландшафт ужат*/ 
   {
    fprintf(ff,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/
    fprintf(ff,"\x0F"); /*Включить ужатый режим*/
    fprintf(ff,"\x1B\x67"); /*15-знаков*/
    otstup=kolih_simv-dlina_str;
    if(otstup > 15)
     otstup=15;
   //printf("%s-kolih_simv=%d kolih_simv=%d dlina_str=%d otstup=%d 15 знаков на дюйм ландшаф ужатый\n",__FUNCTION__,kolih_simv,kolih_simv,dlina_str,otstup);
    if(otstup > 0)
     fprintf(ff,"\x1b\x6C%c",otstup); /*Установка левого поля*/
    return(1);   
   }

  max_razmer=max_razmer_gor; /*переводим в портрет*/
  kolih_simv=max_razmer/(hir_simv/koef15u);
  fprintf(ff,"\x0F"); /*Включить ужатый режим*/
  fprintf(ff,"\x1B\x4D"); /*12-знаков*/

 //printf("%s-kolih_simv=%d dlina_str=%d  max_razmer=%f hir_simv=%f koef12u=%f 15 знаков на дюйм портрет ужат\n",
//  __FUNCTION__,kolih_simv,dlina_str,max_razmer,hir_simv,koef12u);
  return(0);   

 }

class iceb_u_str repl(__FUNCTION__);
repl.plus("-",gettext("Неизвестный код ориентации"));
repl.plus(" ");
repl.plus(orient);
iceb_menu_soob(repl.ravno(),wpredok);

return(-1);




}
/***************************************/
int iceb_ustpeh(const char *imaf,int orient,GtkWidget *wpredok) /*0-любая 1-только портрет 2-только ландшафт 3-сначала попытаться портрет и если не помещается ландшафт*/
{
int kolstr=0;
return(iceb_ustpeh(imaf,orient,&kolstr,wpredok));
}
