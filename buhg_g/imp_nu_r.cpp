/*$Id: imp_nu_r.c,v 1.22 2014/07/31 07:08:25 sasa Exp $*/
/*01.11.2017	09.10.2006	Белых А.И.	imp_nu_r.c
импорт начислений/удержаний из файла
*/
#include  <errno.h>
#include  <sys/stat.h>
#include "buhg_g.h"

class imp_nu_r_data
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
  short mz,gz;
  int prn;
  int kod_nah_ud;
        
  int voz; //0-все в порядке документы загружены 1-нет
  short kon_ras; //0-расчёт завершен 1-нет
  imp_nu_r_data()
   {
    kon_ras=1;
    voz=1;
   }
 };
gboolean   imp_nu_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_nu_r_data *data);
gint imp_nu_r1(class imp_nu_r_data *data);
void  imp_nu_r_v_knopka(GtkWidget *widget,class imp_nu_r_data *data);

extern SQL_baza bd;
extern double okrcn;

int imp_nu_r(const char *imaf,
short mz,short gz,
int prn,
int kod_nah_ud,
GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_spisok repl_s;
class imp_nu_r_data data;
int gor=0;
int ver=0;

data.imafz.new_plus(imaf);
data.mz=mz;
data.gz=gz;
data.prn=prn;
data.kod_nah_ud=kod_nah_ud;
data.name_window.plus(__FUNCTION__);

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

if(data.prn == 1)
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Импорт начислений"));
if(data.prn == 2)
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Импорт удержаний"));

gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(imp_nu_r_key_press),&data);

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
if(data.prn == 1)
  label=gtk_label_new(gettext("Импорт начислений"));
if(data.prn == 2)
  label=gtk_label_new(gettext("Импорт удержаний"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(imp_nu_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);


//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)imp_nu_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  imp_nu_r_v_knopka(GtkWidget *widget,class imp_nu_r_data *data)
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

gboolean   imp_nu_r_key_press(GtkWidget *widget,GdkEventKey *event,class imp_nu_r_data *data)
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

gint imp_nu_r1(class imp_nu_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
iceb_clock sss(data->window);
struct stat work;
class ZARP     zp;
class iceb_u_str shet_nu("");
class iceb_u_str naim("");





if(stat(data->imafz.ravno(),&work) != 0)
 {
  
  sprintf(strsql,"%s %s !",gettext("Не найден файл"),data->imafz.ravno());
  iceb_menu_soob(strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
  
 }


FILE *ff=NULL;

if((ff = fopen(data->imafz.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(data->imafz.ravno(),"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(data->prn == 1)
 {
  sprintf(strsql,"select naik,shet from Nash where kod=%d",data->kod_nah_ud);
 }

if(data->prn == 2)
 {
  sprintf(strsql,"select naik,shet from Uder where kod=%d",data->kod_nah_ud);
 }

SQL_str row;
class SQLCURSOR cur;

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  naim.new_plus(row[0]);  
  shet_nu.new_plus(row[1]);
 }

sprintf(strsql,"%d %s %s\n",data->kod_nah_ud,naim.ravno(),shet_nu.ravno());
iceb_printw(strsql,data->buffer,data->view);



char imafprom[64];
FILE *ffprom=NULL;

sprintf(imafprom,"prom%d.tmp",getpid());
if((ffprom = fopen(imafprom,"w")) == NULL)
 {
  iceb_er_op_fil(imafprom,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

short den=0;
class iceb_u_str shet(shet_nu.ravno());
short mvr,gvr;
int   podr=0;
short nomzap;
double suma=0.;
class iceb_u_str koment("");
short denp=0;
class OPSHET	shetv;
iceb_u_dpm(&denp,&data->mz,&data->gz,5);
float razmer=0.;
short dn,mn,gn;
short du,mu,gu;
class iceb_u_str shetkar("");
int podrkar=0;
int kateg;
int sovm;
int zvan;
class iceb_u_str lgot("");
class iceb_u_str dolg("");
class iceb_u_str strok("");
short metkadat=0;
short mzz=data->mz,gzz=data->gz;
int tabnom=0;

while(iceb_u_fgets(&strok,ff) == 0)
 {
  razmer+=strlen(strok.ravno());

  if(strok.ravno()[0] == '#')
   continue;
   
  
  iceb_printw(strok.ravno(),data->buffer,data->view);

  iceb_pbar(data->bar,work.st_size,razmer);    
  
   
  if(iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),1,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

 
  if(iceb_u_SRAV(strsql,"Дата",0) == 0)
   {
    fprintf(ffprom,"%s",strok.ravno()); //Всегда записываем
    metkadat=0;
    iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),2,'|');    
    if(iceb_u_rsdat1(&mzz,&gzz,strsql) != 0)
     {
      iceb_printw(gettext("Не верно введена дата !"),data->buffer,data->view);
      iceb_printw("\n",data->buffer,data->view);
      
      iceb_menu_soob(gettext("Не верно введена дата !"),data->window);
      metkadat=1;      
      continue;
     }
    denp=1;
    iceb_u_dpm(&denp,&mzz,&gzz,5);
    continue;
   }

  if(metkadat == 1)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }
  
  if((tabnom=atoi(strsql)) == 0)
   {
    sprintf(strsql,"%s%s\n",strok.ravno(),"Табельный номер равен нолю");
    fprintf(ffprom,"%s",strsql);
    iceb_printw(strsql,data->buffer,data->view);
    continue;
   }

  //Проверяем есть ли такой табельный номер  
  sprintf(strsql,"select datn,datk,shet,podr,kodg,kateg,\
sovm,zvan,lgoti,dolg from Kartb where tabn=%d",tabnom);
  if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден табельный номер"),tabnom);
    
    iceb_printw(strsql,data->buffer,data->view);
    iceb_printw("\n",data->buffer,data->view);
    
    iceb_menu_soob(strsql,data->window);
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  iceb_u_rsdat(&dn,&mn,&gn,row[0],2);
  iceb_u_rsdat(&du,&mu,&gu,row[1],2);
  shetkar.new_plus(row[2]);
  podrkar=atoi(row[3]);
  kateg=atoi(row[5]);
  sovm=atoi(row[6]);
  zvan=atoi(row[7]);
  lgot.new_plus(row[8]);
  dolg.new_plus(row[9]);

  if(iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),3,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  den=atoi(strsql);

  if(den < 1 && den > denp)
   {
    
    iceb_printw(gettext("Неправильно введён день начисления/удержания !"),data->buffer,data->view);
    iceb_printw("\n",data->buffer,data->view);

    iceb_menu_soob(gettext("Неправильно введён день начисления/удержания !"),data->window);
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  if(shet_nu.getdlinna() <= 1)
   if(iceb_u_polen(strok.ravno(),&shet,4,'|') != 0)
    {
     fprintf(ffprom,"%s",strok.ravno());
     continue;
    }

   
  //Проверить есть ли счёт в плане счетов
  if(iceb_prsh1(shet.ravno(),&shetv,data->window) != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  //Дата в счёт какого месяца
  if(iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),5,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  if(iceb_u_rsdat1(&mvr,&gvr,strsql) != 0)
   {
    iceb_menu_soob(gettext("Неправильно введено в счёт какого месяца !"),data->window);
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }
  
  //Подразделение     
  if(iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),6,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }
  podr=atoi(strsql);
  //Проверяем код подразделения
  sprintf(strsql,"select naik from Podr where kod=%d",podr);

  if(iceb_sql_readkey(strsql,data->window) != 1)  
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),podr);

    iceb_printw(strsql,data->buffer,data->view);
    iceb_printw("\n",data->buffer,data->view);

    iceb_menu_soob(strsql,data->window);
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  //Номер записи
  if(iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),7,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }
  nomzap=atoi(strsql);

  //Сумма
  if(iceb_u_polen(strok.ravno(),strsql,sizeof(strsql),8,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }
  suma=iceb_u_atof(strsql);
  if(suma == 0.)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }
   
  if(data->prn == 2)
   suma*=-1;
   
  //Коментарий
  if(iceb_u_polen(strok.ravno(),&koment,9,'|') != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }

  zp.tabnom=tabnom;
  zp.prn=data->prn;
  zp.knu=data->kod_nah_ud;
  zp.dz=den;
  zp.mz=data->mz;
  zp.gz=data->gz;
  zp.mesn=mvr; zp.godn=gvr;
  zp.nomz=0;
  zp.podr=podr;
  strcpy(zp.shet,shet.ravno());
      
  if(zapzarpvw(&zp,suma,den,mvr,gvr,0,0,0,0,shet.ravno(),koment.ravno(),nomzap,podr,"",data->window) != 0)
   {
    fprintf(ffprom,"%s",strok.ravno());
    continue;
   }   

  if(data->prn == 1)
   zarsocw(mzz,gzz,tabnom,data->window);
  zaravprw(tabnom,mzz,gzz,NULL,data->window);
  int prov=0;
  
  //Проверяем есть ли запись карточки по этой дате и если нет делаем запись
  sprintf(strsql,"select god from Zarn where tabn=%d and god=%d and mes=%d",tabnom,gzz,mzz);
  if(iceb_sql_readkey(strsql,data->window) == 0)  
    zapzarnw(mzz,gzz,podrkar,tabnom,kateg,sovm,zvan,shetkar.ravno(),lgot.ravno(),dn,mn,gn,du,mu,gu,prov,dolg.ravno(),data->window);

 }
iceb_pbar(data->bar,work.st_size,work.st_size);    

fclose(ff);
fclose(ffprom);
unlink(data->imafz.ravno());

rename(imafprom,data->imafz.ravno());




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
