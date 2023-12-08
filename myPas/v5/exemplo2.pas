program exemplo1 (input, output);
	{ Aqui começa o nível léxico 1 }
	var Abacaxi, abacaxi: integer;
		xuxu, goiaba: double;
		x, y, z: real;

	procedure frutas (x, y, z: double);
		{ Aqui começa o nível léxico 2, inclusive os parâmetros }
		var goiaba, tangerina: real;

		begin
			goiaba := 3.14159;
			tangerina := 2.71828
		end;

	function factorial (n: integer): integer;
		begin
			if n = 0 then factorial := 1
			else if n < 2 then factorial := n
			else factorial := factorial(n - 1) * n
		end;

	{de volta ao nível léxico 1}
	begin
		if x > 3 then
		begin
			goiaba := 3;
			x := 4
		end
		else
			z := 1.2356189
	end.

blah blah blah

uifhsgfshdfgkjasfg hjdgajksdgjhgdas

khgfjksafgbjkhfdkads
