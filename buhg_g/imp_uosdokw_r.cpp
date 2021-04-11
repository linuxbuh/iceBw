/*$Id: imp_uosdokw_r.c,v 1.17 2014/07/31 07:08:25 sasa Exp $*/
/*23.05.2016	20.12.2007	Белых А.И.	imp_uosdokw_r.c
импорт документов в подсистему "Учёт основных средств"
*/
#include  <errno.h>
#include  <sys/stat.h>
#include "buhg_g.h"

class imp_uosdokw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;
    
  class iceb_u_str imafz;
        
  int voz; //0-все в порядке документы загружены 1-нет
  short kon_ras; //0-расчёт завершен 1-нет
  imp_uosdokw_r_data()
   {
    kon_ras=1;
    voz=1;
   }
 };
gboolean   imp_uosdokw_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_uosdokw_r_data *data);
gint imp_uosdokw_r1(class imp_uosdokw_r_data *data);
void  imp_uosdokw_r_v_knopka(GtkWidget *widget,class imp_uosdokw_r_data *data);

int imp_kasord_zag(int metka,int razmer_fil,class iceb_u_file *ff,class imp_uosdokw_r_data*);

extern SQL_baza bd;
extern double okrcn;

int imp_uosdokw_r(const char *imaf,
GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_spisok repl_s;
class imp_uosdokw_r_data data;
int gor=0;
int ver=0;

data.imafz.new_plus(imaf);
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Импорт документов"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(imp_uosdokw_r_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add(GTK_CONTAINER(data.window), vbox);

data.label=gtk_label_new(gettext("Ждите !!!"));

gtk_box_pack_start(GTK_BOX(vbox),data.label,FALSE,FALSE,0);

GtkWidget *label=NULL;
label=gtk_label_new(gettext("Импорт документов"));
gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);

data.view=gtk_text_view_new();

gtk_text_view_set_editable(GTK_TEXT_VIEW(data.view),FALSE); //Запрет на редактирование текста

data.buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data.view));

GtkWidget *sw=gtk_scrolled_window_new(NULL,NULL);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
gtk_container_add(GTK_CONTAINER(sw),data.view);

data.bar=gtk_progress_bar_new();
gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(data.bar),TRUE); /*Показывать текст в строке*/



gtk_box_pack_start (GTK_BOX (vbox), data.bar, FALSE, FALSE, 2);



sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(imp_uosdokw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)imp_uosdokw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  imp_uosdokw_r_v_knopka(GtkWidget *widget,class imp_uosdokw_r_data *data)
{
if(data->kon_ras == 1)
 return;
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   imp_uosdokw_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_uosdokw_r_data *data)
{
switch(event->keyval)
 {
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka,"clicked");
    break;
 }
return(TRUE);
}

/******************************************/
/******************************************/

gint imp_uosdokw_r1(class imp_uosdokw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[512];
iceb_clock sss(data->window);
struct stat work;

if(stat(data->imafz.ravno(),&work) != 0)
 {
  
  sprintf(strsql,"%s %s !",gettext("Нет найден файл"),data->imafz.ravno());
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  
 }

class iceb_u_file fil;

if((fil.ff = fopen(data->imafz.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(data->imafz.ravno(),"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int kolih_oh=0;
if((kolih_oh=imp_kasord_zag(0,work.st_size,&fil,data)) == 0)
 {
  rewind(fil.ff);
  imp_kasord_zag(1,work.st_size,&fil,data);
 }
else
 {
   sprintf(strsql,"%s %d !\n%s",gettext("Количество ошибок"),kolih_oh,
   gettext("Загрузка невозможна !"));
  iceb_menu_soob(strsql,data->window);
 }
fil.close();





data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);



//repl.new_plus(gettext("Загрузка завершена"));
//repl.ps_plus(gettext("Количество документов"));
//repl.plus(":");
//repl.plus(koldok);

//iceb_menu_soob(&repl,data->window);


data->voz=0;

return(FALSE);
}
/*************************/
/*Импорт*/
/**************************/

int imp_kasord_zag(int metka, //0-проверка на ошибки 1- загрузка
int razmer_fil,
class iceb_u_file *fil,class imp_uosdokw_r_data *data)
{
char strsql[1024];
if(metka == 0)
 sprintf(strsql,"%s\n",gettext("Проверка на наличие ошибок"));
if(metka == 1)
 sprintf(strsql,"\n\n%s\n",gettext("Загружаем документы из файла"));

iceb_printw(strsql,data->buffer,data->view);

int		koloh=0;

class iceb_u_str strok("");
short		dd,md,gd;
int		tipz=0;
class iceb_u_str kodop("");
class iceb_u_str kontr("");
class iceb_u_str nomdok("");
class iceb_u_str podr("");
class iceb_u_str motv("");
time_t		vrem;
class iceb_u_str innom("");
double		bsnu=0.;
double		iznu=0.;
double		bsbu=0.;
double		izbu=0.;
class iceb_u_str shetu("");
struct OPSHET	shetv;
class iceb_u_str hzatrat("");
class iceb_u_str hanuh("");
class iceb_u_str grupnu("");
class iceb_u_str grupbu("");
double		popkfnu=0.;
double		popkfbu=0.;
class iceb_u_str sost("");
class iceb_u_str naim("");
short		dv,mv,gv;
int		kol=0;
int		koldok=0;
int		razmer=0;
class iceb_u_str naim_kontr("");
double dpnds=0.; /*Действующий на момент создания документа процент НДС*/
int nom_kod_kontr=1;
time(&vrem);
class iceb_u_str rek1("");

while(iceb_u_fgets(&strok,fil->ff) == 0)
 {
  if(strok.ravno()[0] == '#')
   continue;
  razmer+=strlen(strok.ravno());
  iceb_pbar(data->bar,razmer_fil,razmer);    

  iceb_printw(strok.ravno(),data->buffer,data->view);

  if(iceb_u_polen(strok.ravno(),&rek1,1,'|') != 0)
   {
    continue;
   }

  if(iceb_u_SRAV(rek1.ravno(),"KON",0) == 0) /*запись реквизитов контрагента*/
   {
    iceb_zkvsk(strok.ravno(),&nom_kod_kontr,NULL,data->window);
    continue;
   }

/**************************************/
  if(iceb_u_SRAV(rek1.ravno(),"DOC",0) == 0)
   {
    nomdok.new_plus("");
    dd=md=gd=0;
    tipz=0;
    kol=0;
    
    iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),2,'|');
    if(iceb_u_rsdat(&dd,&md,&gd,strsql,1) != 0)
     {
      iceb_menu_soob(gettext("Не верно введена дата!"),data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),3,'|');

    tipz=0;
    if(strsql[0] == '+')
     {
      tipz=1;
      kol=1;
     }
    if(strsql[0] == '-')
     {
      tipz=2;
      kol=-1;
     }
    if(tipz == 0)
     {
      iceb_menu_soob(gettext("Не определён вид документа (приход/расход)!"),data->window);
      koloh++;
      continue;
     }
    
    iceb_u_polen(strok.ravno(),&kodop,4,'|');

    if(tipz == 1)
      sprintf(strsql,"select kod from Uospri where kod='%s'",kodop.ravno());
    if(tipz == 2)
      sprintf(strsql,"select kod from Uosras where kod='%s'",kodop.ravno());

    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      sprintf(strsql,"%s %s!",gettext("Не найден код операции"),kodop.ravno());
      
      iceb_menu_soob(strsql,data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok.ravno(),&kontr,5,'|');
/****************************
    sprintf(strsql,"select kodkon from Kontragent where kodkon='%s'",
    kontr.ravno());
    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kontr.ravno());
      
      iceb_menu_soob(strsql,data->window);
      koloh++;
      continue;
     }
****************************/
    iceb_u_polen(strok.ravno(),&podr,7,'|');
    sprintf(strsql,"select kod from Uospod where kod=%s",
    podr.ravno());
    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден код подразделения"),podr.ravno());
      
      iceb_menu_soob(strsql,data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok.ravno(),&motv,8,'|');
    sprintf(strsql,"select kod from Uosol where kod=%s",motv.ravno());
    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      sprintf(strsql,"%s %s !",gettext("Не найден мат. ответственный"),motv.ravno());
      
      iceb_menu_soob(strsql,data->window);
      koloh++;
      continue;
     }
   
    iceb_u_polen(strok.ravno(),&dpnds,9,'|');

    if(iceb_u_polen(strok.ravno(),&naim_kontr,10,'|') != 0)
     {
      iceb_menu_soob(gettext("Не найдено наименование контрагента"),data->window);
      koloh++;
      continue;
     }
    if(naim_kontr.getdlinna() <= 1)
     {
      iceb_menu_soob(gettext("Не введено наименование контрагента"),data->window);
      koloh++;
      continue;
     }

    /*определяем код контрагента в общем списке контрагентов*/
    if(iceb_get_kkfname(naim_kontr.ravno(),kontr.ravno(),&kontr,&nom_kod_kontr,data->window) < 0)
     {
      koloh++;
      continue;
     }

    //Номер документа определяем последним так как он является индикатором
    //наличия шапки документа
    iceb_u_polen(strok.ravno(),&nomdok,6,'|');
    if(iceb_u_SRAV(nomdok.ravno(),"авто",0) == 0 || nomdok.ravno()[0] == '\0')
      uosgetnd(gd,tipz,&nomdok,data->window);
    
    
    sprintf(strsql,"select nomd from Uosdok where tipz=%d and \
datd >= '%04d-01-01' and datd <= '%04d-12-31' and nomd='%s'",
    tipz,gd,gd,nomdok.ravno());
    if(iceb_sql_readkey(strsql,data->window) >= 1)
     {
      sprintf(strsql,gettext("С номером %s документ уже есть !"),nomdok.ravno());
      
      iceb_menu_soob(strsql,data->window);
      koloh++;
      continue;
     }

    //Запись в базу
    if(metka != 0 && nomdok.getdlinna() > 1 && koloh == 0)
     {
//      printw("Пишем\n");
//      refresh();
      iceb_printw("Запись шапки\n",data->buffer,data->view);

      sprintf(strsql,"insert into Uosdok (datd,tipz,kodop,kontr,nomd,podr,kodol,prov,ktoz,vrem,pn) \
values ('%04d-%02d-%02d',%d,'%s','%s','%s',%s,%s,%s,%d,%ld,%.10g)",
      gd,md,dd,tipz,kodop.ravno(),kontr.ravno(),nomdok.ravno(),podr.ravno(),motv.ravno(),"1",iceb_getuid(data->window),vrem,dpnds);

      if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
       koloh++;
      else
       koldok++;

     }        
     

   }

/**************************************/
  if(iceb_u_SRAV(rek1.ravno(),"ZP1",0) == 0)
   {
    iceb_u_polen(strok.ravno(),&innom,2,'|');
    sprintf(strsql,"select innom from Uosin where innom=%s",innom.ravno());
    if(iceb_sql_readkey(strsql,data->window) >= 1)
     {
      if(tipz == 1)
       {
        sprintf(strsql,"%s %s",innom.ravno(),gettext("Такой инвентарный номер уже есть !"));
        iceb_menu_soob(strsql,data->window);
        koloh++;
        continue;
       }
     }
    else
     if(tipz == 2)
      {
        sprintf(strsql,"%s %s",gettext("Не найден инвентарный номер"),innom.ravno());
        iceb_menu_soob(strsql,data->window);
        koloh++;
        continue;

      }

    iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),3,'|');
    bsnu=iceb_u_atof(strsql);

    iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),4,'|');
    iznu=iceb_u_atof(strsql);

    iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),5,'|');
    bsbu=iceb_u_atof(strsql);

    iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),6,'|');
    izbu=iceb_u_atof(strsql);

    iceb_u_polen(strok.ravno(),&shetu,7,'|');
    if(iceb_prsh1(shetu.ravno(),&shetv,data->window) != 0)
     {
      koloh++;
      continue;
     }

    iceb_u_polen(strok.ravno(),&hzatrat,8,'|');

    sprintf(strsql,"select kod from Uoshz where kod='%s'",hzatrat.ravno());

    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      sprintf(strsql,"%s %s %s",innom.ravno(),gettext("Не найден шифр аморт-отчислений"),hzatrat.ravno());
      koloh++;
      continue;
     }

    iceb_u_polen(strok.ravno(),&hanuh,9,'|');

    sprintf(strsql,"select kod from Uoshau where kod='%s'",hanuh.ravno());

    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      sprintf(strsql,"%s %s %s",innom.ravno(),gettext("Не найден шифр аналитического учета"),hanuh.ravno());
      iceb_menu_soob(strsql,data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok.ravno(),&grupnu,10,'|');

    sprintf(strsql,"select kod from Uosgrup where kod='%s'",grupnu.ravno());

    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      sprintf(strsql,"%s %s %s\n%s",innom.ravno(),gettext("Не найден код группы"),grupnu.ravno(),gettext("Налоговый учёт"));
      iceb_menu_soob(strsql,data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),11,'|');
    popkfnu=iceb_u_atof(strsql);
    
    iceb_u_polen(strok.ravno(),&grupbu,12,'|');

    sprintf(strsql,"select kod from Uosgrup1 where kod='%s'",grupbu.ravno());

    if(iceb_sql_readkey(strsql,data->window) != 1)
     {
      sprintf(strsql,"%s %s %s\n%s",innom.ravno(),gettext("Не найден код группы"),grupbu.ravno(),gettext("Бухгалтерский учёт"));
      iceb_menu_soob(strsql,data->window);
      koloh++;
      continue;
     }

    iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),13,'|');
    popkfbu=iceb_u_atof(strsql);

    iceb_u_polen(strok.ravno(),&sost,14,'|');

    iceb_u_polen(strok.ravno(),&naim,15,'|');
    
    iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),16,'|');
    iceb_u_rsdat(&dv,&mv,&gv,strsql,1);

    //Запись в базу
    if(metka != 0 && nomdok.getdlinna() > 1 && innom.ravno()[0] != '\0' && koloh == 0)
     {
      if(tipz == 1)
       {
        iceb_printw("Запись инвентарного номера\n",data->buffer,data->view);

        sprintf(strsql,"insert into Uosin \
values (%s,%s,'%s','%s','%s','%s','%s','%04d-%d-%d',%d,%ld)",
        innom.ravno(),"0",naim.ravno_filtr(),"","","","",gv,mv,dv,iceb_getuid(data->window),vrem);

        if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
          continue;
        }      

      sprintf(strsql,"insert into Uosdok1 \
values ('%04d-%02d-%02d',%d,%s,%s,'%s',%s,%s,%d,%.2f,%.2f,%.2f,'%s',%d,%ld,\
%.2f,%.2f,%.2f,'%s')",
      gd,md,dd,tipz,"0",innom.ravno(),nomdok.ravno(),podr.ravno(),motv.ravno(),kol,bsnu,iznu,1.,kodop.ravno(),iceb_getuid(data->window),vrem,\
      bsbu,izbu,0.,"");

      if(iceb_sql_zapis(strsql,1,0,data->window) != 0)
       continue;

      if(tipz == 1)
       { 
        sprintf(strsql,"insert into Uosinp \
values (%s,%d,%d,'%s','%s','%s','%s',%.2f,%s,'%s',%d,%ld,'%s',%.2f)",
        innom.ravno(),md,gd,shetu.ravno(),hzatrat.ravno(),hanuh.ravno(),grupnu.ravno(),popkfnu,sost.ravno(),"",iceb_getuid(data->window),vrem,\
        grupbu.ravno(),popkfbu);

      
        iceb_sql_zapis(strsql,1,0,data->window);
       }
     }
   }

 }

iceb_pbar(data->bar,razmer_fil,razmer_fil); /*изза того что убираются обратные бакслеши и символы возврата карретки*/



return(koloh);
}
