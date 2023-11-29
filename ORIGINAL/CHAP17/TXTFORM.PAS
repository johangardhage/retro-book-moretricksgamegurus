program text_format;
{This program formats text for use by the viewer, wrapping words.}

{We'll need to move the cursor.}
uses crt;

{We'll only use the first 79 columns. Otherwise it automatically wraps at 80}
const rmargin=79;

var docname:string;                     {The name of the file.}

    tarr:array[1..rmargin+2] of char;   {A temporary line buffer.}

    j,                                  {Temporary variable.}

    numbytes,                           {Number of bytes in current line.}

    indent:integer;                     {Level of indentation}

    df:file of char;                    {The output file (readme.txt)}

    tf:text;                            {The input documentation file.}

    ok:boolean;                         {Temp variable to decide when to stop}

{This procedure flushes the temporary line buffer to the file.}
procedure flush;       
begin

     {Write each byte that is in use.}
     for j:=1 to numbytes do begin
         write(tf,tarr[j]);
     end;

     {Reset the characters in the line.}
     numbytes:=0;

     {Update our status indicator.}
     gotoxy(14,wherey);
     write(filepos(df)/filesize(df)*100:3:0,'%');

end;


begin
     {Initialize variables.}
     numbytes:=0;
     indent:=0;

     {The first parameter is the document name. If not, then give help.}
     if paramcount>0 then docname:=paramstr(1)
     else begin
          writeln('TXTFORM - Formats a text file, to display with README.');
          writeln('        Specify the name of the file as the parameter.');
          writeln('        The output will be in the file README.TXT');
          halt(1);
     end;

     {Initialize the files.}
     assign(df,docname);
     reset(df);
     assign(tf,'readme.txt');
     rewrite(tf);

     {Let the user know what we're doing...}
     write('Converting... ');

     {Main loop}
     while not eof(df) do begin

           {Read one character from the file.}
           inc (numbytes);
           read(df,tarr[numbytes]);

           {If it's a tab, convert it to spaces and keep track of the level}
           {of indent so that it indents all paragraphs.}
           if tarr[numbytes]=#9 then begin

              inc(indent);
              for numbytes:=numbytes to numbytes+7 do tarr[numbytes]:=' ';

           end;

           {If it's a carriage return, then...}
           if tarr[numbytes]=#13 then begin

              {New paragraph-reset indent.}
              indent:=0;

              {Read next byte...}
              inc(numbytes);
              read(df,tarr[numbytes]);

              {If it wasn't a line feed, then}
              if tarr[numbytes]<>#10 then begin

                 {Put an LF in.}
                 tarr[numbytes+1]:=#10;
                 seek(df,filepos(df)-1);

              end;

              {Send this line to the file.}
              flush;
           end;


           {If we have reached the end of the line then...}
           if numbytes>rmargin then begin

              {If the last character was whitespace, then we're OK.}
              if tarr[numbytes]<#33 then ok:=true

              {Otherwise we need to wrap it.}
              else ok:=false;

              {Don't quit until we're done.}
              while not ok do begin

                    {Read back one character in the line.}
                    dec(numbytes);

                    {As long as we're not at the start of the line, then}
                    if numbytes>0 then begin

                       {If we've found whitespace, then we're done.}
                       if tarr[numbytes]<#33 then ok:=true;

                    end

                    {If we're at the beginning of the line, and haven't}
                    {found whitespace yet, then we can't wrap it.}
                    else ok :=true;

              end;

              {If we stopped before the beginning of the line...}
              if numbytes>0 then begin

                 {Then start the next line where we found a break, and}
                 seek(df,filepos(df)-(rmargin+1-numbytes));

                 {Put in a CR/LF.}
                 tarr[numbytes]:=#13;tarr[numbytes+1]:=#10;
                 inc(numbytes);

              end

              {If we went all the way to the start of the line,}
              {then just continue from where we left off.}
              else begin
                   seek(df,filepos(df)-1);
                   numbytes:=rmargin+2;
                   tarr[rmargin+1]:=#13;tarr[rmargin+2]:=#10;
              end;

              {Send this line to the file.}
              flush;

              {If we're indented, start the next line with 8 spaces.}
              if indent>0 then for numbytes:=1 to indent*8 do
                 tarr[numbytes]:=' ';

           end;

     end; {End of main loop.}

     {Close our files.}
     writeln(tf);
     writeln(tf);
     close(tf);
     close(df);

end.