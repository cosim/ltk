package.cpath = package.cpath..[[;E:\myworks\ltk\x64\Debug\?.dll]]
package.path = [[E:\myworks\ltk\script\?.lua]]

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

LtkApi.Trace("Hi EmmyLua!");

Ltk.StyleManager:SetColorScheme({
	"#2D2D30", -- clrBackground1
	"#2D2D30", -- clrBackground2
	"#2D2D30", -- clrNormal
	"#3E3E40", -- clrHover
	"#F1F1F1", -- clrText
	"#007ACC", -- clrHighlight
	"#007ACC", -- clrBorder
	"#2D2D30", -- clrCaption
})

---@type Ltk.Window
local wnd = Ltk.Window:new();

wnd:Create({x = 10, y = 10, w = 800, h = 600}, 'borderless');
wnd:SetTitle("LTK测试窗口");

---@type Ltk.RectF
local rc = wnd:GetRect();
print('Window:GetRect()',rc.x, rc.y, rc.w, rc.h);

local w, h = wnd:GetClientSize();
print("Window:GetClientSize()", w, h);

---@type Ltk.BoxLayout
local root = wnd:GetRootSprite()

---@type Ltk.BoxLayout
local vbox = Ltk.BoxLayout:new("vertical");
root:AddLayoutItem(vbox, 0, 1);
vbox:SetMargin(8);
--vbox:AddSpaceItem(30, 0);

---@type Ltk.BoxLayout
local hbox1 = Ltk.BoxLayout:new("horizontal");
vbox:AddLayoutItem(hbox1, 50);

hbox1:AddSpaceItem(0, 1);

---@type Ltk.Label
local label = Ltk.Label:new("Label:");
--label1:SetTextColor(1, 1, 1);
hbox1:AddLayoutItem(label, 40);

---@type Ltk.Button
local btn = Ltk.Button:new("btn1");
hbox1:AddLayoutItem(btn, 100);

hbox1:AddSpaceItem(20, 0);

btn = Ltk.Button:new("按钮样式");
---@type Ltk.Label
local btn_label = btn:GetLabel();
btn_label:SetTextColor(0.5,1,1);
btn_label:SetFontSize(16);
btn_label:SetFontName("宋体");
hbox1:AddLayoutItem(btn, 100);

---@type Ltk.BoxLayout
local hbox2 = Ltk.BoxLayout:new("horizontal");
vbox:AddLayoutItem(hbox2, 40);

btn = Ltk.Button:new("Label Color Test");
g_btn_set_label_color = btn:SetEventHandler({
	OnClick = function() label:SetTextColor(1, 1, 1) end,
	Sprite = btn
})
hbox2:AddLayoutItem(btn, 100, 1);

btn = Ltk.Button:new("Font Size Test");
g_btn_set_label_font_size = btn:SetEventHandler({
	OnClick = function() label:SetFontSize(12) end,
	Sprite = btn
})
hbox2:AddLayoutItem(btn, 100, 1);
hbox2:AddSpaceItem(0, 4);

---@type Ltk.BoxLayout
local hbox3 = Ltk.BoxLayout:new("horizontal");

label = Ltk.Label:new("Hello World");
label:SetTextColor(1,0,0);
label:SetFontSize(20);

hbox3:AddSpaceItem(0, 1);
hbox3:AddLayoutItem(label, 300);
hbox3:AddSpaceItem(0, 1);
--label:Unref();

vbox:AddLayoutItem(hbox3, 50);

vbox:AddSpaceItem(0, 1);

---@type Ltk.Button
local btn_status = Ltk.Button:new("status bar");
root:AddLayoutItem(btn_status, 20);

root:SetSpacing(0);
root:DoLayout();

g_wnd_event = wnd:SetEventHandler({
	OnDestroy = function()
		print('OnDestroy');
		LtkApi.PostQuitMessage(0);
	end
});

local pprint = require('pprint')

local srlz = Ltk.Serializer:new()
local rec = { data = "test text" }
rec.parent = rec;
srlz:Serialize(
		{ {{x = 1, y = 2}, "dfdfadfdfsdfsdf", 232342234}, a = 1, b = 3.2, sub = {
			bad_idea = rec
		}, c = "fuck", [1] = 1, [2] = 2, [3] = 3},
		"help me");
local res = srlz:Deserialize();
pprint(res);
io.flush();

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
	for i = 1, 10 do
		insert(large_tbl, {x = i});
	end
	local i;
	i = 1;
end
