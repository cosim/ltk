package.cpath = package.cpath..[[;E:\myworks\ltk\x64\Debug\?.dll]]


function SetupPrintWithLineInfo()
	local old_print = _G.print;
	_G.print = function(...)
		old_print(...);
		local info = debug.getinfo(2, "Sl");
		local path;
		if info.source:byte(1) == 64 then -- '@'
			path = info.source:sub(2);
		else
			path = info.source;
		end
		old_print("\t" .. path .. ":" .. info.currentline .. ":");
	end
end

SetupPrintWithLineInfo()

print("hello world", _VERSION);

require("ltk");

LtkApi.Trace("hi ZeroBrane");
