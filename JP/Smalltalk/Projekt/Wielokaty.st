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

<body package="Wielokaty" selector="nazwa">nazwa

<body package="Wielokaty" selector="nazwa:">nazwa: nowa_nazwa
</methods>

<methods>
<class-id>Wielokat</class-id> <category>actions</category>

<body package="Wielokaty" selector="obrot">obrot

<body package="Wielokaty" selector="drukuj">drukuj
</methods>

<methods>
<class-id>Wielokat</class-id> <category>initialize-release</category>

<body package="Wielokaty" selector="initialize:name:">initialize: liczbaWierzcholkow name: nowaNazwa
</methods>


<methods>
<class-id>TrojkatRownoramienny</class-id> <category>actions</category>

<body package="Wielokaty" selector="pole">pole
</methods>

<methods>
<class-id>TrojkatRownoramienny</class-id> <category>arithmetic</category>

<body package="Wielokaty" selector="+">+ figura
</methods>

<methods>
<class-id>TrojkatRownoramienny</class-id> <category>initialize-release</category>

<body package="Wielokaty" selector="initialize:and:">initialize: podstawa and: wysokosc
</methods>



</st-source>