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
---@param mode string @overlapped,borderless
function Window:Create(rect, mode) end

---@param title string
function Window:SetTitle(title) end

---@return Ltk.RectF
function Window:GetRect() end

---@return number,number
function Window:GetClientSize() end
