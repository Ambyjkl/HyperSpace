\LoadClass{article}
\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{my_cv}[2011/03/26 My custom CV class]
\RequirePackage{titlesec}
\usepackage[hmargin=1.25cm, vmargin=0.75cm]{geometry}
\usepackage[usenames,dvipsnames]{xcolor}
\definecolor{heading}{HTML}{9C29B3}
\definecolor{sub}{HTML}{595959}
\definecolor{normal}{HTML}{000000}
\definecolor{blue}{HTML}{1A3D8F}

\usepackage[absolute]{textpos}
\usepackage[UKenglish]{isodate}
\setlength{\TPHorizModule}{1mm}
\setlength{\TPVertModule}{1mm}
\newcommand{\lastupdated}{
  \begin{textblock}{60}(153,3)
    {\scriptsize\color{sub}\fontfamily{pag}\selectfont
    Last Updated on \today}
  \end{textblock}
}

\titleformat{\section}                            % Customise the \section command
  {\color{blue}\fontfamily{pag}\selectfont\Large\raggedright}   % Make the \section headers large (\Large),
                                                  % small capitals (\scshape) and left aligned (\raggedright)
  {}{0em}                                         % Can be used to give a prefix to all sections, like 'Section ...'
  {\color{blue}\rule[3pt]{0.5in}{4pt}  }                        % Can be used to insert code before the heading
  %[\titlerule]

\titleformat{\subsection}
  {\fontfamily{pag}\selectfont\Large\raggedright\color{sub}}
  {}{0em}
  {}


\newcommand{\datedsection}[2]{%
  \section[#1]{#1 \hfill #2}%
}

\newcommand{\sub}[1]{%
  {\fontfamily{pag}\selectfont\color{sub}{#1}}
}

\newcommand{\uni}[1]{%
  \color{sub} {#1}
}

\newcommand{\job}[4]{
    \fontfamily{pag}\selectfont{\textbf{#1}, \textit{#2}, {#3}\hfill\textbf{#4}}
}
\newcommand{\jobs}[4]{
    \fontfamily{pag}\selectfont{\textbf{#1}\textit{#2}, {#3}\hfill\textbf{#4}}
}
\newcommand{\ed}[4]{
    \fontfamily{pag}\selectfont{\textbf{#1}\hfill{#2} \\{#3}\hfill{#4}}
}
\newcommand{\sectionsep}{\vspace{8pt}}
\newcommand{\sectionsepnew}{\vspace{3pt}}

\newcommand{\grey}[1]{
  \textbf\color[sub]{#1}
}

\newcommand{\datedsubsection}[4]{%
  \large\subsection[#1]{#1 \\ \uni{#3 \\ \textbf{#4}} \\ \color{normal}{#2}}%
}

\newcommand{\name}[1]{
  \centerline{\fontfamily{pag}\selectfont\huge\color{blue}{#1}}
  \rule{18cm}{0.4pt} \\
}

\newcommand{\text}[1]{
    {\fontfamily{pag}\selectfont\color{normal}{#1}}
}

\newcommand{\contactline}[6]{
  {\fontfamily{pag}\selectfont{#1}} \hfill {\fontfamily{pag}\selectfont{#4}} \\
  {\fontfamily{pag}\selectfont{#2}} \hfill {\fontfamily{pag}\selectfont{#5}} \\
  {\fontfamily{pag}\selectfont{#3}} \hfill {\fontfamily{pag}\selectfont{#6}} \\
  %\rule{18cm}{0.4pt} \\
}

\newcommand{\subpoint}[1]{
  {\fontfamily{pag}\selectfont\color{sub}{\textbullet{}} {#1}}\\
}
\newcommand{\duration}[1]{
  {\fontfamily{pag}\selectfont\color{sub} #1}
}

\newcommand{\gen}[1]{
  {\color{normal}\fontfamily{pag}\selectfont\uni {#1}}
}

\newcommand{\subs}[1]{
  \color{normal}\textbf{#1}
}

\newcommand{\contact}[5]{
    \centerline{#1 {\large\textperiodcentered} #2 {\large\textperiodcentered} #3 {\Large\textperiodcentered} #4  {\Large\textperiodcentered} #5}
    \noindent\rule{\paperwidth}{0.4pt}
}
\newcommand{\longcontact}[5]{
    \center{#1 \hfill {\large\textperiodcentered} \hfill #2 \hfill {\large\textperiodcentered} \hfill #3}
    \center{ {\Large} #4 \hfill {\Large} #5}
}
\newcommand{\contactsec}[5]{
    {#1}
    \vspace{1mm}\\
    {#2}
    \vspace{1mm}\\
    {#3}
    \vspace{1mm}\\
    {#4}
    \vspace{1mm}\\
    {#5}
}
