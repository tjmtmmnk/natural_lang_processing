program checkbreak;
    var n,i : integer;
begin
	n := 0;
	i := 10;
	while i > 0 do
	begin
	    if n >= 5 then
	        break;
	    n := n + 1;
	    i := i - 1
	end;
	writeln(n)
end.