/*$Id: zar_spisr_r.c,v 1.23 2013/09/26 09:47:00 sasa Exp $*/
/*26.09.2019	22.11.2006	Белых А.И.	zar_spisr_r.c
Распечатка списка работников
*/
#include <errno.h>
#include "buhg_g.h"
#include "zar_spisr.h"
#include "dbfhead.h"

class zar_spisr_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;

  class zar_spisr_rek *rk;
  
  short kon_ras; //0-расчёт завершен 1-нет
  int voz;

  zar_spisr_r_data()
   {
    voz=1;
    kon_ras=1;
   }
 };
gboolean   zar_spisr_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_spisr_r_data *data);
gint zar_spisr_r1(class zar_spisr_r_data *data);
void  zar_spisr_r_v_knopka(GtkWidget *widget,class zar_spisr_r_data *data);


extern SQL_baza bd;

int zar_spisr_r(class zar_spisr_rek *datark,GtkWidget *wpredok)
{
char strsql[512];
class iceb_u_str repl;
class zar_spisr_r_data data;
int gor=0;
int ver=0;
data.name_window.plus(__FUNCTION__);

data.rk=datark;

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатка списка работников"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(zar_spisr_r_key_press),&data);

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

repl.plus(gettext("Распечатка списка работников"));
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
g_signal_connect(data.knopka,"clicked",G_CALLBACK(zar_spisr_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);

gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

gtk_widget_show_all(data.window);

gtk_text_buffer_create_tag(data.buffer,"naim_shrift","family","monospace","foreground",NULL,NULL);

//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)zar_spisr_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  zar_spisr_r_v_knopka(GtkWidget *widget,class zar_spisr_r_data *data)
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

gboolean   zar_spisr_r_key_press(GtkWidget *widget,GdkEventKey *event,class zar_spisr_r_data *data)
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
/******************************/
/*Создаем заголовок файла dbf*/
/******************************/
void rspis_dbf_f(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,
int *header_len,int *rec_len)
{

strncpy(f->name, fn,sizeof(f->name)-1);
f->type = ft;
f->length = l1;
f->dec_point = l2;
*header_len=*header_len+sizeof(DBASE_FIELD);
*rec_len=*rec_len+l1;
}

/***********************************************/
void		rspis_dbf_h(char *imaf,long kolz,GtkWidget *wpredok)
{
time_t		tmm;
struct  tm      *bf;
FILE		*ff;
DBASE_HEAD  h;
int fd;
int i;
int header_len, rec_len;
#define kolpol  9
DBASE_FIELD f[kolpol];
memset(&f, '\0', sizeof(f));

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

memset(&h,'\0',sizeof(h));

h.version = 3;

time(&tmm);
bf=localtime(&tmm);

h.l_update[0] = bf->tm_year;       /* yymmdd for last update*/
h.l_update[1] = bf->tm_mon+1;       /* yymmdd for last update*/
h.l_update[2] = bf->tm_mday;       /* yymmdd for last update*/

h.count = kolz;              /* number of records in file*/

header_len = sizeof(h);
rec_len = 0;
int shetshik=0;

rspis_dbf_f(&f[shetshik++],"TN", 'N', 10, 0,&header_len,&rec_len);
rspis_dbf_f(&f[shetshik++],"FIO", 'C', 60, 0,&header_len,&rec_len);
rspis_dbf_f(&f[shetshik++],"INN", 'N', 15, 0,&header_len,&rec_len);
rspis_dbf_f(&f[shetshik++],"DR", 'D', 8, 0,&header_len,&rec_len);
rspis_dbf_f(&f[shetshik++],"ADRES", 'C',100, 0,&header_len,&rec_len);
rspis_dbf_f(&f[shetshik++],"NP", 'C',30, 0,&header_len,&rec_len);
rspis_dbf_f(&f[shetshik++],"KV", 'C', 100, 0,&header_len,&rec_len);
rspis_dbf_f(&f[shetshik++],"DVP", 'D', 8, 0,&header_len,&rec_len);
rspis_dbf_f(&f[shetshik++],"TELEF", 'C', 50, 0,&header_len,&rec_len);

h.header = header_len + 1;/* length of the header
                           * includes the \r at end
                           */
h.lrecl= rec_len + 1;     /* length of a record
                           * includes the delete
                           * byte
                          */
/*
 printw("h.header=%d h.lrecl=%d\n",h.header,h.lrecl);
*/


fd = fileno(ff);

if(write(fd, &h, sizeof(h)) < 0)
 {
  printf("\n%s-%s\n",__FUNCTION__,strerror(errno));
 }

for(i=0; i < kolpol; i++) 
 {
  if(write(fd, &f[i], sizeof(DBASE_FIELD)) < 0)
   {
    printf("\n%s-%s\n",__FUNCTION__,strerror(errno));
   }
  
 }
fputc('\r', ff);

fclose(ff);

}

/***************************/
/*печать реквизитов поиска*/
/***************************/

void zar_spis_r_rp(class zar_spisr_rek *rk,FILE *ff)
{
if(rk->podr.getdlinna() <= 1)
 {
  fprintf(ff,"%s\n",gettext("Список по всем подразделениям"));
 }
else
 {
  fprintf(ff,"%s:%s\n",gettext("Код подразделения"),rk->podr.ravno()); 
 }
if(rk->tabnom.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Табельный номер"),rk->tabnom.ravno());
if(rk->kod_zvan.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код звания"),rk->kod_zvan.ravno());
if(rk->kod_kateg.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Код категории"),rk->kod_kateg.ravno());
if(rk->shetu.getdlinna() > 1)
 fprintf(ff,"%s:%s\n",gettext("Счёт"),rk->shetu.ravno());
}

/******************************************/
/******************************************/

gint zar_spisr_r1(class zar_spisr_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[1024];
int kolstr=0;
class iceb_clock sss(data->window);



class iceb_u_str bros("");
sprintf(strsql,"select * from Kartb");

if(data->rk->metka_sort == 0)
  bros.new_plus(" order by tabn asc");
else
  bros.new_plus(" order by fio asc");
strcat(strsql,bros.ravno());

class SQLCURSOR cur;
class SQLCURSOR curr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),data->window);  
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imaf_dbf_tmp[64];
sprintf(imaf_dbf_tmp,"srk%d.txt",getpid());
class iceb_fopen fil_dbf;
if(fil_dbf.start(imaf_dbf_tmp,"w",data->window) != 0)
 {
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf[64];
sprintf(imaf,"sr%d.lst",getpid());
FILE *ff;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf1[64];

sprintf(imaf1,"sr1%d.lst",getpid());
FILE *ff1;
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }
char imaf2[64];
sprintf(imaf2,"sr2%d.lst",getpid());
FILE *ff2;
if((ff2 = fopen(imaf2,"w")) == NULL)
 {
  iceb_er_op_fil(imaf2,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

char imaf_ks[64];
sprintf(imaf_ks,"sr_ks%d.lst",getpid());
FILE *ff_ks;
if((ff_ks = fopen(imaf_ks,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_ks,"",errno,data->window);
  sss.clear_data();
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(data->rk->metka_ras == 0)
 strcpy(strsql,gettext("Распечатка работающих"));
if(data->rk->metka_ras == 1)
 strcpy(strsql,gettext("Распечатка уволенных"));
if(data->rk->metka_ras == 2)
 strcpy(strsql,gettext("Распечатка работающих совместителей"));
 
iceb_u_zagolov(strsql,0,0,0,0,0,0,iceb_get_pnk("00",0,data->window),ff);
iceb_u_zagolov(strsql,0,0,0,0,0,0,iceb_get_pnk("00",0,data->window),ff1);
iceb_u_zagolov(strsql,0,0,0,0,0,0,iceb_get_pnk("00",0,data->window),ff2);
iceb_u_zagolov(strsql,0,0,0,0,0,0,iceb_get_pnk("00",0,data->window),ff_ks);

if(data->rk->podr.getdlinna() <= 1)
 {
  sprintf(strsql,"%s\n",gettext("Список по всем подразделениям"));
  iceb_printw(strsql,data->buffer,data->view,"naim_shrift");
    
 }
else
 {
  sprintf(strsql,"%s:%s\n",gettext("Код подразделения"),data->rk->podr.ravno()); 
  iceb_printw(strsql,data->buffer,data->view,"naim_shrift");


 }
 

zar_spis_r_rp(data->rk,ff);
zar_spis_r_rp(data->rk,ff1);
zar_spis_r_rp(data->rk,ff2);
zar_spis_r_rp(data->rk,ff_ks);

fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("Т/н |               Ф. И. О.                 |Инд. номер|Счёт |Дата приём./увольнения |  Должность |Категория|Подразделение|Льготы| Звание |\n"));
fprintf(ff,"\
--------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff1,"\
--------------------------------------------------------------------\n");
fprintf(ff1,gettext("\
Т/н |               Ф. И. О.                 |          |          |\n"));
fprintf(ff1,"\
--------------------------------------------------------------------\n");

fprintf(ff2,"\
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
fprintf(ff2,gettext("\
Т/н |               Ф. И. О.                 |Инд. номер|Дата рожд.|                 Адрес                            | N паспорта    |               Кем выдан                          |Дата в/п  | Телефон\n"));
fprintf(ff2,"\
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

fprintf(ff_ks,"\
-------------------------------------------------------------------------------\n");
fprintf(ff_ks,gettext("\
 Т/н |               Ф. И. О.                 |Инд. номер|     Карт-счёт      |\n"));
fprintf(ff_ks,"\
-------------------------------------------------------------------------------\n");
/*
12345 1234567890123456789012345678901234567890 12345678901234567890
*/
short dv,mv,gv;
short dr,mr,gr;

int kolr=0;
float kolstr1=0;
SQL_str row,row1;
class iceb_u_str podr("");
class iceb_u_str kateg("");
class iceb_u_str zvan("");
int nomstr=0;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  if(iceb_u_proverka(data->rk->podr.ravno(),row[4],0,0) != 0)
   continue;
  if(iceb_u_proverka(data->rk->tabnom.ravno(),row[0],0,0) != 0)
   continue;
  if(iceb_u_proverka(data->rk->kod_zvan.ravno(),row[3],0,0) != 0)
   continue;
  if(iceb_u_proverka(data->rk->kod_kateg.ravno(),row[5],0,0) != 0)
   continue;
  if(iceb_u_proverka(data->rk->shetu.ravno(),row[8],0,0) != 0)
   continue;
    
  if(data->rk->metka_ras == 0 && row[7][0] != '0')
     continue;
  if(data->rk->metka_ras  == 1 && row[7][0] == '0')
    continue;
  if(data->rk->metka_ras  == 2 && (row[9][0] == '0' || row[7][0] != '0' ))
    continue;

  kolr++;

  sprintf(strsql,"%-4s %-*s %*s %s\n",row[0],iceb_u_kolbait(40,row[1]),row[1],iceb_u_kolbait(4,row[8]),row[8],row[5]);
  iceb_printw(strsql,data->buffer,data->view,"naim_shrift");

  /*Определяем наименование подразделения*/
  sprintf(strsql,"select naik from Podr where kod=%s",row[4]);
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
   podr.new_plus(row1[0]);
  else
   podr.new_plus("");

  /*Определяем наименование категории*/
  sprintf(strsql,"select naik from Kateg where kod=%s",row[5]);
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
   kateg.new_plus(row1[0]);
  else
   kateg.new_plus("");

  /*Определяем наименование звания*/
  sprintf(strsql,"select naik from Zvan where kod=%s",row[3]);
  if(iceb_sql_readkey(strsql,&row1,&curr,data->window) == 1)
   zvan.new_plus(row1[0]);
  else
   zvan.new_plus("");

  fprintf(ff,"%-4s %-*s %-10s %-*s %-11s %-11s %-*.*s %-*.*s %-*.*s %-*.*s %s\n",
  row[0],iceb_u_kolbait(40,row[1]),row[1],
  row[10],
  iceb_u_kolbait(5,row[8]),row[8],
  row[6],
  row[7],
  iceb_u_kolbait(12,row[2]),
  iceb_u_kolbait(12,row[2]),
  row[2],
  iceb_u_kolbait(9,kateg.ravno()),
  iceb_u_kolbait(9,kateg.ravno()),
  kateg.ravno(),
  iceb_u_kolbait(13,podr.ravno()),
  iceb_u_kolbait(13,podr.ravno()),
  podr.ravno(),
  iceb_u_kolbait(6,row[17]),
  iceb_u_kolbait(6,row[17]),
  row[17],
  zvan.ravno());

  nomstr++;
  fprintf(fil_dbf.ff," %10s%-*.*s%-*.*s%02d%02d%04d%-*.*s%-*.*s%-*.*s%02d%02d%04d%-*.*s",
  row[0], 
  iceb_u_kolbait(60,row[1]),
  iceb_u_kolbait(60,row[1]),
  row[1],
  iceb_u_kolbait(15,row[10]),
  iceb_u_kolbait(15,row[10]),
  row[10],
  dr,mr,gr,
  iceb_u_kolbait(100,row[11]),
  iceb_u_kolbait(100,row[11]),
  row[11],
  iceb_u_kolbait(30,row[12]),
  iceb_u_kolbait(30,row[12]),
  row[12],
  iceb_u_kolbait(100,row[13]),
  iceb_u_kolbait(100,row[13]),
  row[13],
  dv,mv,gv,
  iceb_u_kolbait(50,row[14]),
  iceb_u_kolbait(50,row[14]),
  row[14]);

  fprintf(ff1,"%-4s %-*s|%10s|%10s|\n\
....................................................................\n",
  row[0],iceb_u_kolbait(40,row[1]),row[1]," "," ");

  dr=mr=gr=0;

  iceb_u_rsdat(&dr,&mr,&gr,row[21],2);
  iceb_u_rsdat(&dv,&mv,&gv,row[19],2);
    
  fprintf(ff2,"%-4s %-*s %-10s %02d.%02d.%04d %-*.*s %-*s %-*.*s %02d.%02d.%04d %s\n",
  row[0],iceb_u_kolbait(40,row[1]),row[1],row[10],dr,mr,gr,
  iceb_u_kolbait(50,row[11]),iceb_u_kolbait(50,row[11]),row[11],
  iceb_u_kolbait(15,row[12]),row[12],
  iceb_u_kolbait(50,row[13]),iceb_u_kolbait(50,row[13]),row[13],
  dv,mv,gv,row[14]);
  
  fprintf(ff_ks,"%-5s %-*s %*s %-*s\n",row[0],iceb_u_kolbait(40,row[1]),row[1],
  iceb_u_kolbait(10,row[10]),row[10],
  iceb_u_kolbait(20,row[18]),row[18]);
  
 }

fprintf(ff1,"\
--------------------------------------------------------------------\n");

fprintf(ff_ks,"\
-------------------------------------------------------------------------------\n");

fprintf(ff,"\n%s:%d\n\n",
gettext("Количество работников"),kolr);

iceb_podpis(ff,data->window);
iceb_podpis(ff1,data->window);
iceb_podpis(ff2,data->window);
iceb_podpis(ff_ks,data->window);



fclose(ff);
fclose(ff1);
fclose(ff2);
fclose(ff_ks);

fputc(26,fil_dbf.ff);

fil_dbf.end();

iceb_perecod(2,imaf_dbf_tmp,data->window);
char imafdbf[64];

sprintf(imafdbf,"srk%d.dbf",getpid());

rspis_dbf_h(imafdbf,nomstr,data->window);

/*Сливаем два файла*/
iceb_cat(imafdbf,imaf_dbf_tmp,data->window);
unlink(imaf_dbf_tmp);

sprintf(strsql,"\n%s: %d\n",
gettext("Количество человек"),kolr);

iceb_printw(strsql,data->buffer,data->view,"naim_shrift");



data->rk->imaf.plus(imaf);
data->rk->imaf.plus(imaf1);
data->rk->imaf.plus(imaf2);
data->rk->imaf.plus(imaf_ks);

data->rk->naimf.plus(gettext("Распечатка с реквизитами карточки работника"));
data->rk->naimf.plus(gettext("Распечатка пустографки"));
data->rk->naimf.plus(gettext("Распечатка паспортных данных"));
data->rk->naimf.plus(gettext("Распечатка карт-счетов"));


for(int nom=0; nom < data->rk->imaf.kolih(); nom++)
 iceb_ustpeh(data->rk->imaf.ravno(nom),0,data->window);

data->rk->imaf.plus(imafdbf);
data->rk->naimf.plus(gettext("Распечатка паспортных данных"));

data->kon_ras=0;

gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_set_sensitive(GTK_WIDGET(data->knopka),TRUE);//доступна
gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

data->voz=0;
return(FALSE);

}
