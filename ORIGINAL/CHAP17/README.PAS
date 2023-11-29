program display_doc;
{This program displays a documentation file.}

{We need to do screen and printer output.}
uses crt,printer;

{Declare our filename.}
const filename='readme.txt';

{Use a margin of 79 characters to avoid wrapping.}
const rmargin=79;

{Remove automatic I/O error checking.}
{We want to handle printer errors ourselves.}
{$I-}

var quit,               {Tells us when we're done.}

    print,              {Flag to log text to the printer.}

    redraw,             {Tells us whether or not to redraw the screen.}

    bright,             {Tells us if the text is highlighted.}

    tbool:boolean;      {A temporary boolean, used to ignore results of}
                        {a boolean function, if we want.}

    tchar:char;         {Holds the input key.}

    head,               {Points to the first character of the current screen.}

    thead:word;         {Temporary head pointer if we want to move around.}

    j:integer;          {Temporary loop variable.}

    blockfile:file;     {Our input file.}

    alldoc:array[1..64000] of char;      {Will hold all the text.}

    size:word;          {Size of the file.}

procedure displine;
{This procedure displays whatever line is currently pointed to by head.}

begin

     {Until we reach the end of the current line,}
     while (alldoc[head]<>#10) do begin

           {Only print it if it's not a line-feed or carriage return.}
           if alldoc[head]>#13 then begin


              {Use the double * to select a color.}
              if (alldoc[head]='*') and (alldoc[head+1]='*') then begin

                 bright:=not bright;

                 {Skip the selector for display.}
                 head:=head+2;

              end;

              {Makes sure we are printing with the right color.}
              if bright then textcolor(11) else textcolor(7);

              {Print the current character}
              write(alldoc[head]);

              {Return to default color.}
              textcolor(15);

           end;

           {Go to next byte.}
           inc(head);

     end;

     {Skip this character, it's a line feed.}
     if head<size then inc (head)

end;

procedure back1;
begin
{This procedure moves our head pointer to the previous line of text.}

     {Skip over the previous end-of-line characters.}
     if head>1 then head:=head-2;

     {Go back until we hit a line-feed.}
     while (head>1) and (alldoc[head]<>#10) do dec(head);

     {Go forward one to the next line.}
     if head>1 then inc(head);

end;

function fore1:boolean;
{This function skips to the next line of text. It returns false if it}
{encounters the end of the text and couldn't go forward.}

begin

     {Assume that it will work.}
     fore1:=true;

     {Go forward until the end of the line or the end of the text.}
     while (head<size) and (alldoc[head]<>#10) do inc(head);

     {Only at end of line, skip one to the next line.}
     inc (head);

     {If we are at the end of the text, then...}
     if head>=size-1 then begin

        {We can't move forward.}
        fore1:=false;

        {Go back to the beginning of the line.}
        back1;

     end;
end;

begin

     {Main program.}

     {Initialize our variables.}
     quit:=false;
     print:=false;
     redraw:=false;
     bright:=false;

     {Initialize the file.}
     assign(blockfile,filename);
     reset(blockfile,1);

     {Read the file in one big chunk.}
     blockread(blockfile,alldoc,64000,size);
     close(blockfile);

     {This assembly uses a BIOS routine to turn off the cursor.}
     asm
        mov ch,20h        {Turn off cursor}
        mov ax,0100h      {Subfunction one: set cursor size/shape}
        int 10h           {Video service interrupt}
     end;

     {Blue text background for status bars.}
     textbackground(1);

     {Clear the screen to blue}
     clrscr;

     {Bright white text foreground}
     textcolor(15);

     {The last line displays the control information}
     gotoxy(1,25);
     write('Keys: ',#24,'/',#25,' Arrows,',
           ' PgUp/PgDn, Home/End, P to Print, L to Log to Prn, Esc to quit');

     {Set the background color to black}
     textbackground(0);

     {Clear the main window to black.}
     window(1,2,80,24);
     clrscr;

     {Display the first 23 lines in our main window.}
     head:=1;
     for j:=1 to 23 do begin
         gotoxy(1,j);
         displine;
     end;

     {Reset pointer to start.}
     head:=1;


     {Main program loop.}
     while not quit do begin

           {Set the text window to the upper right status bar.}
           window(58,1,80,1);

           {Blue background}
           textbackground(1);

           {Display the current line, number of lines, and percent of text.}
           write(head/size*100:3:0,'% of file viewed');

           {Back to black background and normal window.}
           textbackground(0);
           window(1,2,80,24);

           {Wait until a key is pressed.}
           while not keypressed do;

           {We've got a key-what is it?}
           tchar:=readkey;

           {If it's escape, then quit.}
           if tchar=#27 then quit:=true;

           {If it's P then print everything.}
           if upcase(tchar)='P' then begin

              {First, display...}
              window(1,1,9,1);
              textbackground(1);

              {In flashing text...}
              textcolor(143);

              {The message "Printing"}
              write('Printing');

              {Now, for each character of text...}
              for j:=1 to size do begin

                  {Skip the color commands.}
                  if (alldoc[j]='*') and (alldoc[j+1]='*') then j:=j+2;

                  {Print the character. The if ioresult simply ignores any}
                  {printer errors. If you wanted to capture them, then you}
                  {could have an else statement and take care of them.}
                  if ioresult=0 then write(lst,alldoc[j]);

              end;

              {Done printing, erase the message and get normal window again.}
              clrscr;
              textcolor(15);
              textbackground(0);
              window(1,2,80,24);

           end;

           {L tells us to turn on or off logging the text to the printer.}
           if upcase(tchar)='L' then begin

              {Turn logging on, or if it is on, turn it off.}
              print:=not print;


              {If we just turned it on, then}
              if print then begin

                 {Display the flashing message, "Logging"}
                 window(1,1,8,1);
                 textbackground(1);
                 textcolor(143);
                 write('Logging');
                 textcolor(15);
                 textbackground(0);
                 window(1,2,80,24);

                 {Print the currently displayed first line.}
                 j:=head-1;

                 repeat 

                       inc(j);

                       {Skip color codes.}
                       if (alldoc[j]='*') and (alldoc[j+1]='*') then j:=j+2;

                       {Write character.}
                       write(lst,alldoc[j]);

                 {Keep going until the end of line.}
                 until alldoc[j]=#10;

                 {Ignore printer errors.}
                 j:=ioresult;
                 {If you wanted to capture printer errors, you could insert}
                 {if ioresult<>0 then begin}
                 {and an error handler.}

              end

              {If we just turned logging off...}
              else begin

                   {Clear the message.}
                   window(1,1,8,1);
                   textbackground(1);
                   clrscr;
                   textbackground(0);
                   window(1,2,80,24);

              end;

           end;

           {If the character is ASCII 0, then it was an extended keystroke.}
           if tchar=#0 then begin

              {Read the next key. This will be the scan code of the key.}
              tchar:=readkey;

              {Assume that we need to redraw the screen.}
              redraw:=true;

              {Figure out which key was pressed.}
              case tchar of

                   {PgUp has a scan code of 73.}
                   #73:if head>1 then for j:=1 to 23 do back1
                   else redraw:=false;

                   {PgDn has a scan code of 81.}
                   #81:if fore1 then for j:=1 to 22 do tbool:=fore1
                   else redraw:=false;

                   {Up Arrow has a scan code of 72.}
                   #72:begin

                       {We will take care of drawing here.}
                       redraw:=false;

                       {If we're not already at the start of the text, then}
                       if head>1 then begin

                          {Skip back one line.}
                          back1;

                          {Call BIOS routine to scroll window down one line.}
                          asm
                             mov ah,07h   {Video function to scroll down}
                             mov ch,1     {Top row of window (0-Based)}
                             mov cl,0     {Left column of window}
                             mov dh,23    {Bottom row of window}
                             mov dl,79    {Right column of window}
                             mov al,1     {Number of lines to scroll}
                             mov bh,15    {Video attribute for new line}
                             int 10h      {Video service interrupt}
                          end;

                          {Display the new line on line one of the window.}
                          gotoxy(1,1);

                          {Temporarily store the pointer so we can jump back.}
                          thead:=head;
                          displine;
                          head:=thead;

                       end;
                   end;

                   {Down Arrow has a scan code of 80.}
                   #80:begin
                       {We take care of drawing here.}
                       redraw:=false;

                       {Skip forward one line, only print if we aren't past}
                       {the end of the file.}
                       if fore1 then begin

                          {Call BIOS scroll routine.}
                          asm
                             mov ah,06h   {Video function to scroll up}
                             mov ch,1     {Top row of window (0-Based)}
                             mov cl,0     {Left column of window}
                             mov dh,23    {Bottom row of window}
                             mov dl,79    {Right column of window}
                             mov al,1     {Number of lines to scroll}
                             mov bh,15    {Video attribute for new line}
                             int 10h      {Video service interrupt}
                          end;

                          {Temporarily save current pointer.}
                          thead:=head;

                          {Read through to the start of the next line.}
                          for j:=1 to 22 do tbool:=fore1;

                          {Display the line.}
                          gotoxy(1,23);
                          displine;

                          {Restore the pointer.}
                          head:=thead;

                       end;
                   end;

                   {Home has a scan code of 71.}
                   #71:{Easy-just set the pointer to the start of the text.}
                   head:=1;

                   {End has a scan code of 79.}
                   #79:begin
                       {Go the end of the text.}
                       head:=size;

                       {Go back so there's something on the screen}
                       back1;
                       back1;
                       back1;
                   end;

                   {It was another extended key, do nothing, no redraw}
                   else redraw:=false;

              end;{case}

              if redraw then begin

                 clrscr;

                 {Save our position}
                 thead:=head;

                 {Draw 23 lines.}
                 for j:=1 to 23 do begin
                     gotoxy(1,j);
                     displine;
                 end;

                 {Restore pointer.}
                 head:=thead;
              end;

              {If logging is on, then write the top line to the printer.}
              if print then begin

                 {Print the currently displayed first line.}
                 j:=head-1;
                 repeat
                       inc(j);
                       if (alldoc[j]='*') and (alldoc[j+1]='*') then j:=j+2;
                       write(lst,alldoc[j]);
                 until alldoc[j]=#10;

                 {Insert printer error handler here, if desired.}
                 j:=ioresult;

              end;


           end;

     end;

     {We're done with everything.}

     {Clear the entire screen.}
     window(1,1,80,25);
     textbackground(0);
     textcolor(7);
     clrscr;

     {Turn the cursor back on.}
     asm
        mov cx,1314h
        mov ax,0100h
        int 10h
     end;

end.
