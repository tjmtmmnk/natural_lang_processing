program sample11pp;
    procedure kazuyomikomi(n : integer; x: boolean);
    begin
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
        data: array[20] of integer; /*global*/
    procedure goukei(n, s : integer);
    var
        data: integer; /*local*/
    begin
        data[0] := 0;
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
        n: integer;x: boolean;
    begin
        call kazuyomikomi(n,x);
        call goukei(n * 2, sum);
        call wakakidasi
    end.