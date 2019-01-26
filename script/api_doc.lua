---@class Ltk.RectF
---@field x number
---@field y number
---@field w number
---@field h number
local RectF = {}

---@class Ltk.Margin
---@field left number
---@field top number
---@field right number
---@field bottom number
local Margin = {}

---@class Ltk
---@field RectF Ltk.RectF
---@field Margin Ltk.Margin
---@field Object Ltk.Object
---@field Window Ltk.Window
---@field Sprite Ltk.Sprite
---@field Button Ltk.Button
---@field BoxLayout Ltk.BoxLayout
---@field Label Ltk.Label
---@field Serializer Ltk.Serializer
---@field StyleManager Ltk.StyleManager
---@field ImageSprite Ltk.ImageSprite
---@field IconSprite Ltk.IconSprite
---@field TextureSprite Ltk.TextureSprite
Ltk = {}

---@class Ltk.Object
local Object = {}

---@param handler table
function Object:SetEventHandler(handler) end

---@
function Object:Unref() end

---@class Ltk.Window
local Window = {}

---@return Ltk.Window
function Window:new() end

---@param rect Ltk.RectF
---@param mode string @overlapped|borderless
function Window:Create(rect, mode) end

---@return Ltk.RectF
function Window:GetRect() end

---@return number,number
function Window:GetClientSize() end

---@param title string
function Window:SetTitle(title) end

---@return Ltk.BoxLayout
function Window:GetRootSprite() end

---@class Ltk.Sprite : Ltk.Object
local Sprite = {}

---@param sp Ltk.Sprite
function Sprite:AddChild(sp) end

---@param rc Ltk.RectF
function Sprite:SetRect(rc) end

---@class Ltk.Button:Ltk.Sprite
local Button = {}

function Button:new() end

---@param text string
function Button:SetText(text) end

---@return Ltk.Label
function Button:GetLabel() end

---@param atlas Ltk.RectF
---@param scale number
function Button:SetIcon(atlas, scale) end

---@class Ltk.BoxLayout: Ltk.Sprite
local BoxLayout = {}

---@param mode string @horizontal|vertical
function BoxLayout:new(mode) end

---@param margin number
function BoxLayout:SetMargin(margin) end

---@param spacing number
function BoxLayout:SetSpacing(spacing) end

---@param item Ltk.Sprite
---@param size number
---@param grow number
function BoxLayout:AddLayoutItem(item, size, grow) end

---@param size number
---@param grow number
function BoxLayout:AddSpaceItem(size, grow) end

function BoxLayout:DoLayout() end

---@class Ltk.Label : Ltk.Sprite
local Label = {}

---@param text string
function Label:new(text) end

---@param text string
function Label:SetText(text) end

---@param a string @leading|trailing|center|justified
function Label:SetTextAlign(a) end

---@param a string @near|far|bottom
function Label:SetParagraphAlign(a) end

---@param r number
---@param g number
---@param b number
function Label:SetTextColor(r, g, b) end

---@param fontSize number
function Label:SetFontSize(fontSize) end

---@param weight string @regular|bold
function Label:SetFontWeight(weight) end

---@param name string
function Label:SetFontName(name) end

---@class Ltk.Serializer
local Serializer = {}

function Serializer:new() end

function Serializer:Serialize(...) end

function Serializer:Deserialize() end

---@class Ltk.StyleManager
local StyleManager = {}

---@param colors table
function StyleManager:SetColorScheme(colors) end

---@class Ltk.ImageSprite
local ImageSprite = {}

---@param path string
function ImageSprite:SetImage(path) end

---@class Ltk.TextureSprite
local TextureSprite = {}

---@param atlas Ltk.RectF
---@param margin Ltk.Margin
---@param scale number
---@param idx number
function TextureSprite:SetTexture(atlas, margin, scale, idx) end

---@class Ltk.IconSprite
local IconSprite = {}

function IconSprite:SetIcon() end

---@class Ltk.ScrollBar
local ScrollBar = {}

---@param mode string @horizontal|vertical
function ScrollBar:new(mode) end

function ScrollBar:Update(size, pos) end
