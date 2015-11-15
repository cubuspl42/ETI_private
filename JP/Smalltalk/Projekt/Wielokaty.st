<?xml version="1.0"?>

<st-source>
<time-stamp>From VisualWorks® Personal Use Edition, 8.1 of 24 lipca 2015 on 14 listopada 2015 at 17:19:01</time-stamp>


<component-created>
<name>Wielokaty</name> <type>package</type>
</component-created><!-- Package Wielokaty* -->


<component-created>
<name>Wielokaty</name> <type>package</type>
</component-created>

<class>
<name>Wielokat</name>
<environment>Smalltalk</environment>
<super>Core.Object</super>
<private>false</private>
<indexed-type>none</indexed-type>
<inst-vars>wierzcholki nazwa </inst-vars>
<class-inst-vars></class-inst-vars>
<imports></imports>
<category>JezykiProgramowania</category>
<attributes>
<package>Wielokaty</package>
</attributes>
</class>

<class>
<name>TrojkatRownoramienny</name>
<environment>Smalltalk</environment>
<super>Wielokat</super>
<private>false</private>
<indexed-type>none</indexed-type>
<inst-vars></inst-vars>
<class-inst-vars></class-inst-vars>
<imports></imports>
<category>JezykiProgramowania</category>
<attributes>
<package>Wielokaty</package>
</attributes>
</class>

<methods>
<class-id>Wielokat</class-id> <category>accessing</category>

<body package="Wielokaty" selector="nazwa">nazwa	"podaje nazwe wielokata"	^nazwa</body>

<body package="Wielokaty" selector="nazwa:">nazwa: nowa_nazwa	"ustawia nowa nazwe wielokata"	nazwa:=nowa_nazwa</body>
</methods>

<methods>
<class-id>Wielokat</class-id> <category>actions</category>

<body package="Wielokaty" selector="obrot">obrot	"obraca wielokąt o 90 stopni w prawo"	wierzcholki:=wierzcholki collect: [ :p | (p y negated)@(p x)]</body>

<body package="Wielokaty" selector="drukuj">drukuj	"wypisuje wierzchołki i pole wielokąta"	Transcript show: 'Wierzcholki: '.	wierzcholki do: [:p | Transcript show: (p x printString), '@', (p y printString), ' '].	Transcript cr.	Transcript show: 'Pole: ', (self pole printString).	Transcript cr.</body>
</methods>

<methods>
<class-id>Wielokat</class-id> <category>initialize-release</category>

<body package="Wielokaty" selector="initialize:name:">initialize: liczbaWierzcholkow name: nowaNazwa	"konstruktor obiektu - wielokata"	nazwa:=nowaNazwa.	wierzcholki:=Array new: liczbaWierzcholkow.	wierzcholki at: 1 put: 0@0.</body>
</methods>


<methods>
<class-id>TrojkatRownoramienny</class-id> <category>actions</category>

<body package="Wielokaty" selector="pole">pole	"licz pole trójkąta równoramiennego"		| spodek podstawa wysokosc |		spodek:=(wierzcholki at: 2) / 2.	podstawa:=(wierzcholki at: 1) dist: (wierzcholki at: 2).	wysokosc:=spodek dist: (wierzcholki at: 3).		^(podstawa * wysokosc) / 2</body>
</methods>

<methods>
<class-id>TrojkatRownoramienny</class-id> <category>arithmetic</category>

<body package="Wielokaty" selector="+">+ figura	"dodaj 2 figury w sensie pola"		| spodek podstawa wysokosc pole k |		spodek:=(wierzcholki at: 2) / 2.	podstawa:=(wierzcholki at: 1) dist: (wierzcholki at: 2).	wysokosc:=spodek dist: (wierzcholki at: 3).	pole:=self pole + figura pole.	k:=pole / (self pole).	k:= k sqrt.		^(TrojkatRownoramienny new) initialize: podstawa * k and: wysokosc * k</body>
</methods>

<methods>
<class-id>TrojkatRownoramienny</class-id> <category>initialize-release</category>

<body package="Wielokaty" selector="initialize:and:">initialize: podstawa and: wysokosc	"tworzy trójkąt równoramienny o danej podstawie i wysokości"	super initialize: 3 name: 'Trojkat rownoramienny'.	wierzcholki at: 2 put: podstawa@0.	wierzcholki at: 3 put: (podstawa/2)@wysokosc.</body>
</methods>



</st-source>
