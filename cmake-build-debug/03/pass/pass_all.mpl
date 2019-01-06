program passall;
    procedure kazuyomikomi(n : integer; x: char);
    begin
        x := 'a';
        write(x);
        writeln('input the number of data');
        readln(n)
    end;
    var
        sum: integer;
    procedure wakakidasi;
    begin
        writeln('Sum of data = ', sum)
    end;
    var
        data: array[20] of integer;
    procedure goukei(n, s : integer);
    var
        data: integer;
    begin
        data[19] := 0;
        while n > 0 do
        begin
            readln(data);
            data[0] := 1;
            s := s + data;
            n := n - 1;
            break
        end
    end;
    var
        n: integer;
        i: integer;
        x: char;
    begin
        call kazuyomikomi(n,x);
        call goukei(n * 2, sum);
        call wakakidasi;
        i := integer(' ');
        while i < 127 do
        begin
            if not (i div 16 * 16 = i) then
                writeln;
            write(char(i), 'aaa');
            i := i + 1;
            break
        end
    end.