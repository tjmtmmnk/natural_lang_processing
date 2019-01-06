program sample11pp;
    procedure kazuyomikomi(n : integer);
    begin
        writeln('input the number of data');
        call kazuyomikomi(n);
        readln(n)
    end;
    var
        n: integer;
    begin
        call kazuyomikomi(n);
    end.