package.cpath = package.cpath..[[;E:\myworks\ltk\Debug\?.dll]]
package.path = [[E:\myworks\ltk\script\?.lua]]

if jit then
	jit.off();
	print(jit.version, " jit.off()")
	jit = nil;
else
	print(_VERSION, " no jit support");
end

coroutine = nil; -- coroutine is forbidden

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
		io.flush();
	end
end

SetupPrintWithLineInfo();

require("ltk");

LtkApi.Trace("Hi EmmyLua!");

-- Dark Scheme
--[[Ltk.StyleManager:SetColorScheme({
	"#2D2D30", -- clrBackground
	"#2D2D30", -- clrNormal
	"#3E3E40", -- clrHover
	"#F1F1F1", -- clrText
	"#007ACC", -- clrHighlight
	"#007ACC", -- clrBorder
	"#2D2D30", -- clrCaption
})]]

-- Light Scheme
Ltk.StyleManager:SetColorScheme({
	"#EEEEF2", -- clrBackground
	"#CCCEDB", -- clrNormal
	"#C9DEF5", -- clrHover
	"#1E1E1E", -- clrText
	"#007ACC", -- clrHighlight
	"#007ACC", -- clrBorder
	"#EEEEF2", -- clrCaption
})

Ltk.StyleManager:RegisterNinePathStyle("default_button", {
	normal = {
		atlas = {x = 0, y = 0, w = 26, h = 70},
		margin = {left = 9, top = 9, right = 10, bottom = 12},
		scale = 1.0,
	},
	hover = {
		atlas = {x = 34, y = 0, w = 26, h = 70},
		margin = {left = 9, top = 9, right = 10, bottom = 12},
		scale = 1.0,
	},
	pressed = {
		atlas = {x = 68, y = 0, w = 26, h = 70},
		margin = {left = 9, top = 9, right = 10, bottom = 12},
		scale = 1.0,
	},
	disable2 = {
		atlas = {x = 102, y = 0, w = 26, h = 70},
		margin = {left = 9, top = 9, right = 10, bottom = 12},
		scale = 1.0,
	},
})

Ltk.CheckBox:SetIconInfo(
		{ atlas = {x = 1, y = 71, w = 23, h = 23}, scale = 1.0 },
		{ atlas = {x = 27, y = 71, w = 23, h = 23}, scale = 1.0 },
		{ atlas = {x = 102, y = 0, w = 26, h = 70}, scale = 1.0 }
)

local DemoFrame = {}

function DemoFrame:OnEvent(sender, event, ...)
	local name = '<unknown>'
	if sender.GetName then
		name = sender:GetName();
	end
	-- print("OnEvent: ", event, " sender: ", sender, " name: ", name);
	if event == 'OnThumbDragging' then
		local pos = ...;
		--print(name, " : ", pos);
	elseif event == 'OnClick' then
		self._label:SetTextColor(0, 0, 1)
		sender:RemoveListener(self);
	elseif event == 'OnDestroy' then
		LtkApi.PostQuitMessage(0);
	end
end

---@type Ltk.Window
local wnd = Ltk.Window:new();

wnd:Create({x = 200, y = 200, w = 700, h = 500}, 'overlapped');
wnd:SetTitle("LTK测试窗口");
wnd:SetBackground({x = 2, y = 98, w = 152, h = 167}, {left = 40, top = 68, right = 40, bottom = 40}, 1.0);

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
DemoFrame._label = Ltk.Label:new("单选框:");
--label1:SetTextColor(1, 1, 1);
hbox1:AddLayoutItem(DemoFrame._label, 40);

local group = Ltk.RadioGroup:new();

local check = Ltk.CheckBox:new();
check:SetText("选项1");
check:SetRadioGroup(group);
hbox1:AddLayoutItem(check, 80);

check = Ltk.CheckBox:new();
check:SetText("选项2");
check:SetRadioGroup(group);
hbox1:AddLayoutItem(check, 80);

check = Ltk.CheckBox:new();
check:SetText("选项3");
check:SetRadioGroup(group);
hbox1:AddLayoutItem(check, 80);

---@type Ltk.Button
local btn = Ltk.Button:new();
hbox1:AddLayoutItem(btn, 100, 0.3);
btn:SetIcon({x = 0, y = 0, w = 64, h = 64}, 16 / 64);
btn:SetText("点击下载");
btn:SetMargin(12);

hbox1:AddSpaceItem(20, 0);

btn = Ltk.Button:new();
btn:SetText("按钮样式 囧");
---@type Ltk.Label
local btn_label = btn:GetLabel();
-- btn_label:SetTextColor(0.5,1,1);
btn_label:SetFontSize(12);
btn_label:SetFontName("宋体");
hbox1:AddLayoutItem(btn, 100);

---@type Ltk.BoxLayout
local hbox2 = Ltk.BoxLayout:new("horizontal");
vbox:AddLayoutItem(hbox2, 30, 0.4);

btn = Ltk.Button:new();
btn:SetText("Label Color Test");
btn:SetName("SetLabelColorBtn");
btn:AddEventListener(DemoFrame);
hbox2:AddLayoutItem(btn, 100, 1);

btn = Ltk.Button:new();
btn:SetText("System Uptime");
--g_btn_set_label_font_size = btn:SetEventHandler({
--	OnClick = function()
--		local tick = LtkApi.GetTickCount();
--		local second = tick / 1000;
--		local minute = second / 60;
--		second = second % 60;
--		local hour = minute / 60;
--		minute = minute % 60;
--		local day = hour / 24;
--		hour = hour % 24;
--		local floor = math.floor;
--		label:SetText(("System Uptime: %dd%02d:%02d:%02d"):format(
--				floor(day), floor(hour), floor(minute), floor(second)));
--	end,
--	Sprite = btn
--})
hbox2:AddLayoutItem(btn, 100, 1);

-----@type Ltk.TextureSprite
--local img = Ltk.TextureSprite:new();
--img:SetTexture({ x = 0, y = 85, w = 600, h = 178},
--		{ left = 100, top = 85, right = 100, bottom = 85}, 0.25, 0);
--hbox2:AddLayoutItem(img, 200, 1);
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

local hbox4 = Ltk.BoxLayout:new("horizontal");
vbox:AddLayoutItem(hbox4, 0, 1);
hbox4:AddSpaceItem(0, 1);
local vsb = Ltk.ScrollBar:new('vertical');
vsb:SetContentSize(1500);
vsb:SetPosition(200);
vsb:Update();
hbox4:AddLayoutItem(vsb, 7, 0);
vsb:AddEventListener(DemoFrame);
vsb:SetName('vsb');
--g_vsb_event = vsb:SetEventHandler({
--	OnThumbDragging = function(pos)
--		local x,y,w,h = vsb:GetRect()
--		print("vsb pos:", pos, h);
--	end
--})

local hsb = Ltk.ScrollBar:new('horizontal');
hsb:SetContentSize(1500);
hsb:SetPosition(200);
hsb:Update();
vbox:AddLayoutItem(hsb, 7, 0);
hsb:SetName('hsb');
hsb:AddEventListener(DemoFrame);
--g_hsb_event = hsb:SetEventHandler({
--	OnThumbDragging = function(pos)
--		local x,y,w,h = hsb:GetRect()
--		print("hsb pos:", pos, w);
--	end
--})

---@type Ltk.Button
--[[local btn_status = Ltk.Button:new();
btn_status:SetText("status bar");
root:AddLayoutItem(btn_status, 20);]]

root:SetSpacing(0);
root:DoLayout();

wnd:AddEventListener(DemoFrame);
--g_wnd_event = wnd:SetEventHandler({
--	OnDestroy = function()
--		print('OnDestroy');
--		LtkApi.PostQuitMessage(0);
--	end
--});

local pprint = require('pprint')

local srlz = Ltk.Serializer:new()
local rec = { data = "test text" }
rec.parent = rec;
srlz:Serialize(
		{ {{x = 1, y = 2}, "dfdfadfdfsdfsdf", 232342234}, a = 1, b = 3.2, sub = {
			-- bad_idea = rec
		}, c = "fuck", [1] = 1, [2] = 2, [3] = 3},
		"help me");
local res = srlz:Deserialize();
pprint(res);

do
	local vu8 = Ltk.VecUInt8:new();
	vu8:PushBack(1);
	vu8:PushBack(2);
	vu8:PushBack(3);
	assert(vu8:Get(1) == 1);
	assert(vu8:Get(2) == 2);
	assert(vu8:Get(3) == 3);
	vu8:Set(3, -1);
	print("VecUInt8 ", vu8:Get(3));

	local vu16 = Ltk.VecUInt16:new();
	vu16:PushBack(65536)
	vu16:PushBack(-1);
	print("VecUInt16 ", vu16:Get(1), vu16:Get(2));

	local vs8 = Ltk.VecInt8:new();
	vs8:PushBack(-129);
	print("VecInt8 ", vs8:Get(1));

	local vs32 = Ltk.VecInt32:new();
	vs32:PushBack(-19);
	vs32:PushBack(0.75);
	print("VecInt32 ", vs32:Get(1), vs32:Get(2));

	local vf32 = Ltk.VecFloat32:new();
	vf32:PushBack(0.4);
	print("VecFloat32 ", vf32:Get(1));
end

if false then
	local tinsert = table.insert;
	local list = {};
	g_vec_list = list;
	local values = {};
	g_vec_values = values;
	local idx_map = {}
	g_vec_idx = idx_map;
	local SIZE = 100 * 10000;

	values.pos = Ltk.VecUInt8:new();
	values.pos:Reserve(SIZE);
	values.char = Ltk.VecUInt16:new();
	values.char:Reserve(SIZE);
	values.x = Ltk.VecFloat32:new();
	values.x:Reserve(SIZE);
	values.y = Ltk.VecFloat32:new();
	values.y:Reserve(SIZE);
	values.w = Ltk.VecFloat32:new();
	values.w:Reserve(SIZE);
	values.h = Ltk.VecFloat32:new();
	values.h:Reserve(SIZE);

	local mt = {
		__index = function (t, k)
			return list[k]:Get(idx_map[t]);
		end,
		__newindex = function (t, k, v)
			list[k]:Set(idx_map[t], v);
		end,
		__gc = function()
			print("gc");
		end
	}
	for i = 1, SIZE do
		local item = {}
		idx_map[item] = i;
		setmetatable(item, mt);
		tinsert(list, item);
		for k,v in pairs(values) do
			v:PushBack(0);
		end
	end

	list[455].char = 33;
end

--[[local error = nil
error[1] = true;]]

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
