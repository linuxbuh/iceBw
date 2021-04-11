#cборка iceBw
message ("Построение подсистем бух. учёта")

#project(iceBw) #имя проекта

#cmake_minimum_required(VERSION 2.8) #минимальная версия cmake

#сборка программы для системного администрирования баз данных i_admint
# Установка переменной со списком исходников
set(SOURSE_BIN i_admin.cpp 
	admin_alx.cpp admin_cp_alxw.cpp admin_del_alxw.cpp admin_l.cpp admin_makebaz_r.cpp admin_makebaz_m.cpp admin_dostup.cpp admin_user.cpp admin_user_p.cpp 
	admin_bases.cpp admin_bases_p.cpp 
	admin_makebaz.cpp admin_newuserw.cpp admin_del_user.cpp 
	admin_alterw.cpp admin_alterw_m.cpp admin_alterw_r.cpp 
	admin_icebuser.cpp admin_icebuser_v.cpp admin_icebuser_v1.cpp 
	admin_provbaz.cpp 
	admin_dumpbazin.cpp admin_dumpbazin_m.cpp
	admin_new_parol.cpp 
	check_tablew.cpp check_tablew_r.cpp 
	docinw.cpp 
	prov_uduserw.cpp poiinw.cpp 
	repair_tablew.cpp repair_tablew_r.cpp)


add_executable(i_admin ${SOURSE_BIN}) #сборка программы

#target_link_libraries(i_admin buh_g util_g sql mysqlclient ${GTK3_LIBRARIES} ) # Линковка программы с библиотеками
target_link_libraries(i_admin buh_g util_g sql ${MYSQLCLIENT_LIB} ${GTK3_LIBRARIES} ) # Линковка программы с библиотеками

#****************************************************************

#сборка программы запуска всех подсистем 
# Установка переменной со списком исходников
set(SOURSE_BIN i_start.cpp optimbazw.cpp check_tablew.cpp check_tablew_r.cpp repair_tablew.cpp repair_tablew_r.cpp)


add_executable(i_start ${SOURSE_BIN}) #сборка программы

target_link_libraries(i_start buh_g util_g sql ${MYSQLCLIENT_LIB} ${GTK3_LIBRARIES} ) # Линковка программы с библиотеками

#****************************************************************

#сборка подсистемы "Главная книга"
# Установка переменной со списком исходников
set(SOURSE_BIN i_xbu.cpp 
	balans_f1w.cpp balans_f2w.cpp 
	buhdenshetw.cpp 
	buhgspw.cpp buhgspw_m.cpp buhgspw_r.cpp 
	buhhahw.cpp buhhahw_m.cpp buhhahw_r.cpp 
	buhmesshetw.cpp 
	buhnast_g.cpp buhkomw.cpp buhkomw_m.cpp buhkomw_r.cpp 
	buhpsdw.cpp buhpsdw_m.cpp buhpsdw_r.cpp 
	buhrpzw.cpp buhrpzw_m.cpp buhrpzw_r.cpp 
	buhrpznpw.cpp buhrpznpw_m.cpp buhrpznpw_r.cpp 
	buhsdkrw.cpp buhsdkrw_m.cpp buhsdkrw_r.cpp 
	buhspkw.cpp buhspkw_m.cpp buhspkw_r.cpp 
	buhvdzw.cpp buhvdzw_m.cpp buhvdzw_r.cpp 
        buhpnkw.cpp buh_rhw.cpp 
	buh_rnr_shw.cpp buh_rnr_shw_m.cpp buh_rnr_shw_r.cpp buh_hpsw.cpp 
	clprov_m.cpp clprov_r.cpp 
	forzappoi.cpp 
	glkniw.cpp glkni_m.cpp glkniw_r.cpp glkni_nbs_m.cpp glkni_nbs_r.cpp gosrsnw_r.cpp gosssw_r.cpp go_m.cpp
	go_srk.cpp go_srv_nbs.cpp go_ss_nbs.cpp 
	hdisk.cpp hrvnks.cpp 
	imp_prov.cpp imp_prov_r.cpp 
	l_kv_start.cpp l_kv.cpp l_kv_m.cpp l_kv_v.cpp 
	l_prov.cpp l_prov_kr.cpp l_prov_krnbs.cpp
	l_saldo_sh.cpp l_saldo_sh_p.cpp l_saldo_sh_v.cpp 
	l_saldo_shk.cpp l_saldo_shk_p.cpp  l_saldo_shk_v.cpp 
	l_sval.cpp l_sval_p.cpp l_sval_v.cpp 
	m_plc_gk.cpp m_poiprov.cpp m_vdd.cpp m_vprov_bs.cpp m_vprov_nbs.cpp m_vprov_bs_gr.cpp m_vprov_nbs_gr.cpp 
	ostkarw.cpp ostuosw.cpp 
	pdokkow.cpp pdokmuw.cpp pdokpdw.cpp pdokukrw.cpp pdokuosw.cpp pdokuslw.cpp pdokzpw.cpp 
	plc_gk.cpp plc_gk_r.cpp poiinw.cpp poiinpdw.cpp poiprovw.cpp provvkp.cpp 
	pib_end_dokw.cpp pib_itlw.cpp pib_strw.cpp 
	pvbanks_m.cpp pvbanks_r.cpp pvbanks_kprovw.cpp 
	r_poiprov.cpp rbd.cpp rpshet_rsw.cpp rpshet_ssw.cpp rpsrsw.cpp rspvk.cpp rspvk_m.cpp rspvk_r.cpp
	saldogkw.cpp saldo_m.cpp saldogk_r.cpp saldonbsw.cpp sapgorsw.cpp shetsrs_m.cpp 
	ukrsib_strw.cpp ukrsib_endw.cpp ukrsib_startw.cpp
	ukreksim_hapw.cpp ukreksim_endw.cpp ukreksim_zstrw.cpp ukreksim_hw.cpp
	vibshet.cpp vregp.cpp 
	xbudkzw.cpp 
	zarbanksw.cpp 
	zvb_start_lstw.cpp zvb_str_lstw.cpp zvb_end_lstw.cpp zvb_head_lstw.cpp zvb_menu_lstw.cpp
	zvb_privatbank_startw.cpp zvb_privatbank_start_v.cpp zvb_privatbank_strw.cpp zvb_privatbank_endw.cpp 
	zvb_hreshatik_startw.cpp zvb_hreshatik_start_v.cpp zvb_hreshatik_strw.cpp zvb_hreshatik_endw.cpp 
	zvb_ibank2ua_endw.cpp zvb_ibank2ua_strw.cpp zvb_ibank2ua_startw.cpp 
	zvb_ukrgaz_endw.cpp zvb_ukrgaz_startw.cpp zvb_ukrgaz_strw.cpp 
	zvb_kievrus_startw.cpp zvb_kievrus_endw.cpp zvb_kievrus_strw.cpp 
	zvb_corp2_startw.cpp zvb_corp2_endw.cpp zvb_corp2_strw.cpp
	zvb_elpay_startw.cpp zvb_elpay_strw.cpp zvb_elpay_endw.cpp )


add_executable(i_xbu ${SOURSE_BIN}) #сборка программы

target_link_libraries(i_xbu buh_g util_g sql ${MYSQLCLIENT_LIB} ${GTK3_LIBRARIES} ) # Линковка программы с библиотеками

#****************************************************************

#сборка подсистемы "Материальный учёт"
# Установка переменной со списком исходников
set(SOURSE_BIN i_xmu.cpp 
	avtpromuw.cpp avtpromu1w.cpp 
	cennikdw.cpp cennikmw.cpp cennikw.cpp copdokw.cpp copdok_m.cpp copdok_r.cpp
	doocw.cpp dooc_m.cpp dooc_r.cpp doocsumw.cpp doocsumw_m.cpp doocsumw_r.cpp dok_end_muw.cpp 
	dvmatw.cpp dvtmcf1w.cpp dvtmcf1_m.cpp dvtmcf1_r.cpp dvtmcf2w.cpp dvtmcf2_m.cpp dvtmcf2_r.cpp 
	eks_ko_v.cpp 
	eksmuw.cpp eksmuw_m.cpp eksmuw_r.cpp 
	eks_dok_muw.cpp 
	findkarw.cpp 
	hrasactw.cpp 
	impmatdokw.cpp impmatdok_r.cpp impmatdok_xml_r.cpp impmatdok_xml_m.cpp impmatostw.cpp impmatostw_r.cpp impmatsm.cpp impmatsm_r.cpp impuslsu.cpp impuslsu_r.cpp 
	in_d12_startw.cpp in_d12_itw.cpp 
	koncinvw.cpp krasactw.cpp kriostw.cpp kriostw_m.cpp kriostw_r.cpp kzvmu1.cpp kzvmu2.cpp 
	l_dokmat.cpp l_doksmat.cpp l_doksmat_p.cpp 
	l_dokummat4.cpp l_dokummat4_p.cpp l_dokummat4_v.cpp l_dokummat4_sm.cpp
	l_kartmat.cpp l_kartmatv.cpp 
	l_kaskcn.cpp l_kaskcn_p.cpp l_kaskcn_v.cpp
	l_kasopr.cpp l_kasopr_p.cpp l_kasopr_v.cpp 
	l_kasopp.cpp l_kasopp_p.cpp l_kasopp_v.cpp 
	l_mater.cpp l_mater_dv.cpp l_mater_p.cpp l_mater_v.cpp l_mater_unz.cpp l_mater_zkm.cpp l_matusl.cpp l_matusl_v.cpp
	l_musnrm.cpp l_musnrm_p.cpp l_musnrm_v.cpp
	l_nalog.cpp l_nalog_v.cpp 
	l_opldok.cpp l_opldok_v.cpp 
	l_poimatdok.cpp l_prov_mu.cpp 
	l_spec.cpp l_spec1.cpp l_spec_v.cpp
	l_zkartmat.cpp l_zkartmat_p.cpp l_zkartmat_v.cpp 
	l_spis_kas.cpp l_spis_kas_v.cpp 
	l_sval.cpp l_sval_p.cpp l_sval_v.cpp 
	l_uslgrup.cpp l_uslgrup_v.cpp l_uslgrup_p.cpp 
	l_uslopp.cpp l_uslopp_p.cpp l_uslopp_v.cpp 
	l_uosopp.cpp l_uosopp_p.cpp l_uosopp_v.cpp 
	l_uslopr.cpp l_uslopr_p.cpp l_uslopr_v.cpp 
        l_uslugi.cpp l_uslugi_v.cpp l_uslugi_p.cpp 
	l_uosopr.cpp l_uosopr_p.cpp l_uosopr_v.cpp 
	makkorw.cpp makkorrw.cpp matnastw.cpp matuddokw.cpp matuddokw_r.cpp 
	matudw.cpp matuddw.cpp menuvnpw.cpp mudmcw.cpp mudmcw_m.cpp mudmcw_r.cpp muinvw.cpp muinv_m.cpp muinv_r.cpp 
	mumow.cpp mumow_m.cpp mumow_r.cpp musow.cpp mugetdppw.cpp mu_pvkuw.cpp
	musspiw.cpp musspiw_r.cpp musspiw_m.cpp
	nomkmw.cpp nomkrw.cpp 
	oplmuw.cpp opssw.cpp opssw_m.cpp opssw_r.cpp ostdokw.cpp ostdok1w.cpp ostkarw.cpp ostkar1w.cpp 
	ost_skl_shetw.cpp ost_skl_shetw_m.cpp ost_skl_shetw_r.cpp 
	perecenpw.cpp perecenrw.cpp plc_mu.cpp plc_mu_r.cpp podtdokw.cpp podtdok1w.cpp podvdokw.cpp podzapm.cpp poigod_mu.cpp 
	prihod_m.cpp priv_k_kartw.cpp poimatdokw.cpp prcnw.cpp prdppw.cpp prihodw.cpp prognozw.cpp prognozw_m.cpp prognozw_r.cpp 
	prosprw.cpp provkolw.cpp provndsw.cpp provudvdokw.cpp 
	rasactmuw.cpp rasdokw.cpp rasnakw.cpp rasmushetw.cpp rasttnw.cpp rasttn1w.cpp rasttn1w_m.cpp rasttn2w.cpp rasttn2w_m.cpp rasdok1w.cpp rasdok2w.cpp rashapw.cpp rashodw.cpp 
	rasmedprw.cpp rasnalnw.cpp rasndsw.cpp rasndsw_m.cpp rasndsw_r.cpp 
	rasoskr_m.cpp rasoskr_r.cpp rasoskrw.cpp raspropw.cpp rasspdokw.cpp rasspdok_r.cpp rasspdok_m.cpp readdokw.cpp 
	readkolkw.cpp rozkor_m.cpp rozkorw.cpp rozkor_r.cpp rozn_oth.cpp rozn_oth_m.cpp rozn_oth1_r.cpp rozn_oth2_r.cpp
	rppvow.cpp rppvow_m.cpp rppvow_r.cpp
	saldo_m.cpp saldomuw.cpp saldomuw_r.cpp sapkkivw.cpp sapkk1ivw.cpp sapvinw.cpp sklkar_m.cpp skoplmuw.cpp sortdokmuw.cpp specrasw.cpp specrasw_r.cpp spis_p_kontrw.cpp 
	spis_p_kontrw_m.cpp spis_p_kontrw_r.cpp spizw.cpp spizw_m.cpp spizw_r.cpp sproznw.cpp sprozn_r.cpp sprozn_m.cpp sumdokw.cpp 
	sumdokwu.cpp sumzap1w.cpp sumzap1uw.cpp sumzapuosw.cpp
	udkarw.cpp 
	vdndw.cpp vdnd1.cpp vdnd2.cpp vbrpw.cpp vib_kodop_mu.cpp vib_kodop_uos.cpp vib_kodop_usl.cpp 
	vkartm.cpp vmat_mc.cpp vmat_medp.cpp vmatw.cpp 
	xml_nn_matw.cpp 
	zagotkl.cpp zagotkl_m.cpp zagotkl_r.cpp zapdokw.cpp zaphkorw.cpp zapkartm.cpp zapkarw.cpp zap_prihodw.cpp zapvdokw.cpp zapvkrw.cpp zapzkorw.cpp 
	zap_s_mudokw.cpp)


add_executable(i_xmu ${SOURSE_BIN}) #сборка программы

target_link_libraries(i_xmu buh_g util_g sql ${MYSQLCLIENT_LIB} ${GTK3_LIBRARIES} ) # Линковка программы с библиотеками

#****************************************************************

#сборка подсистемы "Учёт услуг"
# Установка переменной со списком исходников
set(SOURSE_BIN i_xusl.cpp 
	avtprouslw.cpp avtprousl1w.cpp
	copdokuw.cpp copdoku_r.cpp copdoku_m.cpp 
	dvuslf2w.cpp dvuslf2w_m.cpp dvuslf2w_r.cpp dok_end_uslw.cpp 
	eks_ko_v.cpp 
	findkarw.cpp 
	impusldokw.cpp impusldok_r.cpp impuslsu.cpp impuslsu_r.cpp impusldok_xml_rw.cpp impusldok_xml_mw.cpp 
	l_kaskcn.cpp l_kaskcn_p.cpp l_kaskcn_v.cpp 
	l_kasopr.cpp l_kasopr_p.cpp l_kasopr_v.cpp 
	l_kasopp.cpp l_kasopp_p.cpp l_kasopp_v.cpp 
	l_mater1.cpp l_mater_p.cpp 
	l_opldok.cpp l_opldok_v.cpp 
	l_poiusldok.cpp l_prov_usl.cpp 
	l_spis_kas.cpp l_spis_kas_v.cpp
	l_sval.cpp l_sval_p.cpp l_sval_v.cpp 
	l_usldok.cpp l_usldoks.cpp l_usldoks_p.cpp l_uslgrup.cpp l_uslgrup_p.cpp l_uslgrup_v.cpp 
	l_uslkart.cpp l_uslkart_p.cpp 
	l_uslopp.cpp l_uslopp_p.cpp l_uslopp_v.cpp 
	l_uosopp.cpp l_uosopp_p.cpp l_uosopp_v.cpp 
	l_uosopr.cpp l_uosopr_p.cpp l_uosopr_v.cpp 
	l_uslopr.cpp l_uslopr_p.cpp l_uslopr_v.cpp l_uslugi.cpp l_uslugi_p.cpp l_uslugi_v.cpp
	l_uslpodr.cpp l_uslpodr_p.cpp l_uslpodr_v.cpp
	makkorw.cpp makkorrw.cpp menuvnpw.cpp
	nomkrw.cpp nomkmw.cpp 
	opss_uw.cpp opss_uw_m.cpp opss_uw_r.cpp ostkarw.cpp ostdok1w.cpp 
	perecenuw.cpp podtdok1w.cpp podvdokw.cpp podudokw.cpp podzapusl.cpp poigod_usl.cpp poiusldokw.cpp prdppw.cpp 
	prilogw.cpp priv_k_kartw.cpp prosprusw.cpp 
	rasdokuw.cpp rasnalnuw.cpp rasspdokuw.cpp rasspdokuw_m.cpp rasspdokuw_r.cpp 
	readkolkw.cpp readkoluw.cpp rppvouw.cpp rppvouw_m.cpp rppvouw_r.cpp 
	saldo_kontr.cpp skopluslw.cpp sumzap1w.cpp sumzap1uw.cpp sumzapuosw.cpp 
	uddusl_r.cpp uduvdokw.cpp usldopnalw.cpp uslnastw.cpp usl_act_vrw.cpp usl_shet_fakw.cpp usl_pvkuw.cpp
	usl_spismat.cpp usl_spismat_m.cpp usl_spismat_r.cpp uslpoddokw.cpp usldupkw.cpp usldupkw_m.cpp usldupkw_r.cpp 
	vdnuw.cpp vib_kodop_mu.cpp vib_kodop_usl.cpp vib_kodop_uos.cpp vuslw.cpp vuslw_v.cpp 
	xuslplcw.cpp xuslplcw_r.cpp xml_nn_uslw.cpp 
	zapkarw.cpp zaphkorw.cpp zappoduw.cpp zapuvdokw.cpp zapvdokw.cpp zapvkrw.cpp zap_prihodw.cpp zapzkorw.cpp
	zap_s_mudokw.cpp)


add_executable(i_xusl ${SOURSE_BIN}) #сборка программы

target_link_libraries(i_xusl buh_g util_g sql ${MYSQLCLIENT_LIB} ${GTK3_LIBRARIES} ) # Линковка программы с библиотеками

#****************************************************************

#сборка подсистемы "Учёт кассовых ордеров"
# Установка переменной со списком исходников
set(SOURSE_BIN i_xkas.cpp 
	avtprkasw.cpp 
	imp_kasordw.cpp imp_kasordw_r.cpp itogw.cpp 
	kasnastw.cpp kasothw.cpp kasothw_m.cpp kasothw_r.cpp kasothw_r1.cpp kasgodsw_r.cpp 
	kasothkcn.cpp kasothkcn_m.cpp kasothkcn_r.cpp 
	l_kasdok.cpp 
	l_kasdoks.cpp l_kasdoks_p.cpp 
        l_kaskcn.cpp l_kaskcn_p.cpp l_kaskcn_v.cpp 
	l_kaskcnsl.cpp l_kaskcnsl_p.cpp l_kaskcnsl_v.cpp 
	l_kasopp.cpp l_kasopp_p.cpp l_kasopp_v.cpp 
	l_kasopr.cpp l_kasopr_p.cpp l_kasopr_v.cpp 
	l_kasvedz.cpp l_kasvedz_p.cpp l_kasvedz_v.cpp 
	l_poikasdok.cpp l_prov_kas.cpp 
	l_spis_kas.cpp l_spis_kas_v.cpp 
	l_sval.cpp l_sval_p.cpp l_sval_v.cpp 
	pbkasw.cpp poikasdokw.cpp provpodkow.cpp provprkasw.cpp 
	saldokasw.cpp saldokasw_m.cpp saldokasw_r.cpp 
	sspp1w.cpp sumkasorw.cpp sumkasoruew.cpp sumprvedw.cpp 
	udkasdokw.cpp udkasdokgw.cpp udkasdokgw_r.cpp udpldw.cpp 
	vib_kodop_kas.cpp vkasord1.cpp vkasord2.cpp vvko_kontr.cpp 
	xkasplcw.cpp xkasplcw_r.cpp 
	zaphkorw.cpp zapzkorw.cpp zarstfn53w.cpp zarstvedw.cpp)


add_executable(i_xkas ${SOURSE_BIN}) #сборка программы

target_link_libraries(i_xkas buh_g util_g sql ${MYSQLCLIENT_LIB} ${GTK3_LIBRARIES} ) # Линковка программы с библиотеками

#****************************************************************

#сборка подсистемы "Платёжные документы"
# Установка переменной со списком исходников
set(SOURSE_BIN i_xdk.cpp 
	avtprpldw.cpp 
	b_akb_imexbankw.cpp b_aval.cpp b_brokbiznesw.cpp b_soft_revieww.cpp b_b2_bank_on_linew.cpp b_iBank_2_UAw.cpp b_pervinvw.cpp b_privatw.cpp b_tinyw.cpp 
	b_privatinvestw.cpp b_prominvest.cpp b_nadra.cpp b_skarbw.cpp b_ukrbiznesw.cpp b_ukrinbank.cpp b_ukrpromw.cpp b_ukreximw.cpp b_ukrsocw.cpp b_corp2w.cpp 
	b_stilw.cpp
	dok_doverw.cpp 
	l_banks.cpp 
	l_poixdkdok.cpp 
	l_prov_xdk.cpp 
	l_sptbn1.cpp l_sptbn1_p.cpp 
	l_tippl.cpp l_tippl_p.cpp l_tippl_v.cpp 
	l_xdkdoks.cpp l_xdkdoks_p.cpp 
	l_xdkop.cpp l_xdkop_p.cpp l_xdkop_v.cpp 
	l_xdkpz.cpp l_xdkpz_v.cpp 
	nastdkw.cpp 
	platpw.cpp podzapxdk.cpp poigod_xdk.cpp poixdkdokw.cpp provpdw.cpp pvbanks_kprovw.cpp 
	readkomw.cpp readpdokw.cpp 
	sumprpdw.cpp sumpzpdw.cpp 
	udpldw.cpp 
	vibshbnw.cpp vpld.cpp vzrkbzw.cpp vzrkfw.cpp 
	xdkdok.cpp xdkdok_v.cpp xdkgur.cpp xdkgur_r.cpp xdkgur_v.cpp xdksend.cpp xdksoznkom.cpp xdkudgod.cpp xdkudgod_r.cpp xdkvnp.cpp xdkvnp_rt.cpp 
	xdkplcw.cpp xdkplcw_r.cpp 
	xdkbanksw.cpp xdkbanksw_m.cpp xdkbanksw_r.cpp 
	zappdokw.cpp)


add_executable(i_xdk ${SOURSE_BIN}) #сборка программы

target_link_libraries(i_xdk buh_g util_g sql ${MYSQLCLIENT_LIB} ${GTK3_LIBRARIES} ) # Линковка программы с библиотеками

#****************************************************************

#сборка подсистемы "Заработная плата"
# Установка переменной со списком исходников
set(SOURSE_BIN i_xzp.cpp 
	alimw.cpp avtprozarw.cpp 
	arz.cpp arz_m.cpp arz_r.cpp 
	boln1w.cpp 
	clearzarw.cpp clearzarw_r.cpp creatheaddbfw.cpp 
	dopdominw.cpp 
	dolhnnw.cpp 
	f1df_xml_konw.cpp f1df_xml_strw.cpp f1df_xml_zagw.cpp 
	findnahw.cpp findnahmw.cpp 
	hdisk.cpp hrvnks.cpp 
	impotvrw.cpp impotvrw_r.cpp 
	imp_kr.cpp imp_kr_m.cpp imp_kr_r.cpp imp_nu.cpp imp_nu_r.cpp imp_zardokw.cpp imp_zardokw_r.cpp imp_zar_kart.cpp imp_zar_kart_r.cpp 
	imp_zar_kartsh.cpp imp_zar_kartsh_r.cpp itogizw.cpp itogw.cpp imp_zar_kartf.cpp imp_zar_kartf_r.cpp 
	kategw.cpp kategshw.cpp knnfndsw.cpp koreshw.cpp kreditw.cpp koncf8drw.cpp 
	l_gruppod.cpp l_gruppod_p.cpp l_gruppod_v.cpp 
	l_f1df.cpp l_f1df_v.cpp l_f1dfz.cpp l_f1dfz_p.cpp l_f1dfz_v.cpp l_f1dfz1_v.cpp 
	l_f1dfvs.cpp l_f1dfvs_p.cpp l_f1dfvs_v.cpp 
	l_kaskcn.cpp l_kaskcn_p.cpp l_kaskcn_v.cpp 
	l_kasopp.cpp l_kasopp_p.cpp l_kasopp_v.cpp 
	l_kasopr.cpp l_kasopr_p.cpp l_kasopr_v.cpp 
	l_nahud.cpp l_nahud_p.cpp l_nahud_vz.cpp l_nahud_rasp_v.cpp
	l_nahud_t.cpp l_nahud_t_v.cpp
	l_otrvr.cpp l_otrvr_p.cpp l_otrvr_v.cpp l_otrvr_zk.cpp 
	l_otrvr_t.cpp l_otrvr_t_v.cpp 
	l_poizardok.cpp l_prov_zar.cpp 
	l_spis_kas.cpp l_spis_kas_v.cpp l_sptbn.cpp l_sptbn_p.cpp 
	l_sval.cpp l_sval_p.cpp l_sval_v.cpp 
	l_ukrgrup.cpp l_ukrgrup_p.cpp l_ukrgrup_v.cpp 
	l_ukrzat.cpp l_ukrzat_p.cpp l_ukrzat_v.cpp 
	l_zaresv.cpp l_zaresv_v.cpp 
	l_zargni.cpp l_zargni_p.cpp l_zargni_v.cpp 
	l_zarkateg.cpp l_zarkateg_p.cpp l_zarkateg_v.cpp 
	l_zarkh.cpp l_zarkh_v.cpp 
	l_zarnah.cpp l_zarnah_p.cpp l_zarnah_v.cpp 
	l_zarpodr.cpp l_zarpodr_p.cpp l_zarpodr_v.cpp 
	l_zarsaldo.cpp l_zarsaldo_v.cpp l_zarsaldo_p.cpp 
	l_zarsb.cpp l_zarsb_p.cpp l_zarsb_v.cpp 
	l_zarsocf.cpp l_zarsocf_p.cpp l_zarsocf_v.cpp 
        l_zarsocot.cpp l_zarsocot_v.cpp 
	l_zarud.cpp l_zarud_p.cpp l_zarud_v.cpp 
        l_zarvidtab.cpp l_zarvidtab_p.cpp l_zarvidtab_v.cpp 
	l_zarzvan.cpp l_zarzvan_p.cpp l_zarzvan_v.cpp 
	l_zar_dok.cpp l_zar_dok_p.cpp l_zar_dok_hap.cpp l_zar_dok_zap.cpp l_zar_dok_zap_p.cpp l_zar_dok_zap_v.cpp
	l_zarskrdh.cpp l_zarskrdh_v.cpp 
	l_zarsvd.cpp l_zarsvd_v.cpp 
	l_zarmzpm.cpp l_zarmzpm_v.cpp 
	l_zarrud.cpp l_zarrud_v.cpp 
	l_zarlgot.cpp l_zarlgot_v.cpp 
	l_zargr.cpp l_zargr_v.cpp 
	l_zarsdo.cpp l_zarsdo_p.cpp l_zarsdo_v.cpp 
	masinvw.cpp 
	nahudw.cpp 
	okladw.cpp okladtrw.cpp otrshw.cpp 
	peh1w.cpp pib_end_dokw.cpp pib_itlw.cpp pib_strw.cpp podoh1w.cpp podohrs2016w.cpp poiflw.cpp poigod_zar.cpp poizardokw.cpp 
	prbperw.cpp prbper1w.cpp procentw.cpp profsw.cpp provblokzarpw.cpp provkodw.cpp provzzagw.cpp 
	rasf1df.cpp rasf1df_m.cpp rasnuptn1w.cpp rasshetzw.cpp rasvedw.cpp rasvedw_m.cpp rasvedw_r.cpp raszkartw.cpp 
	ras_podoh1w.cpp read_nast_indw.cpp redkalw.cpp rudnfvw.cpp 
	saldozrw.cpp saldozrw_r.cpp saldo_m.cpp spkvdw.cpp ssf8drw.cpp 
	sapreestpw.cpp sosnsumw.cpp sprtabotpw.cpp sprtabotpw_m.cpp sprtabotpw_r.cpp sspp1w.cpp strfilw.cpp sumprvedw.cpp sumprzw.cpp 
	ukreksim_endw.cpp ukreksim_hapw.cpp ukreksim_hw.cpp ukreksim_zstrw.cpp 
	ukrsib_endw.cpp ukrsib_startw.cpp ukrsib_strw.cpp 
	vkartz.cpp vkartz1.cpp vvod_nahud.cpp vvod_podr.cpp vvod_tabnom.cpp vztbw.cpp 
	zarbanksw.cpp 
	zarbolw.cpp zarbolw_m.cpp zarbolw_r.cpp 
	zardznw.cpp 
	zagrf1df.cpp 
	zagf1dfw.cpp zagf1df_csvw.cpp 
	zaphkorw.cpp zapkrtbw.cpp zapf8drw.cpp zaptabelw.cpp zapzagotw.cpp zapzardkw.cpp zapzarnw.cpp zapzarn1w.cpp zapzarpw.cpp zapzarpvw.cpp zapzkorw.cpp zaravprw.cpp zaravpr_oldw.cpp zaravpt_osw.cpp 
	zarindexw.cpp zarindexvw.cpp 
	zarkmdw.cpp zarkvrt_gkw.cpp zarkvrt1_krw.cpp zarlgotw.cpp zarnastw.cpp zarnvpw.cpp 
	zarpdw.cpp zarpdw_m.cpp zarpdw_r.cpp 
	zarpensmw.cpp zarpensmw_m.cpp zarpensm1w_r.cpp 
	zarpodprw.cpp zarprtnw.cpp zarpvw.cpp zarreadnw.cpp zarrnesvw.cpp zarsaldw.cpp zarsbslw.cpp zarsdf1dfw.cpp zarsf1dfw.cpp zarsocw.cpp zarsprprw.cpp zarsprvmnw.cpp zarsrbolw.cpp 
	zartarrozw.cpp zartarroz1w.cpp zarudkarw.cpp zarudnuw.cpp zaruozw.cpp zarvpnw.cpp zarstfn53w.cpp zarstvedw.cpp 
	zarsnpmw.cpp zarsnpmw_m.cpp zarsnpmw_r.cpp zarsdrvw.cpp 
	zarszkw.cpp zarszkw_m.cpp zarszkw_r.cpp zarkvrt_zapvtablw.cpp zarvnw.cpp 
	zar_alim.cpp zar_alim_r.cpp 
	zar_ar.cpp zar_ar_r.cpp zar_ar_v.cpp zar_eks_ko.cpp zar_eks_ko_v.cpp 
	zar_bpom.cpp zar_bpom_m.cpp zar_bpom_r.cpp 
	zar_dolgor.cpp zar_dolgor_m.cpp zar_dolgor_r.cpp 
	zar_dolgrab.cpp zar_dolgrab_r.cpp 
	zar_f1df.cpp zar_f1df_m.cpp zar_f1df_r.cpp 
	zar_f1pv.cpp zar_f1pv_r.cpp 
	zar_kart.cpp zar_kart_m.cpp zar_kart_r.cpp 
	zar_kateg.cpp zar_kateg_m.cpp zar_kateg_r.cpp 
	zar_krz.cpp zar_krz_r.cpp 
	zar_mg.cpp zar_mg_r.cpp 
	zar_mpslw.cpp zarzapkkw.cpp 
	zar_otrsh.cpp zar_otrsh_m.cpp zar_otrsh_r.cpp 
	zar_otrvr.cpp zar_otrvr_m.cpp zar_otrvr_r.cpp 
	zar_pi.cpp zar_pi_r.cpp 
	zar_podr.cpp zar_podr_m.cpp zar_podr1_r.cpp zar_podr2_r.cpp 
	zar_read_tn1w.cpp zar_sm_shet.cpp zarrsovw.cpp 
	zar_rtabel.cpp zar_rtabel_m.cpp zar_rtabel_r.cpp 
	zar_snahud.cpp zar_snahud_m.cpp zar_snahud_r.cpp 
	zar_snu.cpp zar_snu_m.cpp zar_snu_r.cpp 
	zar_soc_nah.cpp zar_soc_nah_m.cpp zar_soc_nah_r.cpp 
	zar_soc_vz.cpp zar_soc_vz_m.cpp zar_soc_vz_r.cpp 
	zar_spdr.cpp zar_spdr_m.cpp zar_spdr_r.cpp 
	zar_spisr.cpp zar_spisr_m.cpp zar_spisr_r.cpp 
	zar_sprav.cpp zar_sprav_m.cpp zar_sprav_r.cpp 
	zar_srinu.cpp zar_srinu_m.cpp zar_srinu_r.cpp 
	zar_srninu.cpp zar_srninu_m.cpp zar_srninu_r.cpp 
	zar_vedpz.cpp zar_vedpz_m.cpp zar_vedpz_r.cpp 
	zar_pr_startw.cpp zar_pr_endw.cpp zar_pr_insw.cpp zar_pr_showw.cpp 
	zar_sumtnw.cpp zargetfiow.cpp 
	zvaniew.cpp 
	zvb_start_lstw.cpp zvb_str_lstw.cpp zvb_end_lstw.cpp zvb_head_lstw.cpp zvb_menu_lstw.cpp
	zvb_privatbankw.cpp  zvb_privatbank_endw.cpp zvb_privatbank_startw.cpp  zvb_privatbank_start_v.cpp  zvb_privatbank_strw.cpp 
	zvb_alfabankw.cpp  zvb_alfabank_endw.cpp zvb_alfabank_startw.cpp  zvb_alfabank_start_v.cpp  zvb_alfabank_strw.cpp 
	zvb_kreditpromw.cpp 
	zvb_hreshatikw.cpp zvb_hreshatik_endw.cpp zvb_hreshatik_strw.cpp zvb_hreshatik_start_v.cpp zvb_hreshatik_startw.cpp 
	zvb_ibank2uaw.cpp zvb_ibank2ua_endw.cpp zvb_ibank2ua_strw.cpp zvb_ibank2ua_startw.cpp 
	zvb_nadraw.cpp zvb_pibw.cpp zvb_ukreksimw.cpp zvb_ukrpromw.cpp zvb_ukrpromw_m.cpp zvb_ukrpromw_r.cpp 
	zvb_ukrgazw.cpp zvb_ukrgaz_endw.cpp zvb_ukrgaz_startw.cpp zvb_ukrgaz_strw.cpp 
	zvb_promekonomw.cpp zvb_promekonom_endw.cpp zvb_promekonom_startw.cpp zvb_promekonom_strw.cpp 
	zvb_promekonomw.cpp zvb_promekonom_endw.cpp zvb_promekonom_startw.cpp zvb_promekonom_strw.cpp 
	zvb_ukrsibw.cpp zvb_ukrsocw.cpp 
	zvb_kievrusw.cpp zvb_kievrus_startw.cpp zvb_kievrus_endw.cpp zvb_kievrus_strw.cpp 
	zvb_corp2w.cpp zvb_corp2_startw.cpp zvb_corp2_strw.cpp zvb_corp2_endw.cpp
	zvb_elpayw.cpp zvb_elpay_startw.cpp zvb_elpay_strw.cpp zvb_elpay_endw.cpp )


add_executable(i_xzp ${SOURSE_BIN}) #сборка программы

target_link_libraries(i_xzp buh_g util_g sql ${MYSQLCLIENT_LIB} ${GTK3_LIBRARIES} ) # Линковка программы с библиотеками

#****************************************************************

#сборка подсистемы "Учёт основных средств"
# Установка переменной со списком исходников
set(SOURSE_BIN i_xuos.cpp 
	amortw.cpp amort23w.cpp amortbyw.cpp avtprouosw.cpp 
	eksuosw.cpp eksuosw_m.cpp eksuosw_r.cpp 
	bsiz23w.cpp bsiz23pw.cpp bsizpw.cpp bsizw.cpp 
	hrasactw.cpp 
	imp_uosdokw.cpp imp_uosdokw_r.cpp impuosostw.cpp impuosostw_r.cpp 
	in_d12_itw.cpp in_d12_startw.cpp 
	koncinvw.cpp krasactw.cpp 
	l_opldok.cpp l_opldok_v.cpp 
	l_poiuosdok.cpp 
	l_prov_uos.cpp 
	l_uosamort.cpp l_uosamort_v.cpp l_uosamortbu.cpp l_uosamortbu_v.cpp 
	l_uosdm.cpp l_uosdm_p.cpp l_uosdm_v.cpp 
	l_uosdok.cpp l_uosdok_v.cpp 
	l_uosgrbu.cpp l_uosgrbu_p.cpp l_uosgrbu_v.cpp 
	l_uosgrnu.cpp l_uosgrnu_p.cpp l_uosgrnu_v.cpp 
	l_uoshau.cpp l_uoshau_p.cpp l_uoshau_v.cpp 
	l_uoshz.cpp l_uoshz_p.cpp l_uoshz_v.cpp 
	l_uosin.cpp l_uosin_p.cpp 
	l_uosindm.cpp l_uosindm_v.cpp
	l_uosls.cpp l_uosls_v.cpp 
	l_uosmo.cpp l_uosmo_p.cpp l_uosmo_v.cpp 
	l_uosopp.cpp l_uosopp_p.cpp l_uosopp_v.cpp 
	l_uosopr.cpp l_uosopr_p.cpp l_uosopr_v.cpp 
	l_uospodr.cpp l_uospodr_p.cpp l_uospodr_v.cpp 
        l_uosspdok.cpp l_uosspdok_p.cpp 
	menuvnpw.cpp 
	opluosw.cpp ostuosw.cpp 
	poduosdw.cpp poigod_uos.cpp poiinpdw.cpp poiinw.cpp poiuosdokw.cpp provuinw.cpp prpuosw.cpp pudokw.cpp puzapw.cpp
	rasactuosw.cpp rasdokuosw.cpp rasdopuosw.cpp rasmes.cpp rasnalnuosw.cpp rasnasw.cpp 
	rasamortnuw.cpp rasamortnuw_r.cpp rasamortbyw.cpp rasamortbyw_r.cpp 
	sapkk1ivw.cpp sapkkivw.cpp skopluosw.cpp 
	smenakofw.cpp smenakofw_m.cpp sumzapuosw.cpp 
	strms.cpp strpod.cpp 
	udamortw.cpp udinnw.cpp uduosdokw.cpp uduoszdw.cpp 
	uosdpd.cpp uosdpd_m.cpp uosdpd_r.cpp uosdiw.cpp 
	uosdvosw.cpp uosdvosw_m.cpp uosdvosw_r.cpp
	uosrbsw.cpp uosrbsw_m.cpp uosrbsw_r.cpp 
	uosrpw.cpp uosrpw_m.cpp uosrpw_r.cpp 
	uosactw.cpp uosgetinn.cpp uosgetlsw.cpp uosgetnd.cpp uoskart1.cpp uoskart2.cpp uosnastw.cpp uosprhd.cpp uosprovnd.cpp uosprovdok.cpp 
	uosinvedw.cpp uosinvedw_m.cpp uosinvedw_r.cpp 
	uosplcw.cpp uosplcw_r.cpp 
	uosprovarw.cpp uosprovgrw.cpp uosprs_m.cpp uosprs_r.cpp uosprs.cpp 
	uosremw.cpp uosrsdw.cpp uosrsd1w.cpp 
	uossasw.cpp uossosw.cpp 
	uosvamotw.cpp uosvamot_hw.cpp uosvamotw_m.cpp uosvamotw_r.cpp uosvamot1w_r.cpp 
	uosvamotbuw.cpp uosvamotbuw_m.cpp uosvamotbuw_r.cpp
	uosvosiiw.cpp uosvosiiw_m.cpp uosvosiiw_r.cpp 
	uosvosw.cpp uosvosw_m.cpp uosvosw_r.cpp 
	uosvhd.cpp uosvhd1.cpp uosvhd2.cpp uosvhdvp.cpp uosvhdzv.cpp 
	uosvvdw.cpp 
	uoszaphd.cpp uoszvdok.cpp uosvibuw.cpp uosoz6w.cpp uosrappw.cpp uos_menu_vu.cpp uosavpw.cpp uosik818w.cpp uosavve818w.cpp
	uos_spw.cpp uos_spw_m.cpp uos_spw_r.cpp uosasoz818w.cpp	uosastz818w.cpp
	vib_kodop_uos.cpp 
	xml_nn_uosw.cpp)


add_executable(i_xuos ${SOURSE_BIN}) #сборка программы

target_link_libraries(i_xuos buh_g util_g sql ${MYSQLCLIENT_LIB} ${GTK3_LIBRARIES} ) # Линковка программы с библиотеками

#****************************************************************

#сборка подсистемы "Учёт командировочных расходов"
# Установка переменной со списком исходников
set(SOURSE_BIN i_xukr.cpp 
	avtprukrw.cpp 
	l_gruppod.cpp l_gruppod_p.cpp l_gruppod_v.cpp 
	l_poiukrdok.cpp 
	l_prov_ukr.cpp 
	l_ukrdok.cpp l_ukrdok_v.cpp l_ukrdok_pb.cpp l_ukrdok_vn.cpp 
	l_ukrgrup.cpp l_ukrgrup_p.cpp l_ukrgrup_v.cpp 
	l_ukrspdok.cpp l_ukrspdok_p.cpp 
	l_ukrvk.cpp l_ukrvk_p.cpp l_ukrvk_v.cpp 
	l_ukrzat.cpp l_ukrzat_p.cpp l_ukrzat_v.cpp 
	poiukrdokw.cpp poigod_ukr.cpp 
	prosprkrw.cpp 
	ukrkontr.cpp ukrkontr_m.cpp ukrkontr_r.cpp 
	ukrkuw.cpp 
	ukrnastw.cpp 
	ukrprik.cpp ukrprik_m.cpp ukrprik_r.cpp 
	ukrrpw.cpp ukrrpw_m.cpp ukrrpw_r.cpp 
	ukrshkz.cpp ukrshkz_m.cpp ukrshkz_r.cpp 
	ukrspdokw.cpp ukrspdokw_m.cpp ukrspdokw_r.cpp 
	ukrspdok1w.cpp ukrspdok1w_m.cpp ukrspdok1w_r.cpp 
	ukrplcw.cpp ukrplcw_r.cpp 
	ukrgodsw.cpp ukrgodsw_r.cpp 
	ukrvhd.cpp 
	xukroppw.cpp xukroppw_m.cpp xukroppw_r.cpp)

add_executable(i_xukr ${SOURSE_BIN}) #сборка программы

target_link_libraries(i_xukr buh_g util_g sql ${MYSQLCLIENT_LIB} ${GTK3_LIBRARIES} ) # Линковка программы с библиотеками

#****************************************************************

#сборка подсистемы "Учёт путевых листов"
# Установка переменной со списком исходников
set(SOURSE_BIN i_xupl.cpp 
	dvmatw.cpp 
	impmatsm.cpp impmatsm_r.cpp 
	l_kartmat.cpp 
	l_poiupldok.cpp 
	l_uplavt0.cpp l_uplavt0_p.cpp l_uplavt0_v.cpp 
	l_uplavt1.cpp l_uplavt1_p.cpp l_uplavt1_v.cpp 
	l_upldok_1.cpp l_upldok_pb.cpp l_upldok_v1.cpp l_upldok_vs1.cpp l_upldok_vzp.cpp l_upldok_vzr.cpp
	l_upldok2a.cpp l_upldok2a_v.cpp 
	l_upldok3.cpp l_upldok3_p.cpp l_upldok3_v.cpp 
	l_uplmt.cpp l_uplmt_p.cpp l_uplmt_v.cpp 
	l_uplout.cpp l_uplout_p.cpp l_uplout_v.cpp 
	l_uplpod.cpp l_uplpod_p.cpp l_uplpod_v.cpp 
	l_uplsal.cpp l_uplsal_p.cpp l_uplsal_v.cpp 
	l_uplnst.cpp l_uplnst_p.cpp l_uplnst_v.cpp 
	l_uplscnt.cpp l_uplscnt_p.cpp l_uplscnt_v.cpp
	l_uplspdok.cpp l_uplspdok_p.cpp 
	l_mater.cpp l_mater_dv.cpp l_mater_v.cpp l_mater_p.cpp l_mater_zkm.cpp l_mater_unz.cpp 
	l_zkartmat.cpp l_zkartmat_p.cpp l_zkartmat_v.cpp
	poiupldokw.cpp poigod_upl.cpp 
	mugetdppw.cpp mu_pvkuw.cpp
	nomkmw.cpp 
	ostdokw.cpp ostkarw.cpp ostkar1w.cpp ostdok1w.cpp 
	ostvaw.cpp 
	podvdokw.cpp prdppw.cpp prosprw.cpp 
	readkolkw.cpp
	saldo_m.cpp saldouplw.cpp saldouplw_r.cpp 
	uplztfw.cpp 
	uplnastw.cpp uplnomdokw.cpp 
	upl_ovdt.cpp upl_ovdt_m.cpp upl_ovdt_r.cpp 
	upl_pavt.cpp upl_pavt_m.cpp upl_pavt_r.cpp 
	upl_rpavt.cpp upl_rpavt_m.cpp upl_rpavt_r.cpp 
	upl_plc_r.cpp 
	upl_ptw.cpp 
	upl_pvod.cpp upl_pvod_m.cpp upl_pvod_r.cpp 
	upl_shkg.cpp upl_shkg_m.cpp upl_shkg_r.cpp 
	upl_shvod.cpp upl_shvod_m.cpp upl_shvod_r.cpp 
	upl_zapvdokw.cpp 
	uplgodsw.cpp uplgodsw_r.cpp 
	zapkarw.cpp zapkartm.cpp zapvkrw.cpp)

add_executable(i_xupl ${SOURSE_BIN}) #сборка программы

target_link_libraries(i_xupl buh_g util_g sql ${MYSQLCLIENT_LIB} ${GTK3_LIBRARIES} ) # Линковка программы с библиотеками

#****************************************************************

#сборка подсистемы "Реестр налоговых накладных"
# Установка переменной со списком исходников
set(SOURSE_BIN i_xrnn.cpp 
	l_ukrvk.cpp l_ukrvk_p.cpp l_ukrvk_v.cpp 
	l_uosopp.cpp l_uosopp_p.cpp l_uosopp_v.cpp 
	l_uosopr.cpp l_uosopr_p.cpp l_uosopr_v.cpp 
	l_uslopp.cpp l_uslopp_p.cpp l_uslopp_v.cpp 
	l_uslopr.cpp l_uslopr_p.cpp l_uslopr_v.cpp 
	l_xrnngpn.cpp l_xrnngpn_p.cpp l_xrnngpn_v.cpp
	l_xrnngvn.cpp l_xrnngvn_p.cpp l_xrnngvn_v.cpp 
	l_xrnnp.cpp l_xrnnp_p.cpp l_xrnnp_v.cpp 
	l_xrnnv.cpp l_xrnnv_p.cpp l_xrnnv_v.cpp 
	poigod_rnn.cpp 
	rasrnn_pw.cpp rasrnn_p7w.cpp rasrnn_p8w.cpp rasrnn_vw.cpp rasrnn_v7w.cpp rasrnn_v8w.cpp rnn_vsw.cpp 
	ree_nastw.cpp 
	rnnovdw.cpp rnnrd5w.cpp rnnrd5w_5.cpp rnnrd5w_6.cpp rnnvvd.cpp 
	rnn_imp.cpp rnn_imp_m.cpp rnn_imp_ovd.cpp rnn_imp_r1.cpp rnn_imp_r2.cpp 
	rnn_rkontrw.cpp rnn_rkontrw_m.cpp rnn_rkontrw_r.cpp 
	rpvnn_piw.cpp rpvnn_ps_pw.cpp rpvnn_viw.cpp 
	sumzap1w.cpp sumzap1uw.cpp sumzapuosw.cpp 
	xml_nn_matw.cpp xml_nn_ukrw.cpp xml_nn_uosw.cpp xml_nn_uslw.cpp 
	xrnn_kopd.cpp xrnn_kord.cpp 
	xrnn_rasp.cpp xrnn_rasp_m.cpp xrnn_rasp_r.cpp 
	xrnn_uzzp.cpp xrnn_uzzp_m.cpp 
	zapvree1w.cpp zapvreew.cpp)

add_executable(i_xrnn ${SOURSE_BIN}) #сборка программы

target_link_libraries(i_xrnn buh_g util_g sql ${MYSQLCLIENT_LIB} ${GTK3_LIBRARIES} ) # Линковка программы с библиотеками

#****************************************************************

#сборка подсистемы "Учёт доверенностей"
# Установка переменной со списком исходников
set(SOURSE_BIN i_xud.cpp 
	l_dover.cpp l_dover_v.cpp l_dovers.cpp l_dovers_p.cpp 
	l_poixuddok.cpp 
	l_sptbn1.cpp l_sptbn1_p.cpp 
	poixuddokw.cpp poigod_ud.cpp 
	udvhdw.cpp udgodsw.cpp udgodsw_r.cpp 
	xudrasdokw.cpp)

add_executable(i_xud ${SOURSE_BIN}) #сборка программы

target_link_libraries(i_xud buh_g util_g sql ${MYSQLCLIENT_LIB} ${GTK3_LIBRARIES} ) # Линковка программы с библиотеками

#****************************************************************

#записываем программы в каталог откуда они будут запускаться
install(TARGETS i_admin i_start i_xbu i_xdk i_xkas i_xmu i_xrnn i_xud i_xukr i_xuos i_xupl i_xusl i_xzp DESTINATION /usr/local/bin)
