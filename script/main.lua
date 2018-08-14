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

---@type Ltk.Window
local wnd = Ltk.Window:new();

wnd:Create({x = 10, y = 10, w = 800, h = 600}, 'borderless');
wnd:SetTitle("LTK测试窗口");
g_wnd_event = wnd:SetEventHandler({
	OnDestroy = function()
		print('OnDestroy');
		LtkApi.PostQuitMessage(0);
	end
});

LtkApi.RunMessageLoop();



---@class EmmyTest
---@field _root Ltk.Window
local EmmyTest = {}

function EmmyTest:Fun1()

	self._root = Ltk.Window:new();
	self._root:Create()
end

do -- debugger test
	local insert = table.insert
	local large_tbl = {}
	for i = 1, 10000 do
		insert(large_tbl, {x = i});
	end
	local i;
	i = 1;
end
