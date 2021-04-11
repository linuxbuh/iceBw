/*$Id: l_prov_kr.c,v 1.37 2013/09/26 09:46:50 sasa Exp $*/
/*13.02.2016	13.01.2004	Белых А.И.	l_prov_kr.c
Меню корректировки проводки
*/
#include  "buhg_g.h"
#include  "l_prov.h"

enum
{
  KFK2,
  KFK3,
  KFK10,
  KOL_KF_KL
};

enum
 {
  E_KSHET,
  E_KSHETK,
  E_KDATA,
  E_KKOMENT,
  E_KDEBET,
  E_KKREDIT,
  E_KKONTR,
  E_KEKV,
  K_KOLENTER  
 };

class prov_kr_data
 {
  public:
  class prov_rek_data *rek_prov_s;
  class prov_rek_data rek_prov_n;

  GtkWidget *entry[K_KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *kwindow;
  class iceb_u_str name_window;
  short kl_shift;
  short metka_zap; //0-записали 1-не записали
  
  //Конструктор
  prov_kr_data()
   {
    metka_zap=0;
    kl_shift=0;
    kwindow=NULL;
   }
  void read_rek()
   {
    rek_prov_n.shet.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KSHET])));
    rek_prov_n.shetk.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KSHETK])));
    rek_prov_n.datap.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KDATA])));
    rek_prov_n.koment.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KKOMENT])));
    rek_prov_n.debet.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KDEBET])));
    rek_prov_n.kredit.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KREDIT])));
    rek_prov_n.kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KKONTR])));
    rek_prov_n.kekv.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KEKV])));
   }

 };

gboolean   prov_kr_key_press(GtkWidget *widget,GdkEventKey *event,class prov_kr_data *data);
void  prov_kr_knopka(GtkWidget *widget,class prov_kr_data *data);
void    prov_kr_vvod(GtkWidget *widget,class prov_kr_data *data);
int  prov_kr_zap(class prov_kr_data *data);


int l_prov_kr(class prov_rek_data *wdata,GtkWidget *wpredok)
{
int gor=0;
int ver=0;
char strsql[1024];
class iceb_u_str stroka;
class prov_kr_data data;
SQL_str row;
class SQLCURSOR cur;

data.rek_prov_s=wdata;
data.name_window.plus(__FUNCTION__);

//Ищем код контрагента во встречной проводке

short d,m,g;
iceb_u_rsdat(&d,&m,&g,data.rek_prov_s->datap.ravno(),1);

sprintf(strsql,"select kodkon from Prov where \
val=%d and datp='%04d-%d-%d' and sh='%s' and shk='%s' and kto='%s' \
and deb=%.2f and kre=%.2f and vrem=%ld and nomd='%s' and oper='%s' and \
komen = '%s'",
data.rek_prov_s->val,g,m,d,
data.rek_prov_s->shetk.ravno_filtr(),
data.rek_prov_s->shet.ravno_filtr(),
data.rek_prov_s->metka.ravno_filtr(),
atof(data.rek_prov_s->kredit.ravno()),
atof(data.rek_prov_s->debet.ravno()),
data.rek_prov_s->vremz,
data.rek_prov_s->nomdok.ravno_filtr(),
data.rek_prov_s->kodop.ravno_filtr(),
data.rek_prov_s->koment.ravno_filtr());

//  printf("prov_kor-%s\n",strsql);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  printf("%s-%s\n",__FUNCTION__,strsql);
  iceb_menu_soob(gettext("Не найдено парную проводку !"),wpredok);
  return(1);
 }
data.rek_prov_s->kontrk.new_plus(row[0]);





data.kwindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.kwindow),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.kwindow),TRUE);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.kwindow),gor,ver);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировать"));
gtk_window_set_title (GTK_WINDOW (data.kwindow),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.kwindow), 5);

g_signal_connect(data.kwindow,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.kwindow,"destroy",G_CALLBACK(gtk_main_quit),NULL);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.kwindow),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.kwindow),TRUE);
 }

g_signal_connect_after(data.kwindow,"key_press_event",G_CALLBACK(prov_kr_key_press),&data);

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox1),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox2),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox3),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox4 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox4),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox5 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox5),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox6 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox6),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox7 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox7),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox8 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hbox8),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

stroka.new_plus(gettext("Корректировка проводки"));
stroka.ps_plus(gettext("Дата и время записи"));
stroka.plus(" ");
stroka.plus(iceb_u_vremzap(data.rek_prov_s->vremz));
stroka.plus(" ");
stroka.plus(gettext("Записал"));
stroka.plus(" ");
stroka.plus(iceb_kszap(data.rek_prov_s->ktoz,NULL));

GtkWidget *label=gtk_label_new(stroka.ravno());

gtk_container_add (GTK_CONTAINER (data.kwindow), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);
gtk_container_add (GTK_CONTAINER (vbox), hbox1);
gtk_container_add (GTK_CONTAINER (vbox), hbox2);
gtk_container_add (GTK_CONTAINER (vbox), hbox3);
gtk_container_add (GTK_CONTAINER (vbox), hbox4);
gtk_container_add (GTK_CONTAINER (vbox), hbox5);
gtk_container_add (GTK_CONTAINER (vbox), hbox6);
gtk_container_add (GTK_CONTAINER (vbox), hbox7);
gtk_container_add (GTK_CONTAINER (vbox), hbox8);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

label=gtk_label_new(gettext("Счёт"));
data.entry[E_KSHET] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KSHET]),19);
gtk_box_pack_start (GTK_BOX (hbox1), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox1), data.entry[E_KSHET], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KSHET], "activate",G_CALLBACK(prov_kr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KSHET]),data.rek_prov_s->shet.ravno());
gtk_widget_set_name(data.entry[E_KSHET],iceb_u_inttochar(E_KSHET));

label=gtk_label_new(gettext("Счёт корреспондент"));
data.entry[E_KSHETK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KSHETK]),19);
gtk_box_pack_start (GTK_BOX (hbox2), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox2), data.entry[E_KSHETK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KSHETK], "activate",G_CALLBACK(prov_kr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KSHETK]),data.rek_prov_s->shetk.ravno());
gtk_widget_set_name(data.entry[E_KSHETK],iceb_u_inttochar(E_KSHETK));

label=gtk_label_new(gettext("Дата проводки (д.м.г)"));
data.entry[E_KDATA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KDATA]),10);
gtk_box_pack_start (GTK_BOX (hbox3), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox3), data.entry[E_KDATA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KDATA], "activate",G_CALLBACK(prov_kr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KDATA]),data.rek_prov_s->datap.ravno());
gtk_widget_set_name(data.entry[E_KDATA],iceb_u_inttochar(E_KDATA));

label=gtk_label_new(gettext("Коментарий"));
data.entry[E_KKOMENT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KKOMENT]),99);
gtk_box_pack_start (GTK_BOX (hbox4), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox4), data.entry[E_KKOMENT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KKOMENT], "activate",G_CALLBACK(prov_kr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KKOMENT]),data.rek_prov_s->koment.ravno());
gtk_widget_set_name(data.entry[E_KKOMENT],iceb_u_inttochar(E_KKOMENT));

label=gtk_label_new(gettext("Дебет"));
data.entry[E_KDEBET] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KDEBET]),19);
gtk_box_pack_start (GTK_BOX (hbox5), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox5), data.entry[E_KDEBET], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KDEBET], "activate",G_CALLBACK(prov_kr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KDEBET]),data.rek_prov_s->debet.ravno_filtr());
gtk_widget_set_name(data.entry[E_KDEBET],iceb_u_inttochar(E_KDEBET));


label=gtk_label_new(gettext("Кредит"));
data.entry[E_KKREDIT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KKREDIT]),19);
gtk_box_pack_start (GTK_BOX (hbox6), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox6), data.entry[E_KKREDIT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KKREDIT], "activate",G_CALLBACK(prov_kr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KKREDIT]),data.rek_prov_s->kredit.ravno_filtr());
gtk_widget_set_name(data.entry[E_KKREDIT],iceb_u_inttochar(E_KKREDIT));

label=gtk_label_new(gettext("Код контрагента"));
data.entry[E_KKONTR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KKONTR]),19);
gtk_box_pack_start (GTK_BOX (hbox7), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox7), data.entry[E_KKONTR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KKONTR], "activate",G_CALLBACK(prov_kr_vvod),&data);

sprintf(strsql,"%s",data.rek_prov_s->kontr.ravno());

if(data.rek_prov_s->kontrk.ravno()[0] != '\0')
 if(iceb_u_SRAV(data.rek_prov_s->kontr.ravno(),data.rek_prov_s->kontrk.ravno(),0) != 0)
  sprintf(strsql,"%s/%s",data.rek_prov_s->kontr.ravno(),data.rek_prov_s->kontrk.ravno());

gtk_entry_set_text(GTK_ENTRY(data.entry[E_KKONTR]),strsql);
gtk_widget_set_name(data.entry[E_KKONTR],iceb_u_inttochar(E_KKONTR));


label=gtk_label_new(gettext("Код экономической классификации затрат"));
data.entry[E_KEKV] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KEKV]),6);
gtk_box_pack_start (GTK_BOX (hbox8), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox8), data.entry[E_KEKV], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KEKV], "activate",G_CALLBACK(prov_kr_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KEKV]),data.rek_prov_s->kekv.ravno());
gtk_widget_set_name(data.entry[E_KEKV],iceb_u_inttochar(E_KEKV));



sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[KFK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[KFK2],gettext("Запись введенной в меню информации"));
g_signal_connect(data.knopka[KFK2],"clicked",G_CALLBACK(prov_kr_knopka),&data);
gtk_widget_set_name(data.knopka[KFK2],iceb_u_inttochar(KFK2));

sprintf(strsql,"F3 %s",gettext("Контрагенты"));
data.knopka[KFK3]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[KFK3],gettext("Просмотр общего списка контрагентов"));
g_signal_connect(data.knopka[KFK3],"clicked",G_CALLBACK(prov_kr_knopka),&data);
gtk_widget_set_name(data.knopka[KFK3],iceb_u_inttochar(KFK3));

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[KFK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[KFK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[KFK10],"clicked",G_CALLBACK(prov_kr_knopka),&data);
gtk_widget_set_name(data.knopka[KFK10],iceb_u_inttochar(KFK10));


gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[KFK2], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[KFK3], TRUE, TRUE, 0);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[KFK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.kwindow);


gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.metka_zap);
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   prov_kr_key_press(GtkWidget *widget,GdkEventKey *event,class prov_kr_data *data)
{
iceb_u_str kod;
iceb_u_str naim;

//printf("prov_kr_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);

  case GDK_KEY_F3:
    g_signal_emit_by_name(data->knopka[FK3],"clicked");
    return(TRUE);


  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:

    data->kl_shift=1;
     
    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  prov_kr_knopka(GtkWidget *widget,class prov_kr_data *data)
{
iceb_u_str kod;
iceb_u_str naim;

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case KFK2:
//    data->activate();
    prov_kr_zap(data);
    data->metka_zap=0;
    iceb_sizww(data->name_window.ravno(),data->kwindow);
    return;  

  case KFK3:
    iceb_l_kontr(1,&kod,&naim,data->kwindow);
    return;  


  case KFK10:
    iceb_sizww(data->name_window.ravno(),data->kwindow);
    gtk_widget_destroy(data->kwindow);
    data->kwindow=NULL;
    data->metka_zap=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    prov_kr_vvod(GtkWidget *widget,class prov_kr_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));
//g_print("prov_kr_vvod enter=%d\n",enter);

switch (enter)
 {
  case E_KSHET:
    data->rek_prov_n.shet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KSHETK:
    data->rek_prov_n.shetk.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KKONTR:
    data->rek_prov_n.kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KEKV:
    data->rek_prov_n.kekv.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KKOMENT:
    data->rek_prov_n.koment.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KDEBET:
    data->rek_prov_n.debet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KKREDIT:
    data->rek_prov_n.kredit.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KDATA:
    data->rek_prov_n.datap.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


 }

enter+=1;
  
if(enter >= K_KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/*********************************/
/*Запись исправленной проводки   */
/*********************************/
//Если вернули 0-записали 1-не записали
int  prov_kr_zap(class prov_kr_data *data)
{
char strsql[1024];
short d,m,g;  //Дата старая
short d1,m1,g1;  //Дата новая

data->read_rek();

if(data->rek_prov_n.shet.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введён счёт !"),data->kwindow);
  return(1);  
 }
if(data->rek_prov_n.shetk.ravno()[0] == '\0')
 {
  iceb_menu_soob(gettext("Не введён кореспондирующий счёт !"),data->kwindow);
  return(1);  
 }

if(iceb_u_rsdat(&d1,&m1,&g1,data->rek_prov_n.datap.ravno(),0) != 0)
 {
  iceb_menu_soob(gettext("Не верно ведена дата проводки !"),data->kwindow);
  return(1);  
 }

if(atof(data->rek_prov_n.debet.ravno()) != 0. && atof(data->rek_prov_n.kredit.ravno()) != 0.)
 {
  iceb_menu_soob(gettext("Введено дебет и кредит !"),data->kwindow);
  return(1);  
 }


//Проверяем старую дату
iceb_u_rsdat(&d,&m,&g,data->rek_prov_s->datap.ravno(),1);

if(iceb_pvglkni(m,g,data->kwindow) != 0)
 return(1);
if(iceb_pvkup(data->rek_prov_s->ktoz,data->kwindow) != 0)
 return(1);
//Проверяем новую дату
if(iceb_pvglkni(m1,g1,data->kwindow) != 0)
 return(1);

if(provvkp(data->rek_prov_s->metka.ravno(),data->kwindow) != 0)
  return(1);  

class iceb_u_str kontr("");
class iceb_u_str kontr1("");

if(data->rek_prov_n.kontr.ravno()[0] != '\0')
 {
  if(iceb_u_pole(data->rek_prov_n.kontr.ravno(),&kontr,1,'/') != 0)
   {
    kontr.new_plus(data->rek_prov_n.kontr.ravno());
    kontr1.new_plus(data->rek_prov_n.kontr.ravno());
   }
  else
   iceb_u_pole(data->rek_prov_n.kontr.ravno(),&kontr1,2,'/');
 }
data->rek_prov_n.kontr.new_plus(kontr.ravno());
data->rek_prov_n.kontrk.new_plus(kontr1.ravno());

/*Проверяем есть ли счёт в списке счетов*/
OPSHET rek_shet;
if(iceb_prsh1(data->rek_prov_n.shet.ravno(),&rek_shet,data->kwindow) != 0)
  return(1);

if(rek_shet.saldo == 3 && kontr.ravno()[0] == '\0')
 {
  sprintf(strsql,"%s %s !",gettext("Не введён контрагент для счёта"),data->rek_prov_n.shet.ravno());
  iceb_menu_soob(strsql,data->kwindow);
  return(1);
 }

/*Проверяем есть ли такой код контрагента*/
if(rek_shet.saldo == 3)
if(kontr.ravno()[0] != '\0')
 {
  sprintf(strsql,"select kodkon from Skontr where ns='%s' and \
kodkon='%s'",data->rek_prov_n.shet.ravno(),kontr.ravno());
  if(sql_readkey(&bd,strsql) != 1)
   {
    iceb_u_str SOOB;
    sprintf(strsql,"%s %s %s %s!",
    gettext("Не нашли код контрагента"),
    kontr.ravno(),
    gettext("в списке счета"),
    data->rek_prov_n.shet.ravno());
    
    SOOB.plus(strsql);
    iceb_menu_soob(&SOOB,data->kwindow);
    return(1);
   } 
 }

/*Проверяем есть ли счёт корреспондент в списке счетов*/
OPSHET rek_shetk;
if(iceb_prsh1(data->rek_prov_n.shetk.ravno(),&rek_shetk,data->kwindow) != 0)
  return(1);

if(rek_shetk.saldo == 3 && kontr1.ravno()[0] == '\0')
 {
  sprintf(strsql,"%s %s !",gettext("Не введён контрагент для счёта"),data->rek_prov_n.shetk.ravno());
  iceb_menu_soob(strsql,data->kwindow);
  return(1);
 }


/*Проверяем есть ли такой код контрагента*/
if(rek_shetk.saldo == 3)
if(kontr1.ravno()[0] != '\0')
 {
  sprintf(strsql,"select kodkon from Skontr where ns='%s' and \
kodkon='%s'",data->rek_prov_n.shetk.ravno(),kontr1.ravno());
  if(sql_readkey(&bd,strsql) != 1)
   {
    sprintf(strsql,"%s %s %s %s!",
    gettext("Не нашли код контрагента"),
    kontr1.ravno(),
    gettext("в списке счета"),
    data->rek_prov_n.shetk.ravno());
    
    iceb_menu_soob(strsql,data->kwindow);
    return(1);
   } 
 }

if(rek_shetk.saldo != rek_shet.saldo && iceb_u_SRAV(kontr.ravno(),kontr1.ravno(),0) != 0)
 {  
  iceb_menu_soob(gettext("Не верно введён код контрагента !"),data->kwindow);
  return(1);
 }

if((rek_shetk.saldo == 3 || rek_shet.saldo == 3) && (kontr.ravno()[0] == '\0' || kontr1.ravno()[0] == '\0'))
 {  
  iceb_menu_soob(gettext("Не введён код контрагента !"),data->kwindow);
  return(1);
 }
if((rek_shetk.saldo < 3 && rek_shet.saldo < 3) && (kontr.ravno()[0] != '\0' || kontr1.ravno()[0] != '\0'))
 {  
  iceb_menu_soob(gettext("Оба счета со свернутым сальдо ! Код контрагента не вводится !"),data->kwindow);
  return(1);
 }

if(iceb_udprov(data->rek_prov_s->val,g,m,d,
data->rek_prov_s->shet.ravno_filtr(),
data->rek_prov_s->shetk.ravno_filtr(),
data->rek_prov_s->vremz,
atof(data->rek_prov_s->debet.ravno()),
atof(data->rek_prov_s->kredit.ravno()),
data->rek_prov_s->koment.ravno_filtr(),
2,data->kwindow) != 0)
   return(1);

time_t vrem;
time(&vrem);

iceb_zapprov(0,g1,m1,d1,data->rek_prov_n.shet.ravno(),
data->rek_prov_n.shetk.ravno_filtr(),
data->rek_prov_n.kontr.ravno_filtr(),
data->rek_prov_n.kontrk.ravno_filtr(),
"","","",atof(data->rek_prov_n.debet.ravno()),atof(data->rek_prov_n.kredit.ravno()),
data->rek_prov_n.koment.ravno_filtr(),2,0,vrem,0,0,0,
0,
data->rek_prov_n.kekv.ravno_atoi(),
data->kwindow);

gtk_widget_destroy(data->kwindow);
data->kwindow=NULL;
return(0);
}
