/*$Id:$*/
/*22.03.2017	19.10.2004	Белых А.И.	copdok_r.c
Выполнение операции копирования документов в материальном учёте
*/
#include <math.h>
#include "buhg_g.h"
#include "copdok.h"

class copdok_r_data
 {

  public:

  GtkWidget *knopka;
  GtkWidget *window;
  GtkWidget *label;
  GtkWidget *bar;
  GtkWidget *view;
  GtkTextBuffer *buffer;
  class iceb_u_str name_window;
  short prkk;
  
  class copdok_data *rk;
    

 };

gboolean   copdok_r_key_press(GtkWidget *widget,GdkEventKey *event,class copdok_r_data *data);
gint copdok_r1(class copdok_r_data *data);
void  copdok_r_v_knopka(GtkWidget *widget,class copdok_r_data *data);

extern SQL_baza bd;
 
void copdok_r(class copdok_data *rek_ras,GtkWidget *wpredok)
{
char strsql[512];
iceb_u_spisok repl_s;
class copdok_r_data data;
int gor=0;
int ver=0;
data.rk=rek_ras;
data.name_window.plus(__FUNCTION__);

repl_s.plus(gettext("Если не делать привязки к карточкам, то одинаковые материалы\n\
с разных документов и разных карточек будут записаны одной записью с общим\n\
количеством и наибольшей ценой."));

if(rek_ras->tipz == 1)
  repl_s.plus(gettext("При выполнении привязки, привязка выполняется\n\
к карточкам с такою же ценой, или заводятся новые карточки."));

repl_s.plus(gettext("Сделать привязку к карточкам ?"));

data.prkk=iceb_menu_danet(&repl_s,2,wpredok);






data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);
else
  gtk_window_set_default_size (GTK_WINDOW  (data.window),400,400);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Копировать записи из других документов"));
gtk_window_set_title(GTK_WINDOW(data.window),strsql);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(copdok_r_key_press),&data);

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

GtkWidget *label=gtk_label_new(gettext("Копировать записи из других документов"));
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


gtk_widget_show_all(data.window);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka,gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka,"clicked",G_CALLBACK(copdok_r_v_knopka),&data);
gtk_widget_set_name(data.knopka,iceb_u_inttochar(0));
gtk_box_pack_start (GTK_BOX (vbox), data.knopka, FALSE, FALSE, 2);



//gtk_window_set_decorated(GTK_WINDOW(data.window),FALSE); //Убирает рамку вокруг окна

//gtk_widget_set_sensitive(GTK_WIDGET(data.knopka),FALSE);//Недоступна

g_idle_add((GSourceFunc)copdok_r1,&data);

gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));


}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  copdok_r_v_knopka(GtkWidget *widget,class copdok_r_data *data)
{
iceb_sizww(data->name_window.ravno(),data->window);
gtk_widget_destroy(data->window);
return;
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   copdok_r_key_press(GtkWidget *widget,GdkEventKey *event,class copdok_r_data *data)
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

gint copdok_r1(class copdok_r_data *data)
{
time_t vremn;
time(&vremn);
char strsql[512];
class iceb_u_str bros("");
int kolstr;
iceb_u_str repl;
int koldok=0;
int kolsz=0;
sprintf(strsql,"select * from Dokummat \
where datd >= '%s' and datd <= '%s'",
data->rk->datan.ravno_sqldata(),data->rk->datak.ravno_sqldata());
SQLCURSOR cur;
SQLCURSOR cur1;
SQL_str row;
SQL_str row1;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

if(kolstr == 0)
 {
  repl.new_plus(gettext("Не найдено ни одного документа !"));
  iceb_menu_soob(&repl,data->window);
  gtk_widget_destroy(data->window);
  return(FALSE);
 }

int tp;
int tipzz=0;
//if(prra[0] == '+')
if(data->rk->pr_ras.ravno()[0] == '+')
  tipzz=1;
if(data->rk->pr_ras.ravno()[0] == '-')
  tipzz=2;

class iceb_u_str nomdok1("");
int skl1;
short d,m,g;
double sumkor;
float kolstr1=0.;
int kolstr2;
int kodm;
int nk;
double cena;
double cenaz;
double cenap;
double kolih;
double nds;
int mnds;
class iceb_u_str ei("");
class iceb_u_str nomz("");
class iceb_u_str shbm("");
double krt;
double fas;
int kodt;
class iceb_u_str innom("");
class iceb_u_str rnd("");
short denv=0,mesv=0,godv=0; //Дата ввода в эксплуатацию для малоценки
short deng,mesg,godg;
while(cur.read_cursor(&row) != 0)
 {
  iceb_pbar(data->bar,kolstr,++kolstr1);    

  tp=atoi(row[0]);
  if(tipzz != 0 && tipzz != tp)
      continue;

  if(iceb_u_proverka(data->rk->kontr.ravno(),row[3],0,0) != 0)
     continue;

  if(iceb_u_proverka(data->rk->kodop.ravno(),row[6],0,0) != 0)
     continue;

  if(iceb_u_proverka(data->rk->sklad.ravno(),row[2],0,0) != 0)
     continue;

  if(iceb_u_proverka(data->rk->nomdok.ravno(),row[4],0,0) != 0)
     continue;

  nomdok1.new_plus(row[4]);

  iceb_u_rsdat(&d,&m,&g,row[1],2);  

  skl1=atoi(row[2]);
  /*Не копировать себя самого*/
  if(skl1 == data->rk->skl && iceb_u_SRAV(nomdok1.ravno(),data->rk->nomdok_c.ravno(),0) == 0 && data->rk->gd == g)
    continue;

  bros.new_plus("");
  if(tp == 1)
   bros.new_plus("+");
  if(tp == 1)
   bros.new_plus("-");

  sprintf(strsql,"%s %02d.%02d.%d %-3s %-4s %-5s %s\n",
  bros.ravno(),d,m,g,row[6],row[3],row[4],row[2]);

  iceb_printw(strsql,data->buffer,data->view);

  
  /*Читаем сумму корректировки*/
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%s and nomerz=13",
  g,row[4],row[2]);

  sumkor=0.;
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
    sumkor=atof(row1[0]);
  if(sumkor != 0)
   {
    sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=13",
    data->rk->gd,data->rk->nomdok_c.ravno(),data->rk->skl);

    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     {
      sumkor+=atof(row1[0]);
      sprintf(strsql,"delete from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=13",
      data->rk->gd,data->rk->nomdok_c.ravno(),data->rk->skl);
      if(sql_zap(&bd,strsql) != 0)
        iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,data->window);
     }
    sprintf(strsql,"insert into Dokummat2 \
values (%d,%d,'%s',%d,'%.2f')",data->rk->gd,data->rk->skl,data->rk->nomdok_c.ravno(),13,sumkor);
    if(sql_zap(&bd,strsql) != 0)
        iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,data->window);
    
   }  


  sprintf(strsql,"select * from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s'",
  g,m,d,skl1,nomdok1.ravno());
  if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
    continue;
   }

  if(kolstr2 == 0)
    continue;
  deng=mesg=godg=0;         
  koldok++;
  while(cur1.read_cursor(&row1) != 0)
   {

    kodm=atoi(row1[4]);
    nk=atoi(row1[3]);
    cena=atof(row1[6]);
    kolih=atof(row1[5]);
    nds=atof(row1[9]);
    mnds=atoi(row1[10]);
    ei.new_plus(row1[7]);
    nomz.new_plus(row1[17]);
    
    /*Проверяем нет ли уже такой записи*/
    
    if(data->rk->tipz == 2)
     if(data->prkk == 2 || skl1 != data->rk->skl)
       nk=0;
    if(data->rk->tipz == 1 && data->prkk == 2)
       nk=0;

    SQLCURSOR cur2;
    SQL_str row2;
    if(data->rk->tipz == 1 && data->rk->skl != skl1 && nk != 0 && data->prkk == 1) /*Приход*/
     {

/*Читаем реквизиты карточки на чужом складе и если нет на текущем
складе карточки с такимиже реквизитами то заводим ее*/

      sprintf(strsql,"select * from Kart where sklad=%d and nomk=%d",
      skl1,nk);
      if(sql_readkey(&bd,strsql,&row2,&cur2) != 1)
       {
        sprintf(strsql,"Не знайшли картки %d, склад %d, матеріал %d\n",
        nk,skl1,kodm);
        iceb_printw(strsql,data->buffer,data->view);
        continue;
       }

      mnds=atoi(row2[3]);
      ei.new_plus(row2[4]);
      shbm.new_plus(row2[5]);
      cenaz=atof(row2[6]);
      cenap=atof(row2[7]);
      krt=atof(row2[8]);
      nds=atof(row2[9]);
      fas=atof(row2[10]);
      kodt=atoi(row2[11]);
      innom.new_plus(row2[15]);
      rnd.new_plus(row2[16]);
      iceb_u_rsdat(&deng,&mesg,&godg,row[18],2);    
            
      sprintf(strsql,"select nomk from Kart where kodm=%d and \
sklad=%d and shetu='%s' and cena=%.10g and ei='%s' and nds=%.10g \
and cenap=%.10g and krat=%.10g and mnds=%d and innom='%s'",
      kodm,data->rk->skl,shbm.ravno(),cenaz,ei.ravno(),nds,cenap,krt,mnds,innom.ravno());

      if(sql_readkey(&bd,strsql,&row2,&cur2) != 1)
       {
        nk=nomkrw(data->rk->skl,data->window);

        sprintf(strsql,"Записываем новую карточку N%d, склад %d, материал %d\n",
        nk,skl1,kodm);
        iceb_printw(strsql,data->buffer,data->view);

        zapkarw(data->rk->skl,nk,kodm,cenaz,ei.ravno(),shbm.ravno(),krt,fas,kodt,nds,mnds,cenap,\
        denv,mesv,godv,innom.ravno(),rnd.ravno(),nomz.ravno(),deng,mesg,godg,0,data->window);

       }
      else
       nk=atoi(row2[0]);
     }

    kolsz++;

    sprintf(strsql,"select kolih,cena from Dokummat1 where \
datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' and kodm=%d and \
nomkar=%d",
    data->rk->gd,data->rk->md,data->rk->dd,data->rk->skl,data->rk->nomdok_c.ravno(),kodm,nk);
      
    if(sql_readkey(&bd,strsql,&row2,&cur2) == 1)
     {
      kolih+=atof(row2[0]);
      cenaz=atof(row2[1]);
/*
      printw("Перезаписываем %d.%d.%d %s %d\n",data->rk->dd,data->rk->md,data->rk->gd,nomn,data->rk->skl);
      refresh();  
*/
      if(fabs(cenaz-cena) > 0.009)
       {
        sprintf(strsql,"%s %s %s %d.%d.%d, %s %d. %s %.10g %.10g\n\%s.\n",
        gettext("Документ"),
        nomdok1.ravno(),
        gettext("от"),
        d,m,g,
        gettext("материал"),
        kodm,
        gettext("Не совпадает цена !"),
        cena,cenaz,
        gettext("Берем большую цену"));
        iceb_printw(strsql,data->buffer,data->view);

        if(cena-cenaz < 0.)
         cena=cenaz;
       }
      
      sprintf(strsql,"update Dokummat1 \
set \
cena=%.10g,\
kolih=%.10g,\
ktoi=%d \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s' and kodm=%d and nomkar=%d",
      cena,kolih,iceb_getuid(data->window),data->rk->gd,data->rk->md,data->rk->dd,data->rk->skl,data->rk->nomdok_c.ravno(),kodm,nk);

      if(sql_zap(&bd,strsql) != 0)
        iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,data->window);

     }
    else
     {
/*
      printw("Пишем %d.%d.%d %s %d %d\n",data->rk->dd,data->rk->md,data->rk->gd,data->rk->nomdok_c.ravno(),data->rk->skl,nk);
      refresh();  
*/
      zapvdokw(data->rk->dd,data->rk->md,data->rk->gd,data->rk->skl,nk,kodm,
      data->rk->nomdok_c.ravno(),kolih,cena,ei.ravno(),nds,mnds,0,
      data->rk->tipz,0,"",nomz.ravno(),0,"",data->window);

     }


   }


  /*Удаление после копирования*/
/*********************************
  if(mud == 1)
   {
//    sprintf(strsql,"%s/%s/blokmak.alx",putnansi,imabaz);
    if(prblm(m,g,strsql) != 0)
     {
      beep();
      printw(gettext("Документ %s удалить невозможно. Дата %d.%dг. заблокирована !"),
      data->rk->nomdok_c.ravno()1,m,g);
      printw("\n");
      continue;
     }

    if(udprgr(ICEB_MP_MATU,d,m,g,data->rk->nomdok_c.ravno(),skl1,tp) != 0)
     continue;
    matudd(0,0,0,tp,d,m,g,data->rk->nomdok_c.ravno(),skl1,0,0);

   } 
****************************/

 }




gtk_label_set_text(GTK_LABEL(data->label),gettext("Расчёт закончен"));

gtk_widget_grab_focus(data->knopka);
gtk_widget_show_all(data->window);

iceb_printw_vr(vremn,data->buffer,data->view);

sprintf(strsql,"%s %d !",gettext("Количество переписаних документов"),koldok);
repl.new_plus(strsql);
sprintf(strsql,"%s %d !",gettext("Количество переписаних записей"),kolsz);
repl.ps_plus(strsql);
iceb_menu_soob(&repl,data->window);

return(FALSE);
}
