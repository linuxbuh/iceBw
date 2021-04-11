/*$Id: pvbanks_m.c,v 1.23 2014/08/31 06:19:21 sasa Exp $*/
/*12.07.2015	07.04.2004	Белых А.И.	pvbanks_m.c
Меню ввода реквизитов для расчёта перечисления на карт-счета
*/
#include "buhg_g.h"
#include "pvbanks.h"

enum
{
  FK2,
  FK3,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_SHET,
  E_DATAN,
  E_KONTR,
  E_GRUPK,
  E_SHETK,
  E_DATAP,
  E_BANK,
  KOLENTER  
 };

class pvbanks_data
 {
  public:
  pvbanks_rr  *rk;
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *opt;
  GtkWidget *opt_saldo;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет

  class iceb_u_spisok kodbank; /*Код банка в файле zarbanks.alx*/
  class iceb_u_int kod_bank_tabl; /*код банка в таблице Zarsb*/
  class iceb_u_spisok kod00;
  class iceb_u_spisok naimbank;
  int nomer_banka; /*порядковый номер банка в списке банков*/      

  pvbanks_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    nomer_banka=0;
   }

  void read_rek()
   {
    rk->shet.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHET])));
    rk->shet_kor.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHETK])));
    rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAN])));
    rk->datap.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAP])));
    rk->grupk.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_GRUPK])));
    rk->kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KONTR])));
    nomer_banka=gtk_combo_box_get_active (GTK_COMBO_BOX(opt));
    rk->metka_r=gtk_combo_box_get_active (GTK_COMBO_BOX(opt_saldo));
//    for(int i=0; i < KOLENTER; i++)
//      g_signal_emit_by_name(entry[i],"activate");
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
   }
 };


gboolean   pvbanks_v_key_press(GtkWidget *widget,GdkEventKey *event,class pvbanks_data *data);
void    pvbanks_v_vvod(GtkWidget *widget,class pvbanks_data *data);
void  pvbanks_v_knopka(GtkWidget *widget,class pvbanks_data *data);
void   pvbanks_rekviz(class pvbanks_data *data);
void  pvbanks_v_e_knopka(GtkWidget *widget,class pvbanks_data *data);
void pvbanks_m_sb(class pvbanks_data *data);

extern SQL_baza bd;

int   pvbanks_m(class pvbanks_rr *rek_ras)
{
class pvbanks_data data;
char strsql[512];

data.rk=rek_ras;
if(data.rk->datan.getdlinna() <= 1)
  data.rk->datan.poltekdat();
  
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Распечатать документы для перечисления на карт-счета"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(pvbanks_v_key_press),&data);

GtkWidget *label=gtk_label_new(gettext("Распечатать документы для перечисления на карт-счета"));

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }

GtkWidget *hboxradio = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxradio),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет


gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hboxradio);
gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

label=gtk_label_new(gettext("Сальдо"));
gtk_box_pack_start (GTK_BOX (hboxradio),label, FALSE, FALSE, 0);

class iceb_u_spisok spsaldo;
spsaldo.plus(gettext("Дебетoвое"));
spsaldo.plus(gettext("Кредитовое"));

iceb_pm_vibor(&spsaldo,&data.opt_saldo,data.rk->metka_r);
gtk_box_pack_start (GTK_BOX (hboxradio),data.opt_saldo, TRUE, TRUE, 0);




data.knopka_enter[E_SHET]=gtk_button_new_with_label(gettext("Счёт"));
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHET],"clicked",G_CALLBACK(pvbanks_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET],iceb_u_inttochar(E_SHET));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHET] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET], "activate",G_CALLBACK(pvbanks_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rk->shet.ravno());
gtk_widget_set_name(data.entry[E_SHET],iceb_u_inttochar(E_SHET));

sprintf(strsql,"%s",gettext("Дата сальдо"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAN],"clicked",G_CALLBACK(pvbanks_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAN],iceb_u_inttochar(E_DATAN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAN],gettext("Выбор даты"));

data.entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(pvbanks_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));

sprintf(strsql,"%s (,,)",gettext("Код контрагента"));
data.knopka_enter[E_KONTR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.knopka_enter[E_KONTR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KONTR],"clicked",G_CALLBACK(pvbanks_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KONTR],iceb_u_inttochar(E_KONTR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KONTR],gettext("Выбор контрагента"));

data.entry[E_KONTR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.entry[E_KONTR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KONTR], "activate",G_CALLBACK(pvbanks_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.rk->kontr.ravno());
gtk_widget_set_name(data.entry[E_KONTR],iceb_u_inttochar(E_KONTR));


sprintf(strsql,"%s (,,)",gettext("Код группы контрагента"));
data.knopka_enter[E_GRUPK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPK]), data.knopka_enter[E_GRUPK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_GRUPK],"clicked",G_CALLBACK(pvbanks_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_GRUPK],iceb_u_inttochar(E_GRUPK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_GRUPK],gettext("Выбор группы контрагента"));

data.entry[E_GRUPK] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPK]), data.entry[E_GRUPK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_GRUPK], "activate",G_CALLBACK(pvbanks_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUPK]),data.rk->grupk.ravno());
gtk_widget_set_name(data.entry[E_GRUPK],iceb_u_inttochar(E_GRUPK));


data.knopka_enter[E_SHETK]=gtk_button_new_with_label(gettext("Счёт корреспондент"));
gtk_box_pack_start (GTK_BOX (hbox[E_SHETK]), data.knopka_enter[E_SHETK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHETK],"clicked",G_CALLBACK(pvbanks_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHETK],iceb_u_inttochar(E_SHETK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHETK],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHETK] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHETK]), data.entry[E_SHETK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHETK], "activate",G_CALLBACK(pvbanks_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHETK]),data.rk->shet_kor.ravno());
gtk_widget_set_name(data.entry[E_SHETK],iceb_u_inttochar(E_SHETK));


sprintf(strsql,"%s",gettext("Дата проводки (д.м.г)"));
data.knopka_enter[E_DATAP]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAP]), data.knopka_enter[E_DATAP], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAP],"clicked",G_CALLBACK(pvbanks_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAP],iceb_u_inttochar(E_DATAP));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAP],gettext("Выбор даты"));

data.entry[E_DATAP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAP]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAP]), data.entry[E_DATAP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAP], "activate",G_CALLBACK(pvbanks_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAP]),data.rk->datap.ravno());
gtk_widget_set_name(data.entry[E_DATAP],iceb_u_inttochar(E_DATAP));


pvbanks_m_sb(&data); /*читаем список банков*/

label=gtk_label_new(gettext("Банк"));
gtk_box_pack_start (GTK_BOX (hbox[E_BANK]), label, FALSE, FALSE, 0);

iceb_pm_vibor(&data.naimbank,&data.opt,data.nomer_banka);
gtk_box_pack_start (GTK_BOX (hbox[E_BANK]), data.opt, TRUE, TRUE, 0);



sprintf(strsql,"F2 %s",gettext("Расчёт"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать расчёт"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(pvbanks_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F3 %s",gettext("Реквизиты"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK3],"clicked",G_CALLBACK(pvbanks_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(pvbanks_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(pvbanks_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

return(data.voz);

}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  pvbanks_v_e_knopka(GtkWidget *widget,class pvbanks_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_DATAN:

    if(iceb_calendar(&data->rk->datan,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rk->datan.ravno());
      
    return;  

  case E_DATAP:

    if(iceb_calendar(&data->rk->datap,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAP]),data->rk->datap.ravno());
      
    return;  

   
  case E_SHET:

    iceb_vibrek(1,"Plansh",&data->rk->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk->shet.ravno());

    return;  

  case E_SHETK:

    iceb_vibrek(1,"Plansh",&data->rk->shet_kor,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETK]),data->rk->shet_kor.ravno());

    return;  

  case E_KONTR:

    iceb_vibrek(0,"Kontragent",&data->rk->kontr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rk->kontr.ravno());

    return;  

  case E_GRUPK:

    iceb_vibrek(0,"Gkont",&data->rk->grupk,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUPK]),data->rk->grupk.ravno());

    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   pvbanks_v_key_press(GtkWidget *widget,GdkEventKey *event,class pvbanks_data *data)
{

//printf("pvbanks_v_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);

  case GDK_KEY_F3:
    g_signal_emit_by_name(data->knopka[FK3],"clicked");
    return(TRUE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");
    return(TRUE);

    
  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  pvbanks_v_knopka(GtkWidget *widget,class pvbanks_data *data)
{
short dn,mn,gn;
struct OPSHET reksh;

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    
    if(iceb_u_rsdat(&dn,&mn,&gn,data->rk->datan.ravno(),1) != 0)
     {
      iceb_u_str repl;
      repl.plus(gettext("Не верно введена дата !"));
      iceb_menu_soob(&repl,data->window);
      return;
     }
     
    if(data->rk->shet.getdlinna() <= 1)
     {
      iceb_u_str repl;
      repl.plus(gettext("Не введён счёт !"));
      iceb_menu_soob(&repl,data->window);
      return;
     }


    if(iceb_prsh1(data->rk->shet.ravno(),&reksh,data->window) != 0)
     return;

    data->rk->naimshet.new_plus(reksh.naim.ravno());            
    data->rk->vds=reksh.vids;
    data->rk->saldo=reksh.saldo;
    data->rk->stat=reksh.stat;

    if(data->rk->shet_kor.getdlinna() > 1)
     if(iceb_prsh1(data->rk->shet_kor.ravno(),&reksh,data->window) != 0)
       return;

    if(data->rk->datap.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Неправильно введена дата проводки!"),data->window);
      return;
     }        

    data->rk->kod_banka=data->kodbank.ravno_atoi(data->nomer_banka);
    data->rk->kod00.new_plus(data->kod00.ravno(data->nomer_banka));
    data->rk->kod_banka_tabl=data->kod_bank_tabl.ravno(data->nomer_banka);
    gtk_widget_destroy(data->window);
    data->voz=0;
    return;  

  case FK3:
    pvbanks_rekviz(data);

    return;

  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    data->read_rek(); //Читаем реквизиты меню
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    pvbanks_v_vvod(GtkWidget *widget,class pvbanks_data *data)
{

int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_DATAN:
    data->rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SHET:
    data->rk->shet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KONTR:
    data->rk->kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/********************/
/*Работа с реквизитами*/
/***********************/
void   pvbanks_rekviz(class pvbanks_data *data)
{
short nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Реквизиты"));

zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Просмотр списка счетов"));
punkt_m.plus(gettext("Просмотр общего списка контрагентов"));

nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,NULL);



iceb_u_str kod;
iceb_u_str naikod;

switch (nomer)
 {
  case -1:
    return;
    
  case 0:
    iceb_vibrek(1,"Plansh",&data->rk->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk->shet.ravno());
    break;

  case 1:
    iceb_vibrek(1,"Kontragent",&data->rk->kontr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rk->kontr.ravno());
    break;

 }

}
/****************************/
/*формируем список банков*/
/****************************/
void pvbanks_m_sb(class pvbanks_data *data)
{
class iceb_u_str kodb("");
class iceb_u_str kodk("00");
class iceb_u_str nmb("");
class SQLCURSOR cur_alx;
int kod_bank=0;
SQL_str row_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='zarbanks.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }

if(kolstr == 0)
 {
  iceb_menu_soob("Не найдены настройки zarbanks.alx",data->window);
  return;
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  if(iceb_u_polen(row_alx[0],&kodb,1,'|') != 0)
   continue;

  if(iceb_u_polen(row_alx[0],&nmb,2,'|') != 0)
   continue;

  iceb_u_polen(row_alx[0],&kodk,3,'|');
  if(kodk.getdlinna() <= 1)
   kodk.new_plus("00");

  iceb_u_polen(row_alx[0],&kod_bank,4,'|');
   
  data->naimbank.plus(nmb.ravno());
  data->kodbank.plus(kodb.ravno_atoi());  
  data->kod00.plus(kodk.ravno());  
  data->kod_bank_tabl.plus(kod_bank);
  
 }


}
