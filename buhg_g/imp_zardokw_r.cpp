/*$Id: imp_zardokw_r.c,v 1.18 2014/07/31 07:08:25 sasa Exp $*/
/*19.06.2019	10.03.2006	Белых А.И.	imp_zardokw_r.c
Расчёт отчёта 
*/
#include <sys/stat.h>
#include <errno.h>
#include "buhg_g.h"
#include "imp_zardokw.h"

class imp_zardokw_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class imp_zardokw_rr *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  imp_zardokw_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   imp_zardokw_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_zardokw_r_data *data);
gint imp_zardokw_r1(class imp_zardokw_r_data *data);
void  imp_zardokw_r_v_knopka(GtkWidget *widget,class imp_zardokw_r_data *data);


extern SQL_baza bd;
extern int      kol_strok_na_liste;

int imp_zardokw_r(class imp_zardokw_rr *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class imp_zardokw_r_data data;
int gor=0;
int ver=0;

data.rk=datark;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатка списка документов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(imp_zardokw_r_key_press),&data);

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

repl.plus(gettext("Распечатка списка документов"));
sprintf(strsql,"%s:%s %s\n",gettext("База данных"),iceb_get_namebase(),iceb_get_pnk("00",0,wpredok));
repl.ps_plus(strsql);

GtkWidget *label=gtk_label_new(repl.ravno());

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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(imp_zardokw_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)imp_zardokw_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  imp_zardokw_r_v_knopka(GtkWidget *widget,class imp_zardokw_r_data *data)
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

gboolean   imp_zardokw_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_zardokw_r_data *data)
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

gint imp_zardokw_r1(class imp_zardokw_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_u_str repl;
iceb_clock sss(data->window);
SQL_str row;
SQLCURSOR cur;



struct stat work;

if(stat(data->rk->imaf_imp.ravno(),&work) != 0)
 {
  
  sprintf(strsql,"%s %s !",gettext("Не найден файл"),data->rk->imaf_imp.ravno());
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  
 }

FILE *ff=NULL;

if((ff = fopen(data->rk->imaf_imp.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(data->rk->imaf_imp.ravno(),"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }




char imafprom[64];
sprintf(imafprom,"prom%d.tmp",getpid());
FILE *ffprom;
if((ffprom = fopen(imafprom,"w")) == NULL)
 {
  iceb_er_op_fil(imafprom,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
short dd,md,gd;
iceb_u_rsdat(&dd,&md,&gd,data->rk->datadok.ravno(),1);
printf("%d.%d.%d\n",dd,md,gd);

class iceb_u_str strok("");
int razmer=0;
int tabnom=0;
int knah=0;
double suma=0.;
class iceb_u_str shet("");
int podr=0;

struct ZARP     zp;
zp.prn=data->rk->prn;
zp.dz=0; /*Для того чтобы не удаляласть запись перед записью новой*/
zp.mz=md;
zp.gz=gd;
zp.godn=gd;
zp.mesn=md;

int nomzap=0;
int voz=0;
short dnr,mnr,gnr;
short dkr,mkr,gkr;
class iceb_u_str shetkar("");
class iceb_u_str dolg("");
int kateg=0;
int sovm=0;
class iceb_u_str lgoti("");
int zvan=0;
class iceb_u_str koment;

while(iceb_u_fgets(&strok,ff) == 0)
 {
  razmer+=strlen(strok.ravno());

  if(strok.ravno()[0] == '#')
   continue;

  sprintf(strsql,"%s",strok.ravno());
  iceb_printw(strsql,data->buffer,data->view);
  
  iceb_pbar(data->bar,work.st_size,razmer);    
  
  if(iceb_u_polen(strok.ravno(),&tabnom,1,'|') != 0)
   continue;
  
  if(tabnom == 0)
   {
    fprintf(ffprom,"%s\n%s\n",strok.ravno(),"Табельный номер равен нолю");
    continue;
   }
  
  /*Проверяем табельный номер и узнаём код подразделения,счёт в карточке*/
  sprintf(strsql,"select dolg,zvan,podr,kateg,datn,datk,shet,sovm,lgoti from Kartb where tabn=%d",tabnom);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
   {
   
    sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),tabnom);
    iceb_menu_soob(strsql,data->window);
    
    fprintf(ffprom,"%s\n%s\n",strok.ravno(),gettext("Не найден табельный номер"));
    continue;
    
   }
  
  shet.new_plus(row[6]);
  shetkar.new_plus(row[6]);
  podr=atoi(row[2]);
  kateg=atoi(row[3]);
  sovm=atoi(row[7]);
  lgoti.new_plus(row[8]);
  dolg.new_plus(row[0]);
  iceb_u_rsdat(&dnr,&mnr,&gnr,row[4],2);
  iceb_u_rsdat(&dkr,&mkr,&gkr,row[5],2);
  zvan=atoi(row[1]);
      
  iceb_u_polen(strok.ravno(),&knah,2,'|');
  if(knah == 0)
   {
    if(data->rk->prn == 1)
     {
      iceb_menu_soob("Код начисления равен нолю!",data->window);
      fprintf(ffprom,"%s\nКод начисления равен нолю!\n",strok.ravno());
     }
    if(data->rk->prn == 2)
     {
      iceb_menu_soob("Код удержания равен нолю!",data->window);
      fprintf(ffprom,"%s\nКод удержания равен нолю!\n",strok.ravno());
     }
    continue;
   }
  iceb_u_polen(strok.ravno(),&koment,4,'|');
   
  /*Проверяем код начисления*/
  if(data->rk->prn == 1)
    sprintf(strsql,"select kod from Nash where kod=%d",knah);
  if(data->rk->prn == 2)
    sprintf(strsql,"select kod from Uder where kod=%d",knah);
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    if(data->rk->prn == 1)
     {
      sprintf(strsql,"Не найден код начисления %d !\n",knah);
      iceb_menu_soob(strsql,data->window);
      fprintf(ffprom,"%s\nНе найден код начисления %d!\n",strok.ravno(),knah);
     }
    if(data->rk->prn == 2)
     {
      sprintf(strsql,"Не найден код удержания %d !\n",knah);
      iceb_menu_soob(strsql,data->window);
      fprintf(ffprom,"%s\nНе найден код удержания %d!\n",strok.ravno(),knah);
     }
    continue;
   }

  iceb_u_polen(strok.ravno(),&suma,3,'|');
  if(suma == 0.)
   {
    iceb_menu_soob("Сумма равняется нолю!",data->window);
    fprintf(ffprom,"%s\nСумма равняется нолю!\n",strok.ravno());
    continue;
   }

  /*Смотрим счёт в списке начислений на работника*/
  sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='%d' and knah=%d",tabnom,data->rk->prn,knah);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
   {
    shet.new_plus(row[0]);
   }
  else
   {
    if(data->rk->prn == 1) 
      sprintf(strsql,"select shet from Nash where kod=%d",knah);
    if(data->rk->prn == 2) 
      sprintf(strsql,"select shet from Uder where kod=%d",knah);
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      if(row[0][0] != '\0')
       {
        shet.new_plus(row[0]);
       }
     }
         
   }
  nomzap=0;
  zp.tabnom=tabnom;
  zp.knu=knah;
zapis:;  
  if(zapzarpvw(&zp,suma,dd,md,gd,0,0,0,0,shet.ravno(),koment.ravno(),nomzap,podr,data->rk->nomdok.ravno(),data->window) != 0)
   {
    if(voz == 1)
     {
      nomzap++;
      goto zapis;
     }     
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }   

  zarsocw(md,gd,tabnom,data->window);
  zaravprw(tabnom,md,gd,NULL,data->window);
  
  //Проверяем есть ли запись карточки по этой дате и если нет делаем запись
  sprintf(strsql,"select god from Zarn where tabn=%d and god=%d and mes=%d",tabnom,gd,md);
  if(iceb_sql_readkey(strsql,data->window) == 0)  
    zapzarnw(md,gd,podr,tabnom,kateg,sovm,zvan,shetkar.ravno(),lgoti.ravno(),dnr,mnr,gnr,dkr,mkr,gkr,0,dolg.ravno(),data->window);
 }
  iceb_pbar(data->bar,work.st_size,work.st_size);    

fclose(ff);
fclose(ffprom);
unlink(data->rk->imaf_imp.ravno());

rename(imafprom,data->rk->imaf_imp.ravno());




gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

data->kon_ras=0;
gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}









