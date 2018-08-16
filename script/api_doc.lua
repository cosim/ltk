---@class Ltk.RectF
---@field x number
---@field y number
---@field w number
---@field h number
local RectF = {}

---@class Ltk.Window
local Window = {}

---@return Ltk.Window
function Window:new() end

---@param rect Ltk.RectF
---@param mode string @overlapped|borderless
function Window:Create(rect, mode) end

---@param title string
function Window:SetTitle(title) end

---@return Ltk.RectF
function Window:GetRect() end

---@return number,number
function Window:GetClientSize() end

---@return Ltk.BoxLayout
function Window:GetRootSprite() end

---@class Ltk.Sprite
local Sprite = {}

---@param sp Ltk.Sprite
function Sprite:AddChild(sp) end

---@param rc Ltk.RectF
function Sprite:SetRect(rc) end

---@class Ltk.Button:Ltk.Sprite
local Button = {}

---@param text string
function Button:new(text) end

---@class Ltk.BoxLayout: Ltk.Sprite
local BoxLayout = {}

---@param mode string @horizontal|vertical
function BoxLayout:new(mode) end

---@param item Ltk.Sprite
---@param size number
---@param grow number
function BoxLayout:AddLayoutItem(item, size, grow) end

---@param size number
---@param grow number
function BoxLayout:AddSpaceItem(size, grow) end

function BoxLayout:DoLayout() end

