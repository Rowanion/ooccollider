%
% 1. Krimskrams
%
% 1.1 Options
%
\newif\if@title \@titletrue
\newif\if@table \@tabletrue
\newif\if@wide  \@widefalse
\DeclareOption{notitle}{\global\@titlefalse}
\DeclareOption{notable}{\global\@tablefalse}
\DeclareOption{wide}{\global\@widetrue}
%
% 1.2 Scheiße (nicht so viel LaTeX-Geblubber)
%
\let\oldwarn=\@@warning
\def\@@warning#1{}
\input article.cls % Grundstyle
\let\@@warning=\oldwarn
\fontencoding{T1}
\renewcommand{\encodingdefault}{T1}
\selectfont
\catcode223=13 \chardef^^df255
\usepackage{german}
\def\date{\@date}
%
% 2. Pagestyle 
%
% 2.1 Format
%     ganz wie es die Literaten haben wollen:
%  
\oddsidemargin  -1mm     %   Left margin on odd-numbered pages.
\evensidemargin -1mm     %   Left margin on even-numbered pages.
\marginparwidth 1 in        %   Width of marginal notes.
\textwidth 14.1cm % Width of text line.
\textheight 23.4cm
\advance\voffset by -1.2cm
\clubpenalty4000
\widowpenalty4000
\parskip0pt plus 5pt
\if@wide
  \advance\textwidth by 3cm
\else
  \renewcommand{\baselinestretch}{1.25}
\fi
%
% 2.2 Headings (muß noch mal schöner werden)
%
\let\psc=\sc
\def\ps@niceheadings{\let\@mkboth\markboth
  \def\@oddfoot{\small\hfil\rm\thepage}\def\@evenfoot{}
  \def\@oddhead{\small\hfil\rightmark\hfil}% Heading.
  \def\sectionmark##1{\markright{\ifnum \c@secnumdepth >\z@
     \thesection\hskip 1em\relax \fi {\psc ##1}}}}
%
\pagestyle{niceheadings}
%
% 3. User-Macros
%
% 3.1. Abbildungen (mit Einklebemarke und Nr.)
%
\newcounter{abbc}\setcounter{abbc}{1}
\def\theabbc{\arabic{abbc}}
% 
\def\rando{%
    \vskip2mm
    \hbox to \hsize{\hfil .\hfil}}
\def\randu{%
    \hbox to \hsize{\hfil .\hfil}
    \vskip2mm}
%
% 3.1.1 fake macht Kasten
%
\def\fake{\def\rando{\hrule}\def\randu{\hrule}}
%
% 3.1.2 Abb. Label #1  Untertitel #2 Höhe #3 Breite #4
%
\def\abbdef#1#2#3#4{%
    \par
    \penalty200
    \vfill
    \par
    \penalty-1000
    \null
    \vfill
    \hbox to \hsize{
       \hfil\vbox to #3cm{
           \hsize=#4cm\rando\vfill
               \hbox to \hsize{\hskip2mm . \hfil .\hskip2mm}
       \vfill \randu}\hfil}
    \vskip5pt
    \hbox to \hsize{\hfil \bf Abbildung \theabbc: \hfil}
    \vskip5pt
    \hbox to \hsize{\hfil \vbox{\small \hsize=9cm \noindent #2} \hfil} 
    {\refstepcounter{abbc}\label{AbbNr#1}}
    \label{Seite#1}
    \vfill
    \par
    \penalty200
    \vfill
    \par}
%
% 3.1.3 Referenz via \abbref{Label}
% 
\def\abbref#1{Abb.~\ref{AbbNr#1}, S.~\ref{Seite#1}}
%
% 3.2 Zitate (verbiegt quote)
%
\let\oldquote=\quote
\let\oldendquote=\endquote
%
\def\quote{%
  \oldquote
  \small\parskip0pt plus 8pt\parindent1em}
%
\def\endquote{%
  \oldendquote}
%
% 3.3 Table of contents
%
\def\tableofcontents{%
   \section*{\contentsname
     \@mkboth{\psc\contentsname}{\psc\contentsname}}
  \@starttoc{toc}
  \newpage}
\def\ds@notable{\def\tableofcontents{}}
%
% 3.4 Literatur
%
\def\literaturlabel#1{\hspace\labelsep {#1}}
\def\literatur{%
  \newpage
  \gdef\thesection{\Alph{section}}
  \setcounter{section}{0}
  \section{Literaturverzeichnis}
  \flushleft
%
% 3.4.1 new \book AUTHOR: Titel.. blubber
%
  \def\book##1:##2..{%
     \item[\sc{##1:}]\it{##2}. }
%
% 3.4.2 new \arti AUTHOR: Titel In: AUTHOR: Titel.. blubber
%
  \def\arti##1:##2 In:##3: ##4..{%
     \item[\sc{##1:}]\it{##2} In: \sc{##3:} \it{##4}. }
%
% 3.4.3 new \jour AUTHOR: Titel In: Titel.. blubber
%
  \def\jour##1:##2 In:##3..{%
     \item[\sc{##1:}]\it{##2} In: \it{##3}. }
%
     \list{}{\labelwidth\z@ \itemindent-\leftmargin
       \let\makelabel\literaturlabel}}
%
\def\endliteratur{\endlist\endflushleft}
%
% 3.5 footnotes
%
\long\def\@footnotetext#1{\insert\footins{\reset@font\footnotesize
  \interlinepenalty\interfootnotelinepenalty
  \splittopskip\baselinestretch\footnotesep
  \splitmaxdepth \dp\strutbox \floatingpenalty \@MM
  \hsize\columnwidth \@parboxrestore
  \leftskip1em
  \edef\@currentlabel{\csname p@footnote\endcsname\@thefnmark}\@makefntext
   {\rule{\z@}{\baselinestretch\footnotesep}\ignorespaces
    #1\strut}}}
%
\long\def\@makefntext#1{\parindent 1em\noindent
  \hbox to 0pt{\hss$\m@th^{\@thefnmark}$}#1}
%
%  3.6 the line enviroment
%
\newcount\@line \@line=1
\newcount\@linemodfive \@linemodfive=1 % used to label every fifths line
%
\long\def\@linefoottext#1{\insert\footins{\reset@font\small
  \interlinepenalty\interfootnotelinepenalty
  \splittopskip1.4\footnotesep
  \splitmaxdepth \dp\strutbox \floatingpenalty \@MM
  \hsize\columnwidth \@parboxrestore
  \noindent\leftskip2em
  \parskip0pt\parindent1em
  \rule{\z@}{1.4\footnotesep}\hbox to 0pt{\hss{\footnotesize\sf\the\@line}~}\ignorespaces
  #1\strut}}
%
\def\lines{\@ifnextchar[{\@lines}{\@lines[7em]}}
%
\def\@lines[#1]{\medskip\par\small
  \leftmargin=#1
  \labelwidth=#1
  \labelsep=3em
  \advance\leftmargin by \parindent
  \advance\labelwidth by -\labelsep
%
  \parskip0pt
  \leftskip=\leftmargin
%
  \let\oldlinebreak=\\
  \def\\{\@ifnextchar[{\commentline}{\numberline}}
%
  \long\def\commentline[##1]{%
    \@linefoottext{##1}
    \numberline}
%
  \def\numberline{%
    \ifnum\@linemodfive=5
      \global\@linemodfive=0  
      \makelineno
    \else\ifnum\@line=1\makelineno\fi
    \global\advance\@line by 1 
    \global\advance\@linemodfive by 1
    \fi
    \par\noindent\ignorespaces% better than line break (no underfull hbox)
  }
%
  \def\makelineno{\strut%
    \vadjust{\hbox to 0pt{\hskip\leftmargin\vbox to 0pt{\vss
       \leftskip0pt
       \noindent
       \leavevmode\raise3.5pt\hbox to 0pt{\hss\footnotesize\sf\the\@line~}}\hss}}}
%
  \def\item[##1]{\par\noindent\hskip-\leftmargin
    \hbox to \leftmargin{%
      \hfil
      \vtop to 0pt{%
        \hsize=\labelwidth
        \leftskip0pt
        \rightskip0pt plus \labelwidth
        \parindent0pt
        \parskip5pt
        \let\\=\oldlinebreak
        ##1\vss}
       \hskip\labelsep}\ignorespaces}
%
   \noindent\ignorespaces}
%
\let\endlines=\endlist
%
%  \long\def\@mytext##1{%
%      \leftskip1.8em
%      \parindent 1em\noindent
%      \normalsize\hbox to 0pt{\hss \the\lineno~}##1}
%

%
% 4. Titelseite
%    title author 
\def\veranst#1{\def\@veranst{#1}}
\def\@veranst{\@warning{Name der Verstaltung unbekannt.}}
\def\@author{\@warning{Name der/des Authorin/Authors unbekannt.}}
\def\@title{\@warning{Titel der Arbei unbekannt.}}
\def\maketitle{%
  \begin{titlepage}%
    \setcounter{page}{0}
    \null
    \begin{flushleft}
      {\Large\@veranst~\par}%
    \end{flushleft}
\vfill
    \begin{center}%
      {\Huge\@title \par}%
       \vskip25pt
      {\large \date \par}%
    \end{center}\par
\vfill
    \begin{flushright}
     {\large
         \lineskip .75em
         \begin{tabular}[t]{l}\@author
         \end{tabular}\par}
    \end{flushright}

  \end{titlepage}%
  \setcounter{footnote}{0}}
%
% 5. Fonts
%
\let\it=\textit
\let\sl=\textsl
\let\bf=\textbf
\let\tt=\texttt
\let\sf=\textsf
\let\sc=\textsc
%
% 6. begindocument
%
\let\olddocument=\document
%
\def\document{%
  \olddocument
  \frenchspacing
  \tolerance1100
  \if@title\maketitle\fi\if@table\tableofcontents\fi}




