package.cpath = package.cpath..[[;E:\myworks\ltk\x64\Debug\?.dll]]


function SetupPrintWithLineInfo()
	local old_print = _G.print;
	local format = string.format;
	_G.print = function(...)
		old_print(...);
		local info = debug.getinfo(2, "Sl");
		local path;
		if info.source:byte(1) == 64 then -- '@'
			path = info.source:sub(2);
		else
			path = info.source;
		end
		old_print(format("\t%s:%d:", path, info.currentline));
	end
end

SetupPrintWithLineInfo();

print("hello world", _VERSION);

require("ltk");

LtkApi.Trace("hi ZeroBrane");

do -- debugger test
	local insert = table.insert
	local large_tbl = {}
	for i = 1, 10000 do
		insert(large_tbl, {x = i});
	end
	local i;
	i = 1;
end

local wnd = Ltk.Window:new();
wnd:Create({x = 10, y = 10, w = 800, h = 600});
g_wnd_event = wnd:SetEventHandler({
	OnDestroy = function()
		print('OnDestroy');
		LtkApi.PostQuitMessage(0);
	end
});

LtkApi.RunMessageLoop();
