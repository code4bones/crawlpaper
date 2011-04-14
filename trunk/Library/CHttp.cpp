/*
	CHttp.cpp
	Classi per la gestione del protocollo http.
	Luca Piergentili, 05/10/00
	lpiergentili@yahoo.com
*/
#include "env.h"
#include "pragma.h"
#include "macro.h"
#include "window.h"
#include "win32api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strings.h"
#include <ctype.h>
#include "CNodeList.h"
#include "CThread.h"
#include "CTextFile.h"
#include "CDateTime.h"
#include "CFindFile.h"
#include "CRegistry.h"
#include "CSock.h"
#include "CHttp.h"
#include "CUrl.h"

#include "traceexpr.h"
//#define _TRACE_FLAG	_TRFLAG_TRACEOUTPUT
//#define _TRACE_FLAG	_TRFLAG_NOTRACE
#define _TRACE_FLAG		_TRFLAG_NOTRACE
#define _TRACE_FLAG_INFO	_TRFLAG_NOTRACE
#define _TRACE_FLAG_WARN	_TRFLAG_NOTRACE
#define _TRACE_FLAG_ERR	_TRFLAG_NOTRACE

#if (defined(_DEBUG) && defined(_WINDOWS)) && (defined(_AFX) || defined(_AFXDLL))
#ifdef PRAGMA_MESSAGE_VERBOSE
  #pragma message("\t\t\t"__FILE__"("STR(__LINE__)"): using DEBUG_NEW macro")
#endif
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

/*
app/gg									gg
application/avast-aswcs						aswcs
application/avast-asws						asws
application/copernic-csk						csk
application/dia							dia
application/earthviewer						eta
application/envoy							evy
application/epub+zip						epub
application/file-mirror-list					fml
application/fractals						fif
application/fractals						fif
application/futuresplash						spl
application/hta							hta
application/ibe							ibe
application/internet-property-stream			acx
application/iti							iti
application/mac-binhex40						hqx
application/mac-binhex40						hqx
application/ms-infopath.xml					infopathxml
application/ms-vsi							vsi
application/msaccess						accda,accdb,accdc,accde,accdr,accdt,ade,adp,mda,mdb,mde,snp
application/msaccess.addin					accda
application/msaccess.cab						accdc
application/msaccess.exec					accde
application/msaccess.ftemplate				accft
application/msaccess.runtime					accdr
application/msaccess.template					accdt
application/msaccess.webapplication			accdw
application/msonenote						one
application/msonenote						onepkg
application/msword							doc,dot,rtf,wbk,wiz
application/msworks							wps,wpt
application/nmwb							nmw
application/octet-stream						hxd,hxh,hxi,hxq,hxr,hxs,hxw
;application/octet-stream					*,bin,class,dms,exe,lha,lzh
application/oda							oda
application/olescript						axs
application/opensearchdescription+xml			osdx
application/pbautomation						aut
application/pdf							pdf
application/photobubble						bub
application/pics-rules						prf
application/pkcs10							p10
application/pkcs7-mime						p7c
application/pkcs7-mime						p7m
application/pkcs7-signature					p7s
application/pkix-cert						cer,crt,der
application/pkix-crl						crl
application/postscript						ai,eps,ps
application/powershell						psc1
application/rat-file						rat
application/rat-file						ratfile
application/rtf							rtf
application/schdpl32						sc2,scd,sch
application/sdp							sdp
application/set-payment-initiation				setpay
application/set-registration-initiation			setreg
application/sha							cbo
application/smil							smi
application/smil							smil
application/streamingmedia					ssm
application/vnd.adobe.acrobat-security-settings	acrobatsecuritysettings
application/vnd.adobe.adept+xml				acsm
application/vnd.adobe.air-application-installer-package+zip	air
application/vnd.adobe.edn					edn
application/vnd.adobe.pdfxml					pdfxml
application/vnd.adobe.pdx					pdx
application/vnd.adobe.rmf					rmf
application/vnd.adobe.workflow				awf
application/vnd.adobe.xdp+xml					xdp,xfd
application/vnd.adobe.xfdf					xfdf
application/vnd.belarc-bci					bci
application/vnd.belarc-cf					bcf
application/vnd.dpgraph						dpg,dpgraph,mwc
application/vnd.fdf							fdf
application/vnd.google-earth.kml+xml			kml
application/vnd.google-earth.kmz				kmz
application/vnd.groove-injector				grv
application/vnd.groove-vcard					vcg
application/vnd.hp.com.cpxe-org.upload.mpv+xml	mpvuhpcp
application/vnd.hpphotogallery.md-launch		hpphotogallery
application/vnd.intu.qfx						qfx
application/vnd.mindjet.mindmanager			mmap,mmas,mmat,mmmp,mmms,xmmap,xmmas,xmmat,xmmmp
application/vnd.ms-artgalry					cil
application/vnd.ms-excel						xla,xlc,xlm,xls,xlt,xlw,csv,dif,slk,xlb,xld,xlk,xll,xlv
application/vnd.ms-excel.addin.macroenabled.12	xlam
application/vnd.ms-excel.sheet.binary.macroenabled.12	xlsb
application/vnd.ms-excel.sheet.macroenabled.12	xlsm
application/vnd.ms-excel.template.macroenabled.12	xltm
application/vnd.ms-mediapackage				mpf
application/vnd.ms-officetheme				thmx
application/vnd.ms-outlook					msg
application/vnd.ms-pki.certstore				sst
application/vnd.ms-pki.pko					pko
application/vnd.ms-pki.seccat					cat
application/vnd.ms-pki.stl					stl
application/vnd.ms-pkicertstore				sst
application/vnd.ms-pkiseccat					cat
application/vnd.ms-pkistl					stl
application/vnd.ms-powerpoint					pot,pps,ppt,ppa,pwz
application/vnd.ms-powerpoint.addin.macroenabled.12		ppam
application/vnd.ms-powerpoint.presentation.macroenabled.12	pptm
application/vnd.ms-powerpoint.slide.macroenabled.12		sldm
application/vnd.ms-powerpoint.slideshow.macroenabled.12	ppsm
application/vnd.ms-powerpoint.template.macroenabled.12		potm
application/vnd.ms-project					mpp,mpd,mpp,mpt,mpw,mpx
application/vnd.ms-publisher					ols,pub
application/vnd.ms-visio.viewer				vdx,vsd,vss,vst,vsx,vtx
application/vnd.ms-word.document.macroenabled.12	docm,dotm
application/vnd.ms-works						wcm,wdb,wks,wps
application/vnd.ms-wpl						wpl
application/vnd.ms-xpsdocument				xps
application/vnd.oasis.opendocument.formula		odf
application/vnd.oasis.opendocument.graphics-template		otg
application/vnd.oasis.opendocument.graphics				odg
application/vnd.oasis.opendocument.presentation-template	otp
application/vnd.oasis.opendocument.presentation			odp
application/vnd.oasis.opendocument.spreadsheet-template	ots
application/vnd.oasis.opendocument.spreadsheet	ods
application/vnd.oasis.opendocument.text-master	odm
application/vnd.oasis.opendocument.text-template	ott
application/vnd.oasis.opendocument.text-web		oth
application/vnd.oasis.opendocument.text			odt
application/vnd.openofficeorg.extension			oxt
application/vnd.openxmlformats-officedocument.presentationml.presentation	pptx
application/vnd.openxmlformats-officedocument.presentationml.slide		sldx
application/vnd.openxmlformats-officedocument.presentationml.slideshow	ppsx
application/vnd.openxmlformats-officedocument.presentationml.template		potx
application/vnd.openxmlformats-officedocument.spreadsheetml.sheet		xlsx
application/vnd.openxmlformats-officedocument.spreadsheetml.template		xltx
application/vnd.openxmlformats-officedocument.wordprocessingml.document	docx
application/vnd.openxmlformats-officedocument.wordprocessingml.template	dotx
application/vnd.optx-screenwatch				sc
application/vnd.rn-objects					rob
application/vnd.rn-realmedia-secure			rms
application/vnd.rn-realmedia-vbr				rmvb
application/vnd.rn-realmedia					rm
application/vnd.rn-realplayer					rnx
application/vnd.rn-realsystem-rjs				rjs
application/vnd.rn-realsystem-rjt				rjt
application/vnd.rn-realsystem-rmj				rmj
application/vnd.rn-realsystem-rmx				rmx
application/vnd.rn-rn_music_package			rmp
application/vnd.rn-rsml						rsml
application/vnd.spinnerplus					spn
application/vnd.stardivision.calc				sdc
application/vnd.stardivision.chart				sds
application/vnd.stardivision.draw				sda
application/vnd.stardivision.impress-packed		sdp
application/vnd.stardivision.impress			sdd
application/vnd.stardivision.math				smf
application/vnd.stardivision.writer-global		sgl
application/vnd.stardivision.writer			sdw
application/vnd.stardivision.writer			vor
application/vnd.sun.xml.base					odb
application/vnd.sun.xml.calc					sxc
application/vnd.sun.xml.calc.template			stc
application/vnd.sun.xml.draw					sxd
application/vnd.sun.xml.draw.template			std
application/vnd.sun.xml.impress				sxi
application/vnd.sun.xml.impress.template		sti
application/vnd.sun.xml.math					sxm
application/vnd.sun.xml.writer				sxw
application/vnd.sun.xml.writer.global	sxg
application/vnd.sun.xml.writer.template	stw
application/vnd.visio	vdx
application/vnd.visio	vsd
application/vnd.visio	vsl
application/vnd.visio	vss
application/vnd.visio	vst
application/vnd.visio	vsu
application/vnd.visio	vsw
application/vnd.visio	vsx
application/vnd.visio	vtx
application/vnd.visio.webdrawing	vdw
application/vnd.xmind.media-package	xmp
application/vnd.xmind.template	xmt
application/vnd.xmind.workbook	xmind
application/vnd.xmind.xmap	xmap
application/wbload	wba
application/windows-library+xml	library-ms
application/windows-search-connector+xml	searchconnector-ms
application/winhlp	hlp
application/wlmoviemaker	wlmp
application/x-aa0data	aaa
application/x-asp	asp
application/x-att-a2bmusic-purchase	med
application/x-att-a2bmusic	mes
application/x-authorware-bin	aab
application/x-authorware-map	aam
application/x-bcpio	bcpio
application/x-bittorrent	torrent
application/x-bittorrentsearchdescription+xml	btsearch
application/x-blogthis	blogthis
application/x-bridge-url	adobebridge
application/x-bwpreview	bwp
application/x-cdf	cdf
application/x-cdf	cdf
application/x-cdk2kaudio	a2k
application/x-cdk2kvideo	v2k
application/x-cdkgameinstall	g2k
application/x-cfc	cfc
application/x-cfm	cfm
application/x-cnet-vsl	vsl
application/x-compress	z
application/x-compress	z
application/x-compressed	tgz
application/x-compressed	ace
application/x-compressed	ARJ
application/x-compressed	gz
application/x-compressed	lha
application/x-compressed	LZH
application/x-compressed	pmf
application/x-compressed	tar
application/x-compressed	tgz
application/x-compressed	UU
application/x-compressed	UUE
application/x-compressed	XXE
application/x-cpio	cpio
application/x-csh	csh
application/x-css	css
application/x-director	dcr,dir,dxr
application/x-director	dcr
application/x-director	dir
application/x-director	dxr
application/x-dvi	dvi
application/x-ebx	etd
application/x-emms-content	emm
application/x-fiddler-session-archive	fiddlerarchivezip
application/x-fiddler-session-archive	saz
application/x-gom-skin	gps
application/x-gtar	gtar
application/x-gzip	gz
application/x-gzip	gz
application/x-hdf	hdf
application/x-iad	iad
application/x-ica	ica
application/x-icq-hpf	hpf
application/x-icq-pnq	pnq
application/x-icq-scm	scm
application/x-icq	uin
application/x-incredimail-license	imk
application/x-incredimail	ima
application/x-incredimail	imc
application/x-incredimail	ime
application/x-incredimail	imf
application/x-incredimail	imi
application/x-incredimail	imn
application/x-incredimail	ims
application/x-incredimail	imw
application/x-internet-signup	ins,isp
application/x-internet-signup	ins
application/x-internet-signup	isp
application/x-iphone	iii
application/x-iphone	iii
application/x-ipix	ipx
application/x-ipscript	ips
application/x-itaxirc-data	ntw
application/x-itaxirc-data	ntx
application/x-itaxirc-data	nty
application/x-itunes-ipa	ipa
application/x-itunes-ipg	ipg
application/x-itunes-ipsw	ipsw
application/x-itunes-itb	itb
application/x-itunes-ite	ite
application/x-itunes-itlp	itlp
application/x-itunes-itls	itls
application/x-itunes-itms	itms
application/x-itunes-itpc	itpc
application/x-java-jnlp-file	jnlp
application/x-javascript	js
application/x-javascript	ls
application/x-javascript	mocha
application/x-jiafilm-download	jiaf
application/x-jiafilm-vod	jias
application/x-js	js
application/x-jtx+xps	jtx
application/x-laplayer-reg	lar
application/x-latex	latex
application/x-letter-creator	flw
application/x-letter-creator	ltw
application/x-lk-rlestream	lks
application/x-magnet	magnet
application/x-mmjb-bpp	bpp
application/x-mmjb-mmjb	mmjb
application/x-mmjb-mmo	mmo
application/x-mmjb-mmz	mmz
application/x-mmxp	mxp
application/x-mobipocket-ebook	prc
application/x-mobipocket-mobibook	mobi
application/x-mobipocket-mobipackage	mbpkg
application/x-mobipocket-subscription	enews
application/x-mpeg	amc
application/x-ms-application	application
application/x-ms-license	slupkg-ms
application/x-ms-vsto	vsto
application/x-ms-wmd	wmd
application/x-ms-wms	wms
application/x-ms-wmz	wmz
application/x-ms-xbap	xbap
application/x-msaccess	mdb
application/x-mscardfile	crd
application/x-msclip	clp
application/x-msdownload	dll
application/x-msdownload	dll
application/x-msdownload	exe
application/x-msexcel	xls
application/x-msmediaview	m13,m14,mvb
application/x-msmetafile	wmf
application/x-msmoney	mny
application/x-mspowerpoint	ppt
application/x-mspublisher	pub
application/x-msschedule	scd
application/x-msterminal	trm
application/x-mswrite	wri
application/x-musicmatch-mmjb5.20detect	mmjb_mime
application/x-netcdf	cdf,nc
application/x-netcdf	cdf
application/x-pando	pando
application/x-perfmon	pma,pmc,pml,pmr,pmw
application/x-perl	pl
application/x-php	php
application/x-php3	php3
application/x-php4	php4
application/x-picasa-detect	pinstall
application/x-pkcs12	p12,pfx
application/x-pkcs12	p12
application/x-pkcs12	pfx
application/x-pkcs7-certificates	p7b,spc
application/x-pkcs7-certreqresp	p7r
application/x-pkcs7-mime	p7c,p7m
application/x-pkcs7-signature	p7s
application/x-pn-npistream	npi
application/x-pn-virtualink	mbc
application/x-podcast	pcast
application/x-powercam-fs	fs
application/x-previewsystems-vbox-music	mbo
application/x-previewsystems-vbox-music	mbox
application/x-quicktimeplayer	qtl
application/x-quicktimeupdater	qup
application/x-rar-compressed	rar
application/x-rpt	rpt
application/x-rtsp	rtsp
application/x-safari-safariextz	safariextz
application/x-safari-webarchive	webarchive
application/x-server-launch	sdf
application/x-sh	sh
application/x-shar	shar
application/x-shockwave-flash	swf
application/x-shockwave-flash	mfp
application/x-shockwave-flash	swf
application/x-sibelius-score	sco
application/x-sibelius-score	sib
application/x-skype	skype
application/x-smb-directive	sma
application/x-smb-directive	smafile
application/x-stuffit	sit
application/x-sv4cpio	sv4cpio
application/x-sv4crc	sv4crc
application/x-tar	tar
application/x-tar	tar
application/x-tcl	tcl
application/x-tex	tex
application/x-texinfo	texi,texinfo
application/x-troff-man	man
application/x-troff-me	me
application/x-troff-ms	ms
application/x-troff	roff,t,tr
application/x-UltimateAction	ultact
application/x-url-list	urls
application/x-ustar	ustar
application/x-virtools	cmo
application/x-virtoolsplayer	vmo
application/x-vpeg	vpg
application/x-wais-source	src
application/x-webshots	wbc
application/x-webshots	wbp
application/x-webshots	wbz
application/x-wlpg-detect	wlpginstall
application/x-wlpg3-detect	wlpginstall3
application/x-wordperfect6	wpe
application/x-wordperfect6	wps
application/x-wpost	wpost
application/x-x509-ca-cert	cer,crt,der
application/x-xml	xml
application/x-zip-compressed	zip
application/xaml+xml	xaml
application/xbk	xbk
application/xensearch	xensearch
application/xhtml+xml	xht
application/xhtml+xml	xhtml
application/xml-dtd	dtd
application/xml	asa
application/xml	asax
application/xml	ascx
application/xml	ashx
application/xml	asmx
application/xml	aspx
application/xml	config
application/xml	coverage
application/xml	datasource
application/xml	hxa
application/xml	hxc
application/xml	hxe
application/xml	hxf
application/xml	hxk
application/xml	hxt
application/xml	hxv
application/xml	master
application/xml	orderedtest
application/xml	rdlc
application/xml	resx
application/xml	settings
application/xml	sitemap
application/xml	skin
application/xml	snippet
application/xml	svc
application/xml	testrunconfig
application/xml	trx
application/xml	vcproj
application/xml	vscontent
application/xml	vsmdi
application/xml	vspolicy
application/xml	vspolicydef
application/xml	wsdl
application/xml	xdr
application/xml	xmta
application/xml	xsc
application/xml	xsd
application/xml	xslt
application/xml	xss
application/xslic	xslic
application/xsoem	xsoem
application/xsupdate	xsupdate
application/xva	xva
application/ymsgr	ymg
application/ymsgr	yps
application/ynd.ms-pkipko	pko
application/zfx	zfx
application/zip	zip
application\x-intertrust-mpk	mpk
audio/aac	aac
audio/aac	adts
audio/ac3	ac3
audio/aiff	aif
audio/aiff	aifc
audio/aiff	aiff
audio/aiff	cdda
audio/amr	amr
audio/audible	aa
audio/audio/mjuice_voucher	mjv
audio/basic	au,snd
audio/basic	au
audio/basic	snd
audio/basic	ulw
audio/blue-matter-offer	bmo
audio/blue-matter-song	bmt
audio/evrc-qcp	evr
audio/evrc-qcp	evrc
audio/m4a	m4a
audio/m4b	m4b
audio/m4p	m4p
audio/mid	mid,rmi
audio/mid	mid
audio/mid	midi
audio/mid	rmi
audio/midi	mid
audio/midi	midi
audio/midi	rmi
audio/mp1	mp1
audio/mp3	mp3
audio/mpeg	mp3
audio/mpeg	mp1
audio/mpeg	mp2
audio/mpeg	mp3
audio/mpeg	mpga
audio/mpegurl	m3u
audio/mpegurl	xpl
audio/qcelp	qcp
audio/rn-mpeg	mpga
audio/scpls	pls
audio/scpls	xpl
audio/vnd.audible.aax	aax
audio/vnd.dlna.adts	adt
audio/vnd.rn-realaudio	ra
audio/vnd.rn-realmedia	rm
audio/vnd.rn-realpix	rp
audio/vnd.rn-realplayer	rnx
audio/wav	wav
audio/wav	wave
audio/x-aiff	aif,aifc,aiff
audio/x-caf	caf
audio/x-gsm	gsm
audio/x-la-lms	lmsff
audio/x-liquid-file	la1
audio/x-liquid-file	lqt
audio/x-liquid-secure	lavs
audio/x-liquid	lav
audio/x-m4a	m4a
audio/x-m4b	m4b
audio/x-m4p	m4p
audio/x-m4r	m4r
audio/x-matroska	mka
audio/x-mei-aac	acp
audio/x-mpegurl	m3u
audio/x-mpegurl	m3u
audio/x-mpegurl	m3u8
audio/x-ms-wax	wax
audio/x-ms-wma	wma
audio/x-pn-realaudio-plugin	rpm
audio/x-pn-realaudio	ra,ram
audio/x-pn-realaudio	ram
audio/x-pn-realaudio	rmm
audio/x-scpls	pls
audio/x-sd2	sd2
audio/x-vnd.AudioExplosion.MjuiceMediaFile	mjf
audio/x-wav	wav
audio/x-wav	wav
audio/x.pn-realaudio	ram
audio/x.pn-realaudio	rmm
i-world/i-vrml	ivr
icm.div6	divx
image/bmp	bmp
image/bmp	bmp
image/bmp	dib
image/cis-cod	cod
image/gif	gif
image/gif	gif
image/ief	ief
image/jpeg	jpe,jpeg,jpg
image/jpeg	jfif
image/jpeg	jpe
image/jpeg	jpeg
image/jpeg	jpg
image/pict	pct
image/pict	pic
image/pict	pict
image/pipeg	jfif
image/pjpeg	jfif
image/png	png
image/svg-xml	svg
image/svg-xml	svgz
image/svg+xml	svg
image/svg+xml	svg
image/svg+xml	svgz
image/targa	tga
image/tiff	tif,tiff
image/ufo	ufo
image/ufo	ufp
image/vnd.ms-modi	mdi
image/vnd.ms-photo	wdp
image/vnd.rn-realflash	rf
image/vnd.rn-realpix	rp
image/x-bmp	bmp
image/x-bmp	dib
image/x-cmu-raster	ras
image/x-cmx	cmx
image/x-icon	ico
image/x-icon	ico
image/x-jg	art
image/x-macpaint	mac
image/x-macpaint	pnt
image/x-macpaint	pntg
image/x-portable-anymap	pnm
image/x-portable-bitmap	pbm
image/x-portable-graymap	pgm
image/x-portable-pixmap	ppm
image/x-quicktime	qti
image/x-quicktime	qtif
image/x-rgb	rgb
image/x-sgi	sgi
image/x-sif	sif
image/x-sifz	sifz
image/x-targa	tga
image/x-xbitmap	xbm
image/x-xbitmap	fpx
image/x-xbitmap	mix
image/x-xbitmap	xbm
image/x-xpixmap	xpm
image/x-xwindowdump	xwd
interface/x-winamp-lang	wlz
interface/x-winamp-skin	wsz
interface/x-winamp-skin	wsz
interface/x-winamp3-skin	wal
java/*	class
java/*	java
message/rfc822	mht,mhtml,nws
message/rfc822	eml
message/rfc822	mht
message/rfc822	mhtml
message/rfc822	nws
message/rfc822	rss
model/vnd.dwf	dwf
model/vnd.dwf	dwffile
model/vnd.dwfx+xps	dwfx
model/vnd.easmx+xps	easmx
model/vnd.edrwx+xps	edrwx
model/vnd.eprtx+xps	eprtx
skp	skp
text/asa	asa
text/asp	asp
text/blue-matter-content-ref	bmr
text/calendar	ics
text/calendar	outlookfileics
text/css	css
text/css	css
text/dlm	dlm
text/h323	323
text/h323	323
text/html	htm,html,stm
text/html	htm
text/html	html
text/html	htx
text/html	plg
text/html	shtml
text/html	stm
text/html	vcl
text/html	zpa
text/iuls	uls
text/iuls	uls
text/java	java
text/ofc	ofc
text/ofx	ofx
text/pcif	pcif
text/plain	bas,c,h,txt
text/plain	actproj
text/plain	asm
text/plain	c
text/plain	cc
text/plain	cd
text/plain	cod
text/plain	cpp
text/plain	cs
text/plain	csdproj
text/plain	csproj
text/plain	cur
text/plain	cxx
text/plain	dbs
text/plain	dec
text/plain	def
text/plain	dsp
text/plain	dsw
text/plain	etp
text/plain	ext
text/plain	fky
text/plain	gs
text/plain	h
text/plain	hpp
text/plain	hxx
text/plain	i
text/plain	idl
text/plain	inc
text/plain	inl
text/plain	ipproj
text/plain	jsx
text/plain	jsxbin
text/plain	kci
text/plain	lgn
text/plain	log
text/plain	lst
text/plain	mak
text/plain	map
text/plain	mdp
text/plain	mk
text/plain	mmbas
text/plain	odh
text/plain	odl
text/plain	prc
text/plain	py
text/plain	pyw
text/plain	rc
text/plain	rc2
text/plain	rct
text/plain	rgs
text/plain	rte
text/plain	rul
text/plain	s
text/plain	sln
text/plain	sol
text/plain	sor
text/plain	sql
text/plain	srf
text/plain	tab
text/plain	tlh
text/plain	tli
text/plain	trg
text/plain	txt
text/plain	udf
text/plain	udt
text/plain	user
text/plain	usr
text/plain	vb
text/plain	vbdproj
text/plain	vbproj
text/plain	vddproj
text/plain	vdp
text/plain	vdproj
text/plain	viw
text/plain	vspscc
text/plain	vsscc
text/plain	vssscc
text/plain	xoml
text/qif	qif
text/qpg	qpg
text/richtext	rtx
text/scriptlet	sct
text/scriptlet	sct
text/scriptlet	wsc
text/tab-separated-values	tsv
text/vnd-ms.click2record+xml	c2r
text/vnd-ms.click2record+xml	mediacenterc2r
text/vnd.rn-realtext	rt
text/vnd.rn-realtext3d	r3t
text/webviewhtml	htt
text/webviewhtml	htt
text/x-audiosoft-intra	aip
text/x-component	htc
text/x-component	htc
text/x-html-insertion	qht
text/x-html-insertion	qhtm
text/x-ms-contact	contact
text/x-ms-group	group
text/x-ms-iqy	iqy
text/x-ms-odc	odc
text/x-ms-rqy	rqy
text/x-setext	etx
text/x-vcard	vcf
text/x-vcard	outlookfilevcf
text/x-vcard	vcf
text/x-voip	vcc
text/xml	act
text/xml	addin
text/xml	biz
text/xml	dcd
text/xml	disco
text/xml	dtsconfig
text/xml	fo
text/xml	rdf
text/xml	resx
text/xml	ssisdeploymentmanifest
text/xml	tdl
text/xml	vssettings
text/xml	vstemplate
text/xml	wme
text/xml	wpj
text/xml	wsdl
text/xml	xbrl
text/xml	xdr
text/xml	xml
text/xml	xrm-ms
text/xml	xsd
text/xml	xsl
text/xml	xslt
text\plain	sql
txt/plain	upt
video/3gp	3gp
video/3gpp	3g2
video/3gpp	3gp
video/3gpp	3gp2
video/3gpp	3gpp
video/3gpp	sdv
video/3gpp2	3g2
video/3gpp2	3gp2
video/avi	avi
video/avi	divx
video/divx	div
video/divx	divx
video/divx	divx_div_file
video/divx	divx_divx_file
video/divx	divx_mkv_file
video/divx	xvid
video/flc	cel
video/flc	flc
video/flc	fli
video/m4v	m4v
video/mp4	f4v
video/mp4	mp4
video/mp4	mp4v
video/mp4	mpg4
video/mpeg	mp2,mpa,mpe,mpeg,mpg,mpv2
video/mpeg	m1v
video/mpeg	m2p
video/mpeg	m2v
video/mpeg	mod
video/mpeg	mp2
video/mpeg	mp2v
video/mpeg	mp4v
video/mpeg	mpa
video/mpeg	mpe
video/mpeg	mpeg
video/mpeg	mpg
video/mpeg	mps
video/mpeg	mpv2
video/mpeg	mpv4
video/mpeg	vob
video/mpeg4	m4e
video/mpeg4	m4v
video/mpeg4	mp4
video/mpg	mpeg
video/mpg	mpg
video/mpg	mpv
video/quicktime	mov,qt
video/quicktime	hdmov
video/quicktime	mov
video/quicktime	mqv
video/quicktime	qt
video/vnd.dlna.mpeg-tts	m2t
video/vnd.dlna.mpeg-tts	m2ts
video/vnd.dlna.mpeg-tts	mts
video/vnd.dlna.mpeg-tts	tps
video/vnd.dlna.mpeg-tts	ts
video/vnd.dlna.mpeg-tts	tts
video/vnd.rn-realvideo	rmvb
video/vnd.rn-realvideo	rv
video/x-dv	dif
video/x-dv	dv
video/x-f4v	f4v
video/x-flv	flv
video/x-ivf	IVF
video/x-la-asf	lsf,lsx
video/x-la-asf	lsf
video/x-la-asf	lsx
video/x-m4v	m4v
video/x-matroska	mkv
video/x-mpeg	mps
video/x-ms-asf	asf,asr,asx
video/x-ms-asf	asf
video/x-ms-asf	asx
video/x-ms-asx	asx
video/x-ms-wm	wm
video/x-ms-wmp	wmp
video/x-ms-wmv	wmv
video/x-ms-wmx	wmx
video/x-ms-wvx	wvx
video/x-msvideo	avi
video/x-msvideo	avi
video/x-msvideo	vfw
video/x-ogg	ogg
video/x-ogm	ogm
video/x-sgi-movie	movie
video/x-sgi-movie	movie
video/x-webm	webm
vxd/x-ms-wsx	wsx
wink project	wnk
x-world/x-svr						svr
x-world/x-svr						vrt
x-world/x-svr						xvr
x-world/x-vrml						flr,vrml,wrl,wrz,xaf,xof
*/

/*
	HTTP_ERROR_CODE
*/
struct HTTP_ERROR_CODE {
	int		code;
	char*	desc;
};

static const HTTP_ERROR_CODE http_error_code[] = {
	HTTP_STATUS_VOID,					"undefined error",				// 0

	HTTP_STATUS_SUCCESSFUL,				"successful",					// 299
	// successful 2xx 
	HTTP_STATUS_OK,					"OK",						// 200
	HTTP_STATUS_CREATED,				"Created",					// 201
	HTTP_STATUS_ACCEPTED,				"Accepted",					// 202
	HTTP_STATUS_NON_AUTH_INFO,			"Non-Authoritative Information",	// 203
	HTTP_STATUS_NO_CONTENT,				"No Content",					// 204
	HTTP_STATUS_RESET_CONTENT,			"Reset Content",				// 205
	HTTP_STATUS_PARTIAL_CONTENTS,			"Partial Content",				// 206

	HTTP_STATUS_REDIRECTION,				"Redirection",					// 399
	// redirection 3xx
	HTTP_STATUS_MULTIPLE_CHOICES,			"Multiple Choices",				// 300
	HTTP_STATUS_MOVED_PERMANENTLY,		"Moved Permanently",			// 301
	HTTP_STATUS_MOVED_TEMPORARILY,		"Moved Temporarily",			// 302
	HTTP_STATUS_SEE_OTHER,				"See Other",					// 303
	HTTP_STATUS_NOT_MODIFIED,			"Not Modified",				// 304
	HTTP_STATUS_USE_PROXY,				"Use Proxy",					// 305
	HTTP_STATUS_TEMPORARY_REDIRECT,		"Temporary Redirect",			// 307

	HTTP_STATUS_CLIENT_ERROR,			"Client Error",				// 499
	// client error 4xx
	HTTP_STATUS_BAD_REQUEST,				"Bad Request",					// 400
	HTTP_STATUS_UNAUTHORIZED,			"Unauthorized",				// 401,
	HTTP_STATUS_PAYMENT_REQUIRED,			"Payment Required",				// 402,
	HTTP_STATUS_FORBIDDEN,				"Forbidden",					// 403,
	HTTP_STATUS_NOT_FOUND,				"Not Found",					// 404,
	HTTP_STATUS_METHOD_NOT_ALLOWED,		"Method Not Allowed",			// 405,
	HTTP_STATUS_NOT_ACCEPTABLE,			"Not Acceptable",				// 406,
	HTTP_STATUS_PROXY_AUTH_REQUIRED,		"Proxy Authentication Required",	// 407,
	HTTP_STATUS_REQUEST_TIMEOUT,			"Request Timeout",				// 408,
	HTTP_STATUS_CONFLICT,				"Conflict",					// 409,
	HTTP_STATUS_GONE,					"Gone",						// 410,
	
	HTTP_STATUS_LENGTH_REQUIRED,			"Length Required",				// 411,
	HTTP_STATUS_PRECONDITION_FAILED,		"Precondition Failed",			// 412,
	HTTP_STATUS_REQUEST_TOO_LARGE,		"Request Entity Too Large",		// 413,
	HTTP_STATUS_REQUEST_TOO_LONG,			"Request-URI Too Long",			// 414,
	HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE,	"Unsupported Media Type",		// 415,
	HTTP_STATUS_RANGE_NOT_SATISFIABLE,		"Requested Range Not Satisfiable",	// 416,
	HTTP_STATUS_EXPECTATION_FAILED,		"Expectation Failed",			// 417,

	HTTP_STATUS_SERVER_ERROR,			"Server Error",				// 599,
	// server error 5xx
	HTTP_STATUS_INTERNAL,				"Internal Error",				// 500,
	HTTP_STATUS_NOT_IMPLEMENTED,			"Not Implemented",				// 501,
	HTTP_STATUS_BAD_GATEWAY,				"Bad Gateway",					// 502,
	HTTP_STATUS_SERVICE_UNAVAILABLE,		"Service Unavailable",			// 503,
	HTTP_STATUS_GATEWAY_TIMEOUT,			"Gateway Timeout",				// 504,
	HTTP_STATUS_VERSION_NOT_SUPPORTED,		"Version Not Supported",			// 505,
	HTTP_STATUS_PERMISSION_DENIED,		"Permission Denied",			// 550,

	HTTP_STATUS_SERVER_INTERNAL_ERROR,		"Server Internal Error",			// 899,
	// interni
	HTTP_STATUS_BELOW_MIN_SIZE,			"Below Min Size",				// 900,
	HTTP_STATUS_ABOVE_MAX_SIZE,			"Above Max Size",				// 901,
	HTTP_STATUS_UNABLE_TO_CREATE_LOCAL_FILE,"Unable to create local file",	// 902,
	HTTP_STATUS_EMPTY_DIRECTORY,			"Empty directory",				// 903,
	
	// lasciare come ultimo
	HTTP_STATUS_UNKNOW,					"unknown error"				// 1001
};

static	LPCSTR	GetHttpCodeDesc	(UINT nCode);
static	UINT		ListenThread		(LPVOID lpVoid);
static	UINT		HttpThread		(LPVOID lpVoid);

/*
	GetHttpCodeDesc()

	Ricava la descrizione (stringa) relativa al codice di ritorno http (numerico).
*/
LPCSTR GetHttpCodeDesc(UINT nCode)
{
	int i = 0;

	for(i=0; http_error_code[i].code!=HTTP_STATUS_UNKNOW; i++)
		if((UINT)http_error_code[i].code==nCode)
			break;

	return(http_error_code[i].desc);
}

/*
	ListenThread()

	Thread per la gestione dei collegamenti sul socket d'ascolto.
	Entra in un ciclo (infinito) accettando i collegamenti sul socket in ascolto. Per ogni nuova
	connessione crea un nuovo oggetto nella lista dei socket collegati e lancia il thread che si
	occupa della gestione del colloquio http.
	Esce dal ciclo quando il socket d'ascolto viene chiuso. La sincronizzazione col chiamante che
	ha chiuso il socket viene effettuata tramite l'evento ricevuto con la struttura per i parametri.
*/
UINT ListenThread(LPVOID lpVoid)
{
	CSock* pSocket = NULL;
	CConnectedHttpSocket* pConnectedSocket = NULL;

	// parametri ricevuti dal chiamante (CListeningHttpSocket, la classe per la gestione del servizio)
	// ricava il socket d'ascolto su cui effettuare la accept() (ossia il puntatore alla classe di cui
	// sopra) e l'evento per la sincronizzazione
	HTTP_THREAD_PARAMS* pParams = (HTTP_THREAD_PARAMS*)lpVoid;
	CListeningHttpSocket* pListeningHttpSocket = pParams ? pParams->pListenSocket : NULL;
	HANDLE hEvent = pParams ? pParams->hEvent : NULL;

	// cicla fino a che il chiamante non chiude il socket
	if(pListeningHttpSocket && hEvent)
	{
		do
		{
			// accetta i collegamenti (versione bloccante)
			if((pSocket = pListeningHttpSocket->GetSocket()->Accept())!=(CSock*)NULL)
			{
				// blocca l'accesso alla lista dei socket in collegamento per inserire il nuovo socket
				if(pListeningHttpSocket->Lock())
				{
					// crea un nuovo oggetto per il socket in collegamento e lo inserisce nella lista (deve creare
					// un nuovo socket perche' Accept() restituisce un CSock* mentre per l'oggetto in collegamento
					// viene usato un CConnectedHttpSocket*
					if((pConnectedSocket = pListeningHttpSocket->Insert(pSocket))!=(CConnectedHttpSocket*)NULL)
					{
						// crea il thread per la gestione del collegamento HTTP passando l'handle del socket collegato,
						// il puntatore alla classe per la lista dei socket in collegamento e l'evento per la sincronizzazione
						HTTP_THREAD_PARAMS* pParams = new HTTP_THREAD_PARAMS;
						if(pParams)
						{
							memset(pParams,'\0',sizeof(HTTP_THREAD_PARAMS));
							pParams->hConnectedSocket = pConnectedSocket->GetSocket()->GetHandle();
							pParams->pListenSocket = pListeningHttpSocket;
							pParams->hEvent = hEvent;

							// crea il thread per la gestione del colloquio http
							CThread* pThread = ::BeginThread(	HttpThread,
														(LPVOID)pParams,
														THREAD_PRIORITY_HIGHEST,
														0,
														CREATE_SUSPENDED,
														NULL
														);
							
							// imposta i parametri per la supervisione finale (quando la lista verra' distrutta)
							pConnectedSocket->SetThread(pThread);

							// ricava il puntatore all'oggetto per la richiesta http per impostare l'handle e l'ip relativi al socket
							pConnectedSocket->ResetHttpRequest();
							HTTPREQUEST* pHttpRequest = pConnectedSocket->GetHttpRequest();
							if(pThread)
							{
								pHttpRequest->socketinfo.socket = pConnectedSocket->GetSocket()->GetHandle();
								strcpyn(pHttpRequest->socketinfo.ip,pConnectedSocket->GetSocket()->GetIPAddress(),IP_ADDRESS_SIZE+1);
							}
							else
							{
								delete pParams;
								pHttpRequest->socketinfo.socket = INVALID_SOCKET;
								memset(pHttpRequest->socketinfo.ip,'\0',IP_ADDRESS_SIZE+1);
							}
						}
					}

					// sblocca l'accesso alla lista
					pListeningHttpSocket->Unlock();

					// attiva il thread
					if(pConnectedSocket)
						if(pConnectedSocket->GetThread() && pConnectedSocket->GetThread()->GetHandle()!=INVALID_HANDLE_VALUE)
							pConnectedSocket->GetThread()->Resume();
				}
			}
		} while(pSocket);
	}

	// per la sincronizzazione con il chiamante
	if(hEvent)
		::WaitForSingleObject(hEvent,INFINITE);

	return(0L);
}

/*
	HttpThread()

	Thread per la gestione del colloquio http.
	Cerca nella lista l'identificativo del socket relativo all'oggetto ed esegue il codice per la
	gestione del collegamento.
*/
UINT HttpThread(LPVOID lpVoid)
{
	// ricava i parametri passati in input (lista dei socket in collegamento, handle
	// del socket collegato e handle dell'evento per la sincronizzazione)
	HTTP_THREAD_PARAMS* pParams = (HTTP_THREAD_PARAMS*)lpVoid;
	CListeningHttpSocket* pListeningHttpSocket = pParams ? pParams->pListenSocket : NULL;
	SOCKET hSocket = pParams ? pParams->hConnectedSocket : INVALID_SOCKET;
	HANDLE hEvent = pParams ? pParams->hEvent : NULL;
	
	if(pListeningHttpSocket)
	{
		CConnectedHttpSocket* pConnectedSocket = (CConnectedHttpSocket*)NULL;
		
		// blocca l'accesso alla lista dei socket in collegamento per cercare il socket
		if(pListeningHttpSocket->Lock())
		{
			// cerca nella lista il socket che ha generato l'evento
			pConnectedSocket = pListeningHttpSocket->FindSocket(hSocket);

			// sblocca l'accesso alla lista
			pListeningHttpSocket->Unlock();
		}

		// se ha trovato l'oggetto relativo al socket
		if(pConnectedSocket)
		{
			// esegue il colloquio http
			pConnectedSocket->DoHttpConnection(hEvent);			

			// blocca l'accesso alla lista dei socket in collegamento per eliminare il socket
			if(pListeningHttpSocket->Lock())
			{
				// elimina l'oggetto dalla lista
				pListeningHttpSocket->Remove(pConnectedSocket->GetSocket()->GetHandle());

				// sblocca l'accesso alla lista
				pListeningHttpSocket->Unlock();
			}
		}

		delete pParams;
	}

	return(0L);
}

/*
	CConnectedHttpSocket()

	Classe per la gestione del colloquio con i socket in collegamento (lato server).
	La classe definisce le funzioni necessarie per la gestione del colloquio http con i socket che si collegano
	al socket in ascolto sul lato server (implementato tramite la CListeningHttpSocket).
*/
CConnectedHttpSocket::CConnectedHttpSocket(HTTPSERVICE* pHttpService)
{
	m_pHttpService = pHttpService;
	ResetHttpRequest();
	m_hEvent = NULL;
}

/*
	ResetHttpRequest()

	Azzera la struttura per la richiesta http.
*/
void CConnectedHttpSocket::ResetHttpRequest(void)
{
	m_HttpRequest.start = m_HttpRequest.end = 0L;
	memset(&m_HttpRequest.received,'\0',sizeof(m_HttpRequest.received));
	memset(&m_HttpRequest.request,'\0',sizeof(m_HttpRequest.request));
	m_HttpRequest.requestlength = 0;
	memset(&m_HttpRequest.command,'\0',sizeof(m_HttpRequest.command));
	memset(&m_HttpRequest.url,'\0',sizeof(m_HttpRequest.url));
	memset(&m_HttpRequest.version,'\0',sizeof(m_HttpRequest.version));
	memset(&m_HttpRequest.referer,'\0',sizeof(m_HttpRequest.referer));
	memset(&m_HttpRequest.useragent,'\0',sizeof(m_HttpRequest.useragent));
	memset(&m_HttpRequest.filename,'\0',sizeof(m_HttpRequest.filename));
	memset(&m_HttpRequest.ifmodifiedsince,'\0',sizeof(m_HttpRequest.ifmodifiedsince));
	m_HttpRequest.ifmodifiedlength = 0L;
	memset(&m_HttpRequest.lastmodified,'\0',sizeof(m_HttpRequest.lastmodified));
	memset(&m_HttpRequest.contenttype,'\0',sizeof(m_HttpRequest.contenttype));
	m_HttpRequest.contentlen = 0L;
	memset(&m_HttpRequest.cookiename,'\0',sizeof(m_HttpRequest.cookiename));
	memset(&m_HttpRequest.cookievalue,'\0',sizeof(m_HttpRequest.cookievalue));
	memset(&m_HttpRequest.response,'\0',sizeof(m_HttpRequest.response));
	m_HttpRequest.code = 0;
	memset(&m_HttpRequest.htmlfile,'\0',sizeof(m_HttpRequest.htmlfile));
	memset(&m_HttpRequest.socketinfo,'\0',sizeof(SOCKETINFO));
}

/*
	DoHttpConnection()

	Si incarica di gestire la connessione http con il client (lato server).
*/
BOOL CConnectedHttpSocket::DoHttpConnection(HANDLE hEvent)
{
	int nHttpCode = HTTP_STATUS_OK;
				
	// imposta l'evento usato per sincronizzare l'invio (I/O pending)
	m_hEvent = hEvent;

	// imposta la data/ora inizio collegamento
	m_HttpRequest.start = ::GetTickCount();
	m_HttpRequest.end = 0L;
	
	CDateTime dateTime(GMT_SHORT);
	_snprintf(
			m_HttpRequest.received,
			HTTP_MAX_CONTENT,
			"%s",
			dateTime.GetFormattedDate(TRUE)
			);

	// riceve la richiesta dal socket in collegamento
	if((m_HttpRequest.requestlength = GetSocket()->Receive(m_HttpRequest.request,HTTP_MAX_REQUEST)) > 0)
	{
		// analizza l'header della richiesta
		if((nHttpCode = ParseHeader())==HTTP_STATUS_OK)
		{
			// e' stato richiesto il listato di una directory
			if(*(m_HttpRequest.filename + (strlen(m_HttpRequest.filename)-1))=='\\')
			{
				BOOL bHaveDefaultHtmlFile = FALSE;
				char szDefaultHtmlFile[_MAX_PATH+1];
				char szDefaultHtmlFilePath[_MAX_PATH+1];
				strcpyn(szDefaultHtmlFile,m_pHttpService->defaulthtml,sizeof(szDefaultHtmlFile));
				if(strchr(szDefaultHtmlFile,';'))
				{
					char szDefaultHtml[_MAX_PATH+1];
					strcpyn(szDefaultHtml,szDefaultHtmlFile,sizeof(szDefaultHtml));
					char* token = strtok(szDefaultHtml,";");
					while(token)
					{
						strcpyn(szDefaultHtmlFile,token,sizeof(szDefaultHtmlFile));
						_snprintf(szDefaultHtmlFilePath,sizeof(szDefaultHtmlFilePath)-1,"%s%s",m_HttpRequest.filename,szDefaultHtmlFile);
						if((bHaveDefaultHtmlFile = ::FileExist(szDefaultHtmlFilePath))==TRUE)
						{
							int n = strlen(m_HttpRequest.filename);
							_snprintf(m_HttpRequest.filename+n,sizeof(m_HttpRequest.filename)-1-n,"%s",szDefaultHtmlFile);
							break;
						}
						token = strtok((char*)NULL,";");
					}
				}
				else
				{
					_snprintf(szDefaultHtmlFilePath,sizeof(szDefaultHtmlFilePath)-1,"%s%s",m_HttpRequest.filename,szDefaultHtmlFile);
					if((bHaveDefaultHtmlFile = ::FileExist(szDefaultHtmlFilePath))==TRUE)
					{
						int n = strlen(m_HttpRequest.filename);
						_snprintf(m_HttpRequest.filename+n,sizeof(m_HttpRequest.filename)-1-n,"%s",szDefaultHtmlFile);
					}
				}

				if(m_pHttpService->flags & HTTPSERVICE_FLAG_DIRECTORY_LISTING)
				{
					if(!bHaveDefaultHtmlFile)
						nHttpCode = CreateDirectoryListing();
				}
				else
				{
					if(!bHaveDefaultHtmlFile)
						nHttpCode = HTTP_STATUS_UNAUTHORIZED;
				}
			}

			// imposta le contents relative al tipo file richiesto
			if(nHttpCode==HTTP_STATUS_OK)
				nHttpCode = SetFileContents();
		}
	}
	else
		nHttpCode = HTTP_STATUS_BAD_REQUEST;

	// prepara la risposta da inviare al socket in collegamento
	PrepareResponse(nHttpCode);

	// invia la risposta
	SendResponse();

	// imposta l'ora fine collegamento
	m_HttpRequest.end = ::GetTickCount();

	// log della connessione http
	WriteLog();
	
	return(TRUE);
}

/*
	ParseHeader()

	Analizza l'header http ricevuto, impostando di conseguenza i campi della richiesta.
*/
UINT CConnectedHttpSocket::ParseHeader(void)
{
	CUrl url;
	char* p;
	int i,n,len;

	// richiesta vuota
	if(m_HttpRequest.request[0]=='\0')
		return(HTTP_STATUS_BAD_REQUEST);

	// versione http
	p = stristr(m_HttpRequest.request," HTTP/");
	len = 0;
	if(p)
	{
		p++;
		len = p - m_HttpRequest.request;
		for(i=0; i < HTTP_MAX_CONTENT && (*p && *p!=' ' && *p!='\r' && *p!='\n'); i++,p++)
			m_HttpRequest.version[i] = *p;
	}
	
	if(len <= 0 || m_HttpRequest.version[0]=='\0')
		return(HTTP_STATUS_BAD_REQUEST);

	// comando
	for(i=0; i < HTTP_MAX_COMMAND && !isspace(m_HttpRequest.request[i]); i++,len--)
		m_HttpRequest.command[i] = m_HttpRequest.request[i];
	
	// POST
	if(stricmp(m_HttpRequest.command,"POST")==0)
	{
		char szUploadFile[_MAX_PATH+1];
		_snprintf(szUploadFile,sizeof(szUploadFile)-1,"%s\\%ld_%s_%ld.dat",m_pHttpService->uploaddir,::GetTickCount(),GetSocket()->GetIPAddress(),GetSocket()->GetHandle());
		::DeleteFile(szUploadFile);
		
		BOOL bCreated = FALSE;
		CBinFile upload;
		if(upload.Open(szUploadFile))
		{
			upload.Write(m_HttpRequest.request,m_HttpRequest.requestlength);

			BOOL bKeepAlive=TRUE;
			setsockopt(GetSocket()->GetHandle(),SOL_SOCKET,SO_KEEPALIVE,(char*)&bKeepAlive,sizeof(BOOL));
			
			GetSocket()->SetTimeout(15);

			DWORD dwLen = 0L;
			do {
				Sleep(10L);
				::ioctlsocket(GetSocket()->GetHandle(),FIONREAD,&dwLen);
			} while(dwLen==0L);

			DWORD dwTotRecv = m_HttpRequest.requestlength;
			int nRecv;
			int nRetry = 3;
			char szData[8192];
			do {
				memset(szData,'\0',sizeof(szData));
				
				if((nRecv = GetSocket()->Receive(szData,sizeof(szData)-1)) > 0)
					upload.Write(szData,nRecv);
			
				dwTotRecv += nRecv >= 0 ? nRecv : 0;
				TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CConnectedHttpSocket::ParseHeader(): received: %d, tot %ld\n",nRecv,dwTotRecv));

				nRetry = 3;
				do {
					Sleep(10L);
					::ioctlsocket(GetSocket()->GetHandle(),FIONREAD,&dwLen);
					TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CConnectedHttpSocket::ParseHeader(): dwLen: %ld, nRetry %d\n",dwLen,nRetry));
					nRetry--;
				} while(dwLen==0L && nRetry > 0);
				
				TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CConnectedHttpSocket::ParseHeader(): at exit: dwLen: %ld, nRetry %d\n",dwLen,nRetry));

				if(dwLen==0L && nRetry <= 0)
					nRecv = 0;
				
			} while(nRecv > 0);
			
			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CConnectedHttpSocket::ParseHeader(): done\n"));
			
			bCreated = upload.Close();
		}

		if(bCreated)
			ExtractUploadedFile(szUploadFile);

		// non restituire HTTP_STATUS_OK perche' verrebbe chiamato il codice per ricavare il content-type
		return(bCreated ? HTTP_STATUS_CREATED/*HTTP_STATUS_NO_CONTENT*/ : HTTP_STATUS_UNABLE_TO_CREATE_LOCAL_FILE);
	}
	else // HEAD/GET
	{

		if(stricmp(m_HttpRequest.command,"GET")!=0 && stricmp(m_HttpRequest.command,"HEAD")!=0)
			return(HTTP_STATUS_NOT_IMPLEMENTED);
	}

	// url richiesta
	while(isspace(m_HttpRequest.request[i]))
		i++,len--;

	for(n=0; n < len && n < HTTP_MAX_URL; n++,i++)
		m_HttpRequest.url[n] = m_HttpRequest.request[i];

	while(isspace(m_HttpRequest.url[--n]))
		m_HttpRequest.url[n] = '\0';

	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CConnectedHttpSocket::ParseHeader(): requested url: %s\n",m_HttpRequest.url));

	if(m_HttpRequest.url[0]=='\0')
		return(HTTP_STATUS_BAD_REQUEST);

	if(!(m_pHttpService->flags & HTTPSERVICE_FLAG_PARENT_DIR))
		if(strstr(m_HttpRequest.url,"/.."))
			return(HTTP_STATUS_FORBIDDEN);

	url.StripParentFromUrl(m_HttpRequest.url,HTTP_MAX_URL+1);
	url.DecodeUrl(m_HttpRequest.url);

	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CConnectedHttpSocket::ParseHeader(): decoded url: %s\n",m_HttpRequest.url));

	// header http (Referer)
	p = stristr(m_HttpRequest.request,"Referer: ");
	if(p)
	{
		p += 9;
		while(*p && *p==' ')
			p++;
		if(*p)
		{
			for(i=0; i < HTTP_MAX_URL && (*p && *p!='\r' && *p!='\n'); i++,p++)
				m_HttpRequest.referer[i] = *p;
		}
		
		TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CConnectedHttpSocket::ParseHeader(): referer: %s\n",m_HttpRequest.referer));
	}

	// header http (User-Agent)
	p = stristr(m_HttpRequest.request,"User-Agent: ");
	if(p)
	{
		p += 12;
		while(*p && *p==' ')
			p++;
		if(*p)
		{
			for(i=0; i < HTTP_MAX_USERAGENT && (*p && *p!='\r' && *p!='\n'); i++,p++)
				m_HttpRequest.useragent[i] = *p;
		}
		
		TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CConnectedHttpSocket::ParseHeader(): user agent: %s\n",m_HttpRequest.useragent));
	}

	// header http (If-Modified-Since)
	p = stristr(m_HttpRequest.request,"If-Modified-Since: ");
	if(p)
	{
		p += 19;
		while(*p && *p==' ')
			p++;
		if(*p)
		{
			p = stristr(m_HttpRequest.request,"length=");
			if(p)
			{
				p += 7;
				while(*p && *p==' ')
					p++;
				if(*p)
				{
					for(i=0; i < HTTP_MAX_CONTENT && (*p && *p!=' ' && *p!=';' && *p!='\r' && *p!='\n'); i++,p++)
						m_HttpRequest.ifmodifiedsince[i] = *p;
					if(m_HttpRequest.ifmodifiedsince[0]!='\0')
						m_HttpRequest.ifmodifiedlength = _atoi64(m_HttpRequest.ifmodifiedsince);
				}
			}
	
			p = stristr(m_HttpRequest.request,"If-Modified-Since: ") + 19;
			while(*p && *p==' ')
				p++;
			
			for(i=0; i < HTTP_MAX_CONTENT && (*p && *p!=';' && *p!='\r' && *p!='\n'); i++,p++)
				m_HttpRequest.ifmodifiedsince[i] = *p;
		
			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CConnectedHttpSocket::ParseHeader(): if modified since: %s\n",m_HttpRequest.ifmodifiedsince));
		}
	}

	// header http (Cookie)
	p = stristr(m_HttpRequest.request,"Cookie: ");
	if(p)
	{
		p += 8;
		while(*p && *p==' ')
			p++;
		if(*p)
		{
			while(*p)
			{
				while(*p && (*p==' ' || *p=='\r' || *p=='\n'))
					p++;
				
				for(i=0; i < COOKIE_NAME_LEN && (*p && *p!='=' && *p!=' ' && *p!='\r' && *p!='\n'); i++,p++)
					m_HttpRequest.cookiename[i] = *p;
				m_HttpRequest.cookiename[i] = '\0';
				
				while(*p && (*p==' ' || *p=='='))
					p++;

				for(i=0; i < COOKIE_VALUE_LEN && (*p && *p!=';' && *p!=' ' && *p!='\r' && *p!='\n'); i++,p++)
					m_HttpRequest.cookievalue[i] = *p;
				m_HttpRequest.cookievalue[i] = '\0';
				
				while(*p && (*p==' ' || *p==';' || *p=='\r' || *p=='\n'))
					p++;

				if(m_HttpRequest.cookiename[0]!='\0' && m_HttpRequest.cookievalue[0]!='\0')
				{
					COOKIE* c = (COOKIE*)m_HttpRequest.cookies.Add();
					if(c)
					{
						memset(c,'\0',sizeof(COOKIE));
						strcpyn(c->name,m_HttpRequest.cookiename,COOKIE_NAME_LEN+1);
						strcpyn(c->value,m_HttpRequest.cookievalue,COOKIE_VALUE_LEN+1);
						TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CConnectedHttpSocket::ParseHeader(): cookie: name=%s, value=%s\n",c->name,c->value));
					}
				}
			}
		}
	}

	// nome file relativo all'url richiesta
	ITERATOR iter;
	HTTPALIASES* h;
	BOOL bMapped = FALSE;
	
	// controlla se si tratta di un url mappata
	// occhio che con /x-rated/=403 non passa http://127.0.0.1/x-rated/ ma passa http://127.0.0.1/img/x-rated/
	if((iter = m_pHttpService->listHttpAliases.First())!=(ITERATOR)NULL)
	{
		do
		{
			if((h = (HTTPALIASES*)iter->data)!=(HTTPALIASES*)NULL)
				if(strnicmp(m_HttpRequest.url,h->url,strlen(h->url))==0)
				{
					// la mappatura proibisce l'accesso all'url
					if(isdigit(h->path[0]))
					{
						int nHttpCode = atoi(h->path);
						return(nHttpCode > 0 ? nHttpCode : HTTP_STATUS_FORBIDDEN);
					}

					_snprintf(m_HttpRequest.filename,sizeof(m_HttpRequest.filename)-1,"%s\\%s",h->path,(m_HttpRequest.url)+strlen(h->url));
					
					TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CConnectedHttpSocket::ParseHeader(): mapped url: %s\n",m_HttpRequest.url));

					bMapped = TRUE;
					break;
				}
			
			iter = m_pHttpService->listHttpAliases.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}

	// se non si tratta di un url mappata, inserisce la root directory
	if(!bMapped)
		_snprintf(m_HttpRequest.filename,sizeof(m_HttpRequest.filename)-1,"%s%s",m_pHttpService->rootdir,m_HttpRequest.url);

	p = m_HttpRequest.filename;
	while(*p)
	{	if(*p=='/')
			*p='\\';
		p++;
	}

	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CConnectedHttpSocket::ParseHeader(): local resource: %s\n",m_HttpRequest.filename));

	// estensione http
	if(m_pHttpService->pHttpExtension)
	{
		CHttpExtension* pHttpExtension = m_pHttpService->pHttpExtension;
		return(pHttpExtension->ParseHeader(m_pHttpService,&m_HttpRequest));
	}
	else
		return(HTTP_STATUS_OK);
}

UINT CConnectedHttpSocket::ExtractUploadedFile(LPCSTR szUploadFile)
{
	HANDLE	hFile = INVALID_HANDLE_VALUE;
	HANDLE	hFileMap = NULL;
	LPVOID	lpFileView = NULL;
	char*	pData = NULL;
	char*	p;
	QWORD	qwContentLength = 0L;
	char		szContentLength[32] = {0};
	char		szContentDisposition[1024] = {0};
	char		szContentDispositionName[128] = {0};
	char		szContentDispositionNameLabel[128] = {0};
	char		szContentDispositionFileName[_MAX_PATH+1] = {0};

	// apre il file ricevuto in upload
	if((hFile = ::CreateFile(szUploadFile,/*GENERIC_WRITE|*/GENERIC_READ,0L,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE)
		goto done;

	// crea la mappa per il file
	if((hFileMap = ::CreateFileMapping(hFile,NULL,PAGE_READONLY,0L,0L,NULL))==NULL)
	{
		::CloseHandle(hFile);
		goto done;
	}

	// mappa il file
	if((lpFileView = ::MapViewOfFile(hFileMap,FILE_MAP_READ,0L,0L,0L))==NULL)
	{
		::CloseHandle(hFileMap);
		::CloseHandle(hFile);
		goto done;
	}
	
	// puntatore ai dati nel file
	pData = (char*)lpFileView;

	// cerca la dimensione dei dati, occhio che la dimensione include i marcatori:
	//
	// POST / HTTP/1.1
	// Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/vnd.ms-powerpoint, application/vnd.ms-excel, application/msword, application/x-shockwave-flash, */*
	// Referer: http://10.0.3.248:6591/upload.html
	// Accept-Language: it
	// Content-Type: multipart/form-data; boundary=---------------------------7d3bbc30152
	// Accept-Encoding: gzip, deflate
	// User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0)
	// Host: 10.0.3.248:6591
	// Content-Length: 495787
	// Connection: Keep-Alive
	// Cache-Control: no-cache
	// 
	// -----------------------------7d3bbc30152
	// Content-Disposition: form-data; name="FileName"; filename="C:\Download\atomic.exe"
	// Content-Type: application/octet-stream
	// 
	// <...>
	// -----------------------------7d3bbc30152--
	//
	if((p = stristr(pData,"Content-Length: "))!=NULL)
	{
		p += 16;
		while(*p && *p==' ')
			p++;
		for(int i=0; *p && isdigit(*p) && i < sizeof(szContentLength)-1; i++,p++)
			szContentLength[i] = *p;
		qwContentLength = _atoi64(szContentLength);
		
	}
	
	if((p = stristr(pData,"Content-Disposition: "))!=NULL)
	{
		p += 21;
		while(*p && *p==' ')
			p++;
		int i;
		for(i=0; *p && *p!='\r' && *p!='\n' && i < sizeof(szContentDisposition)-1; i++,p++)
			szContentDisposition[i] = *p;
		if((p = stristr(szContentDisposition,"name="))!=NULL)
		{
			p += 5;
			while(*p && (*p==' ' || *p=='\"'))
				p++;
			for(i=0; *p && *p!='\"' && i < sizeof(szContentDispositionName)-1; i++,p++)
				szContentDispositionName[i] = *p;
			if((p = stristr(szContentDisposition,szContentDispositionName))!=NULL)
			{
				p += strlen(szContentDispositionName);
				while(*p && (*p==' ' || *p=='=' || *p=='\"'))
					p++;
				for(i=0; *p && *p!='\"' && i < sizeof(szContentDispositionNameLabel)-1; i++,p++)
					szContentDispositionNameLabel[i] = *p;
				
				if((p = stristr(szContentDisposition,szContentDispositionNameLabel))!=NULL)
				{
					p += strlen(szContentDispositionNameLabel);
					while(*p && (*p==' ' || *p=='=' || *p=='\"'))
						p++;
					for(i=0; *p && *p!='\"' && i < sizeof(szContentDispositionFileName)-1; i++,p++)
						szContentDispositionFileName[i] = *p;
				}
			}
		}
	}
	
	if(qwContentLength > 0L && szContentDispositionFileName[0]!='\0')
		if((p = stristr(pData,"\r\n\r\n"))!=NULL)
		{
			p+=4;
			
			char* t = p;
			char szBoundary[128] = {0};
			for(int i=0; *p && *p!='\r' && *p!='\n' && i < sizeof(szBoundary)-1; i++,p++)
				szBoundary[i] = *p;
			p = t;
			
			char* pFileContent;
			if((pFileContent = stristr(p,"\r\n\r\n"))!=NULL)
			{
				QWORD qwFileSize = (DWORD)qwContentLength - ((pFileContent-p) + strlen(szBoundary) + 10);
				pFileContent+=4;
				
				char* pFileName = strrchr(szContentDispositionFileName,'\\');
				if(pFileName)
					pFileName += 1;
				else
					pFileName = szContentDispositionFileName;
				
				char szFileName[_MAX_PATH+1];
				_snprintf(szFileName,sizeof(szFileName)-1,"%s\\%s",m_pHttpService->uploaddir,pFileName);
#if 1
				CBinFileEx f;
				if(f.Create(szFileName,GENERIC_WRITE,FILE_SHARE))
				{
					f.WriteEx(pFileContent,(DWORD)qwFileSize);
					f.Close();
				}
#else
				// passata a CBinFileEx::WriteEx()
				CBinFile f;
				if(f.Create(szFileName,GENERIC_WRITE,FILE_SHARE))
				{
					QWORD qwTot = 0L;
					QWORD qwAmount = 32768; // il valore del buffer non puo' essere maggiore di DWORD, valore massimo per Write()
					if(qwAmount > qwFileSize)
					{
						f.Write(pFileContent,(DWORD)qwFileSize);
					}
					else
					{
						do
						{
							qwTot += f.Write(pFileContent,(DWORD)qwAmount);
							pFileContent += qwAmount;
							if(qwFileSize - qwTot <= qwAmount)
								qwAmount = qwFileSize - qwTot;
						}
						while(qwTot < qwFileSize);
					}
					f.Close();
				}
#endif
			}
		}

done:

	// chiude la mappa
	if(lpFileView)
		::UnmapViewOfFile(lpFileView);
	if(hFileMap)
		::CloseHandle(hFileMap);

	// chiude il file, eliminando
	if(hFile!=INVALID_HANDLE_VALUE)
		::CloseHandle(hFile);

	return(0L);
}

/*
	CreateDirectoryListing()

	Crea il file html contenente il listato relativo alla directory.
*/
UINT CConnectedHttpSocket::CreateDirectoryListing(void)
{
	int i;
	int nTotFiles = 0;
	CUrl url;
	CDateTime datetime;
	CFindFile findfile;
	FINDFILE* f;
	CTextFile listfile;
	char* lpFileName;
	int nHttpCode = HTTP_STATUS_OK;

	// estensione http
	if(m_pHttpService->pHttpExtension)
	{
		CHttpExtension* pHttpExtension = m_pHttpService->pHttpExtension;
		if(!pHttpExtension->CreateDirectoryListing(m_pHttpService,&m_HttpRequest,&nHttpCode))
			goto done;
	}

	nHttpCode = HTTP_STATUS_NOT_FOUND;

	// ricerca lo skeleton nella directory specificata
	if((nTotFiles = findfile.FindFile(m_HttpRequest.filename,"*.*",FALSE,_A_ALLFILES)) > 0)
	{
		// imposta il nome del file html per il listato
		_snprintf(
				m_HttpRequest.htmlfile,
				HTTP_MAX_FILENAME,
				"%s\\%s_%ld_%04d-%02d-%02d_%02d-%02d-%02d.html",
				m_pHttpService->listdir,
				m_HttpRequest.socketinfo.ip,
				m_HttpRequest.socketinfo.socket,
				datetime.GetYear(),
				datetime.GetMonth(),
				datetime.GetDay(),
				datetime.GetHour(),
				datetime.GetMin(),
				datetime.GetSec()
				);

		// crea il file html per il listato
		if(listfile.Create(m_HttpRequest.htmlfile))
		{
			char szLine[512];
			char szBuffer[1024];
			char szFormat[32];
			char szDateTime[64];
			BOOL bDir;

			// titolo
			_snprintf(
					szLine,
					sizeof(szLine)-1,
					"<html><title>%s</title><body>\r\n",
					m_HttpRequest.url
					);
			listfile.WriteLine(szLine);

			// corpo
			_snprintf(
					szLine,
					sizeof(szLine)-1,
					"<pre>Index of %s<br><hr><br>\r\n",
					m_HttpRequest.url
					);
			listfile.WriteLine(szLine);

			// deve elencare prima le directory e poi i files
			int n;
			for(n=0; n < 2; n++)
				for(i=0; i < nTotFiles; i++)
				{
					f = findfile.GetFindFile(i);
					bDir = f->attrib & _A_SUBDIR;

					if(n==0)
					{
						if(!bDir)
							continue;
					}
					else if(n==1)
					{
						if(bDir)
							continue;
					}

					// formatta la data/ora relativa al file (GMT)
					datetime.SetDateFormat(GMT_SHORT);
					datetime.SetYear(f->datetime.wYear);
					datetime.SetMonth(f->datetime.wMonth);
					datetime.SetDay(f->datetime.wDay);
					datetime.SetHour(f->datetime.wHour);
					datetime.SetMin(f->datetime.wMinute);
					datetime.SetSec(f->datetime.wSecond);
					if(f->size > 0)
						_snprintf(szDateTime,sizeof(szDateTime)-1,"%s - %ld",datetime.GetFormattedDate(FALSE),f->size);
					else
						strcpyn(szDateTime,datetime.GetFormattedDate(FALSE),sizeof(szDateTime));

					// elimina il pathname
					lpFileName = (char*)::StripPathFromFile(f->name);
					
					// crea la referenza (href) al file/directory
					_snprintf(
							szLine,
							sizeof(szLine)-1,
							"<a href=\"%s%s%s\"><img border=0 src=\"/%s/%s.gif\"> %s</a>",
							m_HttpRequest.url,
							lpFileName,
							bDir ? "/" : "",
							SYSDIR,
							bDir ? "folder" : "file",
							lpFileName
							);
					
					// formatta la linea in modo che la data/ora venga posizionata sempre alla stessa distanza
					_snprintf(szFormat,sizeof(szFormat)-1,"%cs%c%ds%cs\r\n",'%','%',64-strlen(lpFileName),'%');
					_snprintf(szBuffer,sizeof(szBuffer)-1,szFormat,szLine," ",szDateTime);
					
					listfile.WriteLine(szBuffer);
				}

			// footer
			listfile.WriteLine("</pre></body></html>\r\n");
			listfile.Close();

			// imposta il nome del file da inviare con quello del file che contiene il listato
			strcpyn(m_HttpRequest.filename,m_HttpRequest.htmlfile,sizeof(m_HttpRequest.filename));
			nHttpCode = HTTP_STATUS_OK;
		}
	}
	else
	{
		// controlla se si tratta di una directory vuota
		if(::GetFileAttributes(m_HttpRequest.filename)!=0xFFFFFFFF)
			nHttpCode = HTTP_STATUS_EMPTY_DIRECTORY;
	}

done:

	return(nHttpCode);
}

/*
	SetFileContents()

	Imposta le contents relative al file.
*/
UINT CConnectedHttpSocket::SetFileContents(void)
{
	char* p;
	char szExtension[_MAX_FNAME+_MAX_EXT+1] = {0};
	CBinFileEx requestedFile;
	int nHttpCode = HTTP_STATUS_OK;

	// tipo mime (imposta il default su tipo sconosciuto)
	strcpyn(m_HttpRequest.contenttype,"unknown",HTTP_MAX_CONTENT+1);

	// ricava l'estensione del file per impostare il tipo mime relativo
	p = strrchr(m_HttpRequest.filename,'.');
	if(p)
	{
		strcpyn(szExtension,p,sizeof(szExtension));

		BOOL bFound = FALSE;
		CContentTypeList* plistContentType = &(m_pHttpService->listContentType);
		
		// cerca l'estensione del file nella lista
		if(plistContentType)
		{
			ITERATOR iter;
			CONTENTTYPE* c;

			// se trova l'estensione ricava il tipo mime relativo
			if((iter = plistContentType->First())!=(ITERATOR)NULL)
			{
				do
				{
					if((c = (CONTENTTYPE*)iter->data)!=(CONTENTTYPE*)NULL)
						if(stricmp(szExtension,c->ext)==0)
						{
							strcpyn(m_HttpRequest.contenttype,c->content,HTTP_MAX_CONTENT+1);
							bFound = TRUE;
							break;
						}
					
					iter = plistContentType->Next(iter);
				
				} while(iter!=(ITERATOR)NULL);
			}
		}

		// l'estensione non esiste nella lista, ricava il tipo mime dal registro e aggiorna la lista
		if(!bFound)
		{
			CRegistry registry;
			REGISTERFILETYPE r;

			if(registry.GetRegisteredFileType(szExtension,&r))
				if(r.contenttype[0]!='\0')
				{
					strcpyn(m_HttpRequest.contenttype,r.contenttype,HTTP_MAX_CONTENT+1);

					//if(plistContentType->Lock())
					{
						CONTENTTYPE* c = (CONTENTTYPE*)plistContentType->Add();
						if(c)
						{
							strcpyn(c->ext,szExtension,HTTP_MAX_CONTENT_EXT+1);
							strcpyn(c->content,r.contenttype,HTTP_MAX_CONTENT+1);
						}

						//plistContentType->Unlock();
					}
				}
		}
	}

	// imposta dimensione e data/ora del file
	m_HttpRequest.contentlen = 0L;
	if(requestedFile.Open(m_HttpRequest.filename,FALSE,GENERIC_READ))
	{
		m_HttpRequest.contentlen = requestedFile.GetFileSizeEx();
		
		CDateTime fileDatetime;
		requestedFile.GetFileTime(fileDatetime);
		
		// se e' stata specificata la direttiva, controlla la dimensione/data di ultimo accesso, impostando il codice http di conseguenza
		if(m_HttpRequest.ifmodifiedsince[0]!='\0')
		{
			CDateTime ifModifiedSince;
			ifModifiedSince.ConvertDate(GMT,BRITISH,m_HttpRequest.ifmodifiedsince,NULL);

			if(	fileDatetime.GetYear()  == ifModifiedSince.GetYear()  &&
				fileDatetime.GetMonth() == ifModifiedSince.GetMonth() &&
				fileDatetime.GetDay()   == ifModifiedSince.GetDay()   &&
				fileDatetime.GetHour()  == ifModifiedSince.GetHour()  &&
				fileDatetime.GetMin()   == ifModifiedSince.GetMin()   &&
				fileDatetime.GetSec()   == ifModifiedSince.GetSec()   &&
				(m_HttpRequest.ifmodifiedlength > 0L ? m_HttpRequest.contentlen==m_HttpRequest.ifmodifiedlength : 1))
				nHttpCode = HTTP_STATUS_NOT_MODIFIED;
		}

		fileDatetime.SetDateFormat(GMT_SHORT);
		_snprintf(m_HttpRequest.lastmodified,HTTP_MAX_CONTENT,"%s",fileDatetime.GetFormattedDate(FALSE));

		requestedFile.Close();
	}
	else
		nHttpCode = HTTP_STATUS_NOT_FOUND;

	return(nHttpCode);
}

/*
	PrepareResponse()

	Prepara la risposta da inviare al client in collegamento impostando i campi dell'header http.
*/
void CConnectedHttpSocket::PrepareResponse(UINT nHttpCode)
{
	CDateTime dateTime(GMT_SHORT);
	char* pHttpCode = (char*)GetHttpCodeDesc(nHttpCode);

	// imposta il codice http della struttura interna
	m_HttpRequest.code = nHttpCode;

	// normalizza il codice http
	// se il codice http e' HTTP_STATUS_EMPTY_DIRECTORY significa che la directory per cui e' stato richiesto
	// il listato e' vuota,  in tal caso deve sostituire il codice http nell'header con HTTP_STATUS_OK per far
	// si che il browser visualizzi il contenuto della pagina (empty directory)
	if(IS_HTTP_INTERNAL_ERROR(nHttpCode))
	{
		switch(nHttpCode)
		{
			case HTTP_STATUS_UNABLE_TO_CREATE_LOCAL_FILE:
				nHttpCode = HTTP_STATUS_INTERNAL;
				break;
			case HTTP_STATUS_EMPTY_DIRECTORY:
				nHttpCode = HTTP_STATUS_OK;
				break;
			case HTTP_STATUS_UNKNOW:
			default:
				nHttpCode = HTTP_STATUS_INTERNAL;
				break;
		}

		pHttpCode = (char*)GetHttpCodeDesc(nHttpCode);
	}

	// costruisce l'intestazione base
	int len = _snprintf(m_HttpRequest.response,
					HTTP_MAX_REQUEST,
					"HTTP/%s %d %s\r\n"
					"Date: %s\r\n"
					"Server: %s\r\n"
					"MIME-version: %s\r\n",
					HTTP_SUPPORTED_VERSION,
					nHttpCode,
					pHttpCode,
					dateTime.GetFormattedDate(TRUE),
					m_pHttpService->server,
					MIME_SUPPORTED_VERSION
					);
	if(len > 0)
	{
		// aggiunge i campi all'intestazione a seconda del codice http
		switch(nHttpCode)
		{
			// ok
			case HTTP_STATUS_OK:
				SetCookies(len);
				_snprintf(m_HttpRequest.response+len,
						HTTP_MAX_REQUEST-len,
						"Last-modified: %s\r\n"
						//"Accept-ranges: bytes\r\n"
						"Content-length: %I64u\r\n"
						"Content-type: %s\r\n"
						"Connection: close\r\n\r\n",
						m_HttpRequest.lastmodified,
						m_HttpRequest.contentlen,
						m_HttpRequest.contenttype
						);
				break;

			// non modificato
			case HTTP_STATUS_NOT_MODIFIED:
				SetCookies(len);
				strcpyn(m_HttpRequest.response+len,"Connection: close\r\n\r\n",(HTTP_MAX_REQUEST-1)-len);
				break;

			// creato (upload)
			case HTTP_STATUS_CREATED:
				SetCookies(len);
				_snprintf(m_HttpRequest.response+len,
						HTTP_MAX_REQUEST-len,
						"Content-type: text/html\r\n"
						"Connection: close\r\n\r\n"
						"<html>"
						"<head>"
						"<title></title>"
						"</head>"
						"</body>"
						"</html>"
						);
				break;

			// nessun contenuto
			case HTTP_STATUS_NO_CONTENT:
				SetCookies(len);
				_snprintf(m_HttpRequest.response+len,
						HTTP_MAX_REQUEST-len,
						"Content-type: text/html\r\n"
						"Connection: close\r\n\r\n"
						"<html>"
						"<head>"
						"<title>%s</title>"
						"</head>"
						"</body>"
						"</html>",
						m_HttpRequest.url
						);
				break;

			// errore
			default:
				_snprintf(m_HttpRequest.response+len,
						HTTP_MAX_REQUEST-len,
						"Content-type: text/html\r\n"
						"Connection: close\r\n\r\n"
						"<html>"
						"<head>"
						"<title>Error %d - %s</title>"
						"</head>"
						"<br>"
						"Error %d - %s"
						"<br>"
						"</body>"
						"</html>",
						nHttpCode,
						pHttpCode,
						nHttpCode,
						pHttpCode
						);
				break;
		}
	}
}

/*
	SetCookies()

	Imposta i cookies relativi all'url richiesta nell'header http.
*/
void CConnectedHttpSocket::SetCookies(int& nOffset)
{
	COOKIE* c;
	ITERATOR iter;

	// per tutti i cookies presenti nella lista interna (caricati dal file)
	if((iter = m_pHttpService->listCookies.First())!=(ITERATOR)NULL)
	{
		do
		{
			if((c = (COOKIE*)iter->data)!=(COOKIE*)NULL)
			{
				TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CConnectedHttpSocket::SetCookies(): request: %s, cookie path: %s\n",m_HttpRequest.url,c->path));
				
				// se il pathname dell'url richiesta soddisfa il pathname relativo al cookie
				if(stristr(m_HttpRequest.url,c->path))
				{
					/*
						<ip>			indirizzo ip del client
						<socket>		numero socket del client
						<date>		data corrente
						<hostaddr>	indirizzo ip dell'host
					
						es.:
						USRID=<ip>_<socket>_<date>;expires=Wed, 26 Aug 2065 04:30:00 GMT;path=/;domain=<hostaddr>
					*/
					char szBuffer[1024] = {0};
					char szValue[512] = {0};
					CDateTime dateTime(ANSI_SHORT,HHMMSS_SHORT);
					
					while(substr(c->value,"<ip>",m_HttpRequest.socketinfo.ip,szBuffer,sizeof(szBuffer)))
					{
						strcpyn(c->value,szBuffer,COOKIE_VALUE_LEN+1);
						memset(szBuffer,'\0',sizeof(szBuffer));
					}

					_snprintf(szValue,sizeof(szValue)-1,"%ld",m_HttpRequest.socketinfo.socket);
					while(substr(c->value,"<socket>",szValue,szBuffer,sizeof(szBuffer)))
					{
						strcpyn(c->value,szBuffer,COOKIE_VALUE_LEN+1);
						memset(szBuffer,'\0',sizeof(szBuffer));
					}
					
					_snprintf(szValue,sizeof(szValue)-1,"%s%s",dateTime.GetFormattedDate(TRUE),dateTime.GetFormattedTime(TRUE));
					while(substr(c->value,"<date>",szValue,szBuffer,sizeof(szBuffer)))
					{
						strcpyn(c->value,szBuffer,COOKIE_VALUE_LEN+1);
						memset(szBuffer,'\0',sizeof(szBuffer));
					}
					
					while(substr(c->domain,"<hostaddr>",m_pHttpService->domain,szBuffer,sizeof(szBuffer)))
					{
						strcpyn(c->domain,szBuffer,COOKIE_DOMAIN_LEN+1);
						memset(szBuffer,'\0',sizeof(szBuffer));
					}
					
					nOffset += _snprintf(	m_HttpRequest.response+nOffset,
										HTTP_MAX_REQUEST-nOffset,
										"Set-Cookie: %s=%s; expires=%s; path=%s; domain=%s\r\n",
										c->name,
										c->value,
										c->expires,
										c->path,
										c->domain);
				}
			}
			
			iter = m_pHttpService->listCookies.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}
}

/*
	SendResponse()

	Invia la risposta al socket in connessione.
*/
BOOL CConnectedHttpSocket::SendResponse(void)
{
	// invia l'header
	BOOL bSent = GetSocket()->WSASend((LPBYTE)m_HttpRequest.response,strlen(m_HttpRequest.response),m_hEvent);

	// invia il file
	if(bSent)
	{
		if(m_HttpRequest.code==HTTP_STATUS_OK)
		{
			if(stricmp(m_HttpRequest.command,"GET")==0)
			{
				CBinFile requestedFile;
				char szBuffer[4096] = {0};

				if(requestedFile.Open(m_HttpRequest.filename,FALSE,GENERIC_READ))
				{
					int n;
					while(((n = requestedFile.Read(szBuffer,sizeof(szBuffer)-1)) > 0) && bSent)
					{
						bSent = GetSocket()->WSASend((LPBYTE)szBuffer,n,m_hEvent);
						//bSent = GetSocket()->Send(szBuffer,n)!=SOCKET_ERROR;
						memset(szBuffer,'\0',sizeof(szBuffer));
					}

					requestedFile.Close();
				}
				else
					bSent = FALSE;
			}

			// se il file inviato era il listato di una directory, lo elimina
			if(m_HttpRequest.htmlfile[0]!='\0' && (m_pHttpService->flags & HTTPSERVICE_FLAG_DELETE_LISTING))
				::DeleteFile(m_HttpRequest.htmlfile);
		}
	}

	return(bSent);
}

/*
	WriteLog()

	Effetua il log della richiesta http.
*/
BOOL CConnectedHttpSocket::WriteLog(void)
{
	// controlla che sia stata specificata una directory
	if(m_pHttpService->logdir[0]!='\0')
	{
		CBinFile logFile;
		char szLogFile[_MAX_PATH+1];
		CDateTime dateTime;
		
		// log richieste
		if(m_pHttpService->flags & HTTPSERVICE_FLAG_REQUEST_LOG)
		{
			_snprintf(
					szLogFile,
					sizeof(szLogFile)-1,
					"%s\\request_%04d-%02d-%02d.log",
					m_pHttpService->logdir,
					dateTime.GetYear(),
					dateTime.GetMonth(),
					dateTime.GetDay()
					);
			
			if(logFile.Open(szLogFile))
			{
				logFile.Seek(0L,FILE_END);
				logFile.Write(m_HttpRequest.request,strlen(m_HttpRequest.request));
				logFile.Close();
			}
		}
		
		// log risposte
		if(m_pHttpService->flags & HTTPSERVICE_FLAG_REQUEST_LOG)
		{
			_snprintf(
					szLogFile,
					sizeof(szLogFile)-1,
					"%s\\response_%04d-%02d-%02d.log",
					m_pHttpService->logdir,
					dateTime.GetYear(),
					dateTime.GetMonth(),
					dateTime.GetDay()
					);
			
			if(logFile.Open(szLogFile))
			{
				char szBuffer[512];
				_snprintf(szBuffer,sizeof(szBuffer)-1,"[request: %s]\r\n[local resource: %s]\r\n%s:%ld\r\n",m_HttpRequest.url,m_HttpRequest.filename,m_HttpRequest.socketinfo.ip,m_HttpRequest.socketinfo.socket);
				logFile.Seek(0L,FILE_END);
				logFile.Write(szBuffer,strlen(szBuffer));
				logFile.Write(m_HttpRequest.response,strlen(m_HttpRequest.response));
				logFile.Close();
			}
		}
				
		// log accessi
		if(m_pHttpService->flags & HTTPSERVICE_FLAG_ACCESS_LOG)
		{
			_snprintf(szLogFile,
					sizeof(szLogFile)-1,
					"%s\\access_%04d-%02d-%02d.log",
					m_pHttpService->logdir,
					dateTime.GetYear(),
					dateTime.GetMonth(),
					dateTime.GetDay()
					);
			
			if(logFile.Open(szLogFile))
			{
				double nElapsed = (double)m_HttpRequest.end - (double)m_HttpRequest.start;
				nElapsed /= (double)1000.0;

				char szBuffer[1024];
				int nLen = _snprintf(szBuffer,
								sizeof(szBuffer)-1,
								"%s - - [%s] \"%s %s %s\" %d - \"%s\" \"%s\" %.3f",
								m_HttpRequest.socketinfo.ip,
								m_HttpRequest.received,
								m_HttpRequest.command,
								m_HttpRequest.url,
								m_HttpRequest.version,
								m_HttpRequest.code,
								m_HttpRequest.referer,
								m_HttpRequest.useragent,
								nElapsed
								);

				// aggiunge gli eventuali cookies
				ITERATOR iter;
				COOKIE* c;
				if((iter = m_HttpRequest.cookies.First())!=(ITERATOR)NULL)
				{
					do
					{
						if((c = (COOKIE*)iter->data)!=(COOKIE*)NULL)
						{
							
							if(nLen > 0)
								nLen += _snprintf(szBuffer+nLen,
												sizeof(szBuffer)-1-nLen,
												" %s=%s",
												c->name,
												c->value);
						}
						
						iter = m_HttpRequest.cookies.Next(iter);
					
					} while(iter!=(ITERATOR)NULL);
				}

				logFile.Seek(0L,FILE_END);
				logFile.Write(szBuffer,strlen(szBuffer));
				logFile.Write("\r\n",2);
				logFile.Close();
			}
		}
	}

	return(TRUE);
}

/*
	CListeningHttpSocket()

	Classe per la gestione della lista dei socket in collegamento (lato server).
	La classe viene usata per gestire le connessioni TCP/IP sul lato server.
*/
CListeningHttpSocket::CListeningHttpSocket(	UINT			nPort,
									const char*	pDomain,
									const char*	pServerName,
									const char*	pRootDir,
									const char*	pSysDir,
									const char*	pUploadDir,
									const char*	pLogDir,
									const char*	pListDir,
									const char*	pDefaultHtmlFile,
									DWORD		dwFlags,
									CHttpExtension* pHttpExtension
									) : CListeningSocket(nPort)
{
	// inizializza
	m_hEvent = NULL;
	m_pListenThread = NULL;
	m_HttpService.port = nPort;
	strcpyn(m_HttpService.domain,pDomain,sizeof(m_HttpService.domain));
	strcpyn(m_HttpService.server,pServerName,sizeof(m_HttpService.server));
	strcpyn(m_HttpService.rootdir,pRootDir,sizeof(m_HttpService.rootdir));
	strcpyn(m_HttpService.uploaddir,pUploadDir,sizeof(m_HttpService.uploaddir));
	strcpyn(m_HttpService.sysdir,pSysDir,sizeof(m_HttpService.sysdir));
	strcpyn(m_HttpService.logdir,pLogDir,sizeof(m_HttpService.logdir));
	strcpyn(m_HttpService.listdir,pListDir,sizeof(m_HttpService.listdir));
	strcpyn(m_HttpService.defaulthtml,pDefaultHtmlFile,sizeof(m_HttpService.defaulthtml));
	m_HttpService.flags = dwFlags;
	m_HttpService.pHttpExtension = pHttpExtension;
	m_HttpParams.pListenSocket = NULL;
	m_HttpParams.hConnectedSocket = INVALID_SOCKET;
	m_HttpParams.hEvent = NULL;
	
	// carica i mappaggi relativi alle directories virtuali
	LoadMappings();
	
	// carica i cookies
	LoadCookies();
}

/*
	~CListeningHttpSocket()
*/
CListeningHttpSocket::~CListeningHttpSocket()
{
	// elimina il thread per accettare i collegamenti
	if(m_pListenThread)
	{
		HANDLE hThread = (HANDLE)m_pListenThread->GetHandle();
		if(hThread!=INVALID_HANDLE_VALUE)
			::WaitForSingleObject(hThread,5000L);
		delete m_pListenThread,m_pListenThread = NULL;
	}

	// ogni nuovo socket in collegamento (CConnectedHttpSocket) viene inserito nella lista
	// associandogli un thread che si incarica della connessione http
	// il thread, quando termina, elimina il socket dalla lista, quindi i socket presenti
	// nella lista a questo punto sono rimasti congelati durante la connessione
	ITERATOR iter;
	if((iter = CNodeList::First())!=(ITERATOR)NULL)
	{
		CConnectedHttpSocket* pSocket;
		
		do
		{
			if((pSocket = (CConnectedHttpSocket*)iter->data)!=(CConnectedHttpSocket*)NULL)
			{
				// forzando la chiusura del socket, in teoria obbliga il thread a terminare
				if(pSocket->GetSocket() && pSocket->GetSocket()->GetHandle()!=INVALID_SOCKET)
				{
					pSocket->GetSocket()->Close();
					::Sleep(1000L);
				}
				
				if(pSocket->GetThread() && pSocket->GetThread()->GetHandle()!=INVALID_HANDLE_VALUE)
					pSocket->GetThread()->Abort();
			}
			
			iter = CNodeList::Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}

	// azzera la lista dei socket in collegamento
	CNodeList::DeleteAll();
}

/*
	Start()

	Inizia il servizio http.
*/
BOOL CListeningHttpSocket::Start(void)
{
	BOOL bStarted = TRUE;

	// crea l'evento per la sincronizzazione con il thread che accetta 
	// i collegamenti sul socket d'ascolto
	if((m_hEvent = ::CreateEvent(NULL,TRUE,FALSE,NULL))==NULL)
		bStarted = FALSE;

	// mette il socket in ascolto
	if(bStarted)
		bStarted = StartListen();

	// lancia il thread per accettare i collegamenti sul socket in ascolto
	if(bStarted)
	{
		// imposta i parametri da passare al thread
		m_HttpParams.pListenSocket = this;
		m_HttpParams.hConnectedSocket = 0L;
		m_HttpParams.hEvent = m_hEvent;

		// crea in sospensione per resettare l'autodelete
		m_pListenThread = ::BeginThread(	ListenThread,
									(LPVOID)&m_HttpParams,
									THREAD_PRIORITY_HIGHEST,
									0,
									CREATE_SUSPENDED,
									NULL
									);

		bStarted = (m_pListenThread!=(CThread*)NULL);
		
		if(bStarted)
		{
			m_pListenThread->SetAutoDelete(FALSE);
			bStarted = m_pListenThread->Resume();
		}
	}

	return(bStarted);
}

/*
	Stop()

	Termina il servizio http.
*/
BOOL CListeningHttpSocket::Stop(void)
{
	BOOL bStopped = FALSE;

	// imposta l'oggetto per la sincronizzazione con il thread che accetta 
	// i collegamenti sul socket d'ascolto
	if(m_hEvent)
		bStopped = ::SetEvent(m_hEvent);

	// termina l'ascolto (chiude il socket)
	StopListen();
	
	// aspetta che il thread termini (quando il thread esce dal ciclo d'ascolto
	// grazie alla chiusura del socket di cui sopra, si sincronizza sull'oggetto)
	if(bStopped)
	{
		if(m_pListenThread)
		{
			HANDLE hThread = (HANDLE)m_pListenThread->GetHandle();
			if(hThread!=INVALID_HANDLE_VALUE)
				bStopped = ::WaitForSingleObject(hThread,5000L)!=WAIT_TIMEOUT;
			delete m_pListenThread,m_pListenThread = NULL;
		}
		else
			bStopped = FALSE;
	}

	// elimina l'evento
	if(bStopped)
	{
		if(m_hEvent)
			::CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}

	return(bStopped);
}

/*
	LoadMappings()
	
	Carica le mappature relative alle directories virtuali presenti nel file.

	mappings.ini:
	;
	;	inserire esattamente come da esempio (url con '/' iniziale e finale e pathname *senza* '\' finale)
	;	specificando un codice http, invece di restituire il contenuto della directory ritorna il codice
	;	(con 0 restituisce 403)
	;
	/inetpub/=c:\tmp\inetpub
	/pict/=c:\tmp\pict
	/x-rated/=403
*/
void CListeningHttpSocket::LoadMappings(void)
{
	CTextFile mappingsFile;
	char szMappingsFile[_MAX_PATH+1];

	strcpyn(szMappingsFile,m_HttpService.sysdir,sizeof(szMappingsFile));
	::EnsureBackslash(szMappingsFile,sizeof(szMappingsFile));
	int n = strlen(szMappingsFile);
	_snprintf(szMappingsFile+n,sizeof(szMappingsFile)-1-n,"%s","mappings.ini");
	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CConnectedHttpSocket::LoadMappings(): mappings file: %s\n",szMappingsFile));

	// se il file esiste, carica il contenuto
	if(mappingsFile.Open(szMappingsFile,FALSE,GENERIC_READ))
	{
		char* p;
		char szMap[HTTPALIASES_URL_LEN + HTTPALIASES_PATH_LEN + 1];

		// legge per linee
		while(mappingsFile.ReadLine(szMap,sizeof(szMap))!=FILE_EOF)
		{
			// elimina gli spazi iniziali
			p = szMap;
			while(*p && *p==' ')
				p++;
			if(!*p)
				continue;
			if(p!=szMap)
				strcpyn(szMap,p,sizeof(szMap));

			// se non si tratta di un commento
			if(szMap[0]!=';')
			{
				// considera solo le linee <url logica> = <path fisica|codice http>
				if((p = strchr(szMap,'='))!=NULL)
				{
					if(*++p)
					{
						while(*p && *p==' ')
							p++;
						
						if(*p)
						{
							HTTPALIASES* h = (HTTPALIASES*)m_HttpService.listHttpAliases.Add();
							if(h)
							{
								strcpyn(h->path,p,HTTPALIASES_PATH_LEN+1);
								p = h->path + strlen(h->path)-1;
								while(*p==' ')
									*p--='\0';
								
								p = szMap;
								while(*p && *p==' ')
									p++;
								int i=0;
								for(; *p && *p!=' ' && *p!='=' && i < HTTPALIASES_URL_LEN+1; i++,p++)
									h->url[i] = *p;
								h->url[i] = '\0';
							}
						}
					}
				}
			}
		}

		mappingsFile.Close();
	}

	// inserisce la directory di sistema
	HTTPALIASES* h = (HTTPALIASES*)m_HttpService.listHttpAliases.Add();
	if(h)
	{
		strcpyn(h->url,"/"SYSDIR"/",HTTPALIASES_URL_LEN+1);
		strcpyn(h->path,m_HttpService.sysdir,HTTPALIASES_PATH_LEN+1);
	}
}

/*
	LoadCookies()

	Carica i cookies presenti nel file.
	La data (expires) va sempre specificata in GMT (assoluta), il pathname (path) con la barra
	iniziale e finale ed il dominio con il nome logico (non l'indirizzo ip) senza il protocollo.
		
	cookies.ini:
	;
	;	<ip>			indirizzo ip del client
	;	<socket>		numero socket del client
	;	<date>		data corrente
	;	<hostaddr>	indirizzo ip dell'host
	;
	USRID=<ip>_<socket>_<date>;expires=Wed, 26 Aug 2065 04:30:00 GMT;path=/;domain=<hostaddr>
*/
void CListeningHttpSocket::LoadCookies(void)
{
	CTextFile cookiesFile;
	char szCookiesFile[_MAX_PATH+1];

	strcpyn(szCookiesFile,m_HttpService.sysdir,sizeof(szCookiesFile));
	::EnsureBackslash(szCookiesFile,sizeof(szCookiesFile));
	int n = strlen(szCookiesFile);
	_snprintf(szCookiesFile+n,sizeof(szCookiesFile)-1-n,"%s","cookies.ini");
	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CConnectedHttpSocket::LoadCookies(): cookies file: %s\n",szCookiesFile));

	// se il file esiste, carica il contenuto
	if(cookiesFile.Open(szCookiesFile,FALSE,GENERIC_READ))
	{
		char* p;
		char szCookie[COOKIE_MAX_LEN+1];

		// legge per linee
		while(cookiesFile.ReadLine(szCookie,sizeof(szCookie))!=FILE_EOF)
		{
			// elimina gli spazi iniziali
			p = szCookie;
			while(*p && *p==' ')
				p++;
			if(!*p)
				continue;
			if(p!=szCookie)
			{
				strcpyn(szCookie,p,sizeof(szCookie));
				p = szCookie;
			}

			// se non si tratta di un commento
			if(szCookie[0]!=';')
			{
				// considera solo le linee contenenti un cookie
				if(strchr(szCookie,'=')!=NULL)
				{
					// *nessun controllo* sulla sintassi della linea, solo sulle
					// dimensioni dei buffer della struttura per il cookie:
					// USRID=<ip>_<socket>_<date>;expires=Wed, 26 Aug 2065 04:30:00 GMT;path=/;domain=<hostaddr>
					COOKIE* c = (COOKIE*)m_HttpService.listCookies.Add();
					if(c)
					{
						int i;
						memset(c,'\0',sizeof(COOKIE));

						for(i=0; i < COOKIE_NAME_LEN+1 && *p && *p!='='; i++,p++)
							c->name[i] = *p;
						
						if(*++p)
							for(i=0; i < COOKIE_VALUE_LEN+1 && *p && *p!=';'; i++,p++)
								c->value[i] = *p;
						
						if(*++p)
						{
							while(*p && *p!='=')
								p++;
							if(*++p)
								for(i=0; i < COOKIE_EXPIRES_LEN+1 && *p && *p!=';'; i++,p++)
									c->expires[i] = *p;
						}
						
						if(*++p)
						{
							while(*p && *p!='=')
								p++;
							if(*++p)
								for(i=0; i < COOKIE_PATH_LEN+1 && *p && *p!=';'; i++,p++)
									c->path[i] = *p;
						}
						
						if(*++p)
						{
							while(*p && *p!='=')
								p++;
							if(*++p)
								for(i=0; i < COOKIE_DOMAIN_LEN+1 && *p && *p!=';'; i++,p++)
									c->domain[i] = *p;
						}
					}
				}
			}
		}

		cookiesFile.Close();
	}
}

/*
	Insert()

	Inserisce un nuovo elemento nella lista restituendone il puntatore.
	Ogni elemento rappresenta una (nuova) connessione sul socket in ascolto.
*/
CConnectedHttpSocket* CListeningHttpSocket::Insert(CSock* pSocket)
{
	// crea l'oggetto per il socket in collegamento
	CConnectedHttpSocket* pConnectedSocket = new CConnectedHttpSocket(GetHttpService());
	if(pConnectedSocket)
	{
		//  trasforma da CSock a CConnectedHttpSocket
		pConnectedSocket->SetSocket(pSocket);
		
		// socket
		pConnectedSocket->GetSocket()->ResetData();

		// thread
		pConnectedSocket->SetThread(NULL);

		// inserisce nella lista
		ITERATOR iter = CNodeList::Insert(pConnectedSocket);
		if(iter->data)
			pConnectedSocket = (CConnectedHttpSocket*)iter->data;
	}

	return(pConnectedSocket);
}

/*
	FindSocket()

	Cerca nella lista l'elemento corrispondente al socket.
*/
CConnectedHttpSocket* CListeningHttpSocket::FindSocket(SOCKET socket)
{
	ITERATOR iter = NULL;
	CConnectedHttpSocket* pConnectedSocket = NULL;

	// scorre la lista delle connessioni
	if((iter = CNodeList::First())!=(ITERATOR)NULL)
	{
		do
		{
			// puntatore al socket (connesso) della lista
			pConnectedSocket = (CConnectedHttpSocket*)iter->data;

			// confronta l'handle del socket della lista con quello specificato
			if(pConnectedSocket)
				if(socket==pConnectedSocket->GetSocket()->GetHandle())
					break;

			iter = CNodeList::Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}

	return(pConnectedSocket);
}

/*
	Remove()

	Rimuove l'elemento dalla lista.
*/
void CListeningHttpSocket::Remove(SOCKET socket)
{
	ITERATOR iter = NULL;
	CConnectedHttpSocket* pConnectedSocket = NULL;

	// scorre la lista delle connessioni
	if((iter = CNodeList::First())!=(ITERATOR)NULL)
	{
		do
		{
			// puntatore al socket (connesso) della lista
			pConnectedSocket = (CConnectedHttpSocket*)iter->data;

			// confronta l'handle del socket della lista con quello specificato
			if(pConnectedSocket)
				if(socket==pConnectedSocket->GetSocket()->GetHandle())
				{
					// elimina l'oggetto dalla lista
					// le risorse associate vengono eliminate dalla PreDelete() di cui sotto
					//CNodeList::Delete(iter);
					CNodeList::Remove(iter);
					break;
				}

			iter = CNodeList::Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}
}

/*
	CHttpConnection()
	
	Classe per il collegamento http (lato client).
	La classe viene usata per gestire la connessione TCP/IP al server http.
*/
CHttpConnection::CHttpConnection()
{
	m_pmutexCookiesFile = NULL;
	CHttpConnection::Reset();
}

/*
	~CHttpConnection()
*/
CHttpConnection::~CHttpConnection()
{
	if(m_pmutexCookiesFile)
		delete m_pmutexCookiesFile,m_pmutexCookiesFile = NULL;
}

/*
	Reset()

	Resetta i dati relativi alla connessione.
*/
void CHttpConnection::Reset(void)
{
	// resetta il socket
	CSock::Reset();

	// resetta i valori interni
	m_nSocketCode = 0;
	memset(m_szSocketCode,'\0',sizeof(m_szSocketCode));
	m_nHttpCode = (int)HTTP_STATUS_VOID;
	memset(m_szHttpCode,'\0',sizeof(m_szHttpCode));
	memset(&m_HttpObject,'\0',sizeof(HTTPOBJECT));
	memset(m_szServerName,'\0',sizeof(m_szServerName));
	memset(m_szHostName,'\0',sizeof(m_szHostName));
	memset(m_szHostAddr,'\0',sizeof(m_szHostAddr));
	m_nPortNumber = HTTP_DEFAULT_PORT;
	memset(&(m_Proxy.address),'\0',HOSTNAME_SIZE+1);
	m_Proxy.port = 0L;
	memset(&(m_Proxy.user),'\0',MAX_USER_LEN+1);
	memset(&(m_Proxy.password),'\0',MAX_PASS_LEN+1);
	memset(&(m_Proxy.auth),'\0',MAX_USER_LEN+1+MAX_PASS_LEN+1);
	m_Proxy.exclude.RemoveAll();
	m_bSkipExisting = FALSE;
	m_bForceSkipExisting = FALSE;
	m_bDoNotSkipExisting = TRUE;
	m_nConnectionRetry = 1;
	m_bAreCookiesAllowed = FALSE;
	memset(m_szCookieFileName,'\0',sizeof(m_szCookieFileName));
	if(m_pmutexCookiesFile)
		delete m_pmutexCookiesFile,m_pmutexCookiesFile = NULL;
	m_nMinSize = m_nMaxSize = 0L;
	m_listHttpHeaders.RemoveAll();
}

/*
	Close()

	Chiude la connessione.
*/
BOOL CHttpConnection::Close(void)
{
	// chiude il socket
	BOOL bClosed = CSock::Close();

	// aggiorna i cookies
	if(CookiesAllowed())
		UpdateCookies();

	return(bClosed);
}

/*
	SetCookieFileName()

	Imposta il nome del file contenente i cookies.
*/
void CHttpConnection::SetCookieFileName(LPCSTR lpcszCookieFileName)
{
	strcpyn(m_szCookieFileName,lpcszCookieFileName,sizeof(m_szCookieFileName));
	
	if(m_pmutexCookiesFile)
		delete m_pmutexCookiesFile,m_pmutexCookiesFile = NULL;
	m_pmutexCookiesFile = new CSyncProcesses();
	if(m_pmutexCookiesFile)
	{
		m_pmutexCookiesFile->SetName(m_szCookieFileName);
		m_pmutexCookiesFile->SetTimeout(SYNC_10_SECS_TIMEOUT);
	}
}

/*
	ParseCookie()

	Estrae il cookie dalla stringa (ricevuta dall'host) per formattarlo nei campi della struttura.
*/
void CHttpConnection::ParseCookie(LPCSTR lpcszCookie,COOKIE* cookie)
{
	int i;
	register char* p = (char*)lpcszCookie;
	
	memset(cookie,'\0',sizeof(COOKIE));

	if(p)
	{
		// name
		if(p)
		{
			while(*p && *p==' ')
				p++;
			for(i = 0; *p && *p!='=' && i < COOKIE_NAME_LEN;)
				cookie->name[i++] = *p++;
			strrtrim(cookie->name);
		}

		// value
		if(p)
		{
			while(*p && (*p==' ' || *p=='='))
				p++;
			for(i = 0; *p && *p!=';' && i < COOKIE_VALUE_LEN;)
				cookie->value[i++] = *p++;
			strrtrim(cookie->value);
		}

		// expires
		memset(cookie->expires,' ',COOKIE_EXPIRES_LEN-1);
		p = stristr(lpcszCookie,"expires");
		if(p)
		{
			p += 7;
			while(*p && (*p=='=' || *p==' '))
				p++;	
			for(i = 0; *p && *p!=';' && i < COOKIE_EXPIRES_LEN;)
				cookie->expires[i++] = *p++;
			strrtrim(cookie->expires);

			char szBuffer[COOKIE_EXPIRES_LEN+1] = {0};
			while(substr(cookie->expires,"-"," ",szBuffer,sizeof(szBuffer)))
			{
				strcpyn(cookie->expires,szBuffer,COOKIE_EXPIRES_LEN+1);
				memset(szBuffer,'\0',sizeof(szBuffer));
			}
		}

		// path
		p = stristr(lpcszCookie,"path");
		if(p)
		{
			p += 4;
			while(*p && (*p==' ' || *p=='='))
				p++;
			for(i = 0; *p && *p!=';' && i < COOKIE_PATH_LEN;)
				cookie->path[i++] = *p++;
			strrtrim(cookie->path);
		}

		// domain
		p = stristr(lpcszCookie,"domain");
		if(p)
		{
			p += 6;
			while(*p && (*p==' ' || *p=='='))
				p++;
			for(i = 0; *p && *p!=';' && i < COOKIE_DOMAIN_LEN;)
				cookie->domain[i++] = *p++;
			strrtrim(cookie->domain);

			// elimina il protocollo
			char* pHost = strstr(cookie->domain,"://");
			if(pHost)
			{
				pHost += 3;
				int i = strlen(pHost);
				memmove(cookie->domain,pHost,i);
				cookie->domain[i] = '\0';
			}
		}
		else // imposta con l'host corrente
			strcpyn(cookie->domain,this->m_szHostName,COOKIE_DOMAIN_LEN+1);

		// secure
		if(p)
		{
			while(*p && (*p==' ' || *p==';'))
				p++;
			if(p)
			{
				if(isdigit(*p))
					cookie->secure = atoi(p);
			}
		}
	}

	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CConnectedHttpSocket::ParseCookie(): received:\n\tname=%s\n\tvalue=%s\n\texpires=%s\n\tdomain=%s\n\tpath=%s\n\tsecure=%d\n\n",cookie->name,cookie->value,cookie->expires,cookie->domain,cookie->path,cookie->secure));
}

/*
	ParseInternalCookie()

	Estrae il cookie dalla stringa (letta dal file) per formattarlo nei campi della struttura.
	Utilizzata internamente dato che la stringa non contiene il cookie nel formato standard (ricevuto
	dall'host), ma secondo quello interno (letto dal file):
	
	<www.somehost.com>;/;<CookieName>=<CookieValue>;Tue, 21-May-2013 13:34:02 GMT;0
*/
void CHttpConnection::ParseInternalCookie(LPCSTR lpcszCookie,COOKIE* cookie)
{
	int i,n;
	register char* p = (char*)lpcszCookie;

	memset(cookie,'\0',sizeof(COOKIE));

	if(p)
	{
		// domain
		for(i = 0,n = 0; p[i] && p[i]!=' ' && p[i]!=';' && n < COOKIE_DOMAIN_LEN; i++,n++)
			cookie->domain[n] = p[i];
		strrtrim(cookie->domain);

		// path
		for(; p[i] && (p[i]==' ' || p[i]==';');)
			i++;
		for(n = 0; p[i] && p[i]!=' ' && p[i]!=';' && n < COOKIE_PATH_LEN; i++,n++)
			cookie->path[n] = p[i];
		strrtrim(cookie->path);
		
		// name
		for(; p[i] && (p[i]==' ' || p[i]==';');)
			i++;
		for(n = 0; p[i] && p[i]!=' ' && p[i]!='=' && n < COOKIE_NAME_LEN; i++,n++)
			cookie->name[n] = p[i];
		strrtrim(cookie->name);
		
		// value
		for(; p[i] && (p[i]==' ' || p[i]=='=');)
			i++;
		for(n = 0; p[i] && p[i]!=' ' && p[i]!=';' && n < COOKIE_VALUE_LEN; i++,n++)
			cookie->value[n] = p[i];
		strrtrim(cookie->value);

		// expires
		memset(cookie->expires,' ',COOKIE_EXPIRES_LEN-1);
		for(; p[i] && (p[i]==';' || p[i]==' ');)
			i++;
		for(n = 0; p[i] && p[i]!=';' && n < COOKIE_EXPIRES_LEN; i++,n++)
			cookie->expires[n] = p[i];
		strrtrim(cookie->expires);

		// secure
		for(; p[i] && (p[i]==' ' || p[i]==';');)
			i++;
		cookie->secure = atoi(p+i);
	}

	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CConnectedHttpSocket::ParseInternalCookie(): loaded:\n\tname=%s\n\tvalue=%s\n\texpires=%s\n\tdomain=%s\n\tpath=%s\n\tsecure=%d\n",cookie->name,cookie->value,cookie->expires,cookie->domain,cookie->path,cookie->secure));
}

/*
	SetCookie()

	Imposta il cookie.
	Il cookie viene impostato nella lista interna e salvato su file solo alla chiusura della sessione.
*/
void CHttpConnection::SetCookie(COOKIE* cookie)
{
	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CConnectedHttpSocket::SetCookie(): received:\n\tname=%s\n\tvalue=%s\n\texpires=%s\n\tdomain=%s\n\tpath=%s\n\tsecure=%d\n",cookie->name,cookie->value,cookie->expires,cookie->domain,cookie->path,cookie->secure));
	
	COOKIE* c;
	ITERATOR iter;
	BOOL bExist = FALSE;

	// cerca il cookie nella lista, se lo trova lo aggiorna altrimenti lo inserisce
	if((iter = m_listCookies.First())!=(ITERATOR)NULL)
	{
		do
		{
			if((c = (COOKIE*)iter->data)!=(COOKIE*)NULL)
			{
				if(	stricmp(cookie->domain,c->domain)==0 &&
					stricmp(cookie->name,c->name)==0     &&
					stricmp(cookie->path,c->path)==0
					)
					{
						strcpyn(c->value,cookie->value,COOKIE_VALUE_LEN);
						strcpyn(c->expires,cookie->expires,COOKIE_EXPIRES_LEN);
						c->secure = cookie->secure;
						bExist = TRUE;
						TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CConnectedHttpSocket::SetCookie(): updated\n"));
						break;
					}
			}

			iter = m_listCookies.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}
	
	if(!bExist)
	{
		c = (COOKIE*)m_listCookies.Add();
		if(c)
		{
			memcpy(c,cookie,sizeof(COOKIE));
			TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CConnectedHttpSocket::SetCookie(): inserted\n"));
		}
	}
}

/*
	GetCookies()

	Carica dal file i cookies relativi al dominio/pathname specificati.
	Da chiamare una sola volta a inizio sessione (connessione).
*/
int CHttpConnection::GetCookies(LPCSTR lpcszHost,LPCSTR lpcszPath)
{
	COOKIE* c;
	COOKIE cookie;
	int nCookies = 0;
	CTextFile cookieFile;
	char szBuffer[COOKIE_MAX_LEN+32];

	TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CConnectedHttpSocket::GetCookies(): host: %s, path: %s\n",lpcszHost,lpcszPath));

	// per l'accesso al file dei cookies
	if(m_pmutexCookiesFile)
		if(!m_pmutexCookiesFile->Lock())
			return(-1);

	// elimina i cookies dalla lista per i campi dell'header http
	DeleteHeader("Cookie");

	// apre il file per i cookies
	if(cookieFile.Open(m_szCookieFileName,FALSE,GENERIC_READ))
	{
		cookieFile.GoTop();
		
		// legge per linee caricando i cookies presenti
		while(cookieFile.ReadLine(szBuffer,sizeof(szBuffer)-1)!=FILE_EOF)
		{
			// carica il cookie nella struttura interna
			ParseInternalCookie(szBuffer,&cookie);

			// imposta/ricava il nome dell'host
			char szHost[HOSTNAME_SIZE+1];
			strcpyn(szHost,lpcszHost,sizeof(szHost));
			char* pHost = strstr(szHost,"://");
			if(pHost)
			{
				pHost += 3;
				int n = strlen(pHost);
				memmove(szHost,pHost,n);
				szHost[n] = '\0';
			}
			//if(isdigit(szHost[0]))
			//	strcpyn(szHost,GetHostByAddr(szHost),sizeof(szHost));

			// confronta dominio e pathname con quelli del cookie, se soddisfano carica il cookie nella lista
			if(stristr(szHost,cookie.domain))
			{
				if(stristr(lpcszPath,cookie.path))
				{
					// cerca il cookie nella lista
					ITERATOR iter;
					BOOL bFound = FALSE;
					if((iter = m_listCookies.First())!=(ITERATOR)NULL)
					{
						do
						{
							if((c = (COOKIE*)iter->data)!=(COOKIE*)NULL)
							{
								if(	stricmp(cookie.domain,c->domain)==0 &&
									stricmp(cookie.name,c->name)==0     &&
									stricmp(cookie.path,c->path)==0
									)
									{
										bFound = TRUE;
										break;
									}
							}

							iter = m_listCookies.Next(iter);
						
						} while(iter!=(ITERATOR)NULL);
					}

					// lo inserisce se non lo trova (e se non e' scaduto)
					if(!bFound && !IsCookieExpired(&cookie))
					{
						c = (COOKIE*)m_listCookies.Add();
						if(c)
						{
							// aggiunge il cookie alla lista interna
							memcpy(c,&cookie,sizeof(COOKIE));
							TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CConnectedHttpSocket::GetCookies(): added:\n\tname=%s\n\tvalue=%s\n\texpires=%s\n\tdomain=%s\n\tpath=%s\n\tsecure=%d\n",c->name,c->value,c->expires,c->domain,c->path,c->secure));

							// aggiunge il cookie alla lista per i campi dell'header http
							_snprintf(szBuffer,sizeof(szBuffer)-1,"%s=%s",cookie.name,cookie.value);
							AddHeader("Cookie",szBuffer);
							nCookies++;
						}
					}
				}
			}
		}
		
		cookieFile.Close();
	}

	// per l'accesso al file dei cookies
	if(m_pmutexCookiesFile)
		m_pmutexCookiesFile->Unlock();

	return(nCookies);
}

/*
	UpdateCookies()

	Aggiorna il contenuto del file per i cookies.
*/
BOOL CHttpConnection::UpdateCookies(void)
{
	COOKIE* c;
	COOKIE* cF;
	COOKIE cookie;
	ITERATOR iter;
	ITERATOR iterF;
	BOOL bUpdated = FALSE;
	BOOL bFound = FALSE;
	CTextFile cookieFile;
	char szBuffer[COOKIE_MAX_LEN+32];
	CCookiesList fileCookiesList;

	// per l'accesso al file dei cookies
	if(m_pmutexCookiesFile)
		if(!m_pmutexCookiesFile->Lock())
			return(-1);

	// elimina i cookies dalla lista per i campi dell'header http
	DeleteHeader("Cookie");

	// carica i cookies presenti nel file nella lista temporanea
	if(cookieFile.Open(m_szCookieFileName,TRUE,GENERIC_READ|GENERIC_WRITE/*,FILE_EXCLUSIVE*/))
	{
		cookieFile.GoTop();
		
		while(cookieFile.ReadLine(szBuffer,sizeof(szBuffer)-1)!=FILE_EOF)
		{
			ParseInternalCookie(szBuffer,&cookie);
			cF = (COOKIE*)fileCookiesList.Add();
			if(cF)
				memcpy(cF,&cookie,sizeof(COOKIE));
		}

		cookieFile.Close();
	}

	// cerca i cookies della lista interna nella lista temporanea caricata dal file
	// se lo trova aggiorna l'elemento della lista temporanea con quello della lista interna
	// se non lo trova lo aggiunge alla lista temporanea
	if((iter = m_listCookies.First())!=(ITERATOR)NULL)
	{
		do
		{
			bFound = FALSE;

			if((c = (COOKIE*)iter->data)!=(COOKIE*)NULL)
			{
				if((iterF = fileCookiesList.First())!=(ITERATOR)NULL)
					while(iterF!=(ITERATOR)NULL)
					{
						if((cF = (COOKIE*)iterF->data)!=(COOKIE*)NULL)
						{
							if(	stricmp(c->domain,cF->domain)==0 &&
								stricmp(c->name,cF->name)==0     &&
								stricmp(c->path,cF->path)==0
								)
								{
									memcpy(cF,c,sizeof(COOKIE));
									bFound = TRUE;
								}
						}

						iterF = fileCookiesList.Next(iterF);
					}
			}

			if(!bFound)
			{
				cF = (COOKIE*)fileCookiesList.Add();
				if(cF)
					memcpy(cF,c,sizeof(COOKIE));
			}

			iter = m_listCookies.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}

	// dopo aver sincronizzato quanto presente su file con il contenuto della
	// lista interna, scorre la lista temporanea per marcare i cookies scaduti
	if((iterF = fileCookiesList.First())!=(ITERATOR)NULL)
		while(iterF!=(ITERATOR)NULL)
		{
			if((cF = (COOKIE*)iterF->data)!=(COOKIE*)NULL)
			{
				// controlla se deve eliminare il cookie
				if(IsCookieExpired(cF))
					cF->expires[0] = ' ';
			}

			iterF = fileCookiesList.Next(iterF);
		}
	
	// ok, mo' bisogna aggiornare il file (lo riscrive per intero e vaffanculo)
	if(cookieFile.Create(m_szCookieFileName))
	{
		cookieFile.GoTop();
		
		if((iterF = fileCookiesList.First())!=(ITERATOR)NULL)
			while(iterF!=(ITERATOR)NULL)
			{
				if((cF = (COOKIE*)iterF->data)!=(COOKIE*)NULL)
				{
#if 1
					// se il file dei cookies supera una certa dimensione, l'aggiornamento
					// con il meccanismo di cui sopra comincia a diventare pesante per cui
					// elimina i cookies di sessione (?,NULL)
					if(	cF->expires[0]!=' ' && cF->expires[0]!='\0' && cF->expires[0]!='?'	&&
						cF->path[0]!='\0'											&&
						cF->name[0]!='\0'											&&
						cF->value[0]!='\0'
						)
#endif
						cookieFile.WriteFormattedLine(
												"%s;%s;%s=%s;%s;%d",
												cF->domain,
												cF->path[0]!='\0' ? cF->path : "?",
												cF->name[0]!='\0' ? cF->name : "?",
												cF->value[0]!='\0' ? cF->value : "?",
												(cF->expires[0]!=' ' && cF->expires[0]!='\0' && cF->expires[0]!='?') ? cF->expires : "?",
												cF->secure
												);
				}
				
				iterF = fileCookiesList.Next(iterF);
			}

		cookieFile.Close();

		bUpdated = TRUE;
	}
	
	// una volta aggiornato il file azzera la lista interna
	m_listCookies.RemoveAll();

	// per l'accesso al file dei cookies
	if(m_pmutexCookiesFile)
		m_pmutexCookiesFile->Unlock();

	return(bUpdated);
}

/*
	IsCookieExpired()

	Verifica se il cookie e' scaduto o meno.
	Per azzerare un cookie il server puo' inviare l'expires a spazio/null, in tal caso il cookie scade a fine
	sessione e deve comunque essere inviato durante le transazioni ma non registrato alla fine della sessione.
*/
BOOL CHttpConnection::IsCookieExpired(COOKIE* cookie)
{
	BOOL bExpired = FALSE;

	// controlla se e' gia' stato marcato come scaduto
	if(cookie->expires[0]=='?' || cookie->expires[0]==' ' || cookie->expires[0]=='\0')
	{
		bExpired = TRUE;
		TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CConnectedHttpSocket::IsCookieExpired(): expired\n"));
	}

	// confronta la data/ora correnti con quelle del cookie
	if(!bExpired)
	{
		int nDays;
		long nSeconds;
		CDateTime currentDate(ANSI_SHORT);
		CDateTime cookieDate(GMT_SHORT);
		cookieDate.ConvertDate(GMT_SHORT,ANSI_SHORT,cookie->expires,NULL);

		CDateTime::GetJulianDateTimeDiff(currentDate,cookieDate,nDays,nSeconds);
		
		if(nDays > 0)
			bExpired = TRUE;
		else if(nDays < 0)
			bExpired = FALSE;
		else if(nDays==0)
			bExpired = nSeconds > 0;
		
		TRACEEXPR((_TRACE_FLAG_INFO,__NOFILE__,__NOLINE__,"CConnectedHttpSocket::IsCookieExpired():\n\tcurrent: %02d/%02d/%04d %02d:%02d:%02d\n\tcookie: %02d/%02d/%04d %02d:%02d:%02d\n\texpired: %s\n",currentDate.GetDay(),currentDate.GetMonth(),currentDate.GetYear(),currentDate.GetHour(),currentDate.GetMin(),currentDate.GetSec(),cookieDate.GetDay(),cookieDate.GetMonth(),cookieDate.GetYear(),cookieDate.GetHour(),cookieDate.GetMin(),cookieDate.GetSec(),bExpired ? "TRUE" : "FALSE"));
	}

	return(bExpired);
}

/*
	EnumCookies()

	Enumera i cookies inseriti nella lista interna
	(non la lista per i cookies, ma quella usata per i campi dell'header http).
*/
HTTPHEADER* CHttpConnection::EnumCookies(void)
{
	static int i = -1;
	HTTPHEADER* httpheader = NULL;
	ITERATOR iter;
next:
	if(++i >= m_listHttpHeaders.Count())
	{
		i = -1;
		httpheader = NULL;
	}
	else
	{
		// enumera solo i cookies
		if((iter = m_listHttpHeaders.FindAt(i))!=(ITERATOR)NULL)
		{
			if((httpheader = (HTTPHEADER*)iter->data)!=(HTTPHEADER*)NULL)
				if(stricmp(httpheader->name,"Cookie")!=0)
				{
					httpheader = NULL;
					goto next;
				}
		}
	}

	return(httpheader);
}

/*
	EnumHeaders()

	Enumera gli headers inseriti nella lista interna.
	(quella usata per i campi dell'header http).
*/
HTTPHEADER* CHttpConnection::EnumHeaders(void)
{
	static int i = -1;
	HTTPHEADER* httpheader = NULL;
	ITERATOR iter;
next:
	if(++i >= m_listHttpHeaders.Count())
	{
		i = -1;
		httpheader = NULL;
	}
	else
	{
		// non enumera i cookies
		if((iter = m_listHttpHeaders.FindAt(i))!=(ITERATOR)NULL)
		{
			if((httpheader = (HTTPHEADER*)iter->data)!=(HTTPHEADER*)NULL)
				if(stricmp(httpheader->name,"Cookie")==0)
				{
					httpheader = NULL;
					goto next;
				}
		}
	}

	return(httpheader);
}

/*
	AddHeader()

	Aggiunge un campo alla lista interna
	(quella usata per i campi dell'header http).
*/
BOOL CHttpConnection::AddHeader(LPCSTR lpcszName,LPCSTR lpcszValue,BOOL bAllowDuplicates/*=TRUE*/)
{
	HTTPHEADER* httpheader;
	ITERATOR iter;
	BOOL bFound = FALSE;

	if(!bAllowDuplicates)
		if((iter = m_listHttpHeaders.First())!=(ITERATOR)NULL)
		{
			do
			{
				if((httpheader = (HTTPHEADER*)iter->data)!=(HTTPHEADER*)NULL)
					if(stricmp(lpcszName,httpheader->name)==0)
						if(stricmp(lpcszValue,httpheader->value)==0)
						{
							bFound = TRUE;
							break;
						}

				iter = m_listHttpHeaders.Next(iter);
			
			} while(iter!=(ITERATOR)NULL);
		}
		
	if(!bFound)
	{
		httpheader = (HTTPHEADER*)m_listHttpHeaders.Add();
		if(httpheader)
		{
			strcpyn(httpheader->name,lpcszName,HTTPHEADER_NAME_LEN+1);
			strcpyn(httpheader->value,lpcszValue,HTTPHEADER_VALUE_LEN+1);
		}
	}

	return(!bFound);
}

/*
	DeleteHeader()

	Elimina un campo alla lista interna
	(quella usata per i campi dell'header http).
*/
void CHttpConnection::DeleteHeader(LPCSTR lpcszName)
{
	HTTPHEADER* httpheader;
	ITERATOR iter;

	if((iter = m_listHttpHeaders.First())!=(ITERATOR)NULL)
	{
		do
		{
			if((httpheader = (HTTPHEADER*)iter->data)!=(HTTPHEADER*)NULL)
				if(stricmp(lpcszName,httpheader->name)==0)
					m_listHttpHeaders.Remove(iter);

			iter = m_listHttpHeaders.Next(iter);
		
		} while(iter!=(ITERATOR)NULL);
	}
}

/*
	DeleteAllHeaders()

	Elimina tutti i campi della lista interna
	(quella usata per i campi dell'header http).
*/
void CHttpConnection::DeleteAllHeaders(void)
{
	m_listHttpHeaders.RemoveAll();
}

/*
	GetErrorString()

	Restituisce la stringa relativa al codice d'errore (http o socket).
*/
LPCSTR CHttpConnection::GetErrorString(void)
{
	char* p;
	if(IsSocketError())
		p = m_szSocketCode;
	else if(IsHttpError())
		p = m_szHttpCode;
	else
		p = "no error";
	return(p);
}

/*
	ResetSocketError()

	Resetta il codice d'errore per il socket.
*/
void CHttpConnection::ResetSocketError(void)
{
	m_nSocketCode = 0;
	memset(m_szSocketCode,'\0',sizeof(m_szSocketCode));
}

/*
	ResetHttpError()

	Resetta il codice d'errore http.
*/
void CHttpConnection::ResetHttpError(void)
{
	m_nHttpCode = (int)HTTP_STATUS_VOID;
	memset(m_szHttpCode,'\0',sizeof(m_szHttpCode));
}

/*
	SetSocketError()

	Imposta il codice d'errore per il socket (numerico/descrizione).
*/
void CHttpConnection::SetSocketError(void)
{
	if(m_nSocketCode==0 && m_szSocketCode[0]=='\0')
	{
		m_nSocketCode = CSock::GetWSAErrorNumber();
		strcpyn(m_szSocketCode,CSock::GetWSAErrorString(),sizeof(m_szSocketCode));
	}
}

/*
	GetHttpErrorString()

	Restituisce la stringa relativa al codice d'errore (http o socket).
*/
LPCSTR CHttpConnection::GetHttpErrorString(int nCode)
{
	return(GetHttpCodeDesc(nCode));
}
